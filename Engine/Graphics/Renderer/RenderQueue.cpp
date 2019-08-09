#include "IRenderable.h"
#include "RenderQueue.h"

using namespace Engine;

void RenderQueue::Reset()
{
    for (auto &queue : m_queues)
        queue.clear();
}

void RenderQueue::Dispatch(ERenderQueueType type, std::function<void(const RenderQueueItem&)> drawFunc)
{
    auto& queue = m_queues[(int)type];
    for (auto& item : queue)
        drawFunc(item);
}