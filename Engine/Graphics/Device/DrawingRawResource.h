#pragma once

#include <memory>
#include <string>
#include <assert.h>
#include <unordered_set>
#include <unordered_map>

#include "DrawingParameter.h"

namespace Engine
{
    class DrawingNativeContext
    {
    };

    class DrawingRawBlendState
    {
    public:
        virtual ~DrawingRawBlendState() = default;
    };

    class DrawingRawDepthState
    {
    public:
        virtual ~DrawingRawDepthState() = default;
    };

    class DrawingRawRasterState
    {
    public:
        virtual ~DrawingRawRasterState() = default;
    };

    class DrawingRawSamplerState
    {
    public:
        virtual ~DrawingRawSamplerState() = default;
    };

    class DrawingRawTexture
    {
    public:
        virtual ~DrawingRawTexture() = default;

        const std::shared_ptr<DrawingRawSamplerState>& GetSamplerState()
        {
            return m_pSamplerState;
        }

        void SetSamplerState(std::shared_ptr<DrawingRawSamplerState> pSamplerState)
        {
            m_pSamplerState = pSamplerState;
        }

    private:
        std::shared_ptr<DrawingRawSamplerState> m_pSamplerState;
    };

    class DrawingRawShader
    {
    public:
        DrawingRawShader(std::shared_ptr<std::string> pShaderName) : m_pShaderName(pShaderName) {}
        virtual ~DrawingRawShader() { m_pShaderName = nullptr; };

        enum DrawingRawShaderType
        {
            RawShader_VS,
            RawShader_PS,
            RawShader_CS,
            RawShader_Count,
        };

        virtual DrawingRawShaderType GetShaderType() const = 0;

    private:
        std::shared_ptr<std::string> m_pShaderName;
    };

    class DrawingRawEffect
    {
    protected:
        std::shared_ptr<std::string> m_pEffectName;
        std::shared_ptr<DrawingParameterSet> m_pParamSet;

    public:
        DrawingRawEffect(std::shared_ptr<std::string> pEffectName) : m_pEffectName(pEffectName),
            m_pParamSet(new DrawingParameterSet())
        {
        }
        virtual ~DrawingRawEffect()
        {
            m_pEffectName = nullptr;
        }

        DrawingParameterSet& GetParameterSet()
        {
            return *m_pParamSet;
        }

        bool UpdateParameter(std::shared_ptr<DrawingParameter> pParam)
        {
            assert(pParam != nullptr);
            int32_t index = m_pParamSet->IndexOfName(pParam->GetName());
            if (index < 0)
                return false;

            auto pDstParam = (*m_pParamSet)[index];
            if (pDstParam == nullptr)
                return false;

            if (pDstParam->GetType() != pParam->GetType())
                return false;

            pDstParam->SetValue(pParam->GetValuePtr(), pParam->GetValueSize());

            return true;
        }

        virtual void Apply() = 0;
        virtual void Terminate() = 0;
    };

    class DrawingRawVertexShader : public DrawingRawShader
    {
    public:
        DrawingRawVertexShader(std::shared_ptr<std::string> pShaderName) : DrawingRawShader(pShaderName) {}
        DrawingRawShaderType GetShaderType() const override { return RawShader_VS; }
    };

    class DrawingRawPixelShader : public DrawingRawShader
    {
    public:
        DrawingRawPixelShader(std::shared_ptr<std::string> pShaderName) : DrawingRawShader(pShaderName) {}
        DrawingRawShaderType GetShaderType() const override { return RawShader_PS; }
    };

    class DrawingRawComputeShader : public DrawingRawShader
    {
    public:
        DrawingRawComputeShader(std::shared_ptr<std::string> pShaderName) : DrawingRawShader(pShaderName) {}
        DrawingRawShaderType GetShaderType() const override { return RawShader_CS; }
    };

    class DrawingRawShader_Common
    {
    public:
        DrawingRawShader_Common() = default;
        virtual ~DrawingRawShader_Common()
        {
            mCBTable.clear();
            mTextureTable.clear();
            mTexBufferTable.clear();
            mUVATable.clear();
            mSamplerTable.clear();

            mVariableTable.clear();
        }

        struct ShaderResourceDesc
        {
            ShaderResourceDesc() : mpName(nullptr), mStartSlot(0), mCount(0), mSizeInBytes(0) {}

            std::shared_ptr<std::string> mpName;

            uint32_t mStartSlot;
            uint32_t mCount;
            uint32_t mSizeInBytes;
        };

        struct VariableDesc
        {
            VariableDesc() : mpName(nullptr), mpCBName(nullptr), mOffset(0), mSizeInBytes(0), mCBSizeInBytes(0), mParamType(0) {}

            std::shared_ptr<std::string> mpName;
            std::shared_ptr<std::string> mpCBName;

            uint32_t mOffset;
            uint32_t mSizeInBytes;
            uint32_t mCBSizeInBytes;
            uint32_t mParamType;
        };

        typedef std::unordered_map<std::shared_ptr<std::string>, ShaderResourceDesc> ShaderResourceTableType;
        typedef std::unordered_map<std::shared_ptr<std::string>, VariableDesc> VariableTableType;

        const ShaderResourceTableType& GetConstanceBufferTable() const
        {
            return mCBTable;
        }

        const ShaderResourceTableType& GetTextureTable() const
        {
            return mTextureTable;
        }

        const ShaderResourceTableType& GetTexBufferTable() const
        {
            return mTexBufferTable;
        }

        const ShaderResourceTableType& GetUVATable() const
        {
            return mUVATable;
        }

        const ShaderResourceTableType& GetSamplerTable() const
        {
            return mSamplerTable;
        }

        const VariableTableType& GetVariableTable() const
        {
            return mVariableTable;
        }

    protected:
        ShaderResourceTableType mCBTable;
        ShaderResourceTableType mTextureTable;
        ShaderResourceTableType mTexBufferTable;
        ShaderResourceTableType mUVATable;
        ShaderResourceTableType mSamplerTable;

        VariableTableType mVariableTable;
    };

    class DrawingRawTexBuffer
    {
    public:
        virtual ~DrawingRawTexBuffer() = default;
    };

    class DrawingRawRWBuffer
    {
    public:
        virtual ~DrawingRawRWBuffer() = default;
    };

    class DrawingRawVertexFormat
    {
    public:
        virtual ~DrawingRawVertexFormat() = default;
    };

    class DrawingRawVertexBuffer
    {
    public:
        virtual ~DrawingRawVertexBuffer() = default;
    };

    class DrawingRawIndexBuffer
    {
    public:
        virtual ~DrawingRawIndexBuffer() = default;
    };

    class DrawingRawConstantBuffer
    {
    public:
        DrawingRawConstantBuffer(uint32_t sizeInBytes) : m_sizeInBytes(sizeInBytes) {}
        virtual ~DrawingRawConstantBuffer() = default;

        virtual void SetValue(uint32_t offset, const void* pVal, uint32_t size)
        {
            assert(m_pData != nullptr);
            assert(offset + size <= m_sizeInBytes);
            assert(pVal != nullptr);

            if (memcmp(pVal, m_pData + offset, size) != 0)
            {
                memcpy(m_pData + offset, pVal, size);
                m_bDirty = true;
            }
        }

        virtual void UpdateToHardware() = 0;

        bool IsDirty() const
        {
            return m_bDirty;
        }

    protected:
        char* m_pData;
        uint32_t m_sizeInBytes;
        bool m_bDirty;
    };

    class DrawingRawTarget
    {
    public:
        enum ETargetType
        {
            eTarget_OffScreen,
            eTarget_SwapChain,
            eTarget_Depth,
        };

        virtual ~DrawingRawTarget() = default;
    };

    class DrawingRawPipelineState
    {
    public:
        virtual ~DrawingRawPipelineState() = default;
    };
}