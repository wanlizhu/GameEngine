#pragma once

#include "IECSWorld.h"
#include "Global.h"

namespace Engine
{
    template<typename T>
    class ComponentBase : public IComponent
    {
    public:
        inline ComponentBase()
        {
            if (m_bInit)
                return;
            m_createFunc = createFunc<T>;
            m_destroyFunc = destroyFunc<T>;
            m_size = sizeof(T);
            m_compID = RegisterComponent(ComponentBase<T>::m_createFunc, ComponentBase<T>::m_destroyFunc, ComponentBase<T>::m_size);

            m_bInit = true;
        }

        inline virtual ~ComponentBase()
        {
        }

        static bool m_bInit;
        static CreateCompFunc m_createFunc;
        static DestroyCompFunc m_destroyFunc;
        static uint32_t m_size;
        static uint32_t m_size2;
        static CompID m_compID;
    };

    template<typename T>
    auto createFunc = [](IComponent* pComponent, std::shared_ptr<IEntity> pEntity, std::vector<uint8_t>& memory) -> CompIndex
    {
        CompIndex index = (CompIndex)memory.size();
        memory.resize(index + T::m_size);
        T* pComp = new(&memory[index])T(*(T*)pComponent);
        pComp->m_pEntity = pEntity;
        return index;
    };

    template<typename T>
    auto destroyFunc = [](IComponent* pComponent) -> void
    {
        T* pComp = (T*)pComponent;
        pComp->~T();
    };

    template<typename T>
    bool ComponentBase<T>::m_bInit = false;

    template<typename T>
    CreateCompFunc ComponentBase<T>::m_createFunc;

    template<typename T>
    DestroyCompFunc ComponentBase<T>::m_destroyFunc;

    template<typename T>
    uint32_t ComponentBase<T>::m_size;

    template<typename T>
    CompID ComponentBase<T>::m_compID;
}
