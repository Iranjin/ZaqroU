cmake_minimum_required(VERSION 3.15)
project(Zaqro_U)

set(CMAKE_CXX_STANDARD 20)

# ===============================
# 実行ファイル・ライブラリ出力先の明示
# ===============================
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

include_directories(${PROJECT_SOURCE_DIR}/include)

file(GLOB_RECURSE CPP_SOURCES CONFIGURE_DEPENDS src/*.cpp include/*.cpp)
file(GLOB_RECURSE C_SOURCES CONFIGURE_DEPENDS src/*.c include/*.c libs/**/*.c)

set(SOURCES ${CPP_SOURCES} ${C_SOURCES})

if(APPLE)
    set(APP_ICON "Zaqro U.icns")

    set_source_files_properties(
        res/icons/${APP_ICON}
        PROPERTIES
        MACOSX_PACKAGE_LOCATION "Resources"
    )

    add_executable(Zaqro_U MACOSX_BUNDLE ${SOURCES} res/icons/${APP_ICON})

    set_target_properties(Zaqro_U PROPERTIES
        OUTPUT_NAME "Zaqro U"
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_ICON_FILE "${APP_ICON}"
    )
else()
    add_executable(Zaqro_U ${SOURCES})
endif()

# target_compile_definitions(Zaqro_U PRIVATE DEBUG_MODE)

target_include_directories(Zaqro_U PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}
)

# ===============================
# 外部依存ライブラリの検索
# ===============================
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(CURL REQUIRED)
find_package(lz4 REQUIRED)
find_package(tinyxml2 REQUIRED)
find_package(tinyfiledialogs REQUIRED)
find_package(boost_asio CONFIG REQUIRED)
find_package(nlohmann_json REQUIRED)

target_link_libraries(Zaqro_U PRIVATE
    glfw
    imgui::imgui
    glad::glad
    OpenGL::GL
    lz4::lz4
    CURL::libcurl
    tinyxml2::tinyxml2
    tinyfiledialogs::tinyfiledialogs
    Boost::asio
    nlohmann_json::nlohmann_json
)

# ========================================
# 📦 Embed all TTF fonts from res/fonts/
# ========================================
file(GLOB FONT_FILES ${CMAKE_CURRENT_SOURCE_DIR}/res/fonts/*.ttf)

set(GENERATED_FONT_HEADERS "")

foreach(FONT_PATH ${FONT_FILES})
    get_filename_component(FONT_NAME ${FONT_PATH} NAME_WE)
    string(REPLACE "-" "_" SAFE_FONT_NAME ${FONT_NAME})
    set(HEADER_PATH ${CMAKE_CURRENT_BINARY_DIR}/${SAFE_FONT_NAME}_ttf.h)
    set(TEMP_TTF ${CMAKE_CURRENT_BINARY_DIR}/${SAFE_FONT_NAME}.ttf)

    add_custom_command(
        OUTPUT ${HEADER_PATH}
        COMMAND ${CMAKE_COMMAND} -E copy ${FONT_PATH} ${TEMP_TTF}
        COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_CURRENT_BINARY_DIR} xxd -i ${SAFE_FONT_NAME}.ttf > ${HEADER_PATH}
        DEPENDS ${FONT_PATH}
        COMMENT "Embedding font: ${SAFE_FONT_NAME}.ttf"
        VERBATIM
    )

    list(APPEND GENERATED_FONT_HEADERS ${HEADER_PATH})
endforeach()

add_custom_target(EmbedFontHeaders ALL DEPENDS ${GENERATED_FONT_HEADERS})

add_dependencies(Zaqro_U EmbedFontHeaders)
target_sources(Zaqro_U PRIVATE ${GENERATED_FONT_HEADERS})
