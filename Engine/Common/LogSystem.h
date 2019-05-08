#pragma once

#include <functional>

#include "ILogSystem.h"
#include "IInputSystem.h"
#include "Global.h"
#include "BaseApplication.h"

#include "ECSSystem.h"

namespace Engine
{
    class LogSystem : public ILogSystem, public ECSSystemBase<>
    {
    public:
        LogSystem() {}
        virtual ~LogSystem() {}

        void Initialize() override;
        void Shutdown() override;

        void Tick(float elapsedTime) override;

        void FlushEntity(std::shared_ptr<IEntity> pEntity) override;

    private:
        #define DEF_InputEventType(event, enum)                                         \
            typedef Event<InputMsg, EInputEvent, enum> event;                           \
            typedef std::shared_ptr<event> event##Ptr;                                  \
            typedef std::function<void(event##Ptr)> event##Func;                        \
            event##Func m_##event##Func

        DEF_InputEventType(LogInputKeyChar, eEv_Input_KeyChar);
        DEF_InputEventType(LogInputKeyDown, eEv_Input_KeyDown);
        DEF_InputEventType(LogInputKeyUp, eEv_Input_KeyUp);
        DEF_InputEventType(LogInputKeyDbClick, eEv_Input_KeyDbClick);

        DEF_InputEventType(LogInputControlMove, eEv_Input_ControlMove);
        DEF_InputEventType(LogInputControlLeave, eEv_Input_ControlLeave);
        DEF_InputEventType(LogInputControlWheel, eEv_Input_ControlWheel);
        DEF_InputEventType(LogInputControlHover, eEv_Input_ControlHover);

        typedef Event<std::string, ESystemEvent, eEv_System_App> LogSystemEvent;
        typedef std::shared_ptr<LogSystemEvent> LogSystemEventPtr;
        typedef std::function<void(LogSystemEventPtr)> LogSystemEventFunc;
    
        void OutputLogSystemStream(LogSystemEventPtr data) const;

        template<EInputEvent e>
        inline void OutputLogInputStream(std::shared_ptr<Event<InputMsg, EInputEvent, e>> data) const {};

    private:
        DECLARE_LISTENER();
        LogSystemEventFunc m_logSystemEventFunc;
    };

    template<>
    inline void LogSystem::OutputLogInputStream<eEv_Input_KeyChar>(LogInputKeyCharPtr data) const
    {
        auto c = static_cast<char>(data->GetMsg().Param1());
        switch (c)
        {
            case 'f':
                std::cout << "Avg FPS: " <<
                            gpGlobal->GetFPSCounter().GetFPSAvgSec() << ", " <<
                            "Cur FPS: " <<
                            gpGlobal->GetFPSCounter().GetFPSCurrent() << std::endl;
            break;
        }
    };

    template<>
    inline void LogSystem::OutputLogInputStream<eEv_Input_KeyDown>(LogInputKeyDownPtr data) const
    {
    };

    template<>
    inline void LogSystem::OutputLogInputStream<eEv_Input_KeyUp>(LogInputKeyUpPtr data) const
    {
    };
}