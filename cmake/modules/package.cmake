include("shared-Release/CPackConfig.cmake")
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(CPACK_INSTALL_CMAKE_PROJECTS
        shared-Release <PROJ> ALL /
        static-Release <PROJ> ALL /
    )
else()
    set(CPACK_INSTALL_CMAKE_PROJECTS
        shared-Debug   <PROJ> ALL /
        shared-Release <PROJ> ALL /
        static-Debug   <PROJ> ALL /
        static-Release <PROJ> ALL /
    )
endif()