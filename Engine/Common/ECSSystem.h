#pragma once

#include "IECSWorld.h"
#include "Algorithm.h"

namespace Engine
{
    template<typename... ReqComps>
    class ECSSystemBase : public IECSSystem
    {
    public:
        ECSSystemBase();
        virtual ~ECSSystemBase() = default;
    };

    template<typename ...ReqComps>
    inline ECSSystemBase<ReqComps...>::ECSSystemBase()
    {
        m_comps = { ReqComps::m_compID... };
        m_compBitset = 0;
        for (CompID id : m_comps)
            AddBit<CompBitset>(m_compBitset, id);
    }

}