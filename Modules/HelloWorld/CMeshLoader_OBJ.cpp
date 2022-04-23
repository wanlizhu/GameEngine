#include "CMeshLoader_OBJ.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

BoundingBox computeBoundingBox(tinyobj::mesh_t const& mesh,
                               tinyobj::attrib_t const& attrib)
{
    BoundingBox box;
    
    for (int i = 0; i < mesh.indices.size(); i++)
    {
        int index = mesh.indices[i].vertex_index;
        glm::vec3 pos;
        pos.x = attrib.vertices[index * 3 + 0];
        pos.y = attrib.vertices[index * 3 + 1];
        pos.z = attrib.vertices[index * 3 + 2];
        
        box.expand(pos);
    }
    
    return box;
}

bool inspectMeshFaces(tinyobj::mesh_t const& mesh,
                      std::unordered_set<int>& primitiveTypes,
                      std::unordered_set<int>& materialIDs)
{
    bool valid = false;
    
    for (int i = 0; i < mesh.num_face_vertices.size(); i++)
    {
        primitiveTypes.insert(mesh.num_face_vertices[i]);
        valid = mesh.num_face_vertices[i] > 0;
        
        if (i < mesh.material_ids.size()) {
            materialIDs.insert(mesh.material_ids[i]);
        }
    }
    
    return valid;
}

bool submeshWithPrimitiveTypeAndMaterialID(tinyobj::mesh_t const& mesh,
                                           int primitiveType,
                                           int materialID,
                                           SubmeshDesc& submesh,
                                           std::vector<int>& cache)
{
    int indexOffset = (int)cache.size();
    int indexCount = 0;
    int indexAgain = 0;
    
    for (int i = 0; i < mesh.num_face_vertices.size(); i++)
    {
        if (mesh.num_face_vertices[i] == primitiveType &&
            mesh.material_ids[i] == materialID)
        {
            for (int j = 0; j < primitiveType; j++) {
                cache.push_back(mesh.indices[indexAgain + j].vertex_index);
            }
            indexCount += primitiveType;
        }
        
        indexAgain += mesh.num_face_vertices[i];
    }
    
    if (indexCount > 0)
    {
        submesh.vertexCountPerFace = primitiveType;
        submesh.indexTypeUint32 = true;
        submesh.indexCount = indexCount;
        submesh.indices = Blob(cache.data() + indexOffset, indexCount * sizeof(int));
    }
    
    return indexCount > 0;
}

std::vector<Mesh::ARC> CMeshLoader_OBJ::load(const fs::path& path, IDevice* device)
{
    assert(sizeof(tinyobj::real_t) == sizeof(float));
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string folder = path.parent_path().string();
    std::string error;
    
    if (!tinyobj::LoadObj(&attrib,
                          &shapes,
                          &materials,
                          &error,
                          path.c_str(),
                          folder.c_str(),
                          true))
    {
        printf("Failed to load mesh from .obj file.\n%s\n", error.c_str());
        return {};
    }
    
    if (attrib.vertices.empty())
    {
        printf("Mesh file is empty: \n%s\n", path.string().c_str());
        return {};
    }
    
    assert(attrib.vertices.size() == attrib.normals.size() || attrib.normals.empty());
    assert(attrib.vertices.size() == attrib.texcoords.size() || attrib.texcoords.empty());
    std::vector<Mesh::ARC> meshes;
    
    for (int i = 0; i < shapes.size(); i++)
    {
        MeshDesc desc;
        desc.name = shapes[i].name;
        desc.positions = Blob(attrib.vertices.data(), attrib.vertices.size() * sizeof(float));
        desc.normals = Blob(attrib.normals.data(), attrib.normals.size() * sizeof(float));
        desc.tangents = Blob(nullptr, 0);
        desc.texcoords = Blob(attrib.texcoords.data(), attrib.texcoords.size() * sizeof(float));
        desc.boundingBox = computeBoundingBox(shapes[i].mesh, attrib);
        desc.submeshes.clear();
        
        assert(shapes[i].mesh.num_face_vertices.size() == shapes[i].mesh.material_ids.size());
        std::unordered_set<int> primitiveTypes;
        std::unordered_set<int> materialIDs = { -1 };
        std::vector<int> cache;
        SubmeshDesc submesh;
        
        if (inspectMeshFaces(shapes[i].mesh, primitiveTypes, materialIDs))
        {
            for (int primitiveType : primitiveTypes)
            {
                for (int materialID : materialIDs)
                {
                    if (submeshWithPrimitiveTypeAndMaterialID(shapes[i].mesh,
                                                              primitiveType,
                                                              materialID,
                                                              submesh,
                                                              cache))
                    {
                        desc.submeshes.push_back(submesh);
                        submesh = SubmeshDesc();
                    }
                }
            }
        }
        
        Mesh::ARC mesh = std::make_shared<Mesh>();
        mesh->initWithMeshDesc(desc, device);
        meshes.push_back(mesh);
    }
    
    return meshes;
}
