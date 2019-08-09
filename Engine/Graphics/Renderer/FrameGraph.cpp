#include "FrameGraph.h"

using namespace Engine;

FrameGraphNode::FrameGraphNode(FrameGraph& frameGraph, uint32_t index, std::shared_ptr<DrawingPass> pPass, FrameGraphFlagBits bits) :
    m_frameGraph(frameGraph), m_index(index), m_pPass(pPass), m_bits(bits)
{
}

FrameGraphNode::~FrameGraphNode()
{
}

bool FrameGraphNode::RunInitializeFunc() const
{
    if (m_initializeFunc)
        return m_initializeFunc();

    return false;
}

bool FrameGraphNode::IfNeedExecuteFunc() const
{
    if (m_needExecuteFunc)
        return m_needExecuteFunc();

    return true;
}

void FrameGraphNode::RunExecuteFunc() const
{
    if (m_executeFunc)
        m_executeFunc();
}

void FrameGraphNode::SetInitializeFunc(std::function<bool ()> func)
{
    m_initializeFunc = std::move(func);
}

void FrameGraphNode::SetNeedExecuteFunc(std::function<bool ()> func)
{
    m_needExecuteFunc = std::move(func);
}

void FrameGraphNode::SetExecuteFunc(std::function<void ()> func)
{
    m_executeFunc = std::move(func);
}

void FrameGraphNode::SetFrameGraph(const FrameGraph& frameGraph)
{
    m_frameGraph = frameGraph;
}

FrameGraph FrameGraphNode::GetFrameGraph() const
{
    return m_frameGraph;
}

uint32_t FrameGraphNode::GetIndex() const
{
    return m_index;
}

FrameGraphFlagBits FrameGraphNode::GetFrameGraphFlagBit() const
{
    return m_bits;
}


std::shared_ptr<DrawingPass> FrameGraphNode::GetDrawingPass() const
{
    return m_pPass;
}

FrameGraph::FrameGraph()
{
}

FrameGraphNode& FrameGraph::AddPass(std::shared_ptr<DrawingPass> pPass, FrameGraphFlagBits bits)
{
    auto pName = pPass->GetName();
    auto itr = m_passIndex.find(pName);
    if (itr != m_passIndex.end())
    {
        return *m_nodes[itr->second];
    }
    else
    {
        uint32_t index = static_cast<uint32_t>(m_nodes.size());
        m_nodes.emplace_back(std::make_shared<FrameGraphNode>(*this, index, pPass, bits));
        m_passIndex[pName] = index;
        return *m_nodes.back();
    }
}

bool FrameGraph::InitializePasses()
{
    for (auto& pNode : m_nodes)
    {
        if (!pNode->RunInitializeFunc())
            return false;
    }

    return true;
}

void FrameGraph::EnqueuePasses(DrawingContext& context)
{
    for (auto& pNode : m_nodes)
    {
        if (!pNode->IfNeedExecuteFunc())
            continue;

        pNode->RunExecuteFunc();
    }
}

void FrameGraph::FetchResources(DrawingResourceTable& resTable)
{
    std::for_each(m_nodes.begin(), m_nodes.end(), [&resTable](std::shared_ptr<FrameGraphNode> pNode)
    {
        if (pNode != nullptr)
        {
            auto& pass = pNode->GetDrawingPass();
            pass->ClearResources();
            pass->FetchResources(resTable);
        }
    });
}