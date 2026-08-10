if(NOT DEFINED MAKEFILE)
    message(FATAL_ERROR "MAKEFILE is required")
endif()

file(READ "${MAKEFILE}" contents)
string(REPLACE "\tranlib $@" "\t$(RANLIB) $@" contents "${contents}")
file(WRITE "${MAKEFILE}" "${contents}")
