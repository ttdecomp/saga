# Contributing

There are many ways to contribute to the project.

- Adding or modifying core code to increase the percentage matching.
- Changes to existing code which improve the overall health of the code without
  impacting percentage matched, e.g. naming or documentation.
- Adding doxygen documentation to methods to describe their purpose and
  parameters.
- Improvements to the `gonk` tool. Several ideas for improvements can be found
  in the project's GitHub issues.
- Encouragement and enthusiasm! It's always nice to know that one's work is
  appreciated.

## ! IMPORTANT NOTE !

If you have access to original source code, _do not_ contribute it to _saga_
unless authorized to do so by the original owners under this project's license.
There are no known source code leaks and there's little chance of a TT Games
employee wanting to contribute original engine code, but just in case.

That said, this author wouldn't mind seeing TT Games make this project obsolete
by releasing the original engine code in its entirety under a permissive
license.

## Matching Code

This requires a copy of the .apk for the Android version of _The Complete Saga_.
The following version is known to work for this project:

| Filename                                    | sha256sum                                                        |
| ------------------------------------------- | ---------------------------------------------------------------- |
| LEGO.SAGA.Android.ver.1.8.60.build.1060.apk | df69d191fdc5b4337dab7d1872cd9b7a6122df6d3b153e5df602aa3ff9fac7fb |

Please note that, for copyright reasons, we do not and cannot provide you with a
copy of the game.

The contents of the .apk can be extracted with a zip tool. The file
`lib/x86/libTTapp.so` must then be placed in the project's `res/` directory.

Once you have placed the shared object in the `res/` directory, mise installs
the pinned Rust toolchain, builds `gonk`, and runs the full matching pipeline:

```bash
mise run match
```

This generates ELF objects from the original shared object corresponding to the
structure of our source tree, as well as a configuration for the `objdiff` tool.

The mise configuration installs the pinned project fork of `objdiff-cli`.

The upstream `objdiff` GUI remains useful for interactive inspection, but the
command-line workflows documented in `doc/decomp/` assume `objdiff-cli 3.6.x`.

Split objects mirror the source-tree hierarchy under `build/split/`; they are
not a flat directory. Prefer `scripts/objdiff-cli.py SYMBOL` for one function,
or query `objdiff.json` for the exact target/base object paths.

## Development Scripts

The `scripts/` directory contains a few small helpers:

- `objdiff-cli.py` — **the primary tool for inspecting why a single symbol does
  not match.** It runs `objdiff-cli diff` against the symbol, discards the
  noisy JSON, and prints only the instructions where the recompiled code and
  the original diverge, aligned with context and decompiling hints (resolved
  call/jump targets, string and float constants, referenced data). It is aimed
  at automated/agent use — one mangled symbol at a time. For manual and
  whole-repository work (aggregate `report.json`, the interactive UI), use the
  `objdiff` GUI/CLI directly. See `doc/decomp/09-objdiff-cli.md`.
- `check_duplicate_definitions.py` — scans the source tree for duplicate type
  (struct/class/union/enum) definitions. Given `--build <dir>` it also runs
  `nm` over the compiled objects to report duplicate symbols (local and global,
  text and data), which catches static (file-local) functions/variables that the
  linker silently allows. The symbol check is advisory by default; pass
  `--fail-on-symbols` to make it an error.
- `check_symbols.py` — diffs the defined text symbols of the build against the
  original binary, so you can see exactly which functions are still missing.
- `matching_report.py` — turns an objdiff `report.json` into per-directory
  matching-progress tables and can write them back into `README.md`.
- `objdiffdiff.py` — compares two objdiff `report.json` files and reports
  regressions.

`check_duplicate_definitions.py` and `check_symbols.py` are run automatically as
part of the `lint` build target.

## Style Guidelines

### Clang-Format

The project uses `clang-format` to enforce a consistent coding style.
Please make sure to either run `clang-format` on any code you contribute, or configure
your editor to do this automatically.

### Naming Conventions

The original code is divided into essentially two parts as far as we can tell:
code which is essentially C and broadly-speaking survives from earlier versions
of the engine, and newer code written in object-oriented C++. Evidence from
debug symbols suggests that conventions were applied inconsistently even within
these sections

As a compromise between being true to the original source code and our own
personal tastes in readability, the following guidelines apply:

- In order to match the original binary, it's necessary to use original names
  for many functions and types. Whenever mangled symbols are present, these are
  to be matched.

- When function and type names are known from another source (e.g., mangled
  symbols from another TT Games release), they are preferred.

- When function names are not known, they should be named in `PascalCase` and
  follow existing patterns to the extent possible.

- Struct and class members, function parameters, and local variables are to be
  given descriptive names in `snake_case`, without regard to original naming.
  Even when original names are known from debug symbols, these are generally
  considered to be insufficiently descriptive or too inconsistently named to be
  appropriate, and there is no need for matching.

- Enum members are named in `UPPER_SNAKE_CASE`.

- Macros are named in `UPPER_SNAKE_CASE`.

- In C-style code:
  - Types are most commonly named in `lowercase` and followed by a tag, `_s` for
    structs and `_e` for enums, accompanied by a typedef of the same name in
    `UPPERCASE` without the trailing tag.

- In C++-style code:
  - Classes and their constituent types are named in `PascalCase`.

## Git hooks

Run `mise run setup` once after cloning. It installs all pinned development
tools and uses prek to install the repository hook.

The hook formats changed C/C++ source files, builds the target, regenerates
`objdiff.json`/`report.json`, updates the README matching table and badge, and
runs the symbol check when `res/libTTapp.so` is available. Review and stage any
generated changes before committing again.

For repository-specific agent guidance, see
`skills/saga-decomp/SKILL.md`. The package uses the Codex `SKILL.md` format and
can be copied to a standard skill-discovery directory if automatic invocation
is desired.
