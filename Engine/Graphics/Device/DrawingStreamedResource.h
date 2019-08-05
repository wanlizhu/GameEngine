#pragma once

#include <memory>

namespace Engine
{
    class DrawingStreamedResource
    {
    public:
        DrawingStreamedResource();
        virtual ~DrawingStreamedResource();

        virtual bool Open() = 0;
        virtual bool Close() = 0;

        void* Map(uint32_t count);
        void UnMap(void* ptr = nullptr);

        void FillData(const void* pData, uint32_t count);
        void ResetData();
        void FlushData();

        bool CheckCapacity(uint32_t count) const;

        uint32_t GetStrideInByte() const;
        uint32_t GetCapacity() const;

        bool IsOpen() const;
        bool IsMapping() const;

        uint32_t GetFlushOffset() const;
        uint32_t GetSystemOffset() const;

    protected:
        virtual uint32_t CurObjectCount() const;

        virtual void ClearDeviceRes() = 0;
        virtual void ClearRawData() = 0;
        virtual void DoDeviceFlush() = 0;

        void ClearCurData();

        void* GetBufferPos(uint32_t offset);
        void UpdateStagingOffset(uint32_t count);
        void UpdateDeviceOffset(uint32_t offset);

        void DoCopy(void* pDst, const void* pSrc, uint32_t eleCount);

        template <class ResType>
        uint32_t GetElementCount(std::shared_ptr<ResType> pDevRes);

        template <class ResType>
        uint32_t GetDeviceStrideInBytes(std::shared_ptr<ResType> pDevRes);

    protected:
        uint32_t m_systemOffset;
        uint32_t m_deviceOffset;
        uint32_t m_elementCount;
        uint32_t m_strideInBytes;

        void* m_pRawData;
        void* m_pCurData;

        bool m_bIsOpen;
        bool m_bIsMapping;
    };

    class DrawingTexture;
    class DrawingVertexBuffer;
    class DrawingIndexBuffer;

    class DrawingTextureDesc;
    class DrawingVertexBufferDesc;
    class DrawingIndexBufferDesc;

    template <class ResType, class DescType>
    class DrawingTransientResource : public DrawingStreamedResource
    {
    public:
        DrawingTransientResource(std::shared_ptr<ResType> pDeviceRes);
        virtual ~DrawingTransientResource();

        bool Open() override;
        bool Close() override;

        std::shared_ptr<ResType> GetDeviceRes();

    private:
        void ClearDeviceRes() override;
        void ClearRawData() override;
        void DoDeviceFlush() override;

    protected:
        std::shared_ptr<ResType> m_pDeviceRes;
    };

    class DrawingTransientTexture : public DrawingTransientResource<DrawingTexture, DrawingTextureDesc>
    {
    public:
        DrawingTransientTexture(std::shared_ptr<DrawingTexture> pDeviceRes);
        virtual ~DrawingTransientTexture();

    private:
        void DoDeviceFlush() override;
    };

    class DrawingTransientVertexBuffer : public DrawingTransientResource<DrawingVertexBuffer, DrawingVertexBufferDesc>
    {
    public:
        DrawingTransientVertexBuffer(std::shared_ptr<DrawingVertexBuffer> pDeviceRes);
        virtual ~DrawingTransientVertexBuffer();
    };

    class DrawingTransientIndexBuffer : public DrawingTransientResource<DrawingIndexBuffer, DrawingIndexBufferDesc>
    {
    public:
        DrawingTransientIndexBuffer(std::shared_ptr<DrawingIndexBuffer> pDeviceRes);
        virtual ~DrawingTransientIndexBuffer();
    };

    template <class ResType, class DescType>
    class DrawingPersistResource : public DrawingStreamedResource
    {
    public:
        DrawingPersistResource(std::shared_ptr<ResType> pDeviceRes);
        virtual ~DrawingPersistResource();

        bool Open() override;
        bool Close() override;

        std::shared_ptr<ResType> GetDeviceRes();
        std::shared_ptr<ResType> GetStagingRes();

    private:
        void ClearDeviceRes() override;
        void ClearStagingRes();
        void ClearRawData() override;
        void DoDeviceFlush() override;

    protected:
        std::shared_ptr<ResType> m_pStagingRes;
        std::shared_ptr<ResType> m_pDeviceRes;
    };

    class DrawingPersistTexture : public DrawingPersistResource<DrawingTexture, DrawingTextureDesc>
    {
    public:
        DrawingPersistTexture(std::shared_ptr<DrawingTexture> pDeviceRes);
        virtual ~DrawingPersistTexture();
    };

    class DrawingPersistVertexBuffer : public DrawingPersistResource<DrawingVertexBuffer, DrawingVertexBufferDesc>
    {
    public:
        DrawingPersistVertexBuffer(std::shared_ptr<DrawingVertexBuffer> pDeviceRes);
        virtual ~DrawingPersistVertexBuffer();
    };

    class DrawingPersistIndexBuffer : public DrawingPersistResource<DrawingIndexBuffer, DrawingIndexBufferDesc>
    {
    public:
        DrawingPersistIndexBuffer(std::shared_ptr<DrawingIndexBuffer> pDeviceRes);
        virtual ~DrawingPersistIndexBuffer();
    };
}