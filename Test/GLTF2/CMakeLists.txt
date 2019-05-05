file(GLOB SRC_GLTF2_TEST
    "*.cpp"
)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/Test/GLTF2)

file(COPY ${PROJECT_SOURCE_DIR}/Asset/Scene/Test DESTINATION ${EXECUTABLE_OUTPUT_PATH}/Debug)

add_executable(
    GLTF2Test
    ${SRC_GLTF2_TEST}
)

include_directories("${PROJECT_SOURCE_DIR}/Thirdparts/glTF2-loader/Include")

find_library(GLTF2_LOADER_LIB gltf2-loader-d.lib HINTS ${PROJECT_SOURCE_DIR}/Thirdparts/glTF2-loader/Lib/Debug)
target_link_libraries(
    GLTF2Test
    ${GLTF2_LOADER_LIB}
)

set_target_properties(
    GLTF2Test
    PROPERTIES
    FOLDER ${FOLDER_TEST}
    RUNTIME_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
)