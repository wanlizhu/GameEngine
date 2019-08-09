#pragma once

#include "Component.h"
#include "IRenderer.h"

namespace Engine
{
    class MeshRendererComponent : public ComponentBase<MeshRendererComponent>
    {
    public:
        MeshRendererComponent();
        virtual ~MeshRendererComponent();
    };
}