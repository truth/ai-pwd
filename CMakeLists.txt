cmake_minimum_required(VERSION 3.21)
project(ai_pwd LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)

add_compile_definitions(NOMINMAX WIN32_LEAN_AND_MEAN UNICODE _UNICODE)

set(AI_WIN_UI_DIR "F:/AI-Projects/ai-win-ui" CACHE PATH "Path to ai-win-ui repository")

# Match static CRT (/MT /MTd) with ai_win_ui_lib
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

set(SOURCES
    src/main.cpp
    src/db/vault_db.h
    src/db/vault_db.cpp
    src/generator/pwd_generator.h
    src/generator/pwd_generator.cpp
    src/ui/layout_builder.h
    src/ui/layout_builder.cpp
    src/app/clipboard_helper.h
    src/app/app_controller.h
    src/app/app_controller.cpp
    src/app.rc
    third_party/sqlite3/sqlite3.c
)

add_executable(ai_pwd WIN32 ${SOURCES})

target_include_directories(ai_pwd PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/sqlite3
    ${AI_WIN_UI_DIR}/include
    ${AI_WIN_UI_DIR}/src
    ${AI_WIN_UI_DIR}/third_party/yoga-3.2.1
)

if(MSVC)
    target_compile_options(ai_pwd PRIVATE /utf-8 /wd4819 /permissive-)
    target_link_options(ai_pwd PRIVATE
        "/alternatename:inflate=Cr_z_inflate"
        "/alternatename:inflateEnd=Cr_z_inflateEnd"
        "/alternatename:inflateInit2_=Cr_z_inflateInit2_"
        "/alternatename:inflateReset=Cr_z_inflateReset"
    )
endif()

# Link ai_win_ui_lib & yogacore (static CRT) & system libraries
target_link_libraries(ai_pwd PRIVATE
    "$<$<CONFIG:Debug>:${AI_WIN_UI_DIR}/build/Debug/ai_win_ui_lib.lib>"
    "$<$<NOT:$<CONFIG:Debug>>:${AI_WIN_UI_DIR}/build/Release/ai_win_ui_lib.lib>"
    "$<$<CONFIG:Debug>:${AI_WIN_UI_DIR}/third_party/yoga-3.2.1/build-staticcrt/yoga/Debug/yogacore.lib>"
    "$<$<NOT:$<CONFIG:Debug>>:${AI_WIN_UI_DIR}/third_party/yoga-3.2.1/build-staticcrt/yoga/Release/yogacore.lib>"
    d2d1
    dwrite
    dxgi
    dwmapi
    imm32
    windowscodecs
    shlwapi
    comdlg32
    shell32
    user32
    gdi32
    ole32
    advapi32
    opengl32
    uiautomationcore
    bcrypt
)

# Link Skia libraries if present
set(SKIA_LIB_DIR "${AI_WIN_UI_DIR}/third_party/skia-m124/out")
if(EXISTS "${SKIA_LIB_DIR}/Release-x64/skia.lib")
    file(GLOB SKIA_DBG_LIBS LIST_DIRECTORIES false "${SKIA_LIB_DIR}/Debug-x64/*.lib")
    file(GLOB SKIA_REL_LIBS LIST_DIRECTORIES false "${SKIA_LIB_DIR}/Release-x64/*.lib")
    target_link_directories(ai_pwd PRIVATE
        "$<$<CONFIG:Debug>:${SKIA_LIB_DIR}/Debug-x64>"
        "$<$<NOT:$<CONFIG:Debug>>:${SKIA_LIB_DIR}/Release-x64>"
    )
    target_link_libraries(ai_pwd PRIVATE
        "$<$<CONFIG:Debug>:${SKIA_DBG_LIBS}>"
        "$<$<NOT:$<CONFIG:Debug>>:${SKIA_REL_LIBS}>"
    )
endif()

# Copy ai-win-ui resources post-build
add_custom_command(TARGET ai_pwd POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${AI_WIN_UI_DIR}/resource"
        "$<TARGET_FILE_DIR:ai_pwd>/resource"
    COMMENT "Copying ai-win-ui resources"
)
