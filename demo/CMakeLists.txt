cmake_minimum_required(VERSION 3.9)

find_package(Qt6 6.2 COMPONENTS Core Gui Widgets REQUIRED)

qt_add_executable(QtModelUtilitiesDemo
    MANUAL_FINALIZATION
    main.cpp
    demowidget.cpp
    demowidget.h
    demowidget.ui
    demohierarchylevelproxymodel.h
    demohierarchylevelproxymodel.cpp
    demohierarchylevelproxymodel.ui
)

target_link_libraries(QtModelUtilitiesDemo PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    QtModelUtilities::QtModelUtilities
)

set_target_properties(QtModelUtilitiesDemo PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
    VERSION ${VERSION_SHORT}
    MACOSX_BUNDLE_GUI_IDENTIFIER demo.qtmodelutilities.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${VERSION_MAJOR}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${VERSION_MAJOR}.${VERSION_MINOR}
)

qt_finalize_executable(QtModelUtilitiesDemo)
