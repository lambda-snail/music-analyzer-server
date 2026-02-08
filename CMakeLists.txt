include(FetchContent)

cmake_minimum_required(VERSION 4.0)
project(music-analyzer-server
    VERSION 0.8.5
)

set(CMAKE_CXX_STANDARD 23)

set(LIBWT_PATH /usr/local/lib/libwt.so.4.12.2)
set(LIBWTDBO_PATH /usr/local/lib/libwtdbo.so.4.12.2)
set(LIBWTDBOSQLITE_PATH /usr/local/lib/libwtdbosqlite3.so.4.12.2)
set(LIBWTHTTP_PATH /usr/local/lib/libwthttp.so.4.12.2)

find_package(Boost
        REQUIRED
        url
)

FetchContent_Declare(
        nlohmann
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.12.0
)

FetchContent_MakeAvailable(nlohmann)

add_library(nlohmann INTERFACE)
target_include_directories(nlohmann INTERFACE ${nlohmann_SOURCE_DIR}/include)

add_executable(music-analyzer-server
        main.cpp
        src/components/process_log.cpp          src/components/process_log.hpp
        src/components/song_view.cpp            src/components/song_view.hpp
        src/pages/processing_page.cpp           src/pages/processing_page.hpp
        src/services/audio_features_service.cpp src/services/audio_features_service.hpp
        src/services/cookie_info.cpp            src/services/cookie_info.hpp
        src/application/lambda_resource.cpp     src/application/lambda_resource.hpp
        src/models/audio.hpp
)

#target_compile_options(music-analyzer-server PUBLIC "-fsanitize=undefined")

target_include_directories(music-analyzer-server PUBLIC src)

target_link_libraries(music-analyzer-server
        PUBLIC
        nlohmann
        curl
        boost_url
        wt
        wthttp
)

# Copy resources

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

# Copy libraries to executable directory for simplicity

add_custom_command(
        TARGET music-analyzer-server PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${LIBWT_PATH}
        ${CMAKE_BINARY_DIR})

add_custom_command(
        TARGET music-analyzer-server PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${LIBWTHTTP_PATH}
        ${CMAKE_BINARY_DIR})

# Installation

## Installs into /usr/lambda-snail/...
set(SERVER_INSTALL_DIR "lambda-snail/music-server")

install(TARGETS music-analyzer-server DESTINATION ${SERVER_INSTALL_DIR})
install(DIRECTORY "${PROJECT_SOURCE_DIR}/config" DESTINATION ${SERVER_INSTALL_DIR})
install(DIRECTORY "${PROJECT_SOURCE_DIR}/resources" DESTINATION ${SERVER_INSTALL_DIR})

set(CPACK_PACKAGE_VENDOR "LambdaSnail")
set(CPACK_PACKAGE_CONTACT "lambdasnail@placeholder.com")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A simple web application for a statistics course")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})

list(APPEND CPACK_GENERATOR DEB)
set(CPACK_DEBIAN_PACKAGE_DEPENDS "${CPACK_DEBIAN_PACKAGE_DEPENDS} libcurl4-gnutls-dev" )
set(CPACK_SOURCE_IGNORE_FILES /.git /.idea /cmake-build-*)
include(CPack)

add_custom_target(
        music-analyzer-server-deb
        COMMENT "Creating DEB package for deployment"
        COMMAND ${CMAKE_CPACK_COMMAND} -G DEB
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
        VERBATIM
)

add_dependencies(music-analyzer-server-deb music-analyzer-server)