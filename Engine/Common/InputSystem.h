#pragma once

#include <queue>
#include <stdint.h>

#include "IInputSystem.h"

#include "ECSSystem.h"

namespace Engine
{
    class InputSystem : public IInputSystem, public ECSSystemBase<>
    {
    public:
        InputSystem() {}
        virtual ~InputSystem() {}

        void Initialize() override;
        void Shutdown() override;
        void Tick(float elapsedTime) override;

        void FlushEntity(IEntity* pEntity) override;

        void DispatchInputEvent(EInputEvent event, InputMsg msg) override;

    private:
        struct InputMsgInfo
        {
            InputMsgInfo(EInputEvent _event, InputMsg _msg) : event(_event), msg(_msg){}
            EInputEvent event;
            InputMsg msg;
        };
        std::queue<InputMsgInfo> m_inputQueue;
    };
}