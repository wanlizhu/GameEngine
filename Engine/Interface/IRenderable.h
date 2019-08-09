#pragma once

#include <memory>
#include <vector>
#include "RenderQueue.h"

namespace Engine
{
    class IRenderable : public std::enable_shared_from_this<IRenderable>
    {
    public:
        virtual ~IRenderable() = default;

        virtual void GetRenderable(RenderQueue &queue, const TransformComponent* pTransformComp) const = 0;
    };
}