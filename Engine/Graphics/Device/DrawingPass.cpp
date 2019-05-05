#include <assert.h>

#include "DrawingDevice.h"
#include "DrawingPass.h"

using namespace Engine;

DrawingPass::DrawingPass(std::shared_ptr<std::string> pName, const std::shared_ptr<DrawingDevice>& pDevice) :
    m_pName(pName), m_pDevice(pDevice), m_pEffectName(nullptr), m_pEffect(nullptr)
{
    assert(m_pDevice != nullptr);
}

DrawingPass::~DrawingPass()
{
}

std::shared_ptr<std::string> DrawingPass::EffectSlotName()
{
    return StaticResourceSlotTable::GetStaticSlotName("Effect", StaticResourceSlotTable::GetEffectSlotID());
}

std::shared_ptr<std::string> DrawingPass::VertexFormatSlotName()
{
    return StaticResourceSlotTable::GetStaticSlotName("VertexFormat", StaticResourceSlotTable::GetVertexFormatSlotID());
}

std::shared_ptr<std::string> DrawingPass::VertexBufferSlotName(uint32_t index)
{
    return StaticResourceSlotTable::GetStaticSlotName("VertexBuffer", StaticResourceSlotTable::GetVertexBufferSlotID(index), (int32_t)index);
}

std::shared_ptr<std::string> DrawingPass::IndexBufferSlotName()
{
    return StaticResourceSlotTable::GetStaticSlotName("IndexBuffer", StaticResourceSlotTable::GetIndexBufferSlotID());
}

std::shared_ptr<std::string> DrawingPass::BlendStateSlotName()
{
    return StaticResourceSlotTable::GetStaticSlotName("BlendState", StaticResourceSlotTable::GetBlendStateSlotID());
}

std::shared_ptr<std::string> DrawingPass::RasterStateSlotName()
{
    return StaticResourceSlotTable::GetStaticSlotName("RasterState", StaticResourceSlotTable::GetRasterStateSlotID());
}

std::shared_ptr<std::string> DrawingPass::DepthStateSlotName()
{
    return StaticResourceSlotTable::GetStaticSlotName("DepthState", StaticResourceSlotTable::GetDepthStateSlotID());
}

std::shared_ptr<std::string> DrawingPass::TargetSlotName(uint32_t index)
{
    return StaticResourceSlotTable::GetStaticSlotName("Target", StaticResourceSlotTable::GetTargetSlotID(index), (int32_t)index);
}

std::shared_ptr<std::string> DrawingPass::DepthBufferSlotName()
{
    return StaticResourceSlotTable::GetStaticSlotName("DepthBuffer", StaticResourceSlotTable::GetDepthBufferSlotID());
}

std::shared_ptr<std::string> DrawingPass::RWBufferSlotName(uint32_t index)
{
    return StaticResourceSlotTable::GetStaticSlotName("RWBuffer", StaticResourceSlotTable::GetRWBufferSlotID(index), (int32_t)index);
}

std::shared_ptr<std::string> DrawingPass::PrimitiveSlotName()
{
    return StaticResourceSlotTable::GetStaticSlotName("Primitive", StaticResourceSlotTable::GetPrimitiveSlotID());
}

std::shared_ptr<std::string> DrawingPass::VaringStatesSlotName()
{
    return StaticResourceSlotTable::GetStaticSlotName("VaringStates", StaticResourceSlotTable::GetVaringStatesSlotID());
}

std::shared_ptr<std::string> DrawingPass::PipelineStateSlotName()
{
    return StaticResourceSlotTable::GetStaticSlotName("PipelineState", StaticResourceSlotTable::GetPipelineStateSlotID());
}

std::shared_ptr<std::string> DrawingPass::GetName() const
{
    return m_pName;
}

bool DrawingPass::AddResourceSlot(std::shared_ptr<std::string> slotName, EResourceSlotType type, std::shared_ptr<std::string> key)
{
    if (type == ResourceSlot_Static)
        return false;

    return m_dynamicTable.AddResourceSlot(slotName, type, key);
}

bool DrawingPass::RemoveResourceSlot(std::shared_ptr<std::string> slotName)
{
    return m_dynamicTable.RemoveResourceSlot(slotName);
}

bool DrawingPass::BindResource(std::shared_ptr<std::string> slotName, std::shared_ptr<std::string> resName)
{
    if (m_dynamicTable.BindResource(slotName, resName))
        return true;

    if (m_staticTable.BindResource(slotName, resName))
        return true;

    return false;
}

bool DrawingPass::UnBindResource(std::shared_ptr<std::string> slotName)
{
    if (m_dynamicTable.UnBindResource(slotName))
        return true;

    if (m_staticTable.UnBindResource(slotName))
        return true;

    return false;
}

void DrawingPass::FetchResources(DrawingResourceTable& resTable)
{
    m_staticTable.FetchResources(resTable);
    m_dynamicTable.FetchResources(resTable);
}

void DrawingPass::ClearResources()
{
    m_staticTable.ClearResources();
    m_dynamicTable.ClearResources();
}

void DrawingPass::UpdateStaticResource(std::shared_ptr<std::string> slotName, std::shared_ptr<std::string> resName, DrawingResourceTable& resTable)
{
    m_staticTable.UpdateSingleResource(slotName, resName, resTable);
}

void DrawingPass::UpdateDynamicResource(std::shared_ptr<std::string> slotName, std::shared_ptr<std::string> resName, DrawingResourceTable& resTable)
{
    m_dynamicTable.UpdateSingleResource(slotName, resName, resTable);
}

const std::shared_ptr<DrawingResourceTable::ResourceEntry> DrawingPass::GetStaticResourceEntry(std::shared_ptr<std::string> slotName) const
{
    return m_staticTable.GetResourceEntry(slotName);
}

const std::shared_ptr<DrawingResourceTable::ResourceEntry> DrawingPass::GetDynamicResourceEntry(std::shared_ptr<std::string> slotName) const
{
    return m_dynamicTable.GetResourceEntry(slotName);
}

bool DrawingPass::Flush(DrawingContext& dc)
{
    if (!LoadEffect())
        return false;

    UpdateInputs();
    UpdateOutputs();

    PushStates();

    UpdateViewport();
    UpdateScissorBox();
    UpdateStates();

    UpdateConstants();
    UpdateTextures();
    UpdateTexBuffers();
    UpdateRWBuffers();
    UpdateBuffers();
    UpdateSamplers();

    UpdatePipelineState();

    BeginEffect(dc);
    DrawPrimitive(dc);
    EndEffect(dc);

    PopStates();

    return true;
}

bool DrawingPass::LoadEffect()
{
    m_pEffect = m_staticTable.LoadEffect();
    return m_pEffect != nullptr;
}

void DrawingPass::UpdateInputs()
{
    m_staticTable.UpdateVertexFormat(m_pDevice);
    m_staticTable.UpdateVertexBuffer(m_pDevice);
    m_staticTable.UpdateIndexBuffer(m_pDevice);
}

void DrawingPass::UpdateOutputs()
{
    m_staticTable.UpdateTargets(m_pDevice);
}

void DrawingPass::UpdateViewport()
{
    m_staticTable.UpdateViewport(m_pDevice);
}

void DrawingPass::UpdateStates()
{
    m_staticTable.UpdateBlendState(m_pDevice);
    m_staticTable.UpdateDepthState(m_pDevice);
    m_staticTable.UpdateRasterState(m_pDevice);
}

void DrawingPass::UpdateScissorBox()
{
    m_staticTable.UpdateScissorBox(m_pDevice);
}

void DrawingPass::UpdateConstants()
{
    m_dynamicTable.UpdateConstants(m_pEffect);
}

void DrawingPass::UpdateTextures()
{
    m_dynamicTable.UpdateTextures(m_pEffect);
}

void DrawingPass::UpdateTexBuffers()
{
    m_dynamicTable.UpdateTexBuffers(m_pEffect);
}

void DrawingPass::UpdateRWBuffers()
{
    m_dynamicTable.UpdateRWBuffers(m_pEffect);
}

void DrawingPass::UpdateBuffers()
{
    m_dynamicTable.UpdateBuffers(m_pEffect);
}

void DrawingPass::UpdateSamplers()
{
    m_dynamicTable.UpdateSamplers(m_pEffect);
}

void DrawingPass::PushStates()
{
    m_pDevice->PushBlendState();
    m_pDevice->PushDepthState();
    m_pDevice->PushRasterState();
}

void DrawingPass::PopStates()
{
    m_pDevice->PopRasterState();
    m_pDevice->PopDepthState();
    m_pDevice->PopBlendState();
}

void DrawingPass::UpdatePipelineState()
{
    m_staticTable.UpdatePipelineState(m_pDevice);
}

void DrawingPass::BeginEffect(DrawingContext& dc)
{
    m_pDevice->BeginEffect(dc, m_pEffect);
}

void DrawingPass::EndEffect(DrawingContext& dc)
{
    m_pDevice->EndEffect(dc, m_pEffect);
}

bool DrawingPass::DrawPrimitive(DrawingContext& dc)
{
    auto pPrim = m_staticTable.LoadPrimitive();
    if (pPrim == nullptr)
        return false;

    return m_pDevice->DrawPrimitive(pPrim);
}

DrawingPass::ResourceSlot::ResourceSlot() : mpName(nullptr), mpResName(nullptr), mpRes(nullptr), mType(ResourceSlot_Unknown), mpKey(nullptr)
{
}

DrawingPass::ResourceSlot::ResourceSlot(uint32_t val) : mpName(nullptr), mpResName(nullptr), mpRes(nullptr), mType(ResourceSlot_Unknown), mpKey(nullptr)
{
}

DrawingPass::ResourceSlot::ResourceSlot(std::shared_ptr<std::string> slotName, EResourceSlotType type, std::shared_ptr<std::string> key) :
    mpName(slotName), mpResName(nullptr), mpRes(nullptr), mType(type), mpKey(key)
{
}

DrawingPass::ResourceSlot::~ResourceSlot()
{
    mpName = nullptr;
    mpResName = nullptr;
    mpRes = nullptr;
    mType = ResourceSlot_Unknown;
    mpKey = nullptr;
}

DrawingPass::ResourceSlotTable::ResourceSlotTable()
{
}

DrawingPass::ResourceSlotTable::~ResourceSlotTable()
{
    ClearResources();
    Clear();
}

bool DrawingPass::ResourceSlotTable::AddResourceSlot(std::shared_ptr<std::string> slotName, EResourceSlotType type, std::shared_ptr<std::string> key)
{
    if (mSlotTable.find(slotName) != mSlotTable.cend())
        return false;

    mSlotTable.emplace(slotName, ResourceSlot(slotName, type, key));

    return true;
}

bool DrawingPass::ResourceSlotTable::RemoveResourceSlot(std::shared_ptr<std::string> slotName)
{
    if (mSlotTable.find(slotName) == mSlotTable.cend())
        return false;

    mSlotTable.erase(slotName);

    return true;
}

void DrawingPass::ResourceSlotTable::FetchResources(DrawingResourceTable& resTable)
{
    std::for_each(mSlotTable.begin(), mSlotTable.end(), [this, &resTable](ResourceSlotTableType::value_type& aElem)
    {
        auto& slot = aElem.second;

        if (slot.mpResName != nullptr)
        {
            slot.mpRes = resTable.GetResourceEntry(slot.mpResName);
            assert(slot.mpRes != nullptr);
        }
    });
}

void DrawingPass::ResourceSlotTable::UpdateSingleResource(std::shared_ptr<std::string> slotName, std::shared_ptr<std::string> resName, DrawingResourceTable& resTable)
{
    auto it = mSlotTable.find(slotName);
    if (it == mSlotTable.cend())
        return;

    auto pRes = resTable.GetResourceEntry(resName);
    assert(pRes != nullptr);

    it->second.mpResName = resName;
    it->second.mpRes = pRes;
}

void DrawingPass::ResourceSlotTable::ClearResources()
{
    std::for_each(mSlotTable.begin(), mSlotTable.end(), [this](ResourceSlotTableType::value_type& aElem)
    {
        aElem.second.mpRes = nullptr;
    });
}

void DrawingPass::ResourceSlotTable::Clear()
{
    mSlotTable.clear();
}

bool DrawingPass::ResourceSlotTable::BindResource(std::shared_ptr<std::string> slotName, std::shared_ptr<std::string> resName)
{
    auto& it = mSlotTable.find(slotName);
    if (it == mSlotTable.cend())
        return false;

    it->second.mpResName = resName;

    return true;
}

bool DrawingPass::ResourceSlotTable::UnBindResource(std::shared_ptr<std::string> slotName)
{
    auto& it = mSlotTable.find(slotName);
    if (it == mSlotTable.cend())
        return false;

    it->second.mpResName = nullptr;

    return true;
}

const std::shared_ptr<DrawingResourceTable::ResourceEntry> DrawingPass::ResourceSlotTable::GetResourceEntry(std::shared_ptr<std::string> slotName) const
{
    const auto it = mSlotTable.find(slotName);
    if (it == mSlotTable.cend())
        return nullptr;

    return it->second.mpRes;
}

std::shared_ptr<DrawingResource> DrawingPass::ResourceSlotTable::GetSlotDeviceResource(const ResourceSlot* pSlot)
{
    auto pEntry = pSlot->mpRes;

    if (pEntry == nullptr)
        return nullptr;

    return pEntry->GetResource();
}

DrawingPass::DynamicResourceSlotTable::DynamicResourceSlotTable()
{
}

DrawingPass::DynamicResourceSlotTable::~DynamicResourceSlotTable()
{
}

void DrawingPass::DynamicResourceSlotTable::UpdateConstants(std::shared_ptr<DrawingEffect> effect)
{
}

void DrawingPass::DynamicResourceSlotTable::UpdateTextures(std::shared_ptr<DrawingEffect> effect)
{
    std::for_each(mSlotTable.cbegin(), mSlotTable.cend(), [&effect](const ResourceSlotTableType::value_type& aElem)
    {
        if (aElem.second.mType == ResourceSlot_Texture)
        {
            auto pTex = std::dynamic_pointer_cast<DrawingTexture>(GetSlotDeviceResource(&aElem.second));
            if (pTex != nullptr)
            {
                auto pDevice = pTex->GetDevice();
                pDevice->UpdateEffectTexture(pTex, aElem.second.mpKey, effect);
            }
        }
    });
}

void DrawingPass::DynamicResourceSlotTable::UpdateTexBuffers(std::shared_ptr<DrawingEffect> effect)
{
    std::for_each(mSlotTable.cbegin(), mSlotTable.cend(), [&effect](const ResourceSlotTableType::value_type& aElem)
    {
        if (aElem.second.mType == ResourceSlot_TexBuffer)
        {
            auto pRes = GetSlotDeviceResource(&aElem.second);
            if (pRes == nullptr)
                return;

            auto pDevice = pRes->GetDevice();
            switch (pRes->GetType())
            {
                case eResource_TexBuffer:
                {
                    auto pTexBuffer = std::dynamic_pointer_cast<DrawingTexBuffer>(pRes);
                    if (pTexBuffer != nullptr)
                        pDevice->UpdateEffectTexBuffer(pTexBuffer, aElem.second.mpKey, effect);
                    break;
                }
                case eResource_RWBuffer:
                {
                    auto pRWBuffer = std::dynamic_pointer_cast<DrawingRWBuffer>(pRes);
                    if (pRWBuffer != nullptr)
                        pDevice->UpdateEffectRWBuffer(pRWBuffer, aElem.second.mpKey, effect);
                    break;
                }
                default:
                    assert(false);
                    break;
            }
        }
    });
}

void DrawingPass::DynamicResourceSlotTable::UpdateRWBuffers(std::shared_ptr<DrawingEffect> effect)
{
    std::for_each(mSlotTable.cbegin(), mSlotTable.cend(), [&effect](const ResourceSlotTableType::value_type& aElem)
    {
        if (aElem.second.mType == ResourceSlot_RWBuffer_Input || aElem.second.mType == ResourceSlot_RWBuffer_Output)
        {
            auto pRWBuffer = std::dynamic_pointer_cast<DrawingRWBuffer>(GetSlotDeviceResource(&aElem.second));
            if (pRWBuffer != nullptr)
            {
                auto pDevice = pRWBuffer->GetDevice();
                if (aElem.second.mType == ResourceSlot_RWBuffer_Input)
                    pDevice->UpdateEffectInputRWBuffer(pRWBuffer, aElem.second.mpKey, effect);
                else if (aElem.second.mType == ResourceSlot_RWBuffer_Output)
                    pDevice->UpdateEffectOutputRWBuffer(pRWBuffer, aElem.second.mpKey, effect);
            }
        }
    });
}

void DrawingPass::DynamicResourceSlotTable::UpdateBuffers(std::shared_ptr<DrawingEffect> effect)
{
}

void DrawingPass::DynamicResourceSlotTable::UpdateSamplers(std::shared_ptr<DrawingEffect> effect)
{
}

DrawingPass::StaticResourceSlotTable::StaticResourceSlotTable()
{
    AddStaticResourceSlot();
}

DrawingPass::StaticResourceSlotTable::~StaticResourceSlotTable()
{
}

std::shared_ptr<std::string> DrawingPass::StaticResourceSlotTable::sResourceSlotName[Max_Static_Slot] = { nullptr };

std::shared_ptr<std::string> DrawingPass::StaticResourceSlotTable::GetStaticSlotName(const char* name, uint32_t id, int32_t sub)
{
    auto pName = sResourceSlotName[id];
    if (pName == nullptr)
    {
        auto key = std::string(name);
        if (sub >= 0)
            key += sub;

        pName = std::make_shared<std::string>(key);
        sResourceSlotName[id] = pName;
    }

    return pName;
}

uint32_t DrawingPass::StaticResourceSlotTable::GetEffectSlotID()
{
    return Effect_ID;
}

uint32_t DrawingPass::StaticResourceSlotTable::GetVertexFormatSlotID()
{
    return Vertex_Format_ID;
}

uint32_t DrawingPass::StaticResourceSlotTable::GetVertexBufferSlotID(uint32_t index)
{
    return Vertex_Buffer_Start + index;
}

uint32_t DrawingPass::StaticResourceSlotTable::GetIndexBufferSlotID()
{
    return Index_Buffer_ID;
}

uint32_t DrawingPass::StaticResourceSlotTable::GetBlendStateSlotID()
{
    return Blend_State_ID;
}

uint32_t DrawingPass::StaticResourceSlotTable::GetRasterStateSlotID()
{
    return Raster_State_ID;
}

uint32_t DrawingPass::StaticResourceSlotTable::GetDepthStateSlotID()
{
    return Depth_State_ID;
}

uint32_t DrawingPass::StaticResourceSlotTable::GetTargetSlotID(uint32_t index)
{
    return Target_Start + index;
}

uint32_t DrawingPass::StaticResourceSlotTable::GetDepthBufferSlotID()
{
    return DepthBuffer_ID;
}

uint32_t DrawingPass::StaticResourceSlotTable::GetRWBufferSlotID(uint32_t index)
{
    return RWBuffer_Start + index;
}

uint32_t DrawingPass::StaticResourceSlotTable::GetPrimitiveSlotID()
{
    return Prim_Info_ID;
}

uint32_t DrawingPass::StaticResourceSlotTable::GetVaringStatesSlotID()
{
    return Varing_State_ID;
}

uint32_t DrawingPass::StaticResourceSlotTable::GetPipelineStateSlotID()
{
    return Pipeline_State_ID;
}

void DrawingPass::StaticResourceSlotTable::UpdateVertexFormat(const std::shared_ptr<DrawingDevice>& device)
{
    auto it = mSlotTable.find(VertexFormatSlotName());
    assert(it != mSlotTable.cend());

    auto pVertexFormat = std::dynamic_pointer_cast<DrawingVertexFormat>(GetSlotDeviceResource(&(it->second)));
    device->SetVertexFormat(pVertexFormat);
}

void DrawingPass::StaticResourceSlotTable::UpdateVertexBuffer(const std::shared_ptr<DrawingDevice>& device)
{
    std::shared_ptr<DrawingVertexBuffer> vertexBuffers[MAX_VERTEX_STREAM] = { nullptr };
    int32_t max_streams = 0;
    for (int32_t i = 0; i < MAX_VERTEX_STREAM; ++i)
    {
        auto it = mSlotTable.find(VertexBufferSlotName(i));
        if (it == mSlotTable.cend())
            continue;

        vertexBuffers[i] = std::dynamic_pointer_cast<DrawingVertexBuffer>(GetSlotDeviceResource(&(it->second)));
        if (vertexBuffers[i] != nullptr)
            max_streams = i + 1;
    }
    device->SetVertexBuffer(vertexBuffers, max_streams);
}

void DrawingPass::StaticResourceSlotTable::UpdateIndexBuffer(const std::shared_ptr<DrawingDevice>& device)
{
    auto it = mSlotTable.find(IndexBufferSlotName());
    assert(it != mSlotTable.cend());

    auto pIndexBuffer = std::dynamic_pointer_cast<DrawingIndexBuffer>(GetSlotDeviceResource(&(it->second)));
    device->SetIndexBuffer(pIndexBuffer);
}

void DrawingPass::StaticResourceSlotTable::UpdateTargets(const std::shared_ptr<DrawingDevice>& device)
{
    std::shared_ptr<DrawingTarget> pTargets[MAX_TARGETS] = { nullptr };
    uint32_t max_targets = 0;
    LoadTargets(pTargets, max_targets);

    auto pDepthBuffer = LoadDepthBuffer();

    std::shared_ptr<DrawingRWBuffer> pRWBuffer[MAX_RW_BUFFER] = { nullptr };
    uint32_t max_rwbuffers = 0;
    LoadRWBuffers(pRWBuffer, max_rwbuffers);

    device->SetTargets(pTargets, max_targets, pDepthBuffer, pRWBuffer, max_rwbuffers);
}

void DrawingPass::StaticResourceSlotTable::UpdateBlendState(const std::shared_ptr<DrawingDevice>& device)
{
    auto it = mSlotTable.find(BlendStateSlotName());
    assert(it != mSlotTable.cend());

    auto pState = std::dynamic_pointer_cast<DrawingBlendState>(GetSlotDeviceResource(&(it->second)));
    device->SetBlendState(pState, float4(1.0f), 0xffffffff);
}

void DrawingPass::StaticResourceSlotTable::UpdateDepthState(const std::shared_ptr<DrawingDevice>& device)
{
    auto it = mSlotTable.find(DepthStateSlotName());
    assert(it != mSlotTable.cend());

    auto pState = std::dynamic_pointer_cast<DrawingDepthState>(GetSlotDeviceResource(&(it->second)));
    device->SetDepthState(pState, 1);
}

void DrawingPass::StaticResourceSlotTable::UpdateRasterState(const std::shared_ptr<DrawingDevice>& device)
{
    auto it = mSlotTable.find(RasterStateSlotName());
    assert(it != mSlotTable.cend());

    auto pState = std::dynamic_pointer_cast<DrawingRasterState>(GetSlotDeviceResource(&(it->second)));
    device->SetRasterState(pState);
}

void DrawingPass::StaticResourceSlotTable::UpdateViewport(const std::shared_ptr<DrawingDevice>& device)
{
    auto it = mSlotTable.find(VaringStatesSlotName());
    assert(it != mSlotTable.cend());

    auto pStates = std::dynamic_pointer_cast<DrawingVaringStates>(GetSlotDeviceResource(&(it->second)));
    if (pStates != nullptr)
    {
        auto vp = pStates->GetViewport();
        device->SetViewport(&vp);
    }
    else
        device->SetViewport(nullptr);
}

void DrawingPass::StaticResourceSlotTable::UpdateScissorBox(const std::shared_ptr<DrawingDevice>& device)
{
}

void DrawingPass::StaticResourceSlotTable::UpdatePipelineState(const std::shared_ptr<DrawingDevice>& device)
{
    auto it = mSlotTable.find(PipelineStateSlotName());
    assert(it != mSlotTable.cend());

    auto pPipelineState = std::dynamic_pointer_cast<DrawingPipelineState>(GetSlotDeviceResource(&(it->second)));
    device->SetPipelineState(pPipelineState);
}

void DrawingPass::StaticResourceSlotTable::RestoreViewport(const std::shared_ptr<DrawingDevice>& device)
{
}

void DrawingPass::StaticResourceSlotTable::RestoreScissorBox(const std::shared_ptr<DrawingDevice>& device)
{
}

std::shared_ptr<DrawingEffect> DrawingPass::StaticResourceSlotTable::LoadEffect()
{
    auto it = mSlotTable.find(EffectSlotName());

    if (it == mSlotTable.cend())
        return nullptr;

    return std::dynamic_pointer_cast<DrawingEffect>(GetSlotDeviceResource(&(it->second)));
}

std::shared_ptr<DrawingPrimitive> DrawingPass::StaticResourceSlotTable::LoadPrimitive()
{
    auto it = mSlotTable.find(PrimitiveSlotName());

    if (it == mSlotTable.cend())
        return nullptr;

    return std::dynamic_pointer_cast<DrawingPrimitive>(GetSlotDeviceResource(&(it->second)));
}

void DrawingPass::StaticResourceSlotTable::AddStaticResourceSlot()
{
    AddStaticResourceSlot(EffectSlotName());

    AddStaticResourceSlot(VertexFormatSlotName());
    
    for (uint32_t i = 0; i < MAX_VERTEX_STREAM; ++i)
        AddStaticResourceSlot(VertexBufferSlotName(i));

    AddStaticResourceSlot(IndexBufferSlotName());

    AddStaticResourceSlot(BlendStateSlotName());
    AddStaticResourceSlot(RasterStateSlotName());
    AddStaticResourceSlot(DepthStateSlotName());

    for (uint32_t i = 0; i < MAX_TARGETS; ++i)
        AddStaticResourceSlot(TargetSlotName(i));

    AddStaticResourceSlot(DepthBufferSlotName());

    for (uint32_t i = 0; i < MAX_RW_BUFFER; ++i)
        AddStaticResourceSlot(RWBufferSlotName(i));

    AddStaticResourceSlot(PrimitiveSlotName());
    AddStaticResourceSlot(VaringStatesSlotName());
    AddStaticResourceSlot(PipelineStateSlotName());
}

void DrawingPass::StaticResourceSlotTable::AddStaticResourceSlot(std::shared_ptr<std::string> slotName)
{
    mSlotTable.emplace(slotName, ResourceSlot(slotName, ResourceSlot_Static, nullptr));
}

void DrawingPass::StaticResourceSlotTable::LoadTargets(std::shared_ptr<DrawingTarget> targets[], uint32_t& targetCount)
{
    for (uint32_t i = 0; i < MAX_TARGETS; ++i)
    {
        auto it = mSlotTable.find(TargetSlotName(i));
        if (it == mSlotTable.cend())
            continue;
        auto pSlot = &(it->second);

        targets[i] = std::dynamic_pointer_cast<DrawingTarget>(GetSlotDeviceResource(pSlot));
        if (pSlot->mpRes != nullptr)
            targetCount = i + 1;
    }
}

std::shared_ptr<DrawingDepthBuffer> DrawingPass::StaticResourceSlotTable::LoadDepthBuffer()
{
    auto it = mSlotTable.find(DepthBufferSlotName());
    auto pSlot = &(it->second);

    return std::dynamic_pointer_cast<DrawingDepthBuffer>(GetSlotDeviceResource(pSlot));
}

void DrawingPass::StaticResourceSlotTable::LoadRWBuffers(std::shared_ptr<DrawingRWBuffer> rwbuffers[], uint32_t& bufferCount)
{
    for (uint32_t i = 0; i < MAX_RW_BUFFER; ++i)
    {
        auto it = mSlotTable.find(RWBufferSlotName(i));
        if (it == mSlotTable.cend())
            continue;
        auto pSlot = &(it->second);

        rwbuffers[i] = std::dynamic_pointer_cast<DrawingRWBuffer>(GetSlotDeviceResource(pSlot));
        if (pSlot->mpRes != nullptr)
            bufferCount = i + 1;
    }
}

std::shared_ptr<DrawingVertexFormat> DrawingPass::StaticResourceSlotTable::LoadVertexFormat()
{
    return nullptr;
}

void DrawingPass::StaticResourceSlotTable::LoadVertexBuffer(std::shared_ptr<DrawingVertexBuffer> vbs[], uint32_t& vbCount)
{
}

std::shared_ptr<DrawingIndexBuffer> DrawingPass::StaticResourceSlotTable::LoadIndexBuffer()
{
    return nullptr;
}