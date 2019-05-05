#pragma once

#include <memory>
#include <unordered_map>
#include <string>

namespace Engine
{
    class DrawingDevice;
    class DrawingVertexShader;
    class DrawingPixelShader;
    class DrawingEffect;
    class DrawingShaderDesc;
    class DrawingVertexShaderDesc;
    class DrawingPixelShaderDesc;
    class DrawingEffectDesc;
    class DrawingEffectPool
    {
    public:
        DrawingEffectPool(const std::shared_ptr<DrawingDevice> pDevice);
        virtual ~DrawingEffectPool();

        bool LoadEffectFromBuffer(const DrawingEffectDesc& desc, std::shared_ptr<DrawingResource>& pRes);
        bool LoadEffectFromString(const DrawingEffectDesc& desc, std::shared_ptr<DrawingResource>& pRes);
        bool LoadEffectFromFile(const DrawingEffectDesc& desc, std::shared_ptr<DrawingResource>& pRes);
        bool LoadEffectFromShader(const DrawingEffectDesc& desc, std::shared_ptr<DrawingResource>& pRes);

        bool LoadVertexShaderFromBuffer(const DrawingVertexShaderDesc& desc, std::shared_ptr<DrawingVertexShader>& pRes);
        bool LoadVertexShaderFromString(const DrawingVertexShaderDesc& desc, std::shared_ptr<DrawingVertexShader>& pRes);
        bool LoadVertexShaderFromFile(const DrawingVertexShaderDesc& desc, std::shared_ptr<DrawingVertexShader>& pRes);

        bool LoadPixelShaderFromBuffer(const DrawingPixelShaderDesc& desc, std::shared_ptr<DrawingPixelShader>& pRes);
        bool LoadPixelShaderFromString(const DrawingPixelShaderDesc& desc, std::shared_ptr<DrawingPixelShader>& pRes);
        bool LoadPixelShaderFromFile(const DrawingPixelShaderDesc& desc, std::shared_ptr<DrawingPixelShader>& pRes);

        std::shared_ptr<DrawingEffect> GetEffect(std::shared_ptr<std::string> pName);
        bool AddEffectToPool(std::shared_ptr<std::string> pName, std::shared_ptr<DrawingEffect> pEffect);
        bool RemoveEffectFromPool(std::shared_ptr<std::string> pName);

        std::shared_ptr<DrawingVertexShader> GetVertexShader(std::shared_ptr<std::string> pName);
        bool AddVertexShaderToPool(std::shared_ptr<std::string> pName, std::shared_ptr<DrawingVertexShader> pVertexShader);
        bool RemoveVertexShaderFromPool(std::shared_ptr<std::string> pName);

        std::shared_ptr<DrawingPixelShader> GetPixelShader(std::shared_ptr<std::string> pName);
        bool AddPixelShaderToPool(std::shared_ptr<std::string> pName, std::shared_ptr<DrawingPixelShader> pPixelShader);
        bool RemovePixelShaderFromPool(std::shared_ptr<std::string> pName);

    private:
        void ClearEffectTable();
        void ClearVertexShaderTable();
        void ClearPixelShaderTable();

        template<typename TypeN>
        bool Load(std::unordered_map<std::shared_ptr<std::string>, std::shared_ptr<TypeN>>& table, std::shared_ptr<std::string> pName, std::shared_ptr<TypeN>& pRes);

        template<typename TypeN, typename DescN>
        bool LoadFromBuffer(std::unordered_map<std::shared_ptr<std::string>, std::shared_ptr<TypeN>>& table, const DescN& desc, std::shared_ptr<TypeN>& pRes);
        template<typename TypeN, typename DescN>
        bool LoadFromString(std::unordered_map<std::shared_ptr<std::string>, std::shared_ptr<TypeN>>& table, const DescN& desc, std::shared_ptr<TypeN>& pRes);
        template<typename TypeN, typename DescN>
        bool LoadFromFile(std::unordered_map<std::shared_ptr<std::string>, std::shared_ptr<TypeN>>& table, const DescN& desc, std::shared_ptr<TypeN>& pRes);

        template<typename TypeN, typename DescN>
        bool DoCreateFromBuffer(const void* pData, uint32_t length, const DescN& desc, std::shared_ptr<TypeN>& pRes);
        template<typename TypeN, typename DescN>
        bool DoCreateFromString(std::string& str, const DescN& desc, std::shared_ptr<TypeN>& pRes);
        template<typename TypeN, typename DescN>
        bool DoCreateFromFile(const DescN& desc, std::shared_ptr<TypeN>& pRes);


    private:
        typedef std::unordered_map<std::shared_ptr<std::string>, std::shared_ptr<DrawingEffect>> EffectTableType;
        typedef std::unordered_map<std::shared_ptr<std::string>, std::shared_ptr<DrawingVertexShader>> VertexShaderTableType;
        typedef std::unordered_map<std::shared_ptr<std::string>, std::shared_ptr<DrawingPixelShader>> PixelShaderTableType;

        std::shared_ptr<DrawingDevice> m_pDevice;

        EffectTableType m_effectTable;
        VertexShaderTableType m_vertexShaderTable;
        PixelShaderTableType m_pixelShaderTable;
    };
}