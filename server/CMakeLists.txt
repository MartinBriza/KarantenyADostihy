include_directories(../common)

set(SOURCE_FILES
    main.cpp
    server.cpp
    clienthandler.cpp
)

add_executable(Server
    ${SOURCE_FILES}
)

target_link_libraries(Server PRIVATE
    Qt5::Core Qt5::Network Qt5::Gui
)

