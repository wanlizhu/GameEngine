file(GLOB SRC_DEVICE
    "Device/*.cpp"
    "Device/*.h"
)

file(GLOB SRC_D3D11
    "Device/D3D11/*.cpp"
    "Device/D3D11/*.h"
)

file(GLOB SRC_D3D12
    "Device/D3D12/*.cpp"
    "Device/D3D12/*.h"
)

file(GLOB SRC_RENDERER
    "Renderer/*.cpp"
    "Renderer/*.h"
)

file(GLOB SRC_MATH
    "../Math/*.h"
)

add_library(
    Graphics
    ${SRC_DEVICE}
    ${SRC_D3D11}
    ${SRC_D3D12}
    ${SRC_RENDERER}
    ${SRC_MATH}
)

include_directories("${PROJECT_SOURCE_DIR}/Thirdparts/Effect11/Include")
include_directories("${PROJECT_SOURCE_DIR}/Thirdparts/D3DX12/Include")

find_library(Effect11_LIB Effects11.lib HINTS ${PROJECT_SOURCE_DIR}/Thirdparts/Effect11/Lib)

target_link_libraries(
    Graphics
    ${Effect11_LIB}
    d3d11.lib
    d3d12.lib
    dxgi.lib
    d3dcompiler.lib
    dxguid.lib
)

source_group(Device FILES ${SRC_DEVICE})
source_group(Device\\D3D11 FILES ${SRC_D3D11})
source_group(Device\\D3D12 FILES ${SRC_D3D12})
source_group(Renderer FILES ${SRC_RENDERER})
source_group(Math FILES ${SRC_MATH})

set_target_properties(
    Graphics
    PROPERTIES
    FOLDER ${FOLDER_ENGINE}
)