file(GLOB SRC_ENTITY
    "*.cpp"
    "*.h"
)

file(GLOB SRC_LIGHT
    "Light/*.cpp"
    "Light/*.h"
)

file(GLOB SRC_MESH
    "Mesh/*.cpp"
    "Mesh/*.h"
)

file(GLOB SRC_IMPORT
    "Import/*.cpp"
    "Import/*.h"
)

add_library(
    Entity
    ${SRC_ENTITY}
    ${SRC_LIGHT}
    ${SRC_MESH}
    ${SRC_IMPORT}
)

include_directories("${PROJECT_SOURCE_DIR}/Thirdparts/glTF2-loader/Include")
find_library(GLTF2_LOADER_LIB gltf2-loader-d.lib HINTS ${PROJECT_SOURCE_DIR}/Thirdparts/glTF2-loader/Lib/Debug)
target_link_libraries(
    Entity
    ${GLTF2_LOADER_LIB}
)

source_group(Light FILES ${SRC_LIGHT})
source_group(Mesh FILES ${SRC_MESH})

set_target_properties(
    Entity
    PROPERTIES
    FOLDER ${FOLDER_ENGINE}
)