#include <array>
#include <iostream> 
#include <algorithm> 

#include "Macros.h"
#include "DrawingRawResource_D3D11.h"

using namespace Engine;

static void UpdateValue(DrawingParameter* pParam, ID3DX11EffectVariable* pVar)
{
    auto pData = pParam->GetValuePtr();
    auto dataSize = pParam->GetValueSize();

    pVar->SetRawValue(pData, 0, dataSize);
}

static void UpdateTexture(DrawingParameter* pParam, ID3DX11EffectVariable* pVar)
{
    ID3DX11EffectShaderResourceVariable* pTexVar = pVar->AsShaderResource();
    if (pTexVar == nullptr) return;

    auto pTex = static_cast<const DrawingRawTexture_D3D11*>(pParam->AsTexture());
    pTexVar->SetResource(pTex != nullptr ? pTex->GetShaderResourceView().get() : nullptr);
}

static void UpdateBuffer(DrawingParameter* pParam, ID3DX11EffectVariable* pVar)
{
    ID3DX11EffectShaderResourceVariable* pTexVar = pVar->AsShaderResource();
    if (pTexVar == nullptr) return;

    auto pBuffer = static_cast<const DrawingRawTexBuffer_D3D11*>(pParam->AsBuffer());
    pTexVar->SetResource(pBuffer != nullptr ? pBuffer->GetShaderResourceView().get() : nullptr);
}

static void UpdateOutputRWBuffer(DrawingParameter* pParam, ID3DX11EffectVariable* pVar)
{
    ID3DX11EffectUnorderedAccessViewVariable* pUAV = pVar->AsUnorderedAccessView();
    if (pUAV == nullptr) return;

    auto pRWBuffer = static_cast<const DrawingRawRWBuffer_D3D11*>(pParam->AsRWBuffer());
    pUAV->SetUnorderedAccessView(pRWBuffer != nullptr ? pRWBuffer->GetUnorderedAccessView().get() : nullptr);
}

static void UpdateInputRWBuffer(DrawingParameter* pParam, ID3DX11EffectVariable* pVar)
{
    auto pSRV = pVar->AsShaderResource();
    if (pSRV == nullptr) return;

    auto pRWBuffer = static_cast<const DrawingRawRWBuffer_D3D11*>(pParam->AsRWBuffer());
    pSRV->SetResource(pRWBuffer != nullptr ? pRWBuffer->GetShaderResourceView().get() : nullptr);
}

static void UpdateTexBuffer(DrawingParameter* apParam, ID3DX11EffectVariable* apVar)
{
    auto* lpTexBufferVar = apVar->AsShaderResource();
    if (nullptr == lpTexBufferVar) return;

    auto pTexBuffer = static_cast<const DrawingRawTexBuffer_D3D11*>(apParam->AsTexBuffer());
    lpTexBufferVar->SetResource(pTexBuffer != nullptr ? pTexBuffer->GetShaderResourceView().get() : nullptr);
}

static void UpdateSampler(DrawingParameter* pParam, ID3DX11EffectVariable* pVar)
{
    ID3DX11EffectSamplerVariable* pSampler = pVar->AsSampler();
    if (pSampler == nullptr) return;

    auto pState = static_cast<const DrawingRawSamplerState_D3D11*>(pParam->AsSampler());
    pSampler->SetSampler(0, pState != nullptr ? pState->Get().get() : nullptr);
}

void DrawingRawFxEffect_D3D11::_SParamVar::UpdateParamValue(const DrawingRawFxEffect_D3D11* apEffect)
{
    assert(m_pParam != nullptr);
    assert(mpVar != nullptr);

    auto paramType = m_pParam->GetType();
    auto objType = DrawingParameter::GetObjectType(paramType);

    switch (objType)
    {
        case EObjectType::eObject_Value: UpdateValue(m_pParam.get(), mpVar); break;
        case EObjectType::eObject_Texture: UpdateTexture(m_pParam.get(), mpVar); break;
        case EObjectType::eObject_Buffer: UpdateBuffer(m_pParam.get(), mpVar); break;
        case EObjectType::eObject_TexBuffer: UpdateTexBuffer(m_pParam.get(), mpVar); break;

        case EObjectType::eObject_RWBuffer:
        {
            if (apEffect->m_inputRWBufferNames.find(m_pParam->GetName()) != apEffect->m_inputRWBufferNames.cend())
                UpdateInputRWBuffer(m_pParam.get(), mpVar);
            else if (apEffect->m_outputRWBufferNames.find(m_pParam->GetName()) != apEffect->m_outputRWBufferNames.cend())
                UpdateOutputRWBuffer(m_pParam.get(), mpVar);
            break;
        }
    }
}

void DrawingRawFxEffect_D3D11::Apply()
{
    std::for_each(mVarList.begin(), mVarList.end(), [this](SParamVar& aVar) { aVar.UpdateParamValue(this); });

    if (m_bComputeShaderPass)
    {
        m_pPass->Apply(0, m_pDevice->GetDeviceContext().get());
        return;
    }

    std::array<ID3D11UnorderedAccessView*, DrawingDevice_D3D11::MAX_UAV_SLOT_COUNT> lpUAView;
    m_pDevice->GetDeviceContext()->OMGetRenderTargetsAndUnorderedAccessViews(0, nullptr, nullptr, 0, DrawingDevice_D3D11::MAX_UAV_SLOT_COUNT, lpUAView.data());
    m_pPass->Apply(0, m_pDevice->GetDeviceContext().get());

    auto lpFirtUAView = std::find_if(lpUAView.cbegin(), lpUAView.cend(), [](const ID3D11UnorderedAccessView* apUAView) { return apUAView != nullptr; });
    if (lpFirtUAView != lpUAView.cend())
    {
        UINT lUAVStartSlot = static_cast<UINT>(lpFirtUAView - lpUAView.cbegin());
        UINT lNumUAVs = DrawingDevice_D3D11::MAX_UAV_SLOT_COUNT - lUAVStartSlot;
        m_pDevice->GetDeviceContext()->OMSetRenderTargetsAndUnorderedAccessViews(D3D11_KEEP_RENDER_TARGETS_AND_DEPTH_STENCIL, nullptr, nullptr, lUAVStartSlot, lNumUAVs, &(*lpFirtUAView), nullptr);
        std::for_each(lpUAView.begin(), lpUAView.end(), [](ID3D11UnorderedAccessView* apUAView) {if (apUAView != nullptr) apUAView->Release(); });
    }
}

void DrawingRawFxEffect_D3D11::Terminate()
{
}

void DrawingRawFxEffect_D3D11::BuildEffect()
{
    LoadTechAndPass();
    BuildParameterSet();
}

void DrawingRawFxEffect_D3D11::LoadTechAndPass()
{
    m_pTech = std::shared_ptr<ID3DX11EffectTechnique>(m_pEffect->GetTechniqueByName(m_pTechName->c_str()), D3D11Releaser<ID3DX11EffectTechnique>);
    if (m_pTech != nullptr)
    {
        m_pPass = std::shared_ptr<ID3DX11EffectPass>(m_pTech->GetPassByIndex(0), D3D11Releaser<ID3DX11EffectPass>);

        if (m_pPass != nullptr)
        {
            D3DX11_PASS_DESC passDesc;
            m_pPass->GetDesc(&passDesc);

            m_pInputSignature = passDesc.pIAInputSignature;
            m_byteCodeLength = passDesc.IAInputSignatureSize;

            D3DX11_PASS_SHADER_DESC lCSDesc;
            if (SUCCEEDED(m_pPass->GetComputeShaderDesc(&lCSDesc)))
            {
                ID3D11ComputeShader* lpCS;
                lCSDesc.pShaderVariable->GetComputeShader(lCSDesc.ShaderIndex, &lpCS);
                m_bComputeShaderPass = lpCS != nullptr;
            }
        }
    }
}

void DrawingRawFxEffect_D3D11::BuildParameterSet()
{
    D3DX11_EFFECT_DESC desc;
    m_pEffect->GetDesc(&desc);

    m_pParamSet->Clear();
    mVarList.clear();

    for (unsigned int i = 0; i < desc.GlobalVariables; i++)
    {
        ID3DX11EffectVariable* pVar = m_pEffect->GetVariableByIndex(i);
        ProcessVariable(pVar);
    }
}

void DrawingRawFxEffect_D3D11::ProcessVariable(ID3DX11EffectVariable* pVar)
{
    D3DX11_EFFECT_VARIABLE_DESC desc;
    pVar->GetDesc(&desc);

    D3DX11_EFFECT_TYPE_DESC descType;
    pVar->GetType()->GetDesc(&descType);

    uint32_t dataSize = 0;
    auto paramType = DrawingDevice::GetParamType(descType, dataSize);

    if (paramType != EParamType::EParam_Invalid)
    {
        void* pInitData{ nullptr };

        if (paramType != EParamType::EParam_Texture &&
            paramType != EParamType::EParam_Buffer &&
            paramType != EParamType::EParam_RWBuffer &&
            paramType != EParamType::EParam_Sampler &&
            paramType != EParamType::EParam_TexBuffer)
        {
            static std::vector<char> initData;
            initData.resize(dataSize);
            pInitData = &initData[0];
            pVar->GetRawValue(pInitData, 0, dataSize);
        }

        auto pParam = std::make_shared<DrawingParameter>(std::make_shared<std::string>(desc.Name), paramType, pInitData, std::make_shared<std::string>(desc.Semantic));
        m_pParamSet->Add(pParam);
        mVarList.emplace_back(SParamVar(pParam, pVar));
    }
}

static void CollectVariables(DrawingDevice::ConstBufferPropTable& cbPropTable, const DrawingRawShader_D3D11* pShader)
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

static void BuildCBPropTable(DrawingDevice::ConstBufferPropTable& cbPropTable, const DrawingRawShader_D3D11* pShader)
{
    CollectVariables(cbPropTable, pShader);
    SortVariables(cbPropTable);
}

void DrawingRawShader_D3D11::ProcessVariables(std::shared_ptr<std::string> pName, uint32_t size, ID3D11ShaderReflectionConstantBuffer* pBuffer, uint32_t count)
{
    for (uint32_t i = 0; i < count; ++i)
    {
        ID3D11ShaderReflectionVariable* pD3D11Var = pBuffer->GetVariableByIndex(i);
        D3D11_SHADER_VARIABLE_DESC varDesc;
        pD3D11Var->GetDesc(&varDesc);

        if (0 == (varDesc.uFlags & D3D_SVF_USED))
            continue;

        VariableDesc imp;
        imp.mpName = strPtr(varDesc.Name);
        imp.mpCBName = pName;
        imp.mOffset = varDesc.StartOffset;
        imp.mSizeInBytes = varDesc.Size;
        imp.mCBSizeInBytes = size;

        ID3D11ShaderReflectionType* pD3D11Type = pD3D11Var->GetType();
        D3D11_SHADER_TYPE_DESC typeDesc;
        pD3D11Type->GetDesc(&typeDesc);

        uint32_t dataSize = 0;
        imp.mParamType = DrawingDevice::GetParamType(typeDesc, dataSize);
        if (dataSize < 4)
            dataSize = 4;
        assert(dataSize == varDesc.Size);

        mVariableTable.emplace(imp.mpName, imp);
    }
}

void DrawingRawShader_D3D11::BuildResourceBindingTable(DrawingRawShader::DrawingRawShaderType type)
{
    D3D11_SHADER_DESC desc;
    m_pReflection->GetDesc(&desc);

    for (uint32_t i = 0; i < desc.BoundResources; ++i)
    {
        D3D11_SHADER_INPUT_BIND_DESC resDesc;
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
                assert(imp.mStartSlot < D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT);
                mCBTable.emplace(imp.mpName, imp);

                ID3D11ShaderReflectionConstantBuffer* pD3D11CB = m_pReflection->GetConstantBufferByName(resDesc.Name);
                D3D11_SHADER_BUFFER_DESC bufDesc;
                pD3D11CB->GetDesc(&bufDesc);
                imp.mSizeInBytes = bufDesc.Size;
                ProcessVariables(imp.mpName, bufDesc.Size, pD3D11CB, bufDesc.Variables);
                break;
            }
            case D3D_SIT_STRUCTURED:
            {
                assert(imp.mStartSlot + imp.mCount < D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT);
                mTexBufferTable.emplace(imp.mpName, imp);
                break;
            }
            case D3D_SIT_TBUFFER:
            case D3D_SIT_TEXTURE:
            case D3D_SIT_BYTEADDRESS:
            {
                assert(imp.mStartSlot + imp.mCount < D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT);
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
                assert(imp.mStartSlot + imp.mCount < D3D11_PS_CS_UAV_REGISTER_COUNT);
                mUVATable.emplace(imp.mpName, imp);
                break;
            }
            case D3D_SIT_SAMPLER:
            {
                assert(imp.mStartSlot + imp.mCount < D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT);
                mSamplerTable.emplace(imp.mpName, imp);
                break;
            }
            default:
                break;
        }
    }
}

DrawingRawShaderEffect_D3D11::DrawingRawShaderEffect_D3D11(std::shared_ptr<DrawingDevice_D3D11> pDevice, std::shared_ptr<std::string> pEffectName, const std::vector<std::shared_ptr<DrawingRawShader>>& shaders) :
    DrawingRawEffect_D3D11(pDevice, pEffectName)
{
    for (auto& shader : shaders)
    {
        const auto type = shader->GetShaderType();
        m_shaderBlocks[type] = std::make_shared<ShaderBlock>();

        DrawingRawShader_D3D11* shaderImpl = nullptr;
        switch (type)
        {
            case DrawingRawShader::RawShader_VS:
            {
                auto vsShader = std::static_pointer_cast<DrawingRawVertexShader_D3D11>(shader);
                assert(vsShader != nullptr);

                shaderImpl = vsShader->GetShaderImpl();
                m_shaderBlocks[type]->mpShader = shader;
                break;
            }
            case DrawingRawShader::RawShader_PS:
            {
                auto psShader = std::static_pointer_cast<DrawingRawPixelShader_D3D11>(shader);
                assert(psShader != nullptr);

                shaderImpl = psShader->GetShaderImpl();
                m_shaderBlocks[type]->mpShader = shader;
                break;
            }
            default:
                break;
        }

        assert(shaderImpl != nullptr);
        LoadShaderInfo(shaderImpl, type);
    }
}

DrawingRawShaderEffect_D3D11::DrawingRawShaderEffect_D3D11(std::shared_ptr<DrawingDevice_D3D11> pDevice, std::shared_ptr<std::string> pEffectName, std::shared_ptr<DrawingRawVertexShader_D3D11> pVertexShader, std::shared_ptr<DrawingRawPixelShader_D3D11> pPixelShader) :
    DrawingRawEffect_D3D11(pDevice, pEffectName)
{
    assert(pVertexShader != nullptr && pPixelShader != nullptr);

    auto vsShaderImpl = pVertexShader->GetShaderImpl();
    auto psShaderImpl = pPixelShader->GetShaderImpl();

    if (vsShaderImpl != nullptr)
    {
        m_shaderBlocks[DrawingRawShader::RawShader_VS] = std::make_shared<ShaderBlock>();
        m_shaderBlocks[DrawingRawShader::RawShader_VS]->mpShader = pVertexShader;
        LoadShaderInfo(vsShaderImpl, DrawingRawShader::RawShader_VS);
    }
    if (psShaderImpl != nullptr)
    {
        m_shaderBlocks[DrawingRawShader::RawShader_PS] = std::make_shared<ShaderBlock>();
        m_shaderBlocks[DrawingRawShader::RawShader_PS]->mpShader = pPixelShader;
        LoadShaderInfo(vsShaderImpl, DrawingRawShader::RawShader_PS);
    }
}

void DrawingRawShaderEffect_D3D11::Apply()
{
    UpdateParameterValues();
    UpdateConstantBuffers();
    UpdateDevice();
}

void DrawingRawShaderEffect_D3D11::Terminate()
{
}

void DrawingRawShaderEffect_D3D11::SParamVar::UpdateValues(void)
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

void DrawingRawShaderEffect_D3D11::CheckAndAddResource(const DrawingRawShader_Common::ShaderResourceDesc& desc, uint32_t paramType, const DrawingRawShader::DrawingRawShaderType shaderType, std::unordered_map<std::shared_ptr<std::string>, SParamRes>& resTable) const
{
    auto paramIndex = m_pParamSet->IndexOfName(desc.mpName);

    if (paramIndex != DrawingParameterSet::npos)
    {
        assert((*m_pParamSet)[paramIndex] != nullptr && (*m_pParamSet)[paramIndex]->GetType() == paramType);

        auto iter = resTable.find(desc.mpName);
        if (iter != resTable.end())
            (iter->second).mStartSlot[shaderType] = desc.mStartSlot;
    }
    else
    {
        auto pParam = std::make_shared<DrawingParameter>(desc.mpName, paramType);
        m_pParamSet->Add(pParam);

        SParamRes paramRes;
        paramRes.mpParam = pParam;
        paramRes.mCount = desc.mCount;
        paramRes.mStartSlot[shaderType] = desc.mStartSlot;

        resTable.emplace(desc.mpName, paramRes);
    }
}

void DrawingRawShaderEffect_D3D11::LoadShaderInfo(const DrawingRawShader_D3D11* pShader, const DrawingRawShader::DrawingRawShaderType shaderType)
{
    LoadConstantBufferFromShader(pShader, shaderType);
    LoadTexturesFromShader(pShader, shaderType);
}

void DrawingRawShaderEffect_D3D11::LoadConstantBufferFromShader(const DrawingRawShader_D3D11* pShader, const DrawingRawShader::DrawingRawShaderType shaderType)
{
    DrawingDevice::ConstBufferPropTable cbPropTable;
    BuildCBPropTable(cbPropTable, pShader);
    BindConstantBuffer(cbPropTable, pShader, shaderType);
    GenerateParameters(pShader, shaderType);
}

void DrawingRawShaderEffect_D3D11::LoadTexturesFromShader(const DrawingRawShader_D3D11* pShader, const DrawingRawShader::DrawingRawShaderType shaderType)
{
    for (auto& item : pShader->GetTextureTable())
    {
        auto& desc = item.second;
        auto paramType = COMPOSE_TYPE(eObject_Texture, eDataSet_Object, eBasic_FP32, desc.mCount <= 1 ? 0 : desc.mCount, 0, 0);
        CheckAndAddResource(desc, paramType, shaderType, mTexTable);
    }
}

void DrawingRawShaderEffect_D3D11::UpdateParameterValues()
{
    for (auto& item : mVarTable)
        item.second.UpdateValues();
}

void DrawingRawShaderEffect_D3D11::UpdateConstantBuffers()
{
    for (auto& item : mConstBufferTable)
    {
        auto& pCB = item.second.mpCB;
        if (pCB->IsDirty())
            pCB->UpdateToHardware();
    }
}

void DrawingRawShaderEffect_D3D11::UpdateDevice()
{
    for (auto& shaderBlock : m_shaderBlocks)
    {
        if (!shaderBlock)
            continue;

        auto type = shaderBlock->mpShader->GetShaderType();
        SetConstBufferSlots(*shaderBlock, type);
        SetTextureSlots(*shaderBlock, type);
        SetSamplerSlots(*shaderBlock, type);
        SetTexBufferSlots(*shaderBlock, type);
        SetRWBufferSlots(*shaderBlock, type);

        switch (type)
        {
        case DrawingRawShader::RawShader_CS:
            UpdateComputeShader(*shaderBlock);
            break;
        case DrawingRawShader::RawShader_VS:
            UpdateVertexShader(*shaderBlock);
            break;
        case DrawingRawShader::RawShader_PS:
            UpdatePixelShader(*shaderBlock);
            break;
        default:
            assert(false);
            break;
        }
    }
}

void DrawingRawShaderEffect_D3D11::UpdateComputeShader(const ShaderBlock& shaderBlock) const
{
    auto context = m_pDevice->GetDeviceContext();

    context->CSSetConstantBuffers(0, shaderBlock.mCBSlotsCount + 1, shaderBlock.mCBSlots.data());
    context->CSSetSamplers(0, shaderBlock.mSamplerSlotsCount + 1, shaderBlock.mSamplerSlots.data());
    context->CSSetShaderResources(0, shaderBlock.mSRVSlotsCount + 1, shaderBlock.mSRVSlots.data());
    context->CSSetUnorderedAccessViews(0, shaderBlock.mUAVSlotsCount + 1, shaderBlock.mUAVSlots.data(), nullptr);
    context->CSSetShader(std::static_pointer_cast<DrawingRawComputeShader_D3D11>(shaderBlock.mpShader)->GetShader(), nullptr, 0);
}

void DrawingRawShaderEffect_D3D11::UpdateVertexShader(const ShaderBlock& shaderBlock) const
{
    auto context = m_pDevice->GetDeviceContext();

    context->VSSetConstantBuffers(0, shaderBlock.mCBSlotsCount + 1, shaderBlock.mCBSlots.data());
    context->VSSetSamplers(0, shaderBlock.mSamplerSlotsCount + 1, shaderBlock.mSamplerSlots.data());
    context->VSSetShaderResources(0, shaderBlock.mSRVSlotsCount + 1, shaderBlock.mSRVSlots.data());
    context->VSSetShader(std::static_pointer_cast<DrawingRawVertexShader_D3D11>(shaderBlock.mpShader)->GetShader(), nullptr, 0);
}

void DrawingRawShaderEffect_D3D11::UpdatePixelShader(const ShaderBlock& shaderBlock) const
{
    auto context = m_pDevice->GetDeviceContext();

    context->PSSetConstantBuffers(0, shaderBlock.mCBSlotsCount + 1, shaderBlock.mCBSlots.data());
    context->PSSetSamplers(0, shaderBlock.mSamplerSlotsCount + 1, shaderBlock.mSamplerSlots.data());
    context->PSSetShaderResources(0, shaderBlock.mSRVSlotsCount + 1, shaderBlock.mSRVSlots.data());
    context->PSSetShader(std::static_pointer_cast<DrawingRawPixelShader_D3D11>(shaderBlock.mpShader)->GetShader(), nullptr, 0);
}

void DrawingRawShaderEffect_D3D11::SetConstBufferSlots(ShaderBlock& shaderBlock, const DrawingRawShader::DrawingRawShaderType shaderType)
{
    for (auto& item : mConstBufferTable)
    {
        auto& constDesc = item.second;
        if (constDesc.mStartSlot[shaderType] == EMPTY_SLOT)
            continue;

        auto pCB = std::dynamic_pointer_cast<DrawingRawConstantBuffer_D3D11>(constDesc.mpCB);
        assert(pCB != nullptr);
        shaderBlock.mCBSlots[constDesc.mStartSlot[shaderType]] = pCB->GetBuffer().get();
        shaderBlock.mCBSlotsCount = max(shaderBlock.mCBSlotsCount, constDesc.mStartSlot[shaderType]);
    }
}

void DrawingRawShaderEffect_D3D11::SetTextureSlots(ShaderBlock& shaderBlock, const DrawingRawShader::DrawingRawShaderType shaderType)
{
    for (auto& item : mTexTable)
    {
        auto& resDesc = item.second;
        if (nullptr == resDesc.mpParam || resDesc.mStartSlot[shaderType] == EMPTY_SLOT)
            continue;

        uint32_t arraySize{ 0 };
        auto pTexArray = resDesc.mpParam->AsTextureArray(arraySize);
        assert(arraySize == 0 || arraySize == resDesc.mCount);

        for (uint32_t index = 0; index < resDesc.mCount; ++index)
        {
            auto pTex = static_cast<const DrawingRawTexture_D3D11*>(pTexArray[index]);
            if (pTex == nullptr)
                continue;

            uint32_t curSlot = index + resDesc.mStartSlot[shaderType];
            shaderBlock.mSRVSlots[curSlot] = pTex->GetShaderResourceView().get();
            shaderBlock.mSRVSlotsCount = max(shaderBlock.mSRVSlotsCount, curSlot);
        }
    }
}

void DrawingRawShaderEffect_D3D11::SetSamplerSlots(ShaderBlock& shaderBlock, const DrawingRawShader::DrawingRawShaderType shaderType)
{
}

void DrawingRawShaderEffect_D3D11::SetTexBufferSlots(ShaderBlock& shaderBlock, const DrawingRawShader::DrawingRawShaderType shaderType)
{
}

void DrawingRawShaderEffect_D3D11::SetRWBufferSlots(ShaderBlock& shaderBlock, const DrawingRawShader::DrawingRawShaderType shaderType)
{
}

void DrawingRawShaderEffect_D3D11::BindConstantBuffer(DrawingDevice::ConstBufferPropTable& cbPropTable, const DrawingRawShader_D3D11* pShader, const DrawingRawShader::DrawingRawShaderType shaderType)
{
    for (auto& lItem : pShader->GetConstanceBufferTable())
    {
        auto& desc = lItem.second;
        auto& cbPropIt = cbPropTable.find(desc.mpName);
        if (cbPropIt == cbPropTable.end())
            continue;

        auto& cbProp = cbPropIt->second;
        auto pDevCBProp = m_pDevice->FindConstantBuffer(cbProp);

        if (nullptr == pDevCBProp)
        {
            cbProp.mpCB = std::make_shared<DrawingRawConstantBuffer_D3D11>(m_pDevice, cbProp.mSizeInBytes);
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

void DrawingRawShaderEffect_D3D11::GenerateParameters(const DrawingRawShader_D3D11* pShader, const DrawingRawShader::DrawingRawShaderType shaderType)
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
