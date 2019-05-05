file(GLOB SRC_EVENT_TEST
    "*.cpp"
)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/Test/Event)

add_executable(
    EventTest
    ${SRC_EVENT_TEST}
)

target_link_libraries(
    EventTest
    Common
)

set_target_properties(
    EventTest
    PROPERTIES
    FOLDER ${FOLDER_TEST}
    RUNTIME_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
)