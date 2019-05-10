#include "Macros.h"
#include "DrawingRawResource_D3D12.h"

using namespace Engine;

void DrawingRawShader_D3D12::BuildResourceBindingTable(DrawingRawShader::DrawingRawShaderType type)
{
    D3D12_SHADER_DESC desc;
    m_pReflection->GetDesc(&desc);

    for (uint32_t i = 0; i < desc.BoundResources; ++i)
    {
        D3D12_SHADER_INPUT_BIND_DESC resDesc;
        m_pReflection->GetResourceBindingDesc(i, &resDesc);

        ShaderResourceDesc imp;
        imp.mStartSlot = resDesc.BindPoint;
        imp.mCount = resDesc.BindCount;
        imp.mpName = strPtr(resDesc.Name);

        switch (resDesc.Type)
        {
            case D3D_SIT_CBUFFER:
            {
                assert(imp.mCount == 1);
                assert(imp.mStartSlot < D3D12_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT);
                mCBTable.emplace(imp.mpName, imp);

                ID3D12ShaderReflectionConstantBuffer* pD3D12CB = m_pReflection->GetConstantBufferByName(resDesc.Name);
                D3D12_SHADER_BUFFER_DESC bufDesc;
                pD3D12CB->GetDesc(&bufDesc);
                imp.mSizeInBytes = bufDesc.Size;
                ProcessVariables(imp.mpName, bufDesc.Size, pD3D12CB, bufDesc.Variables);
                break;
            }
            case D3D_SIT_STRUCTURED:
            {
                assert(imp.mStartSlot + imp.mCount < D3D12_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT);
                mTexBufferTable.emplace(imp.mpName, imp);
                break;
            }
            case D3D_SIT_TBUFFER:
            case D3D_SIT_TEXTURE:
            case D3D_SIT_BYTEADDRESS:
            {
                assert(imp.mStartSlot + imp.mCount < D3D12_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT);
                mTextureTable.emplace(imp.mpName, imp);
                break;
            }
            case D3D_SIT_UAV_RWTYPED:
            case D3D_SIT_UAV_RWSTRUCTURED:
            case D3D_SIT_UAV_RWBYTEADDRESS:
            case D3D_SIT_UAV_APPEND_STRUCTURED:
            case D3D_SIT_UAV_CONSUME_STRUCTURED:
            case D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER:
            {
                assert(imp.mStartSlot + imp.mCount < D3D12_PS_CS_UAV_REGISTER_COUNT);
                mUVATable.emplace(imp.mpName, imp);
                break;
            }
            case D3D_SIT_SAMPLER:
            {
                assert(imp.mStartSlot + imp.mCount < D3D12_COMMONSHADER_SAMPLER_SLOT_COUNT);
                mSamplerTable.emplace(imp.mpName, imp);
                break;
            }
            default:
                break;
        }
    }
}

void DrawingRawShader_D3D12::ProcessVariables(std::shared_ptr<std::string> pName, uint32_t size, ID3D12ShaderReflectionConstantBuffer* pBuffer, uint32_t count)
{
    for (uint32_t i = 0; i < count; ++i)
    {
        ID3D12ShaderReflectionVariable* pD3D12Var = pBuffer->GetVariableByIndex(i);
        D3D12_SHADER_VARIABLE_DESC varDesc;
        pD3D12Var->GetDesc(&varDesc);

        if (0 == (varDesc.uFlags & D3D_SVF_USED))
            continue;

        VariableDesc imp;
        imp.mpName = strPtr(varDesc.Name);
        imp.mpCBName = pName;
        imp.mOffset = varDesc.StartOffset;
        imp.mSizeInBytes = varDesc.Size;
        imp.mCBSizeInBytes = size;

        ID3D12ShaderReflectionType* pD3D12Type = pD3D12Var->GetType();
        D3D12_SHADER_TYPE_DESC typeDesc;
        pD3D12Type->GetDesc(&typeDesc);

        uint32_t dataSize = 0;
        imp.mParamType = DrawingDevice::GetParamType(typeDesc, dataSize);
        if (dataSize < 4)
            dataSize = 4;
        assert(dataSize == varDesc.Size);

        mVariableTable.emplace(imp.mpName, imp);
    }
}

DrawingRawShaderEffect_D3D12::DrawingRawShaderEffect_D3D12(std::shared_ptr<DrawingDevice_D3D12> pDevice, std::shared_ptr<std::string> pEffectName, std::shared_ptr<DrawingRawVertexShader_D3D12> pVertexShader, std::shared_ptr<DrawingRawPixelShader_D3D12> pPixelShader) :
    DrawingRawEffect_D3D12(pDevice, pEffectName)
{
    assert(pVertexShader != nullptr && pPixelShader != nullptr);

    auto vsShaderImpl = pVertexShader->GetShaderImpl();
    auto psShaderImpl = pPixelShader->GetShaderImpl();

    if (vsShaderImpl != nullptr)
    {
        m_shaderBlocks[DrawingRawShader::RawShader_VS] = pVertexShader;
        LoadShaderInfo(vsShaderImpl, DrawingRawShader::RawShader_VS);
    }
    if (psShaderImpl != nullptr)
    {
        m_shaderBlocks[DrawingRawShader::RawShader_PS] = pPixelShader;
        LoadShaderInfo(psShaderImpl, DrawingRawShader::RawShader_PS);
    }

    CreateRootSignature();
}

void DrawingRawShaderEffect_D3D12::Apply()
{
    UpdateParameterValues();
    UpdateConstantBuffers();
}

void DrawingRawShaderEffect_D3D12::Terminate()
{
}

void DrawingRawShaderEffect_D3D12::SParamVar::UpdateValues(void)
{
    assert(mpParam != nullptr);
    std::shared_ptr<DrawingRawConstantBuffer> pPrevCB = nullptr;
    for (auto index = 0; index < DrawingRawShader::RawShader_Count; ++index)
    {
        auto pCurCB = mpCB[index];
        if (pCurCB != nullptr && pCurCB != pPrevCB)
        {
            pCurCB->SetValue(mOffset[index], mpParam->GetValuePtr(), mpParam->GetValueSize());
            pPrevCB = pCurCB;
            mpParam->SetDirty(false);
        }
    }
}

void DrawingRawShaderEffect_D3D12::LoadShaderInfo(const DrawingRawShader_D3D12* pShader, const DrawingRawShader::DrawingRawShaderType shaderType)
{
    LoadConstantBufferFromShader(pShader, shaderType);
}

static void CollectVariables(DrawingDevice::ConstBufferPropTable& cbPropTable, const DrawingRawShader_D3D12* pShader)
{
    const auto& varTable = pShader->GetVariableTable();
    for (auto iter = varTable.begin(); iter != varTable.end(); ++iter)
    {
        auto& varDesc = iter->second;
        DrawingDevice::VarProp var_prop;
        var_prop.mpName = varDesc.mpName;
        var_prop.mType = varDesc.mParamType;
        var_prop.mSizeInBytes = varDesc.mSizeInBytes;
        var_prop.mOffset = varDesc.mOffset;
        auto cbPropIt = cbPropTable.find(varDesc.mpCBName);
        if (cbPropIt == cbPropTable.end())
        {
            DrawingDevice::ConstBufferProp local_cb_prop;
            local_cb_prop.mpName = varDesc.mpCBName;
            local_cb_prop.mSizeInBytes = varDesc.mCBSizeInBytes;
            local_cb_prop.mVarProps.emplace_back(var_prop);
            cbPropTable.emplace(varDesc.mpCBName, local_cb_prop);
        }
        else
        {
            auto cbProp = cbPropIt->second;
            if ((cbProp.mpName != varDesc.mpCBName) ||
                (cbProp.mSizeInBytes != varDesc.mCBSizeInBytes))
            {
                assert(false);
                continue;
            }
            cbProp.mVarProps.emplace_back(var_prop);
        }
    }
}

static void SortVariables(DrawingDevice::ConstBufferPropTable& cbPropTable)
{
    for (auto iter = cbPropTable.begin(); iter != cbPropTable.end(); ++iter)
    {
        DrawingDevice::ConstBufferProp& cbProp = iter->second;
        std::qsort(cbProp.mVarProps.data(), cbProp.mVarProps.size(), sizeof(DrawingDevice::VarProp), [](const void* a, const void* b)
        {
            const DrawingDevice::VarProp& var1 = *(static_cast<const DrawingDevice::VarProp*>(a));
            const DrawingDevice::VarProp& var2 = *(static_cast<const DrawingDevice::VarProp*>(b));
            if (var1.mOffset < var2.mOffset)
                return -1;
            else if (var1.mOffset > var2.mOffset)
                return 1;
            return 0;
        });
    }
}

static void BuildCBPropTable(DrawingDevice::ConstBufferPropTable& cbPropTable, const DrawingRawShader_D3D12* pShader)
{
    CollectVariables(cbPropTable, pShader);
    SortVariables(cbPropTable);
}

void DrawingRawShaderEffect_D3D12::LoadConstantBufferFromShader(const DrawingRawShader_D3D12* pShader, const DrawingRawShader::DrawingRawShaderType shaderType)
{
    DrawingDevice::ConstBufferPropTable cbPropTable;
    BuildCBPropTable(cbPropTable, pShader);
    BindConstantBuffer(cbPropTable, pShader, shaderType);
    GenerateParameters(pShader, shaderType);
}

bool DrawingRawShaderEffect_D3D12::CreateRootSignature()
{
    D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
                                                    D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;
    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription;
    rootSignatureDescription.Init_1_1((UINT)m_rootParameters.size(), m_rootParameters.data(), 0, nullptr, rootSignatureFlags);

    ID3D12RootSignature* pRootSignatureRaw = nullptr;
    ID3DBlob* rootSignatureBlob;
    ID3DBlob* errorBlob;
    HRESULT hr = D3DX12SerializeVersionedRootSignature(&rootSignatureDescription, D3D_ROOT_SIGNATURE_VERSION_1_1, &rootSignatureBlob, &errorBlob);
    assert(SUCCEEDED(hr));

    hr = m_pDevice->GetDevice()->CreateRootSignature(0, rootSignatureBlob->GetBufferPointer(), rootSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)&pRootSignatureRaw);
    assert(SUCCEEDED(hr));
    m_pRootSignature = std::shared_ptr<ID3D12RootSignature>(pRootSignatureRaw);

    return true;
}

void DrawingRawShaderEffect_D3D12::UpdateParameterValues()
{
    for (auto& item : mVarTable)
        item.second.UpdateValues();
}

void DrawingRawShaderEffect_D3D12::UpdateConstantBuffers()
{
    for (auto& item : mConstBufferTable)
    {
        auto& pCB = item.second.mpCB;
        if (pCB->IsDirty())
            pCB->UpdateToHardware();
    }
}

void DrawingRawShaderEffect_D3D12::BindConstantBuffer(DrawingDevice::ConstBufferPropTable& cbPropTable, const DrawingRawShader_D3D12* pShader, const DrawingRawShader::DrawingRawShaderType shaderType)
{
    for (auto& lItem : pShader->GetConstanceBufferTable())
    {
        auto& desc = lItem.second;
        CD3DX12_ROOT_PARAMETER1 param;

        if (shaderType == DrawingRawShader::RawShader_VS)
            param.InitAsConstantBufferView(desc.mStartSlot, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_VERTEX);
        else if (shaderType == DrawingRawShader::RawShader_PS)
            param.InitAsConstantBufferView(desc.mStartSlot, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_PIXEL);
        else
            continue;
        m_rootParameters.emplace_back(param);

        auto& cbPropIt = cbPropTable.find(desc.mpName);
        if (cbPropIt == cbPropTable.end())
            continue;

        auto& cbProp = cbPropIt->second;
        auto pDevCBProp = m_pDevice->FindConstantBuffer(cbProp);

        if (nullptr == pDevCBProp)
        {
            cbProp.mpCB = std::make_shared<DrawingRawConstantBuffer_D3D12>(m_pDevice, cbProp.mSizeInBytes, desc.mStartSlot);
            m_pDevice->AddConstantBuffer(cbProp);
        }
        else
            cbProp.mpCB = pDevCBProp->mpCB;

        auto& cbIt = mConstBufferTable.find(desc.mpName);
        if (cbIt == mConstBufferTable.end())
        {
            SConstBuffer local_cb_desc;
            local_cb_desc.mpName = cbProp.mpName;
            local_cb_desc.mSizeInBytes = cbProp.mSizeInBytes;
            local_cb_desc.mpCB = cbProp.mpCB;
            local_cb_desc.mStartSlot[shaderType] = desc.mStartSlot;

            mConstBufferTable.emplace(desc.mpName, local_cb_desc);
        }
        else
            (cbIt->second).mStartSlot[shaderType] = desc.mStartSlot;
    }
}

void DrawingRawShaderEffect_D3D12::GenerateParameters(const DrawingRawShader_D3D12* pShader, const DrawingRawShader::DrawingRawShaderType shaderType)
{
    for (auto& lItem : pShader->GetVariableTable())
    {
        auto& desc = lItem.second;
        auto& varIt = mVarTable.find(desc.mpName);
        auto& cbIt = mConstBufferTable.find(desc.mpCBName);

        if (varIt == mVarTable.end())
        {
            SParamVar local_var_desc;

            local_var_desc.mpParam = std::make_shared<DrawingParameter>(desc.mpName, desc.mParamType);
            local_var_desc.mSizeInBytes = desc.mSizeInBytes;
            local_var_desc.mOffset[shaderType] = desc.mOffset;
            if (cbIt != mConstBufferTable.end())
                local_var_desc.mpCB[shaderType] = (cbIt->second).mpCB;
            else
                local_var_desc.mpCB[shaderType] = nullptr;

            mVarTable.emplace(desc.mpName, local_var_desc);
            m_pParamSet->Add(local_var_desc.mpParam);
        }
        else
        {
            auto& var = varIt->second;
            if (var.mpParam->GetType() != desc.mParamType)
            {
                assert(false);
                continue;
            }

            var.mOffset[shaderType] = desc.mOffset;
            var.mpCB[shaderType] = cbIt != mConstBufferTable.end() ? (cbIt->second).mpCB : nullptr;
        }
    }
}