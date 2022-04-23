#pragma once

#include "Utils.h"
#include "IDevice.h"
#include "Material.h"

class IResourceTable;
class Mesh;

struct SubmeshDesc
{
    Blob indices;
    int  indexCount = 0;
    bool indexTypeUint32 = true;
    int  vertexCountPerFace = 0;
};

struct MeshDesc
{
    std::string name;
    Blob positions;
    Blob normals;
    Blob tangents;
    Blob texcoords;
    
    std::vector<SubmeshDesc> submeshes;
    BoundingBox boundingBox;
};

struct Submesh
{
    using ARC = std::shared_ptr<Submesh>;
    Mesh* mesh = nullptr;
    std::string materialName;
    
    IDeviceObject::ARC indexBuffer = nullptr;
    int  indexCount = 0;
    int  indexOffset = 0;
    bool indexTypeBits = 32;
    const char* primitiveType = "T"; // "P"oint, "L"ine, "T"riangle
};

class Mesh
{
public:
    using ARC = std::shared_ptr<Mesh>;
    
    Mesh() = default;
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;
    
    virtual void initWithMeshDesc(const MeshDesc& desc, IDevice* device);
    virtual void draw(IDevice* device, IResourceTable* resourceTable);
    virtual void expandBoundingBox(IResourceTable* resourceTable, BoundingBox* boxP);
    
private:
    std::string _name = "NoName";
    BoundingBox _boundingBox;
    glm::mat4 _modelMatrix;
    
    std::vector<Submesh::ARC> _submeshes;
    IDeviceObject::ARC _vertexArray = nullptr;
    IDeviceObject::ARC _uniforms = nullptr;
};
