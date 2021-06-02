include("shared-Release/CPackConfig.cmake")

set(CPACK_INSTALL_CMAKE_PROJECTS
    shared-Debug   <PROJ> ALL /
    shared-Release <PROJ> ALL /
    static-Debug   <PROJ> ALL /
    static-Release <PROJ> ALL /
)