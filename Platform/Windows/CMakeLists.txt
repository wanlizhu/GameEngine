file(GLOB SRC_WINDOWS_APP
    "*.cpp"
    "*.h"
)

add_library(
    WindowsApp
    ${SRC_WINDOWS_APP}
)

target_link_libraries(
    WindowsApp
    Common
)

set_target_properties(
    WindowsApp
    PROPERTIES
    FOLDER ${FOLDER_APP}
)

