#pragma once

#include <string>
#include <memory>

#include "Mesh.h"

namespace Engine
{
    class GLTF2Mesh : public Mesh
    {
    public:
        GLTF2Mesh(std::string filename);
        virtual ~GLTF2Mesh();

    private:
        std::string m_fileName;
    };
}