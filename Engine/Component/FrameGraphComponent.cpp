#include "FrameGraphComponent.h"

using namespace Engine;

std::shared_ptr<FrameGraph> FrameGraphComponent::GetFrameGraph() const
{
    return m_pFrameGraph;
}

void FrameGraphComponent::SetFrameGraph(std::shared_ptr<FrameGraph> pFrameGraph)
{
    m_pFrameGraph = pFrameGraph;
}