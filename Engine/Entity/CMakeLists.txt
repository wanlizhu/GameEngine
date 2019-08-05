file(GLOB SRC_ENTITY
    "*.cpp"
    "*.h"
)

file(GLOB SRC_COMPONENT
    "Component/*.cpp"
    "Component/*.h"
    "Component/Geometry/*.cpp"
    "Component/Geometry/*.h"
)

file(GLOB SRC_MESH
    "Mesh/*.cpp"
    "Mesh/*.h"
)

file(GLOB SRC_SCENE
    "Scene/*.cpp"
    "Scene/*.h"
)

source_group(Component FILES ${SRC_COMPONENT})
source_group(Scene FILES ${SRC_SCENE})

add_library(
    Entity
    ${SRC_ENTITY}
    ${SRC_COMPONENT}
    ${SRC_MESH}
    ${SRC_SCENE}
    ${SRC_SYSTEM}
)

include_directories("${PROJECT_SOURCE_DIR}/Thirdparts/glTF2-loader/Include")
find_library(GLTF2_LOADER_LIB gltf2-loader-d.lib HINTS ${PROJECT_SOURCE_DIR}/Thirdparts/glTF2-loader/Lib/Debug)
target_link_libraries(
    Entity
    ${GLTF2_LOADER_LIB}
)

set_target_properties(
    Entity
    PROPERTIES
    FOLDER ${FOLDER_ENGINE}
)