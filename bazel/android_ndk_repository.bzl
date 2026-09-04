"""Repository rule provisioning the legacy Android NDK r8e x86 toolchain."""

_NDK_ARCHIVES = {
    "linux": (
        "android-ndk-r8e-linux-x86_64.tar.bz2",
        "07371eaa242490096be1bf494ecea1e5a6b7839cbead329fb868c0f54b716f24",
        "linux-x86_64",
        "",
    ),
    "mac os x": (
        "android-ndk-r8e-darwin-x86_64.tar.bz2",
        "da702155a718513de213aa9ac80e72c0dedd952ffa8278fba9c3ec2985fc70cc",
        "darwin-x86_64",
        "",
    ),
    "windows": (
        "android-ndk-r8e-windows-x86_64.zip",
        "337bc76630053e650e285b5840e071b59a82c74d23ed38097afec98192e649fa",
        "windows-x86_64",
        ".exe",
    ),
}

def _build_file(host_tag, tool_suffix, real_ndk_root):
    return """
load("@rules_cc//cc/toolchains:cc_toolchain.bzl", "cc_toolchain")
load(":cc_toolchain_config.bzl", "android_cc_toolchain_config")

package(default_visibility = ["//visibility:public"])

filegroup(
    name = "toolchain_files",
    srcs = glob(["ndk/**"]),
)

android_cc_toolchain_config(
    name = "config",
    host_system_name = "{host_tag}",
    ndk_marker = "ndk/RELEASE.TXT",
    real_ndk_root = {real_ndk_root},
    tool_suffix = "{tool_suffix}",
)

cc_toolchain(
    name = "cc_toolchain",
    all_files = ":toolchain_files",
    ar_files = ":toolchain_files",
    compiler_files = ":toolchain_files",
    dwp_files = ":toolchain_files",
    linker_files = ":toolchain_files",
    objcopy_files = ":toolchain_files",
    strip_files = ":toolchain_files",
    supports_param_files = 0,
    toolchain_config = ":config",
)

toolchain(
    name = "toolchain",
    target_compatible_with = [
        "@platforms//cpu:x86_32",
        "@platforms//os:android",
    ],
    toolchain = ":cc_toolchain",
    toolchain_type = "@bazel_tools//tools/cpp:toolchain_type",
)
""".format(
        host_tag = host_tag,
        real_ndk_root = json.encode(real_ndk_root),
        tool_suffix = tool_suffix,
    )

def _android_ndk_repository_impl(ctx):
    host_name = ctx.os.name.lower()
    if host_name.startswith("windows"):
        host_name = "windows"
    if host_name not in _NDK_ARCHIVES:
        fail("Android NDK r8e is not available for Bazel host OS %r" % ctx.os.name)

    archive, sha256, host_tag, tool_suffix = _NDK_ARCHIVES[host_name]
    local_ndk = ctx.os.environ.get("SAGA_NDK_R8E", "")
    if local_ndk:
        local_path = ctx.path(local_ndk)
        if not local_path.exists:
            fail("SAGA_NDK_R8E does not exist: %s" % local_ndk)
        ctx.symlink(local_path, "ndk")
        real_ndk_root = str(local_path)
    else:
        ctx.download_and_extract(
            output = "download",
            sha256 = sha256,
            url = "https://dl.google.com/android/ndk/" + archive,
        )
        ctx.symlink(ctx.path("download/android-ndk-r8e"), "ndk")
        real_ndk_root = str(ctx.path("download/android-ndk-r8e"))

    ctx.template(
        "cc_toolchain_config.bzl",
        ctx.attr._config_template,
        substitutions = {},
    )
    ctx.file("BUILD.bazel", _build_file(host_tag, tool_suffix, real_ndk_root))

android_ndk_repository = repository_rule(
    implementation = _android_ndk_repository_impl,
    attrs = {
        "_config_template": attr.label(
            default = Label("//bazel:android_cc_toolchain_config.bzl"),
        ),
    },
    environ = ["SAGA_NDK_R8E"],
)
