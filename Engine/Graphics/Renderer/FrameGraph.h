#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>

#include "Vector.h"
#include "DrawingDevice.h"
#include "DrawingPass.h"

namespace Engine
{
    enum FrameGraphFlagBits
    {
        GraphicsBit = 1 << 0,
        ComputeBit = 1 << 1,
        AsyncGraphicsBit = 1 << 2,
        AsyncComputeBit = 1 << 3,
    };

    class FrameGraph;
    class FrameGraphNode
    {
    public:
        FrameGraphNode(FrameGraph& frameGraph, uint32_t index, std::shared_ptr<DrawingPass> pPass, FrameGraphFlagBits bits);
        virtual ~FrameGraphNode();

        bool RunInitializeFunc() const;
        bool IfNeedExecuteFunc() const;
        void RunExecuteFunc() const;

        void SetInitializeFunc(std::function<bool ()> func);
        void SetNeedExecuteFunc(std::function<bool ()> func);
        void SetExecuteFunc(std::function<void ()> func);

        void SetFrameGraph(const FrameGraph& frameGraph);
        FrameGraph GetFrameGraph() const;
        uint32_t GetIndex() const;
        FrameGraphFlagBits GetFrameGraphFlagBit() const;
        std::shared_ptr<DrawingPass> GetDrawingPass() const;

    private:
        FrameGraph& m_frameGraph;

        uint32_t m_index;
        FrameGraphFlagBits m_bits;
        std::shared_ptr<DrawingPass> m_pPass;

        std::function<bool ()> m_initializeFunc;
        std::function<bool ()> m_needExecuteFunc;
        std::function<void ()> m_executeFunc;
    };

    class FrameGraph
    {
    public:
        FrameGraph();
        FrameGraphNode& AddPass(std::shared_ptr<DrawingPass> pPass, FrameGraphFlagBits bits);
        bool InitializePasses();
        void EnqueuePasses(DrawingContext& context);
        void FetchResources(DrawingResourceTable& resTable);

    private:
        std::vector<std::shared_ptr<FrameGraphNode>> m_nodes;
        std::unordered_map<std::shared_ptr<std::string>, uint32_t> m_passIndex;
        
    };
}