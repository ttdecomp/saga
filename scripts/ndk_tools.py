import os
import shutil


def find_ndk_tool(tool):
    env = os.environ.get(tool.upper())
    if env and os.path.isfile(env):
        return env

    for candidate in (f"i686-linux-android-{tool}", f"llvm-{tool}", tool):
        path = shutil.which(candidate)
        if path:
            return path
    return None
