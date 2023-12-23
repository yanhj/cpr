cmake_minimum_required(VERSION 3.15)

if("${CPR_CPACK}" STREQUAL "DEB")
    # Docs: https://cmake.org/cmake/help/latest/cpack_gen/deb.html
    message(STATUS "CPack debian backend enabled")

    set(CPACK_GENERATOR "DEB")
    set(CPACK_DEB_COMPONENT_INSTALL ON)
    set(CPACK_DEBIAN_PACKAGE_NAME "lib${PROJECT_NAME}")
    set(CPACK_DEBIAN_FILE_NAME "lib${PROJECT_NAME}_${PROJECT_VERSION}.deb")
    set(CPACK_DEBIAN_PACKAGE_VERSION "${PROJECT_VERSION}")
    set(CPACK_DEBIAN_PACKAGE_RELEASE "1")
    execute_process(COMMAND dpkg --print-architecture
                    OUTPUT_VARIABLE CPACK_DEBIAN_PACKAGE_ARCHITECTURE)

    if(CPR_USE_SYSTEM_CURL)
        set(CPACK_DEBIAN_PACKAGE_DEPENDS "libcurl4")
    else()
        message(WARNING "Debian package build enabled without using system curl. This is discouraged since debian packages are required to use system dependencies! Ref: https://www.debian.org/doc/debian-policy/ch-sharedlibs.html")
    endif()

    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Fabian Sauter <deb@libcpr.org>")
    set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "${PROJECT_DESCRIPTION}")
    set(CPACK_DEBIAN_PACKAGE_SECTION "libs")
    set(CPACK_DEBIAN_ARCHIVE_TYPE "gnutar")
    set(CPACK_DEBIAN_COMPRESSION_TYPE "gzip")
    set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
    set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "${PROJECT_HOMEPAGE_URL}")

    set(CPACK_SOURCE_IGNORE_FILES "/\\\\.git"
                                  "/\\\\.github"
                                  "/debian/"
                                  "/.*build-.*/"
                                  ${PROJECT_BINARY_DIR})
    set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
endif()

include(CPack)