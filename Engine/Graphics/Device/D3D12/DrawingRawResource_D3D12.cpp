#include "DrawingRawResource_D3D12.h"

using namespace Engine;

DrawingRawShaderEffect_D3D12::DrawingRawShaderEffect_D3D12(std::shared_ptr<DrawingDevice_D3D12> pDevice, std::shared_ptr<std::string> pEffectName, std::shared_ptr<DrawingRawVertexShader_D3D12> pVertexShader, std::shared_ptr<DrawingRawPixelShader_D3D12> pPixelShader) :
    DrawingRawEffect_D3D12(pDevice, pEffectName)
{
}

void DrawingRawShaderEffect_D3D12::Apply()
{
}

void DrawingRawShaderEffect_D3D12::Terminate()
{
}