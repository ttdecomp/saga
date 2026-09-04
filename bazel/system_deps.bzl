"""Expose native host libraries from pkg-config as a generated cc_library.

This rule is intentionally for native host dependencies only.  It does not
download, cross-compile, or select Android libraries; the caller can create a
separate repository instance for each host platform and select the desired
one from its BUILD/MODULE configuration.

Example::

    pkg_config_repository = use_repo_rule(
        "//bazel:system_deps.bzl",
        "pkg_config_repository",
    )
    pkg_config_repository(
        name = "system_host_deps",
        packages = ["sdl3", "vorbisfile"],
    )

The generated ``@system_host_deps//:host_deps`` target carries ``hdrs``,
``includes``, ``defines``, ``copts``, and ``linkopts``.  pkg-config does not
publish a complete, portable header manifest, so ``hdrs`` is an
empty-tolerant glob for headers that happen to be generated in the external
repository; system headers are exposed through their include search paths.

The parser handles shell quoting and backslash escaping (including paths with
spaces), separate ``-I``/``-L`` values, and macOS framework pairs such as
``-framework Cocoa`` without invoking a shell.
"""

def _shell_split(text):
    """Split pkg-config/pkgconf output into argv-like words.

    This is a deliberately small POSIX/MSYS2 shell-word parser.  It supports
    the quoting forms emitted by pkg-config, but does not evaluate expansions,
    command substitutions, or shell operators.
    """
    words = []
    word = []
    quote = ""
    escaped = False

    for char in text.elems():
        if escaped:
            if quote == '"' and char not in ['"', "\\", "$", "`", "\n"]:
                word.append("\\")
            word.append(char)
            escaped = False
        elif quote == "'":
            if char == "'":
                quote = ""
            else:
                word.append(char)
        elif quote == '"':
            if char == '"':
                quote = ""
            elif char == "\\":
                escaped = True
            else:
                word.append(char)
        elif char == "\\":
            escaped = True
        elif char == "'" or char == '"':
            quote = char
        elif char in [" ", "\t", "\r", "\n"]:
            if word:
                words.append("".join(word))
                word = []
        else:
            word.append(char)

    if escaped:
        # A dangling backslash is more likely to be a Windows path character
        # than an intended escape in malformed pkg-config output.
        word.append("\\")
    if quote:
        fail("pkg-config returned an unterminated %s quote: %r" % (quote, text))
    if word:
        words.append("".join(word))
    return words

def _append_unique(values, value):
    if value not in values:
        values.append(value)

def _value_after(tokens, index, flag):
    if index + 1 >= len(tokens):
        fail("pkg-config returned %s without a following value" % flag)
    return tokens[index + 1]

def _parse_cflags(tokens):
    """Return the transitive include roots, defines, and remaining cflags."""
    includes = []
    defines = []
    copts = []
    skip_next = False

    for index in range(len(tokens)):
        if skip_next:
            skip_next = False
            continue

        token = tokens[index]
        if token in ["-I", "-isystem"]:
            _append_unique(includes, _value_after(tokens, index, token))
            skip_next = True
            continue
        if token.startswith("-I") and len(token) > 2:
            _append_unique(includes, token[2:])
            continue
        if token.startswith("-isystem") and len(token) > len("-isystem"):
            _append_unique(includes, token[len("-isystem"):])
            continue
        if token == "-D":
            _append_unique(defines, _value_after(tokens, index, token))
            skip_next = True
            continue
        if token.startswith("-D") and len(token) > 2:
            _append_unique(defines, token[2:])
            continue

        # Keep options such as -pthread, -fvisibility=hidden, and -iquote
        # intact.  Unlike includes, cc_library.copts are target-local, but
        # this target is designed to be depended on by the host binary.
        copts.append(token)

    return struct(includes = includes, defines = defines, copts = copts)

def _parse_libs(tokens):
    """Keep linker flags in order, preserving framework and search-path pairs."""
    linkopts = []
    skip_next = False

    for index in range(len(tokens)):
        if skip_next:
            skip_next = False
            continue

        token = tokens[index]
        if token in ["-framework", "-weak_framework", "-L", "-F"]:
            linkopts.extend([token, _value_after(tokens, index, token)])
            skip_next = True
            continue
        linkopts.append(token)

    return linkopts

def _quote(value):
    """Quote one value for a generated BUILD file."""
    return "'" + value.replace("\\", "\\\\").replace("'", "\\'").replace("\n", "\\n").replace("\r", "\\r") + "'"

def _shell_quote_token(value):
    """Quote a value that Bazel will Bourne-tokenize from copts/linkopts."""
    needs_quotes = False
    for char in value.elems():
        if char in [" ", "\t", "\r", "\n", "'", '"', "\\", "$", "`", ";", "|", "&", "<", ">", "(", ")", "*", "?", "[", "]", "{", "}"]:
            needs_quotes = True
            break
    if not needs_quotes:
        return value

    # The result is consumed by Bazel's Bourne tokenizer, not by a shell.
    return "'" + value.replace("'", "'\\''") + "'"

def _render_list(name, values, shell_tokenize = False):
    lines = ["    %s = [" % name]
    for value in values:
        if shell_tokenize:
            value = _shell_quote_token(value)
        lines.append("        %s," % _quote(value))
    lines.append("    ],")
    return "\n".join(lines)

def _render_build(cflags, linkopts):
    return "\n".join([
        "# Generated by //bazel:system_deps.bzl; do not edit.",
        "load('@rules_cc//cc:cc_library.bzl', 'cc_library')",
        "",
        "# pkg-config has no complete portable header-manifest interface.",
        "cc_library(",
        "    name = 'host_deps',",
        "    # Include roots below expose the installed package headers.",
        "    hdrs = glob([",
        "        '**/*.h',",
        "        '**/*.hh',",
        "        '**/*.hpp',",
        "        '**/*.inc',",
        "    ], allow_empty = True),",
        _render_list("includes", cflags.includes),
        _render_list("defines", cflags.defines, shell_tokenize = True),
        _render_list("copts", cflags.copts, shell_tokenize = True),
        _render_list("linkopts", linkopts, shell_tokenize = True),
        "    visibility = ['//visibility:public'],",
        ")",
        "",
    ])

def _run_pkg_config(ctx, executable, arguments):
    result = ctx.execute([executable] + arguments, quiet = True, timeout = 30)
    if result.return_code != 0:
        command = " ".join([str(executable)] + arguments)
        detail = result.stderr.strip()
        if result.stdout.strip():
            detail = (detail + "\n" + result.stdout.strip()).strip()
        fail("\n".join([
            "pkg-config failed while resolving native host dependencies.",
            "  command: %s" % command,
            "  exit code: %d" % result.return_code,
            "  output: %s" % (detail or "(no diagnostic output)"),
            "Install the requested development packages and ensure pkg-config " +
            "can find them (check PKG_CONFIG_PATH/PKG_CONFIG_LIBDIR). On MSYS2, " +
            "run Bazel from the matching MSYS2 environment; on macOS, install " +
            "the Homebrew formulae or provide their .pc directory.",
        ]))
    return _shell_split(result.stdout)

def _pkg_config_repository_impl(ctx):
    packages = ctx.attr.packages
    if not packages:
        fail("pkg_config_repository requires at least one package name")
    for package in packages:
        if not package or package.startswith("-"):
            fail("pkg_config_repository package names must be non-empty names, got %r" % package)

    executable = ctx.which(ctx.attr.pkg_config)
    if executable == None:
        fail(
            (
                "Could not find %r in PATH while fetching native host dependencies. " +
                "Install pkg-config/pkgconf and the requested development packages, " +
                "then retry (or set pkg_config to an executable path)."
            ) % ctx.attr.pkg_config,
        )

    package_args = list(packages)
    cflags = _parse_cflags(_run_pkg_config(ctx, executable, ["--cflags"] + package_args))
    linkopts = _parse_libs(_run_pkg_config(ctx, executable, ["--libs"] + package_args))
    linkopts = [ctx.attr.linkopt_replacements.get(value, value) for value in linkopts]
    ctx.file("BUILD.bazel", _render_build(cflags, linkopts))

pkg_config_repository = repository_rule(
    implementation = _pkg_config_repository_impl,
    attrs = {
        "packages": attr.string_list(
            mandatory = True,
            doc = "pkg-config package names, for example ['sdl3', 'vorbisfile'].",
        ),
        "pkg_config": attr.string(
            default = "pkg-config",
            doc = "pkg-config/pkgconf executable name or path.",
        ),
        "linkopt_replacements": attr.string_dict(
            doc = "Exact linker-option corrections for broken package metadata.",
        ),
    },
    # These repositories intentionally reflect locally installed host SDKs.
    local = True,
    configure = True,
    environ = [
        "PATH",
        "PKG_CONFIG_PATH",
        "PKG_CONFIG_LIBDIR",
        "PKG_CONFIG_SYSROOT_DIR",
    ],
    doc = "Generate @name//:host_deps from native pkg-config packages.",
)

# Short name retained for callers that prefer the rule name from the file.
system_deps = pkg_config_repository
