cmake_minimum_required(VERSION 3.15)

project(KeyToRecover LANGUAGES CXX)

if(NOT WIN32 OR NOT MSVC)
    message(FATAL_ERROR "KeyToRecover requires Windows and an MSVC-compatible toolchain for DxLib.")
endif()

set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(DXLIB_DIR "C:/DxLib_VC/プロジェクトに追加すべきファイル_VC用"
    CACHE PATH "Directory containing the DxLib VC headers and libraries")
if(NOT EXISTS "${DXLIB_DIR}/DxLib.h")
    message(FATAL_ERROR "DxLib.h was not found in \"${DXLIB_DIR}\". Set -DDXLIB_DIR to the DxLib VC directory.")
endif()

set(GAME_SOURCES
    "${SOURCE_DIR}/app/Game.cpp"
    "${SOURCE_DIR}/app/Main.cpp"
    "${SOURCE_DIR}/engine/objects/GameObject.cpp"
    "${SOURCE_DIR}/engine/scene/Scene.cpp"
    "${SOURCE_DIR}/engine/scene/ScenePhysics.cpp"
    "${SOURCE_DIR}/game/scenes/Stage1.cpp"
    "${SOURCE_DIR}/platform/dxlib/AudioManager.cpp"
    "${SOURCE_DIR}/platform/dxlib/ResourceManager.cpp"
)

# List headers in generated IDE projects. Compilation units are listed explicitly above.
file(GLOB_RECURSE GAME_HEADERS CONFIGURE_DEPENDS "${SOURCE_DIR}/*.h" "${SOURCE_DIR}/*.hpp")
source_group(TREE "${SOURCE_DIR}" FILES ${GAME_SOURCES} ${GAME_HEADERS})

add_executable(KeyToRecover WIN32 ${GAME_SOURCES} ${GAME_HEADERS})
set_target_properties(KeyToRecover PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
)
target_include_directories(KeyToRecover PRIVATE "${SOURCE_DIR}" "${DXLIB_DIR}")
target_compile_definitions(KeyToRecover PRIVATE _MBCS)
# DxLib headers select the libraries for the active architecture and runtime.
target_link_directories(KeyToRecover PRIVATE "${DXLIB_DIR}")
target_link_libraries(KeyToRecover PRIVATE shlwapi vfw32)

add_custom_command(TARGET KeyToRecover POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/Resource"
        "$<TARGET_FILE_DIR:KeyToRecover>/Resource"
    COMMENT "Synchronizing assets to output directory..."
    VERBATIM
)
