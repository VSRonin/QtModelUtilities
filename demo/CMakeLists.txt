cmake_minimum_required(VERSION 3.9)

find_package(Qt6 6.3 COMPONENTS Core Gui Widgets LinguistTools REQUIRED)
find_package(Qt6 6.3 COMPONENTS Test)
include(GNUInstallDirs)

qt_add_executable(QtModelUtilitiesDemo
    MANUAL_FINALIZATION
    main.cpp
    demowidget.cpp
    demowidget.h
    demowidget.ui
    demohierarchylevelproxymodel.h
    demohierarchylevelproxymodel.cpp
    demohierarchylevelproxymodel.ui
    demorootindexproxymodel.h
    demorootindexproxymodel.cpp
    demorootindexproxymodel.ui
    roleseditdialog.cpp
    roleseditdialog.h
    roleseditdialog.ui
    templatedemoview.h
)

target_link_libraries(QtModelUtilitiesDemo PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    QtModelUtilities::QtModelUtilities
)

if(${Qt6Test_FOUND})
    target_link_libraries(QtModelUtilitiesDemo PRIVATE
        Qt6::Test
    )
endif()

set_target_properties(QtModelUtilitiesDemo PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    VERSION ${VERSION_SHORT}
    MACOSX_BUNDLE_GUI_IDENTIFIER demo.qtmodelutilities.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${VERSION_MAJOR}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${VERSION_MAJOR}.${VERSION_MINOR}
)

qt_add_translations(QtModelUtilitiesDemo TS_FILES qmud_en.ts QM_FILES_OUTPUT_VARIABLE qmud_qm_files)

qt_finalize_executable(QtModelUtilitiesDemo)

if(MODEL_UTILITIES_INSTALL)
    install(TARGETS QtModelUtilitiesDemo
        BUNDLE  DESTINATION .
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

    qt_generate_deploy_app_script(
        TARGET QtModelUtilitiesDemo
        OUTPUT_SCRIPT qmud_deploy_script
        NO_UNSUPPORTED_PLATFORM_ERROR
    )

    install(FILES ${qmud_qm_files} DESTINATION "translations")

    install(SCRIPT ${qmud_deploy_script})
endif()
