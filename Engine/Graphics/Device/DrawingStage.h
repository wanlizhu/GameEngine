#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

namespace Engine
{
    class DrawingPass;
    class DrawingResourceTable;
    class DrawingContext;
    class DrawingStage
    {
    public:
        DrawingStage(std::shared_ptr<std::string> pName);
        ~DrawingStage();

        std::shared_ptr<std::string> GetName();

        void AppendDrawingPass(std::shared_ptr<DrawingPass> pPass);
        void RemoveDrawingPass(std::shared_ptr<DrawingPass> pPass);
        void ClearDrawingPassList();

        std::shared_ptr<DrawingPass> GetDrawingPass(const std::shared_ptr<std::string> pName);

        void FetchResources(DrawingResourceTable& resTable);

        void Flush(DrawingContext& dc);

    typedef std::vector<std::shared_ptr<DrawingPass>> DrawingPassListType; 

    private:
        std::shared_ptr<std::string> m_pName;
        DrawingPassListType m_passList;
    };

    class DrawingStageTable
    {
    public:
        DrawingStageTable();
        ~DrawingStageTable();

        void AddDrawingStage(std::shared_ptr<std::string> pName, std::shared_ptr<DrawingStage> pStage);
        void ClearDrawingStageTable();

        std::shared_ptr<DrawingStage> GetDrawingStage(const std::shared_ptr<std::string> pName);
        std::shared_ptr<DrawingPass> GetDrawingPass(const std::shared_ptr<std::string> pStageName, const std::shared_ptr<std::string> pPassName);

        void FetchResources(DrawingResourceTable& resTable);
        void FlushStages(DrawingContext& dc);

    typedef std::unordered_map<std::shared_ptr<std::string>, std::shared_ptr<DrawingStage>> DrawingStageTableType;

    private:
        DrawingStageTableType m_stageTable;
    };
}