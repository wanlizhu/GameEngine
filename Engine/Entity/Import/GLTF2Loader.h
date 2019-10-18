#pragma once

#include <string>
#include <vector>

#include <glTF2.hpp>
#include <Global.h>

#include <Mesh.h>
#include <StandardMaterial.h>

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

        std::vector<Mesh*> m_pMeshes;
        std::vector<StandardMaterial*> m_pMaterials;
    };
}