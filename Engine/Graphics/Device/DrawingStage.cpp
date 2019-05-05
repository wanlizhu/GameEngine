#include <algorithm>

#include "DrawingDevice.h"
#include "DrawingPass.h"
#include "DrawingStage.h"

using namespace Engine;

DrawingStage::DrawingStage(std::shared_ptr<std::string> pName) : m_pName(pName)
{
}

DrawingStage::~DrawingStage()
{
    ClearDrawingPassList();
    m_pName = nullptr;
}

std::shared_ptr<std::string> DrawingStage::GetName()
{
    return m_pName;
}

void DrawingStage::AppendDrawingPass(std::shared_ptr<DrawingPass> pPass)
{
    m_passList.emplace_back(pPass);
}

void DrawingStage::RemoveDrawingPass(std::shared_ptr<DrawingPass> pPass)
{
    const auto it = std::find(m_passList.cbegin(), m_passList.cend(), pPass);
    if (it != m_passList.cend())
        m_passList.erase(it);
}

void DrawingStage::ClearDrawingPassList()
{
    for (auto& pass : m_passList)
        pass = nullptr;

    m_passList.clear();
}

std::shared_ptr<DrawingPass> DrawingStage::GetDrawingPass(std::shared_ptr<std::string> pName)
{
    const auto it = std::find_if(m_passList.cbegin(), m_passList.cend(), [&pName](const std::shared_ptr<DrawingPass> pass)
    {
        return pass != nullptr && pass->GetName() == pName;
    });

    if (it == m_passList.cend())
        return nullptr;

    return *it;
}

void DrawingStage::FetchResources(DrawingResourceTable& resTable)
{
    std::for_each(m_passList.begin(), m_passList.end(), [&resTable](std::shared_ptr<DrawingPass> pass)
    {
        if (pass != nullptr)
        {
            pass->ClearResources();
            pass->FetchResources(resTable);
        }
    });
}

void DrawingStage::Flush(DrawingContext& dc)
{
    std::for_each(m_passList.begin(), m_passList.end(), [&dc](std::shared_ptr<DrawingPass> pass)
    {
        if (pass != nullptr)
            pass->Flush(dc);
    });
}

DrawingStageTable::DrawingStageTable()
{
}

DrawingStageTable::~DrawingStageTable()
{
    ClearDrawingStageTable();
}

void DrawingStageTable::AddDrawingStage(std::shared_ptr<std::string> pName, std::shared_ptr<DrawingStage> pStage)
{
    const auto it = m_stageTable.find(pName);
    if (it != m_stageTable.cend())
        return;

    m_stageTable.emplace(pName, pStage);
}

void DrawingStageTable::ClearDrawingStageTable()
{
}

std::shared_ptr<DrawingStage> DrawingStageTable::GetDrawingStage(const std::shared_ptr<std::string> pName)
{
    const auto it = m_stageTable.find(pName);
    if (it == m_stageTable.cend())
        return nullptr;

    return it->second;
}

std::shared_ptr<DrawingPass> DrawingStageTable::GetDrawingPass(const std::shared_ptr<std::string> pStageName, const std::shared_ptr<std::string> pPassName)
{
    auto stage = GetDrawingStage(pStageName);

    if (stage == nullptr)
        return nullptr;

    return stage->GetDrawingPass(pPassName);
}

void DrawingStageTable::FetchResources(DrawingResourceTable& resTable)
{
    std::for_each(m_stageTable.begin(), m_stageTable.end(), [&resTable](DrawingStageTableType::value_type& elem)
    {
        if (auto& stage = elem.second)
            stage->FetchResources(resTable);
    });
}

void DrawingStageTable::FlushStages(DrawingContext& dc)
{
    std::for_each(m_stageTable.begin(), m_stageTable.end(), [&dc](DrawingStageTableType::value_type& elem)
    {
        if (auto& stage = elem.second)
            stage->Flush(dc);
    });
}