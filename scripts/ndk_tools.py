import os
import sys


def find_ndk_tool(root, tool):
    env = os.environ.get(tool.upper())
    if env and os.path.isfile(env):
        return env

    if sys.platform == "win32":
        host_tag = "windows-x86_64"
        suffix = ".exe"
    elif sys.platform == "darwin":
        host_tag = "darwin-x86_64"
        suffix = ""
    else:
        host_tag = "linux-x86_64"
        suffix = ""

    return os.path.join(
        root,
        "ndk",
        "android-ndk-r8e",
        "toolchains",
        "x86-4.7",
        "prebuilt",
        host_tag,
        "bin",
        f"i686-linux-android-{tool}{suffix}",
    )
