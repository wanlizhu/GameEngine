#pragma once

#include <memory>
#include <vector>

#include "Component.h"
#include "IMaterial.h"

namespace Engine
{
    class MeshRendererComponent : public ComponentBase<MeshRendererComponent>
    {
    public:
        MeshRendererComponent();
        virtual ~MeshRendererComponent();

        uint32_t GetMaterialSize() const;
        void SetMaterialSize(uint32_t size);

        std::shared_ptr<IMaterial> GetMaterial(uint32_t index) const;
        void SetMaterial(std::shared_ptr<IMaterial> pMaterial, uint32_t index = 0);

    private:
        uint32_t m_materialSize;
        std::vector<std::shared_ptr<IMaterial>> m_pMaterialList;
    };
}