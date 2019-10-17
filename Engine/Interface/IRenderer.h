#pragma once

#include <functional>

#include "Macros.h"
#include "DrawingResourceTable.h"
#include "DrawingDevice.h"
#include "FrameGraph.h"
#include "IRenderable.h"
#include "IMesh.h"

namespace Engine
{
    class IRenderer
    {
    public:
        virtual ~IRenderer() {};

        virtual void DefineResources(DrawingResourceTable& resTable) = 0;
        virtual void SetupBuffers(DrawingResourceTable& resTable) = 0;

        virtual void AddRenderables(RenderQueueItemListType renderables) = 0;

        virtual void Clear(DrawingResourceTable& resTable, std::shared_ptr<DrawingPass> pPass) = 0;
        virtual void Render(DrawingResourceTable& resTable, std::shared_ptr<DrawingPass> pPass) = 0;
        virtual void RenderRect(DrawingResourceTable& resTable, std::shared_ptr<DrawingPass> pPass) = 0;
        virtual void CopyRect(DrawingResourceTable& resTable, std::shared_ptr<std::string> pSrcName, std::shared_ptr<std::string> pDstName, const int2& dstOrigin) = 0;

        virtual void AttachDevice(const std::shared_ptr<DrawingDevice>& pDevice, const std::shared_ptr<DrawingContext>& pContext) = 0;
        virtual void AttachMesh(const IMesh* pMesh) = 0;

        virtual void BuildPass() = 0;
        virtual std::shared_ptr<DrawingPass> GetPass(std::shared_ptr<std::string> pName) = 0;
        virtual void CreateDataResources(DrawingResourceTable& resTable) = 0;
    };

#define FuncResourceName(name)                              \
    static inline std::shared_ptr<std::string> name()       \
    {                                                       \
        static auto str = strPtr(#name);                    \
        return str;                                         \
    }

}