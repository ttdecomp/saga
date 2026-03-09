
function(get_ndk)
    set(NDK_DIR "${CMAKE_SOURCE_DIR}/ndk")
    set(NDK_EXTRACT_DIR "${NDK_DIR}/android-ndk-r8e")
    set(NDK_SYSROOT "${NDK_EXTRACT_DIR}/platforms/android-9/arch-x86" PARENT_SCOPE)
    
    if (CMAKE_HOST_WIN32)
        set(NDK "android-ndk-r8e-windows-x86_64.zip")
        set(NDK_TOOLCHAIN "${NDK_EXTRACT_DIR}/toolchains/x86-4.7/prebuilt/windows-x86_64" PARENT_SCOPE)
    else()
        set(NDK "android-ndk-r8e-linux-x86_64.tar.bz2")
        set(NDK_TOOLCHAIN "${NDK_EXTRACT_DIR}/toolchains/x86-4.7/prebuilt/linux-x86_64" PARENT_SCOPE)
    endif()
    
    set(NDK_URL "https://dl.google.com/android/ndk/${NDK}")
    set(NDK_ARCHIVE "${NDK_DIR}/${NDK}")

    include_directories(SYSTEM "${NDK_EXTRACT_DIR}/sources/cxx-stl/system/include/")
    
    # Download Android NDK if not present
    if(NOT EXISTS "${NDK_EXTRACT_DIR}")
        file(MAKE_DIRECTORY "${NDK_DIR}")

        if(NOT EXISTS "${NDK_ARCHIVE}")
            message(STATUS "Downloading Android NDK r8e...")
            file(DOWNLOAD "${NDK_URL}" "${NDK_ARCHIVE}"
                SHOW_PROGRESS
                STATUS download_status)
            list(GET download_status 0 download_code)
            if(NOT download_code EQUAL 0)
                message(FATAL_ERROR "Failed to download NDK: ${download_status}")
            endif()
        endif()

        message(STATUS "Extracting Android NDK r8e...")
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E tar xjf "${NDK_ARCHIVE}"
            WORKING_DIRECTORY "${NDK_DIR}"
            RESULT_VARIABLE extract_result)
        if(NOT extract_result EQUAL 0)
            message(FATAL_ERROR "Failed to extract NDK")
        endif()

        file(REMOVE "${NDK_ARCHIVE}")
        message(STATUS "Android NDK r8e ready")
    else()
        message(STATUS "Android NDK r8e already present")
    endif()
endfunction()
