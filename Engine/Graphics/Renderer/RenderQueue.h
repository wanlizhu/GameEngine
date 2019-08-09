#pragma once

#include <vector>
#include <functional>

#include "Traits.h"
#include "TransformComponent.h"

namespace Engine
{
    class IRenderable;
    enum class ERenderQueueType : unsigned
    {
        Background = 0,
        Opaque,
        AlphaTest,
        Transparent,
        Overlay,
        Count
    };

    struct RenderQueueItem
    {
        const IRenderable* pRenderable;
        const TransformComponent* pTransformComp;
    };

    typedef std::vector<RenderQueueItem> RenderQueueItemListType;

    class RenderQueue
    {
    public:
        RenderQueue() = default;
        virtual ~RenderQueue() = default;

        void Reset();
        void Dispatch(ERenderQueueType type, std::function<void(const RenderQueueItem&)> drawFunc);

        template <typename T>
        const T* Add(ERenderQueueType type, const RenderQueueItem& item)
        {
            m_queues[enum_cast(type)].push_back(item);
            return static_cast<const T*>(item.pRenderable);
        }

    private:
        RenderQueueItemListType m_queues[ERenderQueueType::Count];
    };
}