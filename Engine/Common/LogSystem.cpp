#include <memory>
#include <iostream>

#include "ILogSystem.h"
#include "IEventSystem.h"
#include "LogSystem.h"

using namespace Engine;

void LogSystem::Initialize()
{
    m_logSystemEventFunc = std::bind(&LogSystem::OutputLogSystemStream, this, std::placeholders::_1);
    LISTEN_EVENT(eEv_System_App, m_logSystemEventFunc);

    m_LogInputKeyCharFunc = std::bind(&LogSystem::OutputLogInputStream<eEv_Input_KeyChar>, this, std::placeholders::_1);
    LISTEN_EVENT(eEv_Input_KeyChar, m_LogInputKeyCharFunc);

    m_LogInputKeyDownFunc = std::bind(&LogSystem::OutputLogInputStream<eEv_Input_KeyDown>, this, std::placeholders::_1);
    LISTEN_EVENT(eEv_Input_KeyDown, m_LogInputKeyDownFunc);

    m_LogInputKeyUpFunc = std::bind(&LogSystem::OutputLogInputStream<eEv_Input_KeyUp>, this, std::placeholders::_1);
    LISTEN_EVENT(eEv_Input_KeyUp, m_LogInputKeyUpFunc);
}

void LogSystem::Shutdown()
{
    DISPATCH_EVENT(eEv_System_App, m_logSystemEventFunc);
    DISPATCH_EVENT(eEv_Input_KeyChar, m_LogInputKeyCharFunc);
    DISPATCH_EVENT(eEv_Input_KeyDown, m_LogInputKeyDownFunc);
    DISPATCH_EVENT(eEv_Input_KeyUp, m_LogInputKeyUpFunc);
}

void LogSystem::Tick()
{
}

void LogSystem::FlushEntity(std::shared_ptr<IEntity> pEntity)
{
}

void LogSystem::OutputLogSystemStream(LogSystemEventPtr data) const
{
    std::cout << data->GetMsg() << std::endl;
}