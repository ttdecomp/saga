if(NOT DEFINED CONFIG_SUB)
    message(FATAL_ERROR "CONFIG_SUB is required")
endif()

file(READ "${CONFIG_SUB}" contents)
string(REPLACE
    "nto-qnx* | linux-gnu* | linux-dietlibc"
    "nto-qnx* | linux-gnu* | linux-android* | linux-dietlibc"
    contents "${contents}"
)
string(REPLACE
    "-mingw32* | -linux-gnu* | -linux-newlib*"
    "-mingw32* | -linux-gnu* | -linux-android* | -linux-newlib*"
    contents "${contents}"
)
file(WRITE "${CONFIG_SUB}" "${contents}")
