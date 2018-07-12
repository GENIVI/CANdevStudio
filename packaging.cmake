# Package generation

set(CPACK_PACKAGE_DESCRIPTION "CANDevStudio - CAN simulation software")
set(CPACK_PACKAGE_VERSION_MAJOR "${VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${VERSION_MINOR}")
if(GIT_COMMIT_HASH)
    set(CPACK_PACKAGE_VERSION_PATCH ${GIT_COMMIT_HASH})
else()
    set(CPACK_PACKAGE_VERSION_PATCH "${VERSION_PATCH}")
endif()
if(STANDALONE)
    set(SYSTEM_NAME ${CMAKE_SYSTEM_NAME})
    if(WIN32)
        # For some reason this name is set to Windows rather than win32. To keep naming compiant change win32.
        set(SYSTEM_NAME win32)
    endif()
    set(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${VERSION_MAJOR}.${VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}-${SYSTEM_NAME}-standalone")
endif()
set(CPACK_PACKAGE_VENDOR "Mobica")
set(CPACK_PACKAGE_CONTACT "remigiusz.kollataj@mobica.com")
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/LICENSE)

if(APPLE)
    include("${CMAKE_SOURCE_DIR}/cmake/osx_bundle.cmake")
endif()

include(CPack)
