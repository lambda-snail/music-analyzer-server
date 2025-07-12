include(FetchContent)

cmake_minimum_required(VERSION 3.30)
project(music-analyzer-server)

set(CMAKE_CXX_STANDARD 23)

FetchContent_Declare(
        nlohmann
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.12.0
)

FetchContent_MakeAvailable(nlohmann)

add_library(nlohmann INTERFACE)
target_include_directories(nlohmann INTERFACE ${nlohmann_SOURCE_DIR}/include)

add_executable(music-analyzer-server main.cpp
        src/components/todo_item_view.cpp
        src/components/todo_item_view.hpp
        src/components/song_view.cpp
        src/components/song_view.hpp
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
        src/pages/registration_view.hpp
        src/services/audio_features_service.cpp
        src/services/audio_features_service.hpp
        src/models/audio.hpp)

target_include_directories(music-analyzer-server PUBLIC src)

target_link_libraries(music-analyzer-server
        PUBLIC
        nlohmann)

target_link_libraries(music-analyzer-server PUBLIC curl wt wthttp wtdbo wtdbosqlite3)

add_custom_command(
        TARGET music-analyzer-server PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/config
        ${CMAKE_BINARY_DIR}/config)

add_custom_command(
        TARGET music-analyzer-server PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/resources
        ${CMAKE_BINARY_DIR}/resources)