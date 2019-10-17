#pragma once

#include <string>
#include <vector>

#include <glTF2.hpp>
#include <Global.h>

#include <IMesh.h>
#include <IMaterial.h>

namespace Engine
{
    class GLTF2Loader
    {
    public:
        GLTF2Loader();
        virtual ~GLTF2Loader();

        void Load(std::string filename);
        void ApplyToWorld();

    protected:
        void LoadMaterials();
        void LoadMeshes();

    private:
        gltf2::Asset m_asset;

        std::vector<IMesh*> m_pMeshes;
        std::vector<IMaterial*> m_pMaterials;
    };
}