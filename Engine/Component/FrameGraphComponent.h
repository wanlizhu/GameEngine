#pragma once

#include "Component.h"
#include "FrameGraph.h"

namespace Engine
{
    class FrameGraphComponent : public ComponentBase<FrameGraphComponent>
    {
    public:
        FrameGraphComponent() : ComponentBase<FrameGraphComponent>() {}
        virtual ~FrameGraphComponent() = default;

        std::shared_ptr<FrameGraph> GetFrameGraph() const;
        void SetFrameGraph(std::shared_ptr<FrameGraph> pFrameGraph);

    private:
        std::shared_ptr<FrameGraph> m_pFrameGraph = nullptr;
    };
}