#include <glTF2.hpp>
#include <fstream>
#include <iterator>

#include "GLTF2Mesh.h"

using namespace Engine;

GLTF2Mesh::GLTF2Mesh(std::string filename) : m_fileName(filename)
{
    gltf2::Asset asset = gltf2::load(m_fileName);

    auto primitives = asset.meshes[0].primitives[0];
    auto attributes = primitives.attributes;

    auto pData = asset.buffers[0].data;
    auto size = asset.buffers[0].byteLength;

    std::for_each(attributes.begin(), attributes.end(), [&](gltf2::Attributes::value_type& aElem){
        auto str = aElem.first;
        auto num = aElem.second;

        Attribute::ESemanticType type;
        if (str == "POSITION")
            type = Attribute::ESemanticType::Position;
        else if (str == "NORMAL")
            type = Attribute::ESemanticType::Normal;
        else
            return;

        int bufferView = asset.accessors[num].bufferView;
        int count = asset.accessors[num].count;

        int offset = asset.bufferViews[bufferView].byteOffset;
        int subsize = asset.bufferViews[bufferView].byteLength;

        char* data = new char[subsize];
        memcpy(data, pData + offset, subsize);

        AttachVertexData(data, subsize, count, type, Attribute::EFormatType::Float3, str);
    });

    auto indexNum = primitives.indices;

    int bufferView = asset.accessors[indexNum].bufferView;
    int count = asset.accessors[indexNum].count;

    int offset = asset.bufferViews[bufferView].byteOffset;
    int subsize = asset.bufferViews[bufferView].byteLength;

    char* data = new char[subsize];
    memcpy(data, pData + offset, subsize);

    AttachIndexData(data, subsize, count);
}

GLTF2Mesh::~GLTF2Mesh()
{
}