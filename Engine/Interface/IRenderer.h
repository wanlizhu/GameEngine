#pragma once

#include "IRuntimeModule.h"
#include "Macros.h"
#include "DrawingResourceTable.h"
#include "DrawingDevice.h"
#include "IMesh.h"

namespace Engine
{
    class IRenderer : public IRuntimeModule
    {
    public:
        virtual ~IRenderer() {};

        virtual void Initialize() override = 0;
        virtual void Shutdown() override = 0;

        virtual void Tick(float elapsedTime) override = 0;

        virtual void DefineResources(DrawingResourceTable& resTable) = 0;
        virtual void SetupStages() = 0;
        virtual void SetupBuffers(DrawingResourceTable& resTable) = 0;
        virtual void Cleanup() = 0;

        virtual void BeginDrawPass() = 0;
        virtual void EndDrawPass() = 0;

        virtual void FlushData() = 0;
        virtual void ResetData() = 0;
        virtual void UpdatePrimitive(DrawingResourceTable& resTable) = 0;
        virtual void Draw(DrawingResourceTable& resTable) = 0;

        virtual void AttachDevice(const std::shared_ptr<DrawingDevice>& pDevice, const std::shared_ptr<DrawingContext>& pContext) = 0;
        virtual void AttachMesh(std::shared_ptr<IMesh> pMesh) = 0;

        virtual void MapResources(DrawingResourceTable& resTable) = 0;
        virtual void CreateDataResources(DrawingResourceTable& resTable) = 0;
    };

#define FuncResourceName(name)                              \
    static inline std::shared_ptr<std::string> name()       \
    {                                                       \
        static auto str = strPtr(#name);                    \
        return str;                                         \
    }

}