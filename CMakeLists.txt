cmake_minimum_required(VERSION 3.30)
project(todo)

set(CMAKE_CXX_STANDARD 23)

add_executable(todo main.cpp
        src/components/todo_item_view.cpp
        src/components/todo_item_view.hpp
        src/components/file_view.cpp
        src/components/file_view.hpp
        src/pages/processing_page.cpp
        src/pages/processing_page.hpp
        src/application/user.hpp
        src/application/user.cpp
        src/application/session.cpp
        src/application/session.hpp
        src/models/user_details.cpp
        src/models/user_details.hpp
        src/pages/authentication_page.cpp
        src/pages/authentication_page.hpp
        src/pages/registration_view.cpp
        src/pages/registration_view.hpp)

target_include_directories(todo PUBLIC src)

target_link_libraries(todo PUBLIC wt wthttp wtdbo wtdbosqlite3)

add_custom_command(
        TARGET todo PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/config
        ${CMAKE_BINARY_DIR}/config)

add_custom_command(
        TARGET todo PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/resources
        ${CMAKE_BINARY_DIR}/resources)