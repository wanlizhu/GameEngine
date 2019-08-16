file(GLOB SRC_GAME_TEST
    "*.cpp"
)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/Test/Game)

file(GLOB RUNTIME_FILES
    ${PROJECT_SOURCE_DIR}/Thirdparts/*/Bin/*.dll)
file(COPY ${RUNTIME_FILES} DESTINATION ${EXECUTABLE_OUTPUT_PATH}/Debug)
file(COPY ${PROJECT_SOURCE_DIR}/Asset DESTINATION ${EXECUTABLE_OUTPUT_PATH}/Debug)

add_executable(
    GameTest
    ${SRC_GAME_TEST}
)

target_link_libraries(
    GameTest
    WindowsApp
    Common
    Component
    Graphics
    Entity
)

set_target_properties(
    GameTest
    PROPERTIES
    FOLDER ${FOLDER_TEST}
    RUNTIME_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
)