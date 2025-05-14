cmake_minimum_required(VERSION 3.15)
project(Zaqro_U)

set(CMAKE_CXX_STANDARD 20)

include_directories(${PROJECT_SOURCE_DIR}/include)

file(GLOB_RECURSE CPP_SOURCES CONFIGURE_DEPENDS src/*.cpp)
file(GLOB_RECURSE C_SOURCES CONFIGURE_DEPENDS src/*.c libs/**/*.c)

list(APPEND CPP_SOURCES
    include/imgui_impl_glfw.cpp
    include/imgui_impl_opengl3.cpp
)

set(SOURCES ${CPP_SOURCES} ${C_SOURCES})

add_executable(Zaqro_U ${SOURCES})

target_include_directories(Zaqro_U PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(Zaqro_U PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(CURL REQUIRED)
find_package(lz4 REQUIRED)
find_package(tinyxml2 REQUIRED)
find_package(tinyfiledialogs REQUIRED)
find_package(Boost REQUIRED system)
find_package(nlohmann_json REQUIRED system)

target_link_libraries(Zaqro_U PRIVATE
    glfw
    imgui::imgui
    glad::glad
    OpenGL::GL
    lz4::lz4
    CURL::libcurl
    tinyxml2::tinyxml2
    tinyfiledialogs::tinyfiledialogs
    Boost::system
    nlohmann_json::nlohmann_json
)
