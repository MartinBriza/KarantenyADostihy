# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check http://doc.qt.io/qt-5/deployment-android.html for more information.
# They need to be set before the find_package(Qt5 ...) call.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

include_directories(../common)

set(TS_FILES Client_en_US.ts Client_cs_CZ.ts)

qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
configure_file(translations.qrc ${CMAKE_BINARY_DIR} COPYONLY)

set(SOURCE_FILES
    main.cpp
    game.cpp
    datamodel.cpp

    ../common/protocol.h

    ../def/fields.h
    ../def/chance.h
    ../def/finance.h

    ${CMAKE_BINARY_DIR}/translations.qrc
    ${QM_FILES}
)

if(ANDROID)
    add_library(Client SHARED
      qml.qrc
      ${SOURCE_FILES}
      ${TS_FILES}
    )
else()
    add_executable(Client MACOSX_BUNDLE
      qml.qrc
      ${SOURCE_FILES}
      ${TS_FILES}
    )
endif()

target_compile_definitions(Client
  PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(Client
  PRIVATE Qt5::Core Qt5::Quick Qt5::Network)

install(TARGETS Client DESTINATION bin)
