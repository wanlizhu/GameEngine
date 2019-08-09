file(GLOB SRC_COMPONENT
    "*.cpp"
    "*.h"
)

add_library(
    Component
    ${SRC_COMPONENT}
)

target_link_libraries(
    Component
    Entity
    Graphics
)

set_target_properties(
    Component
    PROPERTIES
    FOLDER ${FOLDER_ENGINE}
)