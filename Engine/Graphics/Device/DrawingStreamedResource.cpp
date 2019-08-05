#include <assert.h>

#include "DrawingResourceDesc.h"
#include "DrawingDevice.h"

#include "DrawingStreamedResource.h"

using namespace Engine;

DrawingStreamedResource::DrawingStreamedResource() :
    m_pRawData(nullptr), m_pCurData(nullptr), m_systemOffset(0), m_deviceOffset(0), m_elementCount(0), m_strideInBytes(0), m_bIsOpen(false), m_bIsMapping(false)
{
}

DrawingStreamedResource::~DrawingStreamedResource()
{
    ClearCurData();
}

void* DrawingStreamedResource::Map(uint32_t count)
{
    assert(IsOpen());
    assert(!IsMapping());
    assert(CheckCapacity(count));
    assert(count > 0);

    UpdateStagingOffset(count);

    m_bIsMapping = true;
    return m_pCurData;
}

void DrawingStreamedResource::UnMap(void* ptr)
{
    assert(IsMapping());

    if (ptr == nullptr)
        m_pCurData = GetBufferPos(m_systemOffset);
    else
        m_pCurData = ptr;

    m_bIsMapping = false;
}

void DrawingStreamedResource::FillData(const void* pData, uint32_t count)
{
    assert(IsOpen());
    assert(!IsMapping());
    assert(CheckCapacity(count));

    void* pDst = m_pCurData;
    DoCopy(pDst, pData, count);

    UpdateStagingOffset(count);
    m_pCurData = GetBufferPos(m_systemOffset);
}

void DrawingStreamedResource::ResetData()
{
    m_deviceOffset = 0;
    m_systemOffset = 0;
    m_pCurData = m_pRawData;
}

void DrawingStreamedResource::FlushData()
{
    if (m_systemOffset == 0)
        return;

    assert(m_systemOffset >= m_deviceOffset);

    if (m_systemOffset == m_deviceOffset)
        return;

    bool openStatus = IsOpen();
    if (openStatus)
    {
        if (Close() == false)
            return;
    }
    DoDeviceFlush();

    if (openStatus)
        Open();

    UpdateDeviceOffset(m_systemOffset); 
}

bool DrawingStreamedResource::CheckCapacity(uint32_t count) const
{
    return m_systemOffset + count <= m_elementCount;
}

uint32_t DrawingStreamedResource::GetStrideInByte() const
{
    return m_strideInBytes;
}

uint32_t DrawingStreamedResource::GetCapacity() const
{
    return m_elementCount;
}

bool DrawingStreamedResource::IsOpen() const
{
    return m_bIsOpen;
}

bool DrawingStreamedResource::IsMapping() const
{
    return m_bIsMapping;
}

uint32_t DrawingStreamedResource::GetFlushOffset() const
{
    return m_deviceOffset;
}

uint32_t DrawingStreamedResource::GetSystemOffset() const
{
    return m_systemOffset;
}

uint32_t DrawingStreamedResource::CurObjectCount() const
{
    return 0;
}

void DrawingStreamedResource::ClearCurData()
{
    m_pCurData = nullptr;
}

void* DrawingStreamedResource::GetBufferPos(uint32_t offset)
{
    assert(offset <= m_elementCount);
    return (uint8_t*)m_pRawData + offset * m_strideInBytes;
}

void DrawingStreamedResource::UpdateStagingOffset(uint32_t count)
{
    m_systemOffset += count;
}

void DrawingStreamedResource::UpdateDeviceOffset(uint32_t offset)
{
    m_deviceOffset = offset;
}

void DrawingStreamedResource::DoCopy(void* pDst, const void* pSrc, uint32_t count)
{
    auto sizeInBytes = count * m_strideInBytes;
    memcpy(pDst, pSrc, sizeInBytes);
}

template <>
uint32_t DrawingStreamedResource::GetElementCount<DrawingTexture>(std::shared_ptr<DrawingTexture> pDevRes)
{
    assert(pDevRes != nullptr);

    auto pDesc = std::dynamic_pointer_cast<const DrawingTextureDesc>(pDevRes->GetDesc());
    assert(pDesc != nullptr);

    uint32_t array_size = pDesc->mArraySize == 0 ? 1 : pDesc->mArraySize;

    return pDesc->mWidth * pDesc->mHeight * pDesc->mDepth * array_size;
}

template <>
uint32_t DrawingStreamedResource::GetElementCount<DrawingVertexBuffer>(std::shared_ptr<DrawingVertexBuffer> pDevRes)
{
    assert(pDevRes != nullptr);

    auto pDesc = std::dynamic_pointer_cast<const DrawingVertexBufferDesc>(pDevRes->GetDesc());
    assert(pDesc != nullptr);

    return pDesc->mSizeInBytes / pDesc->mStrideInBytes;
}

template <>
uint32_t DrawingStreamedResource::GetElementCount<DrawingIndexBuffer>(std::shared_ptr<DrawingIndexBuffer> pDevRes)
{
    assert(pDevRes != nullptr);

    auto pDesc = std::dynamic_pointer_cast<const DrawingIndexBufferDesc>(pDevRes->GetDesc());
    assert(pDesc != nullptr);

    return pDesc->mSizeInBytes / pDesc->mStrideInBytes;
}

template <>
uint32_t DrawingStreamedResource::GetDeviceStrideInBytes<DrawingTexture>(std::shared_ptr<DrawingTexture> pDevRes)
{
    assert(pDevRes != nullptr);

    auto pDesc = std::dynamic_pointer_cast<const DrawingTextureDesc>(pDevRes->GetDesc());
    assert(pDesc != nullptr);

    auto pDevice = pDevRes->GetDevice();

    return pDevice->FormatBytes(pDesc->mFormat);
}

template <>
uint32_t DrawingStreamedResource::GetDeviceStrideInBytes<DrawingVertexBuffer>(std::shared_ptr<DrawingVertexBuffer> pDevRes)
{
    assert(pDevRes != nullptr);

    auto pDesc = std::dynamic_pointer_cast<const DrawingVertexBufferDesc>(pDevRes->GetDesc());
    assert(pDesc != nullptr);

    return pDesc->mStrideInBytes;
}

template <>
uint32_t DrawingStreamedResource::GetDeviceStrideInBytes<DrawingIndexBuffer>(std::shared_ptr<DrawingIndexBuffer> pDevRes)
{
    assert(pDevRes != nullptr);

    auto pDesc = std::dynamic_pointer_cast<const DrawingIndexBufferDesc>(pDevRes->GetDesc());
    assert(pDesc != nullptr);

    return pDesc->mStrideInBytes;
}

template <class ResType, class DescType>
DrawingTransientResource<ResType, DescType>::DrawingTransientResource(std::shared_ptr<ResType> pDeviceRes) :
    DrawingStreamedResource(), m_pDeviceRes(pDeviceRes)
{
    m_elementCount = GetElementCount(pDeviceRes);
    m_strideInBytes = GetDeviceStrideInBytes(pDeviceRes);

    auto size = m_elementCount * m_strideInBytes;
    m_pRawData = new uint8_t[size];
    memset(m_pRawData, 0, size);

    m_pCurData = m_pRawData;
}

template <class ResType, class DescType>
DrawingTransientResource<ResType, DescType>::~DrawingTransientResource()
{
    if (IsMapping())
        UnMap();

    if (IsOpen())
        Close();

    ClearRawData();
    ClearDeviceRes();
}

template <class ResType, class DescType>
bool DrawingTransientResource<ResType, DescType>::Open()
{
    if (m_pRawData == nullptr)
        return false;

    m_bIsOpen = true;

    return m_bIsOpen;
}

template <class ResType, class DescType>
bool DrawingTransientResource<ResType, DescType>::Close()
{
    if (m_pRawData == nullptr)
        return false;

    m_bIsOpen = false;

    return true;
}

template <class ResType, class DescType>
std::shared_ptr<ResType> DrawingTransientResource<ResType, DescType>::GetDeviceRes()
{
    return m_pDeviceRes;
}

template <class ResType, class DescType>
void DrawingTransientResource<ResType, DescType>::ClearDeviceRes()
{
    m_pDeviceRes = nullptr;
}

template <class ResType, class DescType>
void DrawingTransientResource<ResType, DescType>::ClearRawData()
{
    delete[] m_pRawData;
    m_pRawData = nullptr;
}

template <class ResType, class DescType>
void DrawingTransientResource<ResType, DescType>::DoDeviceFlush()
{
    auto pDevice = m_pDeviceRes->GetDevice();

    uint32_t rowPitch = 0;
    uint32_t slicePitch = 0;

    uint32_t eleCount = m_systemOffset - m_deviceOffset;

    void* pDst = nullptr;
    if (m_deviceOffset == 0)
        pDst = pDevice->Map(m_pDeviceRes, 0, eAccess_Write_Discard, rowPitch, slicePitch, 0, eleCount * m_strideInBytes);
    else
        pDst = pDevice->Map(m_pDeviceRes, 0, eAccess_Write_Append, rowPitch, slicePitch, 0, eleCount * m_strideInBytes);

    void* pSrc = GetBufferPos(m_deviceOffset);
    assert(pDst != nullptr && pSrc != nullptr);

    DoCopy(pDst, pSrc, eleCount);

    pDevice->UnMap(m_pDeviceRes, 0);
}

DrawingTransientTexture::DrawingTransientTexture(std::shared_ptr<DrawingTexture> pDeviceRes) :
    DrawingTransientResource<DrawingTexture, DrawingTextureDesc>(pDeviceRes)
{
}

DrawingTransientTexture::~DrawingTransientTexture()
{
}

void DrawingTransientTexture::DoDeviceFlush()
{
    auto pDevice = m_pDeviceRes->GetDevice();

    uint32_t rowPitch = 0;
    uint32_t slicePitch = 0;

    uint32_t eleCount = m_systemOffset - m_deviceOffset;

    void* pDst = pDevice->Map(m_pDeviceRes, 0, eAccess_Write_Discard, rowPitch, slicePitch, 0, eleCount * m_strideInBytes);

    void* pSrc = GetBufferPos(m_deviceOffset);
    assert(pDst != nullptr && pSrc != nullptr);

    auto pDesc = std::dynamic_pointer_cast<DrawingTextureDesc>(m_pDeviceRes->GetDesc());

    if (pDesc->mBytesPerRow == (uint32_t)rowPitch)
        DoCopy(pDst, pSrc, eleCount);
    else
    {
        uint32_t copy_count = 0;

        for (uint32_t row = 0; row < pDesc->mHeight; ++row)
        {
            DoCopy(pDst, pSrc, pDesc->mWidth);
            pSrc = (uint8_t*)pSrc + pDesc->mBytesPerRow;
            pDst = (uint8_t*)pDst + rowPitch;

            copy_count += pDesc->mWidth;

            if (copy_count >= eleCount)
                break;
        }
    }
}

DrawingTransientVertexBuffer::DrawingTransientVertexBuffer(std::shared_ptr<DrawingVertexBuffer> pDeviceRes) :
    DrawingTransientResource<DrawingVertexBuffer, DrawingVertexBufferDesc>(pDeviceRes)
{
}

DrawingTransientVertexBuffer::~DrawingTransientVertexBuffer()
{
}

DrawingTransientIndexBuffer::DrawingTransientIndexBuffer(std::shared_ptr<DrawingIndexBuffer> pDeviceRes) :
    DrawingTransientResource<DrawingIndexBuffer, DrawingIndexBufferDesc>(pDeviceRes)
{
    
}

DrawingTransientIndexBuffer::~DrawingTransientIndexBuffer()
{
}

bool DoCreateResource(std::shared_ptr<DrawingDevice> pDevice, std::shared_ptr<DrawingTextureDesc> pDesc, std::shared_ptr<DrawingTexture>& pRes)
{
    return pDevice->CreateTexture(*pDesc, pRes);
}

bool DoCreateResource(std::shared_ptr<DrawingDevice> pDevice, std::shared_ptr<DrawingVertexBufferDesc> pDesc, std::shared_ptr<DrawingVertexBuffer>& pRes)
{
    return pDevice->CreateVertexBuffer(*pDesc, pRes);
}

bool DoCreateResource(std::shared_ptr<DrawingDevice> pDevice, std::shared_ptr<DrawingIndexBufferDesc> pDesc, std::shared_ptr<DrawingIndexBuffer>& pRes)
{
    return pDevice->CreateIndexBuffer(*pDesc, pRes);
}

template <class ResType, class DescType>
std::shared_ptr<ResType> CreateStagingResource(std::shared_ptr<ResType> pDevRes)
{
    assert(pDevRes != nullptr);

    auto pDesc = std::dynamic_pointer_cast<const DescType>(pDevRes->GetDesc());
    assert(pDesc != nullptr);
    assert(pDesc->mUsage == eUsage_Default);
    assert(pDesc->IsExternalResource() == false);

    auto pStagingDesc = std::shared_ptr<DescType>(static_cast<DescType*>(pDesc->Clone()));
    assert(pStagingDesc != nullptr);

    pStagingDesc->mUsage = eUsage_Staging;
    pStagingDesc->mAccess = eAccess_Write;
    pStagingDesc->mFlags = 0;

    auto pDevice = pDevRes->GetDevice();
    std::shared_ptr<ResType> pRes = nullptr;
    if (!DoCreateResource(pDevice, pStagingDesc, pRes))
    {
        pStagingDesc = nullptr;
        return nullptr;
    }

    pStagingDesc = nullptr;

    return pRes;
}

template <class ResType, class DescType>
DrawingPersistResource<ResType, DescType>::DrawingPersistResource(std::shared_ptr<ResType> pDeviceRes) :
    DrawingStreamedResource(), m_pDeviceRes(pDeviceRes)
{
    auto pStagingRes = CreateStagingResource<ResType, DescType>(pDeviceRes);
    m_pStagingRes = pStagingRes;

    m_elementCount = GetElementCount(pDeviceRes);
    m_strideInBytes = GetDeviceStrideInBytes(pDeviceRes);
}

template <class ResType, class DescType>
DrawingPersistResource<ResType, DescType>::~DrawingPersistResource()
{
    if (IsMapping())
        UnMap();

    if (IsOpen())
        Close();

    ClearRawData();
    ClearStagingRes();
    ClearDeviceRes();
}

template <class ResType, class DescType>
bool DrawingPersistResource<ResType, DescType>::Open()
{
    if (m_pStagingRes == nullptr)
        return false;

    auto pDevice = m_pDeviceRes->GetDevice();

    uint32_t rowPitch = 0;
    uint32_t slicePitch = 0;

    m_pRawData = pDevice->Map(m_pStagingRes, 0, eAccess_Write, rowPitch, slicePitch);

    if (m_pRawData != nullptr)
        m_bIsOpen = true;

    m_pCurData = GetBufferPos(m_systemOffset);

    return m_bIsOpen;
}

template <class ResType, class DescType>
bool DrawingPersistResource<ResType, DescType>::Close()
{
    if (m_pStagingRes == nullptr)
        return false;

    if (m_pRawData == nullptr)
        return false;

    auto pDevice = m_pDeviceRes->GetDevice();
    pDevice->UnMap(m_pStagingRes, 0);

    m_pRawData = nullptr;
    m_pCurData = nullptr;

    m_bIsOpen = false;

    return true;
}

template <class ResType, class DescType>
std::shared_ptr<ResType> DrawingPersistResource<ResType, DescType>::GetDeviceRes()
{
    return m_pDeviceRes;
}

template <class ResType, class DescType>
std::shared_ptr<ResType> DrawingPersistResource<ResType, DescType>::GetStagingRes()
{
    return m_pStagingRes;
}

template <class ResType, class DescType>
void DrawingPersistResource<ResType, DescType>::ClearDeviceRes()
{
    m_pDeviceRes = nullptr;
}

template <class ResType, class DescType>
void DrawingPersistResource<ResType, DescType>::ClearStagingRes()
{
    m_pStagingRes = nullptr;
}

template <class ResType, class DescType>
void DrawingPersistResource<ResType, DescType>::ClearRawData()
{
    m_pRawData = nullptr;
}

template <class ResType, class DescType>
void DrawingPersistResource<ResType, DescType>::DoDeviceFlush()
{
}

DrawingPersistTexture::DrawingPersistTexture(std::shared_ptr<DrawingTexture> pDeviceRes) :
    DrawingPersistResource<DrawingTexture, DrawingTextureDesc>(pDeviceRes)
{
}

DrawingPersistTexture::~DrawingPersistTexture()
{
}

DrawingPersistVertexBuffer::DrawingPersistVertexBuffer(std::shared_ptr<DrawingVertexBuffer> pDeviceRes) :
    DrawingPersistResource<DrawingVertexBuffer, DrawingVertexBufferDesc>(pDeviceRes)
{
}

DrawingPersistVertexBuffer::~DrawingPersistVertexBuffer()
{
}

DrawingPersistIndexBuffer::DrawingPersistIndexBuffer(std::shared_ptr<DrawingIndexBuffer> pDeviceRes) :
    DrawingPersistResource<DrawingIndexBuffer, DrawingIndexBufferDesc>(pDeviceRes)
{
}

DrawingPersistIndexBuffer::~DrawingPersistIndexBuffer()
{
}