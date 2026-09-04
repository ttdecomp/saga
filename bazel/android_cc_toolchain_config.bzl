"""C++ toolchain configuration for the matching Android x86 build."""

load("@rules_cc//cc:action_names.bzl", "ACTION_NAMES")
load("@rules_cc//cc:cc_toolchain_config_lib.bzl", "feature", "flag_group", "flag_set", "tool_path")
load("@rules_cc//cc/common:cc_common.bzl", "cc_common")
load("@rules_cc//cc/toolchains:cc_toolchain_config_info.bzl", "CcToolchainConfigInfo")

def _android_cc_toolchain_config_impl(ctx):
    ndk_root = ctx.file.ndk_marker.dirname
    toolchain = ndk_root + "/toolchains/x86-4.7/prebuilt/" + ctx.attr.host_system_name
    real_toolchain = ctx.attr.real_ndk_root + "/toolchains/x86-4.7/prebuilt/" + ctx.attr.host_system_name
    prefix = "ndk/toolchains/x86-4.7/prebuilt/" + ctx.attr.host_system_name + "/bin/i686-linux-android-"
    suffix = ctx.attr.tool_suffix
    compile_actions = [
        ACTION_NAMES.c_compile,
        ACTION_NAMES.cpp_compile,
        ACTION_NAMES.cpp_header_parsing,
        ACTION_NAMES.cpp_module_codegen,
        ACTION_NAMES.cpp_module_compile,
    ]
    link_actions = [
        ACTION_NAMES.cpp_link_dynamic_library,
        ACTION_NAMES.cpp_link_executable,
        ACTION_NAMES.cpp_link_nodeps_dynamic_library,
    ]
    features = [
        feature(
            name = "force_c_language",
            enabled = True,
            flag_sets = [
                flag_set(
                    actions = [ACTION_NAMES.c_compile],
                    flag_groups = [flag_group(flags = ["-x", "c"])],
                ),
            ],
        ),
        feature(
            name = "ndk_system_stl",
            enabled = True,
            flag_sets = [
                flag_set(
                    actions = compile_actions,
                    flag_groups = [
                        flag_group(flags = [
                            "-isystem",
                            ndk_root + "/sources/cxx-stl/system/include",
                        ]),
                    ],
                ),
            ],
        ),
        feature(
            name = "ndk_gnu_stl",
            enabled = False,
            flag_sets = [
                flag_set(
                    actions = compile_actions,
                    flag_groups = [
                        flag_group(flags = [
                            "-isystem",
                            ndk_root + "/sources/cxx-stl/gnu-libstdc++/4.7/include",
                            "-isystem",
                            ndk_root + "/sources/cxx-stl/gnu-libstdc++/4.7/libs/x86/include",
                        ]),
                    ],
                ),
            ],
        ),
    ]
    if ctx.attr.host_system_name == "windows-x86_64":
        # Preserve the flags applied by the former CMake build when driving
        # the Android NDK from Windows.
        features.append(feature(
            name = "windows_host_compat",
            enabled = True,
            flag_sets = [
                flag_set(
                    actions = [ACTION_NAMES.cpp_compile],
                    flag_groups = [flag_group(flags = [
                        "-Wno-return-type",
                        "-static-libgcc",
                        "-static-libstdc++",
                    ])],
                ),
                flag_set(
                    actions = link_actions,
                    flag_groups = [flag_group(flags = [
                        "-static-libgcc",
                        "-static-libstdc++",
                        "-rdynamic",
                    ])],
                ),
            ],
        ))
    paths = [
        tool_path(name = "ar", path = prefix + "ar" + suffix),
        tool_path(name = "cpp", path = prefix + "cpp" + suffix),
        # Bazel uses this tool path for C, C++, and the final link. The
        # force_c_language feature restores C semantics for c_compile while
        # retaining the original g++ executable link driver.
        tool_path(name = "gcc", path = prefix + "g++" + suffix),
        tool_path(name = "gcov", path = prefix + "gcov" + suffix),
        tool_path(name = "ld", path = prefix + "ld" + suffix),
        tool_path(name = "nm", path = prefix + "nm" + suffix),
        tool_path(name = "objcopy", path = prefix + "objcopy" + suffix),
        tool_path(name = "objdump", path = prefix + "objdump" + suffix),
        tool_path(name = "strip", path = prefix + "strip" + suffix),
    ]

    return cc_common.create_cc_toolchain_config_info(
        ctx = ctx,
        abi_libc_version = "bionic-api-9",
        abi_version = "x86",
        builtin_sysroot = ndk_root + "/platforms/android-9/arch-x86",
        compiler = "gcc-4.7",
        cxx_builtin_include_directories = [
            ndk_root + "/platforms/android-9/arch-x86/usr/include",
            toolchain + "/lib/gcc/i686-linux-android/4.7/include",
            toolchain + "/lib/gcc/i686-linux-android/4.7/include-fixed",
            ndk_root + "/sources/cxx-stl/system/include",
            ndk_root + "/sources/cxx-stl/gnu-libstdc++/4.7/include",
            ndk_root + "/sources/cxx-stl/gnu-libstdc++/4.7/libs/x86/include",
            ctx.attr.real_ndk_root + "/platforms/android-9/arch-x86/usr/include",
            real_toolchain + "/lib/gcc/i686-linux-android/4.7/include",
            real_toolchain + "/lib/gcc/i686-linux-android/4.7/include-fixed",
            ctx.attr.real_ndk_root + "/sources/cxx-stl/system/include",
            ctx.attr.real_ndk_root + "/sources/cxx-stl/gnu-libstdc++/4.7/include",
            ctx.attr.real_ndk_root + "/sources/cxx-stl/gnu-libstdc++/4.7/libs/x86/include",
        ],
        features = features,
        host_system_name = ctx.attr.host_system_name,
        target_cpu = "x86_32",
        target_libc = "bionic",
        target_system_name = "android",
        tool_paths = paths,
        toolchain_identifier = "android-ndk-r8e-x86-api9-" + ctx.attr.host_system_name,
    )

android_cc_toolchain_config = rule(
    implementation = _android_cc_toolchain_config_impl,
    attrs = {
        "host_system_name": attr.string(mandatory = True),
        "ndk_marker": attr.label(allow_single_file = True, mandatory = True),
        "real_ndk_root": attr.string(mandatory = True),
        "tool_suffix": attr.string(default = ""),
    },
    provides = [CcToolchainConfigInfo],
)
