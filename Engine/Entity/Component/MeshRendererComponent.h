#pragma once

#include "Component.h"
#include "IRenderer.h"

namespace Engine
{
    class MeshRendererComponent : public ComponentBase<MeshRendererComponent>
    {
    public:
        MeshRendererComponent() : ComponentBase<MeshRendererComponent>() {}
        virtual ~MeshRendererComponent() = default;

        std::shared_ptr<IRenderer> GetRenderer();
        void SetRenderer(std::shared_ptr<IRenderer> pRenderer);

    private:
        std::shared_ptr<IRenderer> m_pRenderer;
    };
}