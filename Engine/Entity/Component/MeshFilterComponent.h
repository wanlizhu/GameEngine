#pragma once

#include "Component.h"
#include "IMesh.h"

namespace Engine
{
    class MeshFilterComponent : public ComponentBase<MeshFilterComponent>
    {
    public:
        MeshFilterComponent() : ComponentBase<MeshFilterComponent>() {}
        virtual ~MeshFilterComponent() = default;

        std::shared_ptr<IMesh> GetMesh();
        void SetMesh(std::shared_ptr<IMesh> pMesh);

    private:
        std::shared_ptr<IMesh> m_pMesh;
    };
}