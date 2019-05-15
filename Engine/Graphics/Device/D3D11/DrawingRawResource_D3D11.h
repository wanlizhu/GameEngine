#pragma once

#include <vector>
#include <array>
#include <string>
#include <assert.h>
#include <d3d11.h>
#include <dxgi.h>
#include <d3d11shader.h>
#include <d3dx11effect.h>

#include "DrawingDevice_D3D11.h"
#include "DrawingRawResource.h"
#include "DrawingUtil_D3D11.h"

namespace Engine
{
    class DrawingRawBlendState_D3D11 : public DrawingRawBlendState
    {
    public:
        DrawingRawBlendState_D3D11(std::shared_ptr<DrawingDevice_D3D11> pDevice, const D3D11_BLEND_DESC& desc) : m_pDevice(pDevice)
        {
            ID3D11BlendState* pRaw = nullptr;
            HRESULT hr = m_pDevice->GetDevice()->CreateBlendState(&desc, &pRaw);
            assert(SUCCEEDED(hr));

            m_pBlendState = std::shared_ptr<ID3D11BlendState>(pRaw, D3D11Releaser<ID3D11BlendState>);
        }

        DrawingRawBlendState_D3D11(std::shared_ptr<DrawingDevice_D3D11> pDevice, std::shared_ptr<ID3D11BlendState> blendState) : m_pDevice(pDevice), m_pBlendState(blendState)
        {
        }

        std::shared_ptr<ID3D11BlendState> Get() const
        {
            return m_pBlendState;
        }

    private:
        std::shared_ptr<DrawingDevice_D3D11> m_pDevice;
        std::shared_ptr<ID3D11BlendState> m_pBlendState;
    };

    class DrawingRawDepthState_D3D11 : public DrawingRawDepthState
    {
    public:
        DrawingRawDepthState_D3D11(std::shared_ptr<DrawingDevice_D3D11> pDevice, const D3D11_DEPTH_STENCIL_DESC& desc) : m_pDevice(pDevice)
        {
            ID3D11DepthStencilState* pRaw = nullptr;
            HRESULT hr = m_pDevice->GetDevice()->CreateDepthStencilState(&desc, &pRaw);
            assert(SUCCEEDED(hr));

            m_pDepthState = std::shared_ptr<ID3D11DepthStencilState>(pRaw, D3D11Releaser<ID3D11DepthStencilState>);
        }

        DrawingRawDepthState_D3D11(std::shared_ptr<DrawingDevice_D3D11> pDevice, std::shared_ptr<ID3D11DepthStencilState> depthState) : m_pDevice(pDevice), m_pDepthState(depthState)
        {
        }

        std::shared_ptr<ID3D11DepthStencilState> Get() const
        {
            return m_pDepthState;
        }

    private:
        std::shared_ptr<DrawingDevice_D3D11> m_pDevice;
        std::shared_ptr<ID3D11DepthStencilState> m_pDepthState;
    };

    class DrawingRawRasterState_D3D11 : public DrawingRawRasterState
    {
    public:
        DrawingRawRasterState_D3D11(std::shared_ptr<DrawingDevice_D3D11> pDevice, const D3D11_RASTERIZER_DESC& desc) : m_pDevice(pDevice)
        {
            ID3D11RasterizerState* pRaw = nullptr;
            HRESULT hr = m_pDevice->GetDevice()->CreateRasterizerState(&desc, &pRaw);
            assert(SUCCEEDED(hr));

            m_pRasterState = std::shared_ptr<ID3D11RasterizerState>(pRaw, D3D11Releaser<ID3D11RasterizerState>);
        }

        DrawingRawRasterState_D3D11(std::shared_ptr<DrawingDevice_D3D11> pDevice, std::shared_ptr<ID3D11RasterizerState> rasterState) : m_pDevice(pDevice), m_pRasterState(rasterState)
        {
        }

        std::shared_ptr<ID3D11RasterizerState> Get() const
        {
            return m_pRasterState;
        }

    private:
        std::shared_ptr<DrawingDevice_D3D11> m_pDevice;
        std::shared_ptr<ID3D11RasterizerState> m_pRasterState;
    };

    class DrawingRawSamplerState_D3D11 : public DrawingRawSamplerState
    {
    public:
        DrawingRawSamplerState_D3D11(std::shared_ptr<DrawingDevice_D3D11> pDevice, const D3D11_SAMPLER_DESC& desc) : m_pDevice(pDevice)
        {
            ID3D11SamplerState* pRaw = nullptr;
            HRESULT hr = m_pDevice->GetDevice()->CreateSamplerState(&desc, &pRaw);
            assert(SUCCEEDED(hr));

            m_pSamplerState = std::shared_ptr<ID3D11SamplerState>(pRaw, D3D11Releaser<ID3D11SamplerState>);
        }

        std::shared_ptr<ID3D11SamplerState> Get() const
        {
            return m_pSamplerState;
        }

    private:
        std::shared_ptr<DrawingDevice_D3D11> m_pDevice;
        std::shared_ptr<ID3D11SamplerState> m_pSamplerState;
    };

    class DrawingRawTexture_D3D11 : public DrawingRawTexture
    {
    public:
        typedef ID3D11Resource TextureType;
        DrawingRawTexture_D3D11(std::shared_ptr<DrawingDevice_D3D11> pDevice, uint32_t sizeInBytes = 0, uint32_t startOffset = 0) : m_pDevice(pDevice), m_sizeInBytes(sizeInBytes), m_startOffset(startOffset) {}
        DrawingRawTexture_D3D11(std::shared_ptr<DrawingDevice_D3D11> pDevice, std::shared_ptr<ID3D11ShaderResourceView> pShaderResourceView, uint32_t sizeInBytes = 0, uint32_t startOffset = 0) :
            m_pDevice(pDevice), m_pShaderResourceView(pShaderResourceView), m_sizeInBytes(sizeInBytes), m_startOffset(startOffset) {}

        std::shared_ptr<ID3D11ShaderResourceView> GetShaderResourceView() const
        {
            return m_pShaderResourceView;
        }

        virtual TextureType* GetBuffer() const = 0;

    protected:
        uint32_t m_sizeInBytes;
        uint32_t m_startOffset;
        std::shared_ptr<DrawingDevice_D3D11> m_pDevice;
        std::shared_ptr<ID3D11ShaderResourceView> m_pShaderResourceView; 
    };

    class DrawingRawTexture1D_D3D11 : public DrawingRawTexture_D3D11
    {
    public:
        typedef ID3D11Texture1D TextureType;
        DrawingRawTexture1D_D3D11(std::shared_ptr<DrawingDevice_D3D11> pDevice, const D3D11_TEXTURE1D_DESC& desc, const std::vector<D3D11_SUBRESOURCE_DATA>& data) : DrawingRawTexture_D3D11(pDevice, desc.Width)
        {
            ID3D11Texture1D* pTextureRaw = nullptr;
            HRESULT hr = m_pDevice->GetDevice()->CreateTexture1D(&desc, data.data(), &pTextureRaw);
            assert(SUCCEEDED(hr));
            m_pTexture1D = std::shared_ptr<ID3D11Texture1D>(pTextureRaw, D3D11Releaser<ID3D11Texture1D>);

            ID3D11ShaderResourceView* pResourceViewRaw = nullptr;
            hr = m_pDevice->GetDevice()->CreateShaderResourceView(pTextureRaw, nullptr, &pResourceViewRaw);
            assert(SUCCEEDED(hr));
            m_pShaderResourceView = std::shared_ptr<ID3D11ShaderResourceView>(pResourceViewRaw, D3D11Releaser<ID3D11ShaderResourceView>);
        }

        TextureType* GetBuffer() const override
        {
            return m_pTexture1D.get();
        }

    private:
        std::shared_ptr<TextureType> m_pTexture1D;
    };

    class DrawingRawTexture2D_D3D11 : public DrawingRawTexture_D3D11
    {
    public:
        typedef ID3D11Texture2D TextureType;
        DrawingRawTexture2D_D3D11(std::shared_ptr<DrawingDevice_D3D11> pDevice, const D3D11_TEXTURE2D_DESC& desc, const std::vector<D3D11_SUBRESOURCE_DATA>& data) : DrawingRawTexture_D3D11(pDevice, desc.Width)
        {
            ID3D11Texture2D* pTextureRaw = nullptr;
            HRESULT hr = m_pDevice->GetDevice()->CreateTexture2D(&desc, data.data(), &pTextureRaw);
            assert(SUCCEEDED(hr));
            m_pTexture2D = std::shared_ptr<ID3D11Texture2D>(pTextureRaw, D3D11Releaser<ID3D11Texture2D>);

            ID3D11ShaderResourceView* pResourceViewRaw = nullptr;
            hr = m_pDevice->GetDevice()->CreateShaderResourceView(pTextureRaw, nullptr, &pResourceViewRaw);
            assert(SUCCEEDED(hr));
            m_pShaderResourceView = std::shared_ptr<ID3D11ShaderResourceView>(pResourceViewRaw, D3D11Releaser<ID3D11ShaderResourceView>);
        }

        TextureType* GetBuffer() const override
        {
            return m_pTexture2D.get();
        }

    private:
        std::shared_ptr<TextureType> m_pTexture2D;
    };

    class DrawingRawTexture3D_D3D11 : public DrawingRawTexture_D3D11
    {
    public:
        typedef ID3D11Texture3D TextureType;
        DrawingRawTexture3D_D3D11(std::shared_ptr<DrawingDevice_D3D11> pDevice, const D3D11_TEXTURE3D_DESC& desc, const std::vector<D3D11_SUBRESOURCE_DATA>& data) : DrawingRawTexture_D3D11(pDevice, desc.Width)
        {
            ID3D11Texture3D* pTextureRaw = nullptr;
            HRESULT hr = m_pDevice->GetDevice()->CreateTexture3D(&desc, data.data(), &pTextureRaw);
            assert(SUCCEEDED(hr));
            m_pTexture3D = std::shared_ptr<ID3D11Texture3D>(pTextureRaw, D3D11Releaser<ID3D11Texture3D>);

            ID3D11ShaderResourceView* pResourceViewRaw = nullptr;
            hr = m_pDevice->GetDevice()->CreateShaderResourceView(pTextureRaw, nullptr, &pResourceViewRaw);
            assert(SUCCEEDED(hr));
            m_pShaderResourceView = std::shared_ptr<ID3D11ShaderResourceView>(pResourceViewRaw, D3D11Releaser<ID3D11ShaderResourceView>);
        }

        TextureType* GetBuffer() const override
        {
            return m_pTexture3D.get();
        }


    private:
        std::shared_ptr<TextureType> m_pTexture3D;
    };

    class DrawingRawShader_D3D11 : public DrawingRawShader_Common
    {
    public:
        DrawingRawShader_D3D11(std::shared_ptr<DrawingDevice_D3D11> pDevice, std::shared_ptr<ID3D11ShaderReflection> pReflection) : DrawingRawShader_Common(), m_pDevice(pDevice), m_pReflection(pReflection) {}
        virtual ~DrawingRawShader_D3D11() = default;

        void BuildResourceBindingTable(DrawingRawShader::DrawingRawShaderType type);

    private:
        void ProcessVariables(std::shared_ptr<std::string> pName, uint32_t size, ID3D11ShaderReflectionConstantBuffer* pBuffer, uint32_t count);

    protected:
        std::shared_ptr<DrawingDevice_D3D11> m_pDevice;
        std::shared_ptr<ID3D11ShaderReflection> m_pReflection; 
    };

    class DrawingRawVertexShader_D3D11 : public DrawingRawVertexShader
    {
    public:
        typedef ID3D11VertexShader ShaderType;
        DrawingRawVertexShader_D3D11(std::shared_ptr<DrawingDevice_D3D11> pDevice, std::shared_ptr<std::string> pShaderName, std::shared_ptr<ID3D11ShaderReflection> pReflection, std::shared_ptr<ShaderType> pShader) :
            DrawingRawVertexShader(pShaderName), m_pShader(pShader), m_pShaderImpl(std::make_shared<DrawingRawShader_D3D11>(pDevice, pReflection))
        {
            assert(m_pShaderImpl != nullptr);
            m_pShaderImpl->BuildResourceBindingTable(GetShaderType());
        }

        virtual ~DrawingRawVertexShader_D3D11() = default;

        ShaderType* GetShader() const
        {
            return m_pShader.get();
        }

        DrawingRawShader_D3D11* GetShaderImpl() const
        {
            return m_pShaderImpl.get();
        }

        const DrawingRawShader_D3D11::ShaderResourceTableType& GetConstanceBufferTable() const
        {
            return m_pShaderImpl->GetConstanceBufferTable();
        }

        const DrawingRawShader_D3D11::ShaderResourceTableType& GetTextureTable() const
        {
            return m_pShaderImpl->GetTextureTable();
        }

        const DrawingRawShader_D3D11::ShaderResourceTableType& GetTexBufferTable() const
        {
            return m_pShaderImpl->GetTexBufferTable();
        }

        const DrawingRawShader_D3D11::ShaderResourceTableType& GetUVATable() const
        {
            return m_pShaderImpl->GetUVATable();
        }

        const DrawingRawShader_D3D11::ShaderResourceTableType& GetSamplerTable() const
        {
            return m_pShaderImpl->GetSamplerTable();
        }

        const DrawingRawShader_D3D11::VariableTableType& GetVariableTable() const
        {
            return m_pShaderImpl->GetVariableTable();
        }

    private:
        std::shared_ptr<ShaderType> m_pShader;
        std::shared_ptr<DrawingRawShader_D3D11> m_pShaderImpl;
    };

    class DrawingRawPixelShader_D3D11 : public DrawingRawPixelShader
    {
    public:
        typedef ID3D11PixelShader ShaderType;
        DrawingRawPixelShader_D3D11(std::shared_ptr<DrawingDevice_D3D11> pDevice, std::shared_ptr<std::string> pShaderName, std::shared_ptr<ID3D11ShaderReflection> pReflection, std::shared_ptr<ShaderType> pShader) :
            DrawingRawPixelShader(pShaderName), m_pShader(pShader), m_pShaderImpl(std::make_shared<DrawingRawShader_D3D11>(pDevice, pReflection))
        {
            assert(m_pShaderImpl != nullptr);
            m_pShaderImpl-> BuildResourceBindingTable(GetShaderType());
        }

        virtual ~DrawingRawPixelShader_D3D11() = default;

        ShaderType* GetShader() const
        {
            return m_pShader.get();
        }

        DrawingRawShader_D3D11* GetShaderImpl() const
        {
            return m_pShaderImpl.get();
        }

        const DrawingRawShader_D3D11::ShaderResourceTableType& GetConstanceBufferTable() const
        {
            return m_pShaderImpl->GetConstanceBufferTable();
        }

        const DrawingRawShader_D3D11::ShaderResourceTableType& GetTextureTable() const
        {
            return m_pShaderImpl->GetTextureTable();
        }

        const DrawingRawShader_D3D11::ShaderResourceTableType& GetTexBufferTable() const
        {
            return m_pShaderImpl->GetTexBufferTable();
        }

        const DrawingRawShader_D3D11::ShaderResourceTableType& GetUVATable() const
        {
            return m_pShaderImpl->GetUVATable();
        }

        const DrawingRawShader_D3D11::ShaderResourceTableType& GetSamplerTable() const
        {
            return m_pShaderImpl->GetSamplerTable();
        }

        const DrawingRawShader_D3D11::VariableTableType& GetVariableTable() const
        {
            return m_pShaderImpl->GetVariableTable();
        }

    private:
        std::shared_ptr<ShaderType> m_pShader;
        std::shared_ptr<DrawingRawShader_D3D11> m_pShaderImpl;
    };

    class DrawingRawComputeShader_D3D11 : public DrawingRawComputeShader
    {
    public:
        typedef ID3D11ComputeShader ShaderType;
        DrawingRawComputeShader_D3D11(std::shared_ptr<DrawingDevice_D3D11> pDevice, std::shared_ptr<std::string> pShaderName, std::shared_ptr<ID3D11ShaderReflection> pReflection, std::shared_ptr<ShaderType> pShader) :
            DrawingRawComputeShader(pShaderName), m_pShader(pShader), m_pShaderImpl(std::make_shared<DrawingRawShader_D3D11>(pDevice, pReflection))
        {
            assert(m_pShaderImpl != nullptr);
            m_pShaderImpl-> BuildResourceBindingTable(GetShaderType());
        }

        virtual ~DrawingRawComputeShader_D3D11() = default;

        ShaderType* GetShader() const
        {
            return m_pShader.get();
        }

        DrawingRawShader_D3D11* GetShaderImpl() const
        {
            return m_pShaderImpl.get();
        }

        const DrawingRawShader_D3D11::ShaderResourceTableType& GetConstanceBufferTable() const
        {
            return m_pShaderImpl->GetConstanceBufferTable();
        }

        const DrawingRawShader_D3D11::ShaderResourceTableType& GetTextureTable() const
        {
            return m_pShaderImpl->GetTextureTable();
        }

        const DrawingRawShader_D3D11::ShaderResourceTableType& GetTexBufferTable() const
        {
            return m_pShaderImpl->GetTexBufferTable();
        }

        const DrawingRawShader_D3D11::ShaderResourceTableType& GetUVATable() const
        {
            return m_pShaderImpl->GetUVATable();
        }

        const DrawingRawShader_D3D11::ShaderResourceTableType& GetSamplerTable() const
        {
            return m_pShaderImpl->GetSamplerTable();
        }

        const DrawingRawShader_D3D11::VariableTableType& GetVariableTable() const
        {
            return m_pShaderImpl->GetVariableTable();
        }

    private:
        std::shared_ptr<ShaderType> m_pShader;
        std::shared_ptr<DrawingRawShader_D3D11> m_pShaderImpl;
    };

    class DrawingRawEffect_D3D11 : public DrawingRawEffect
    {
    public:
        DrawingRawEffect_D3D11(std::shared_ptr<DrawingDevice_D3D11> pDevice, std::shared_ptr<std::string> pEffectName) :
            DrawingRawEffect(pEffectName), m_pDevice(pDevice), m_pInputSignature(nullptr), m_byteCodeLength(0) {}

        virtual ~DrawingRawEffect_D3D11() = default;

        const void* GetInputSignature() const
        {
            return m_pInputSignature;
        }

        uint64_t GetByteCodeLength() const
        {
            return m_byteCodeLength;
        }

        void AddInputRWBufferName(const std::shared_ptr<std::string> apParamName)
        {
            m_inputRWBufferNames.insert(apParamName);
        }

        void AddOutputRWBufferName(const std::shared_ptr<std::string> apParamName)
        {
            m_outputRWBufferNames.insert(apParamName);
        }

    protected:
        std::shared_ptr<DrawingDevice_D3D11> m_pDevice;
        const void* m_pInputSignature;
        uint64_t m_byteCodeLength;

        std::unordered_set<std::shared_ptr<std::string>> m_inputRWBufferNames;
        std::unordered_set<std::shared_ptr<std::string>> m_outputRWBufferNames;
    };

    class DrawingRawFxEffect_D3D11 : public DrawingRawEffect_D3D11
    {
    public:
        DrawingRawFxEffect_D3D11(std::shared_ptr<DrawingDevice_D3D11> pDevice, std::shared_ptr<std::string> pEffectName, std::shared_ptr<std::string> pTechName,
            std::shared_ptr<ID3DX11Effect> pD3D11Effect) : DrawingRawEffect_D3D11(pDevice, pEffectName),
            m_pEffect(pD3D11Effect),
            m_pTech(nullptr),
            m_pPass(nullptr),
            m_bComputeShaderPass(false),
            m_pTechName(pTechName)
        {
            assert(pTechName != nullptr);
            assert(m_pEffect != nullptr);
            BuildEffect();
        }

        virtual ~DrawingRawFxEffect_D3D11()
        {
            m_pTechName = nullptr;
            mVarList.clear();
        }

        void Apply() override;
        void Terminate() override;

    private:
        void BuildEffect();
        void LoadTechAndPass();
        void BuildParameterSet();
        void ProcessVariable(ID3DX11EffectVariable* pVar);

    private:
        std::shared_ptr<ID3DX11Effect> m_pEffect;
        std::shared_ptr<ID3DX11EffectTechnique> m_pTech;
        std::shared_ptr<ID3DX11EffectPass> m_pPass;
        bool m_bComputeShaderPass;
        std::shared_ptr<std::string> m_pTechName;

        typedef struct _SParamVar
        {
            std::shared_ptr<DrawingParameter> m_pParam;
            ID3DX11EffectVariable* mpVar;

            _SParamVar(std::shared_ptr<DrawingParameter> pParam, ID3DX11EffectVariable* pVar) :
                m_pParam(pParam), mpVar(pVar) { }

            void UpdateParamValue(const DrawingRawFxEffect_D3D11* apEffect);
        } SParamVar;

        std::vector<SParamVar> mVarList;
    };

    class DrawingRawShaderEffect_D3D11 : public DrawingRawEffect_D3D11
    {
    public:
        DrawingRawShaderEffect_D3D11(std::shared_ptr<DrawingDevice_D3D11> pDevice, std::shared_ptr<std::string> pEffectName, const std::vector<std::shared_ptr<DrawingRawShader>>& shaders);
        DrawingRawShaderEffect_D3D11(std::shared_ptr<DrawingDevice_D3D11> pDevice, std::shared_ptr<std::string> pEffectName, std::shared_ptr<DrawingRawVertexShader_D3D11> pVertexShader, std::shared_ptr<DrawingRawPixelShader_D3D11> pPixelShader);

        void Apply() override;
        void Terminate() override;

    private:
        struct SConstBuffer
        {
            std::shared_ptr<std::string> mpName = nullptr;
            uint32_t mSizeInBytes = 0;

            std::array<uint32_t, DrawingRawShader::RawShader_Count> mStartSlot;
            std::shared_ptr<DrawingRawConstantBuffer> mpCB;
            SConstBuffer()
            {
                mStartSlot.fill(EMPTY_SLOT);
            }
        };

        struct SParamRes
        {
            std::shared_ptr<DrawingParameter> mpParam = nullptr;
            uint32_t mCount = 0;

            std::array<uint32_t, DrawingRawShader::RawShader_Count> mStartSlot;

            SParamRes()
            {
                mStartSlot.fill(EMPTY_SLOT);
            }
        };

        struct SParamVar
        {
            std::shared_ptr<DrawingParameter> mpParam = nullptr;
            uint32_t mSizeInBytes = 0;

            std::array<uint32_t, DrawingRawShader::RawShader_Count> mOffset;
            std::array<std::shared_ptr<DrawingRawConstantBuffer>, DrawingRawShader::RawShader_Count> mpCB;

            SParamVar()
            {
                mOffset.fill(0);
            }

            void UpdateValues(void);
        };

        struct ShaderBlock
        {
            std::shared_ptr<DrawingRawShader> mpShader;
            uint32_t mCBSlotsCount;
            uint32_t mSRVSlotsCount;
            uint32_t mSamplerSlotsCount;
            uint32_t mUAVSlotsCount;

            std::array<ID3D11Buffer*, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT> mCBSlots;
            std::array<ID3D11ShaderResourceView*, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT> mSRVSlots;
            std::array<ID3D11SamplerState*, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT> mSamplerSlots;
            std::array<ID3D11UnorderedAccessView*, D3D11_PS_CS_UAV_REGISTER_COUNT> mUAVSlots;

            ShaderBlock()
            {
                mCBSlotsCount = 0;
                mSRVSlotsCount = 0;
                mSamplerSlotsCount = 0;
                mUAVSlotsCount = 0;

                mCBSlots.fill(nullptr);
                mSRVSlots.fill(nullptr);
                mSamplerSlots.fill(nullptr);
                mUAVSlots.fill(nullptr);
            }
        };
        void CheckAndAddResource(const DrawingRawShader_Common::ShaderResourceDesc& desc, uint32_t paramType, const DrawingRawShader::DrawingRawShaderType shaderType, std::unordered_map<std::shared_ptr<std::string>, SParamRes>& resTable) const;

        void LoadShaderInfo(const DrawingRawShader_D3D11* pShader, const DrawingRawShader::DrawingRawShaderType shaderType);

        void LoadConstantBufferFromShader(const DrawingRawShader_D3D11* pShader, const DrawingRawShader::DrawingRawShaderType shaderType);
        void LoadTexturesFromShader(const DrawingRawShader_D3D11* pShader, const DrawingRawShader::DrawingRawShaderType shaderType);

        void UpdateParameterValues();
        void UpdateConstantBuffers();
        void UpdateDevice();

        void UpdateComputeShader(const ShaderBlock& shaderBlock) const;
        void UpdateVertexShader(const ShaderBlock& shaderBlock) const;
        void UpdatePixelShader(const ShaderBlock& shaderBlock) const;

        void SetConstBufferSlots(ShaderBlock& shaderBlock, const DrawingRawShader::DrawingRawShaderType shaderType);
        void SetTextureSlots(ShaderBlock& shaderBlock, const DrawingRawShader::DrawingRawShaderType shaderType);
        void SetSamplerSlots(ShaderBlock& shaderBlock, const DrawingRawShader::DrawingRawShaderType shaderType);
        void SetTexBufferSlots(ShaderBlock& shaderBlock, const DrawingRawShader::DrawingRawShaderType shaderType);
        void SetRWBufferSlots(ShaderBlock& shaderBlock, const DrawingRawShader::DrawingRawShaderType shaderType);

        void BindConstantBuffer(DrawingDevice::ConstBufferPropTable& cbPropTable, const DrawingRawShader_D3D11* pShader, const DrawingRawShader::DrawingRawShaderType shaderType);
        void GenerateParameters(const DrawingRawShader_D3D11* pShader, const DrawingRawShader::DrawingRawShaderType shaderType);

        static constexpr const int32_t EMPTY_SLOT = -1;

        std::array<std::shared_ptr<ShaderBlock>, DrawingRawShader::RawShader_Count> m_shaderBlocks;

        std::unordered_map<std::shared_ptr<std::string>, SParamRes> mTexTable;
        std::unordered_map<std::shared_ptr<std::string>, SParamRes> mSamplerTable;
        std::unordered_map<std::shared_ptr<std::string>, SParamRes> mTexBufferTable;
        std::unordered_map<std::shared_ptr<std::string>, SParamRes> mRWBufferTable;
        std::unordered_map<std::shared_ptr<std::string>, SConstBuffer> mConstBufferTable;
        std::unordered_map<std::shared_ptr<std::string>, SParamVar> mVarTable;
    };

    class DrawingRawVertexFormat_D3D11 : public DrawingRawVertexFormat
    {
    public:
        DrawingRawVertexFormat_D3D11(std::shared_ptr<DrawingDevice_D3D11> pDevice, const std::vector<D3D11_INPUT_ELEMENT_DESC>& desc,
                                     const void* pShaderBytecodeWithInputSignature, uint32_t byteCodeLength) : m_pDevice(pDevice)
        {
            ID3D11InputLayout* pRaw = nullptr;
            HRESULT hr = m_pDevice->GetDevice()->CreateInputLayout(desc.data(), static_cast<UINT>(desc.size()), pShaderBytecodeWithInputSignature, byteCodeLength, &pRaw);
            assert(SUCCEEDED(hr));
            m_pInputLayout = std::shared_ptr<ID3D11InputLayout>(pRaw, D3D11Releaser<ID3D11InputLayout>);
        }

        virtual ~DrawingRawVertexFormat_D3D11() = default;

        std::shared_ptr<ID3D11InputLayout> Get() const
        {
            return m_pInputLayout;
        }

    private:
        std::shared_ptr<DrawingDevice_D3D11> m_pDevice;
        std::shared_ptr<ID3D11InputLayout> m_pInputLayout;
    };

    class DrawingRawBuffer_D3D11
    {
    public:
        DrawingRawBuffer_D3D11(std::shared_ptr<DrawingDevice_D3D11> pDevice, const D3D11_BUFFER_DESC& desc, uint32_t stride, uint32_t offset) :
            m_pDevice(pDevice), m_sizeInBytes(desc.ByteWidth), m_stride(stride), m_startOffset(offset)
        {
            ID3D11Buffer* pRaw = nullptr;
            HRESULT hr = m_pDevice->GetDevice()->CreateBuffer(&desc, nullptr, &pRaw);
            assert(SUCCEEDED(hr));
            m_pBuffer = std::shared_ptr<ID3D11Buffer>(pRaw, D3D11Releaser<ID3D11Buffer>);
        }

        DrawingRawBuffer_D3D11(std::shared_ptr<DrawingDevice_D3D11> pDevice, const D3D11_BUFFER_DESC& desc, const D3D11_SUBRESOURCE_DATA& data, uint32_t stride, uint32_t offset) :
            m_pDevice(pDevice), m_sizeInBytes(desc.ByteWidth), m_stride(stride), m_startOffset(offset)
        {
            ID3D11Buffer* pRaw = nullptr;
            HRESULT hr = m_pDevice->GetDevice()->CreateBuffer(&desc, &data, &pRaw);
            assert(SUCCEEDED(hr));
            m_pBuffer = std::shared_ptr<ID3D11Buffer>(pRaw, D3D11Releaser<ID3D11Buffer>);
        }

        virtual ~DrawingRawBuffer_D3D11() = default;

        std::shared_ptr<ID3D11Buffer> Get() const
        {
            return m_pBuffer;
        }

        uint32_t GetSizeInBytes() const
        {
            return m_sizeInBytes;
        }

        uint32_t GetStride() const
        {
            return m_stride;
        }

        uint32_t GetStartOffset() const
        {
            return m_startOffset;
        }

    private:
        std::shared_ptr<DrawingDevice_D3D11> m_pDevice;
        uint32_t m_sizeInBytes;
        uint32_t m_stride;
        uint32_t m_startOffset;
        std::shared_ptr<ID3D11Buffer> m_pBuffer;
    };

    class DrawingRawTexBuffer_D3D11 : public DrawingRawTexBuffer
    {
    public:
        DrawingRawTexBuffer_D3D11(std::shared_ptr<DrawingDevice_D3D11> pDevice, const D3D11_BUFFER_DESC& desc, const D3D11_SUBRESOURCE_DATA& data, uint32_t stride, uint32_t offset) :
            m_pDevice(pDevice), m_pBufferImpl(std::make_shared<DrawingRawBuffer_D3D11>(pDevice, desc, data, stride, offset))
        {
            ID3D11ShaderResourceView* pShaderResourceViewRaw = nullptr;
            HRESULT hr = m_pDevice->GetDevice()->CreateShaderResourceView(GetBuffer().get(), nullptr, &pShaderResourceViewRaw);
            assert(SUCCEEDED(hr));
            m_pShaderResourceView = std::shared_ptr<ID3D11ShaderResourceView>(pShaderResourceViewRaw, D3D11Releaser<ID3D11ShaderResourceView>);
        }

        virtual ~DrawingRawTexBuffer_D3D11() = default;

        DrawingRawBuffer_D3D11* GetBufferImpl()
        {
            return m_pBufferImpl.get();
        }

        std::shared_ptr<ID3D11Buffer> GetBuffer()
        {
            return m_pBufferImpl->Get();
        }

        std::shared_ptr<ID3D11ShaderResourceView> GetShaderResourceView() const
        {
            return m_pShaderResourceView;
        }

        uint32_t GetSizeInBytes() const
        {
            return m_pBufferImpl->GetSizeInBytes();
        }

        uint32_t GetStride() const
        {
            return m_pBufferImpl->GetStride();
        }

    private:
        std::shared_ptr<DrawingDevice_D3D11> m_pDevice;
        std::shared_ptr<DrawingRawBuffer_D3D11> m_pBufferImpl;
        std::shared_ptr<ID3D11ShaderResourceView> m_pShaderResourceView;
    };

    class DrawingRawRWBuffer_D3D11 : public DrawingRawRWBuffer
    {
    public:
        DrawingRawRWBuffer_D3D11(std::shared_ptr<DrawingDevice_D3D11> pDevice, const D3D11_BUFFER_DESC& desc, uint32_t stride, uint32_t offset) :
            m_pDevice(pDevice), m_pBufferImpl(std::make_shared<DrawingRawBuffer_D3D11>(pDevice, desc, stride, offset))
        {
            ID3D11ShaderResourceView* pShaderResourceViewRaw = nullptr;
            HRESULT hr = m_pDevice->GetDevice()->CreateShaderResourceView(GetBuffer().get(), nullptr, &pShaderResourceViewRaw);
            assert(SUCCEEDED(hr));
            m_pShaderResourceView = std::shared_ptr<ID3D11ShaderResourceView>(pShaderResourceViewRaw, D3D11Releaser<ID3D11ShaderResourceView>);

            ID3D11UnorderedAccessView* pUnorderedAccessViewRaw = nullptr;
            hr = m_pDevice->GetDevice()->CreateUnorderedAccessView(GetBuffer().get(), nullptr, &pUnorderedAccessViewRaw);
            assert(SUCCEEDED(hr));
            m_pUnorderedAccessView = std::shared_ptr<ID3D11UnorderedAccessView>(pUnorderedAccessViewRaw, D3D11Releaser<ID3D11UnorderedAccessView>);
        }

        virtual ~DrawingRawRWBuffer_D3D11() = default;

        DrawingRawBuffer_D3D11* GetBufferImpl()
        {
            return m_pBufferImpl.get();
        }

        std::shared_ptr<ID3D11Buffer> GetBuffer()
        {
            return m_pBufferImpl->Get();
        }

        std::shared_ptr<ID3D11ShaderResourceView> GetShaderResourceView() const
        {
            return m_pShaderResourceView;
        }

        std::shared_ptr<ID3D11UnorderedAccessView> GetUnorderedAccessView() const
        {
            return m_pUnorderedAccessView;
        }

        uint32_t GetSizeInBytes() const
        {
            return m_pBufferImpl->GetSizeInBytes();
        }

        uint32_t GetStride() const
        {
            return m_pBufferImpl->GetStride();
        }

    private:
        std::shared_ptr<DrawingDevice_D3D11> m_pDevice;
        std::shared_ptr<DrawingRawBuffer_D3D11> m_pBufferImpl;
        std::shared_ptr<ID3D11ShaderResourceView> m_pShaderResourceView;
        std::shared_ptr<ID3D11UnorderedAccessView> m_pUnorderedAccessView;
    };

    class DrawingRawVertexBuffer_D3D11 : public DrawingRawVertexBuffer
    {
    public:
        DrawingRawVertexBuffer_D3D11(std::shared_ptr<DrawingDevice_D3D11> pDevice, const D3D11_BUFFER_DESC& desc, D3D11_SUBRESOURCE_DATA& data, uint32_t stride, uint32_t offset) :
            m_pDevice(pDevice), m_pBufferImpl(std::make_shared<DrawingRawBuffer_D3D11>(pDevice, desc, data, stride, offset)) {}

        virtual ~DrawingRawVertexBuffer_D3D11() = default;

        DrawingRawBuffer_D3D11* GetBufferImpl()
        {
            return m_pBufferImpl.get();
        }

        std::shared_ptr<ID3D11Buffer> GetBuffer()
        {
            return m_pBufferImpl->Get();
        }

        uint32_t GetSizeInBytes() const
        {
            return m_pBufferImpl->GetSizeInBytes();
        }

        uint32_t GetStride() const
        {
            return m_pBufferImpl->GetStride();
        }

        uint32_t GetStartOffset() const
        {
            return m_pBufferImpl->GetStartOffset();
        }

    private:
        std::shared_ptr<DrawingDevice_D3D11> m_pDevice;
        std::shared_ptr<DrawingRawBuffer_D3D11> m_pBufferImpl;
    };

    class DrawingRawIndexBuffer_D3D11 : public DrawingRawIndexBuffer
    {
    public:
        DrawingRawIndexBuffer_D3D11(std::shared_ptr<DrawingDevice_D3D11> pDevice, const D3D11_BUFFER_DESC& desc, D3D11_SUBRESOURCE_DATA& data, uint32_t stride, uint32_t offset) :
            m_pDevice(pDevice), m_pBufferImpl(std::make_shared<DrawingRawBuffer_D3D11>(pDevice, desc, data, stride, offset)) {}

        virtual ~DrawingRawIndexBuffer_D3D11() = default;

        DrawingRawBuffer_D3D11* GetBufferImpl()
        {
            return m_pBufferImpl.get();
        }

        std::shared_ptr<ID3D11Buffer> GetBuffer()
        {
            return m_pBufferImpl->Get();
        }

        uint32_t GetSizeInBytes() const
        {
            return m_pBufferImpl->GetSizeInBytes();
        }

        uint32_t GetStride() const
        {
            return m_pBufferImpl->GetStride();
        }

        uint32_t GetStartOffset() const
        {
            return m_pBufferImpl->GetStartOffset();
        }

        DXGI_FORMAT GetBufferFormat() const
        {
            return (sizeof(uint16_t) == GetStride()) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
        }

    private:
        std::shared_ptr<DrawingDevice_D3D11> m_pDevice;
        std::shared_ptr<DrawingRawBuffer_D3D11> m_pBufferImpl;
    };

    class DrawingRawConstantBuffer_D3D11 : public DrawingRawConstantBuffer
    {
    public:
        DrawingRawConstantBuffer_D3D11(std::shared_ptr<DrawingDevice_D3D11> pDevice, uint32_t sizeInBytes) :
            DrawingRawConstantBuffer(sizeInBytes), m_pDevice(pDevice)
        {
            ID3D11Buffer* pRaw = nullptr;
            D3D11_BUFFER_DESC desc;
            desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            desc.ByteWidth = sizeInBytes;
            desc.Usage = D3D11_USAGE_DEFAULT;
            desc.CPUAccessFlags = 0;
            desc.MiscFlags = 0;
            desc.StructureByteStride = 0;

            HRESULT hr = m_pDevice->GetDevice()->CreateBuffer(&desc, nullptr, &pRaw);
            assert(SUCCEEDED(hr));
            m_pBuffer = std::shared_ptr<ID3D11Buffer>(pRaw, D3D11Releaser<ID3D11Buffer>);

            m_pData = new char[sizeInBytes];
            memset(m_pData, 0, sizeof(char) * sizeInBytes);
        }

        virtual ~DrawingRawConstantBuffer_D3D11() = default;

        std::shared_ptr<ID3D11Buffer> GetBuffer() const
        {
            return m_pBuffer;
        }

        void UpdateToHardware() override
        {
            assert(m_pDevice != nullptr);
            assert(m_pBuffer != nullptr);
            assert(m_pData != nullptr);

            auto pContext = m_pDevice->GetDeviceContext();
            assert(pContext != nullptr);

            pContext->UpdateSubresource(m_pBuffer.get(), 0, nullptr, m_pData, m_sizeInBytes, 0);
            m_bDirty = false;
        }

    private:
        std::shared_ptr<DrawingDevice_D3D11> m_pDevice;
        std::shared_ptr<ID3D11Buffer> m_pBuffer;
    };

    class DrawingRawTarget_D3D11 : public DrawingRawTarget
    {
    public:
        DrawingRawTarget_D3D11(std::shared_ptr<DrawingDevice_D3D11> pDevice) : m_pDevice(pDevice) {}

        DrawingRawTarget_D3D11(std::shared_ptr<DrawingDevice_D3D11> pDevice, const D3D11_TEXTURE2D_DESC& desc) : m_pDevice(pDevice)
        {
            ID3D11Texture2D* pRaw = nullptr;
            HRESULT hr = m_pDevice->GetDevice()->CreateTexture2D(&desc, nullptr, &pRaw);
            assert(SUCCEEDED(hr));
            m_pTarget = std::shared_ptr<ID3D11Texture2D>(pRaw, D3D11Releaser<ID3D11Texture2D>);
        }

        virtual ~DrawingRawTarget_D3D11() = default;

        std::shared_ptr<ID3D11Texture2D> GetTarget() const
        {
            return m_pTarget;
        }

        virtual ETargetType GetTargetType() const = 0;

    protected:
        std::shared_ptr<DrawingDevice_D3D11> m_pDevice;
        std::shared_ptr<ID3D11Texture2D> m_pTarget;
    };

    class DrawingRawFragmentTarget_D3D11 : public DrawingRawTarget_D3D11
    {
    public:
        using DrawingRawTarget_D3D11::DrawingRawTarget_D3D11;
        DrawingRawFragmentTarget_D3D11(std::shared_ptr<DrawingDevice_D3D11> pDevice, const D3D11_TEXTURE2D_DESC& desc) : DrawingRawTarget_D3D11(pDevice, desc)
        {
            auto pTargetRaw = m_pTarget.get();
            ID3D11RenderTargetView* pRenderTargetViewRaw = nullptr;
            HRESULT hr = m_pDevice->GetDevice()->CreateRenderTargetView(pTargetRaw, nullptr, &pRenderTargetViewRaw);
            assert(SUCCEEDED(hr));
            m_pRenderTargetView = std::shared_ptr<ID3D11RenderTargetView>(pRenderTargetViewRaw, D3D11Releaser<ID3D11RenderTargetView>);
        }

        virtual ~DrawingRawFragmentTarget_D3D11() = default;

        std::shared_ptr<ID3D11RenderTargetView> GetRenderTargetView() const
        {
            return m_pRenderTargetView;
        }

    protected:
        std::shared_ptr<ID3D11RenderTargetView> m_pRenderTargetView;
    };

    class DrawingRawRenderTarget_D3D11 : public DrawingRawFragmentTarget_D3D11
    {
    public:
        DrawingRawRenderTarget_D3D11(std::shared_ptr<DrawingDevice_D3D11> pDevice, const D3D11_TEXTURE2D_DESC& desc) : DrawingRawFragmentTarget_D3D11(pDevice, desc)
        {
            auto pTargetRaw = m_pTarget.get();
            ID3D11ShaderResourceView* pShaderResourceViewRaw = nullptr;
            HRESULT hr = m_pDevice->GetDevice()->CreateShaderResourceView(pTargetRaw, nullptr, &pShaderResourceViewRaw);
            assert(SUCCEEDED(hr));
            m_pShaderResourceView = std::shared_ptr<ID3D11ShaderResourceView>(pShaderResourceViewRaw, D3D11Releaser<ID3D11ShaderResourceView>);
        }

        virtual ~DrawingRawRenderTarget_D3D11() = default;

        std::shared_ptr<ID3D11ShaderResourceView> GetShaderResourceView() const
        {
            return m_pShaderResourceView;
        }

        ETargetType GetTargetType() const override
        {
            return eTarget_OffScreen;
        }

    protected:
        std::shared_ptr<ID3D11ShaderResourceView> m_pShaderResourceView;
    };

    class DrawingRawSwapChain_D3D11 : public DrawingRawFragmentTarget_D3D11
    {
    public:
        DrawingRawSwapChain_D3D11(std::shared_ptr<DrawingDevice_D3D11> pDevice, DXGI_SWAP_CHAIN_DESC& desc) : DrawingRawFragmentTarget_D3D11(pDevice)
        {
            IDXGISwapChain* pDXGISwapChainRaw = nullptr;
            HRESULT hr = m_pDevice->GetDXGIFactory()->CreateSwapChain(m_pDevice->GetDevice().get(), &desc, &pDXGISwapChainRaw);
            assert(SUCCEEDED(hr));
            m_pDXGISwapChain = std::shared_ptr<IDXGISwapChain>(pDXGISwapChainRaw, D3D11Releaser<IDXGISwapChain>);

            ID3D11Texture2D* pTargetRaw = nullptr;
            hr = m_pDXGISwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pTargetRaw));
            assert(SUCCEEDED(hr));
            m_pTarget = std::shared_ptr<ID3D11Texture2D>(pTargetRaw, D3D11Releaser<ID3D11Texture2D>);

            ID3D11RenderTargetView* pRenderTargetViewRaw = nullptr;
            hr = m_pDevice->GetDevice()->CreateRenderTargetView(pTargetRaw, nullptr, &pRenderTargetViewRaw);
            assert(SUCCEEDED(hr));
            m_pRenderTargetView = std::shared_ptr<ID3D11RenderTargetView>(pRenderTargetViewRaw, D3D11Releaser<ID3D11RenderTargetView>);
        }

        virtual ~DrawingRawSwapChain_D3D11() = default;

        std::shared_ptr<IDXGISwapChain> GetSwapChain() const
        {
            return m_pDXGISwapChain;
        }

        ETargetType GetTargetType() const override
        {
            return eTarget_SwapChain;
        }

        HRESULT Present(uint32_t syncInterval)
        {
            if (m_pDXGISwapChain != nullptr)
                return m_pDXGISwapChain->Present(syncInterval, 0);

            return S_OK;
        }

    private:
        std::shared_ptr<IDXGISwapChain> m_pDXGISwapChain;
    };

    class DrawingRawDepthTarget_D3D11 : public DrawingRawTarget_D3D11
    {
    public:
        DrawingRawDepthTarget_D3D11(std::shared_ptr<DrawingDevice_D3D11> pDevice, const D3D11_TEXTURE2D_DESC& desc) : DrawingRawTarget_D3D11(pDevice, desc)
        {
            auto pTargetRaw = m_pTarget.get();
            ID3D11DepthStencilView* pDepthStencilViewRaw = nullptr;
            HRESULT hr = m_pDevice->GetDevice()->CreateDepthStencilView(pTargetRaw, nullptr, &pDepthStencilViewRaw);
            assert(SUCCEEDED(hr));
            m_pDepthStencilView = std::shared_ptr<ID3D11DepthStencilView>(pDepthStencilViewRaw, D3D11Releaser<ID3D11DepthStencilView>);
        }

        virtual ~DrawingRawDepthTarget_D3D11() = default;

        std::shared_ptr<ID3D11DepthStencilView> GetDepthStencilView()
        {
            return m_pDepthStencilView;
        }

        ETargetType GetTargetType() const override
        {
            return eTarget_Depth;
        }

    private:
        std::shared_ptr<ID3D11DepthStencilView> m_pDepthStencilView;
    };
}