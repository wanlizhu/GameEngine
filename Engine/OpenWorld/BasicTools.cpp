#include "BasicTools.h"
#include "IDevice.h"
#include "Shaders/ShaderConstants.h"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "tiny_gltf.h"

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

std::string getEnv(const char* name)
{
#if defined(_WIN32)
    if (uint32_t size = GetEnvironmentVariable(name, nullptr, 0))
    {
        std::vector<char> buffer(size);
        GetEnvironmentVariable(name.c_str(), buffer.data(), size);
        return std::string(buffer.data());
    }
#else
    if (const char* const result = getenv(name))
    {
        return std::string(result);
    }
#endif
    
    return "";
}

void setEnv(const char* name, const char* value)
{
#if defined(_WIN32)
    SetEnvironmentVariable(name, value) != 0;
#else
    setenv(name, value, true);
#endif
}

void split(const std::string& str,
           const char* seps,
           std::vector<std::string>* results)
{
    auto last = str.find_first_not_of(seps);
    auto pos = str.find_first_of(seps, last);
    
    while (pos != std::string::npos || last != std::string::npos)
    {
        results->push_back(str.substr(last, pos - last));
        last = str.find_first_not_of(seps, pos);
        pos = str.find_first_of(seps, last);
    }
}

std::string stringFormat(const char* format, ...)
{
    char buffer[256] = {};
    
    va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);
    
    return buffer;
}

#ifndef __APPLE__
std::string currentPath()
{
    return std::filesystem::current_path();
}
#endif

std::string resourcePathWithName(const std::string& name)
{
    namespace fs = std::filesystem;
    
    for (const auto& entry : fs::recursive_directory_iterator(currentPath()))
    {
        if (entry.path().filename() == name)
            return entry.path().string();
    }
    
    return "";
}

std::vector<std::string> resourcePathsWithType(const std::string& type)
{
    namespace fs = std::filesystem;
    std::vector<std::string> paths;
    
    for (const auto& entry : fs::recursive_directory_iterator(currentPath()))
    {
        if (entry.path().extension().string() == type)
            paths.push_back(entry.path().string());
    }
    
    return paths;
}

bool loadBinaryFile(const std::string& path, std::vector<uint8_t>* bytes)
{
    FILE* file = fopen(path.c_str(), "rb");
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);
    bytes->resize(size);
    
    size_t offset = 0;
    
    while (size > 0)
    {
        size_t num = fread(bytes->data() + offset, sizeof(uint8_t), size, file);
        size -= num;
        offset += num;
        
        if (num == 0) {
            break;
        }
    }
    
    return size == 0;
}

bool loadImageFile(const std::string& path,
                   std::vector<uint32_t>* pixels,
                   int* width,
                   int* height)
{
    assert(false);
    return true;
}

void loadMatrix(const double* m, glm::mat4* matrix)
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            (*matrix)[i][j] = m[4 * i + j];
        }
    }
}

DataType getElementType(int type, int componentType)
{
    switch (type)
    {
        case TINYGLTF_TYPE_SCALAR:
            if (componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE) return DATA_Uchar;
            if (componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) return DATA_Ushort;
            if (componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT) return DATA_Uint;
            if (componentType == TINYGLTF_COMPONENT_TYPE_FLOAT) return DATA_Float;
            break;
        case TINYGLTF_TYPE_VEC2: assert(componentType == TINYGLTF_COMPONENT_TYPE_FLOAT); return DATA_Float2;
        case TINYGLTF_TYPE_VEC3: assert(componentType == TINYGLTF_COMPONENT_TYPE_FLOAT); return DATA_Float3;
        case TINYGLTF_TYPE_VEC4: assert(componentType == TINYGLTF_COMPONENT_TYPE_FLOAT); return DATA_Float4;
        default: assert(false); return InvalidDataType;
    }
}

int getElementSizeInBytes(int type, int componentType)
{
    return tinygltf::GetNumComponentsInType(type) * tinygltf::GetComponentSizeInBytes(componentType);
}

std::shared_ptr<std::vector<uint8_t>> getData(const tinygltf::Model& model,
                                              const tinygltf::Accessor& accessor)
{
    auto data = std::make_shared<std::vector<uint8_t>>();
    data->resize(accessor.count * getElementSizeInBytes(accessor.type, accessor.componentType));
    
    const auto& bufferView = model.bufferViews[accessor.bufferView];
    const auto& buffer = model.buffers[bufferView.buffer];
    int offset = bufferView.byteOffset + accessor.byteOffset;
    int stride = accessor.ByteStride(model.bufferViews[accessor.bufferView]);
    int elementSizeInBytes = getElementSizeInBytes(accessor.type, accessor.componentType);
    
    const unsigned char* src = buffer.data.data() + offset;
    unsigned char* dst = data->data();
    
    for (int i = 0; i < accessor.count; i++)
    {
        memcpy(dst + i * elementSizeInBytes,
               src + i * stride,
               elementSizeInBytes);
    }
    
    return data;
}

PrimitiveType getPrimitiveType(int mode)
{
    switch (mode)
    {
        case TINYGLTF_MODE_POINTS: return PRIM_Points;
        case TINYGLTF_MODE_LINE: return PRIM_Lines;
        case TINYGLTF_MODE_TRIANGLES: return PRIM_Triangles;
        default: assert(false); return InvalidPrimitiveType;
    }
}

MaterialDesc getMaterialDesc(const tinygltf::Model& model,
                             const tinygltf::Primitive& primitive)
{
    if (primitive.material <= -1 || primitive.material >= model.materials.size())
        return {};
    
    const tinygltf::Material& material = model.materials[primitive.material];
    MaterialDesc desc;
    desc.name = material.name;
    desc.uniforms.flags = 0;
    desc.uniforms.baseColor = float3(1, 0, 0);
    desc.uniforms.metallic = 1.0;
    desc.uniforms.roughness = 0.0;
    desc.uniforms.emissive = float3(0, 0, 0);
    desc.uniforms.ao = 1.0;
    //TODO
    //assert(false);
    
    return desc;
}

ProgramDesc getProgramDesc(const tinygltf::Model& model)
{
    ProgramDesc desc;
    desc.name = "NoName";
    desc.library = "default";
    desc.vertexFunc = "VertexShaderDefault";
    desc.fragmentFunc = "FragmentShaderDefault";
    
    desc.constantsFloat.clear();
    desc.constantsVec4.clear();
    
    desc.depthTestEnabled = false; // TODO: enable by default
    desc.depthWriteEnabled = false;
    desc.stencilTestEnabled = false;
    desc.stencilWriteEnabled = false;
    
    desc.colorPixelFormats.push_back(PixelFormat_SystemRTV);
    desc.blendingEnables.push_back(true);
    desc.depthPixelFormat = Depth32Stencil8;
    desc.stencilPixelFormat = Depth32Stencil8;
    
    return desc;
}

int translateAttributeName(const std::string& semantic)
{
    if (semantic == "POSITION") return VAO_POSITION_INDEX;
    if (semantic == "NORMAL") return VAO_NORMAL_INDEX;
    if (semantic == "TEXCOORD_0") return VAO_TEXCOORD_INDEX;
    
    assert(false);
    return -1;
}

void loadMesh_glTF(const tinygltf::Model& model,
                   const tinygltf::Mesh& mesh,
                   const glm::mat4& transform,
                   ModelDesc* desc)
{
    for (int i = 0; i < mesh.primitives.size(); i++)
    {
        const tinygltf::Primitive& primitive = mesh.primitives[i];
        if (primitive.indices < 0)
            continue;
        
        MeshDesc meshDesc;
        meshDesc.name = mesh.name;
        meshDesc.transform = transform;
        meshDesc.material = getMaterialDesc(model, primitive);
        
        for (const auto& [semantic, index] : primitive.attributes)
        {
            const tinygltf::Accessor& attributeAccessor = model.accessors[index];
        
            BufferDesc bufferDesc;
            bufferDesc.elementType = getElementType(attributeAccessor.type, attributeAccessor.componentType);
            bufferDesc.elementCount = attributeAccessor.count;
            bufferDesc.data = getData(model, attributeAccessor);
            
            int elementSizeInBytes = getElementSizeInBytes(attributeAccessor.type, attributeAccessor.componentType);
            assert(bufferDesc.data->size() == bufferDesc.elementCount * elementSizeInBytes);
            assert(attributeAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);
            
            int attributeIndex = translateAttributeName(semantic);
            meshDesc.attributes[attributeIndex] = bufferDesc;
        }
        
        const tinygltf::Accessor& indexAccessor = model.accessors[primitive.indices];
        
        IndexBufferDesc indexBufferDesc;
        indexBufferDesc.indexTypeBites = indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT ? 32 : 16;
        indexBufferDesc.primitiveType = getPrimitiveType(primitive.mode);
        indexBufferDesc.elementType = getElementType(indexAccessor.type, indexAccessor.componentType);
        indexBufferDesc.elementCount = indexAccessor.count;
        indexBufferDesc.data = getData(model, indexAccessor);
        
        meshDesc.indices.push_back(indexBufferDesc);
        
        desc->meshes.push_back(meshDesc);
    }
}

void visitNode_glTF(const tinygltf::Model& model,
                    const tinygltf::Node& node,
                    glm::mat4 transform,
                    ModelDesc* desc)
{
    if (node.matrix.size() == 16)
    {
        glm::mat4 matrix(1.0);
        loadMatrix(node.matrix.data(), &matrix);
        transform = transform * matrix;
    }
    else
    {
        glm::mat4 matrix(1.0);
        
        if (node.translation.size() == 3)
        {
            glm::vec3 delta((float)node.translation[0],
                            (float)node.translation[1],
                            (float)node.translation[2]);
            matrix = matrix * glm::translate(delta);
        }
        
        if (node.rotation.size() == 4)
        {
            glm::quat quaternion((float)node.rotation[3],
                                 (float)node.rotation[0],
                                 (float)node.rotation[1],
                                 (float)node.rotation[2]);
            assert(FLOAT_EQ(quaternion.w, node.rotation[3]));
            matrix = matrix * glm::toMat4(quaternion);
        }
        
        if (node.scale.size() == 3)
        {
            glm::vec3 scale((float)node.scale[0],
                            (float)node.scale[1],
                            (float)node.scale[2]);
            matrix = matrix * glm::scale(scale);
        }
    }
    
    if (node.mesh > -1)
    {
        assert(node.mesh < model.meshes.size());
        loadMesh_glTF(model, model.meshes[node.mesh], transform, desc);
    }
    
    for (int i = 0; i < node.children.size(); i++)
    {
        assert(node.children[i] < model.nodes.size());
        visitNode_glTF(model, model.nodes[node.children[i]], transform, desc);
    }
}

bool loadModelFile_glTF(const std::string& path, ModelDesc* desc)
{
    tinygltf::TinyGLTF loader;
    tinygltf::Model model;
    std::string warning;
    std::string error;
    bool success = false;
    
    if (std::filesystem::path(path).extension().string() == ".glb")
    {
        success = loader.LoadBinaryFromFile(&model, &error, &warning, path.c_str());
    }
    else
    {
        success = loader.LoadASCIIFromFile(&model, &error, &warning, path.c_str());
    }
    
    if (!warning.empty())
    {
        printf("Warning: %s\n", warning.c_str());
    }
    
    if (!error.empty())
    {
        printf("Error: %s\n", error.c_str());
    }
    
    if (!success)
    {
        printf("Failed to load glTF: %s\n", path.c_str());
        return false;
    }
    
    int scene_index = model.defaultScene > -1 ? model.defaultScene : 0;
    const tinygltf::Scene& scene = model.scenes[scene_index];
    
    desc->name = std::filesystem::path(path).stem().string();
    desc->program = getProgramDesc(model);
    
    for (int i = 0; i < (int)scene.nodes.size(); i++)
    {
        const tinygltf::Node& node = model.nodes[scene.nodes[i]];
        glm::mat4 transform(1.0);
        visitNode_glTF(model, node, transform, desc);
    }
    
    return true;
}

bool loadHelloTriangle(ModelDesc* desc)
{
    std::vector<glm::vec3> pos = {
        {0, 0, 0},
        {0, 1, 0},
        {1, 1, 0},
    };
    
    std::vector<glm::vec3> norm = {
        {0, 0, 1},
        {0, 0, 1},
        {0, 0, 1},
    };
    
    std::vector<glm::vec2> texc = {
        {0, 0},
        {1, 0},
        {1, 1},
    };
    
    std::vector<uint32_t> index = {
        0, 1, 2,
    };
    
    auto vao_pos = std::make_shared<std::vector<uint8_t>>();
    auto vao_norm = std::make_shared<std::vector<uint8_t>>();
    auto vao_texc = std::make_shared<std::vector<uint8_t>>();
    auto ibo = std::make_shared<std::vector<uint8_t>>();
    
    vao_pos->resize(3 * sizeof(glm::vec3));
    vao_norm->resize(3 * sizeof(glm::vec3));
    vao_texc->resize(3 * sizeof(glm::vec2));
    ibo->resize(3 * sizeof(int));
    
    memcpy(vao_pos->data(), pos.data(), 3 * sizeof(glm::vec3));
    memcpy(vao_norm->data(), norm.data(), 3 * sizeof(glm::vec3));
    memcpy(vao_texc->data(), texc.data(), 3 * sizeof(glm::vec2));
    memcpy(ibo->data(), index.data(), 3 * sizeof(int));
    
    desc->name = "Unit Box";
    desc->program = getProgramDesc({});
    desc->meshes.resize(1);
    desc->meshes[0].name = "Box Mesh";
    desc->meshes[0].transform = glm::mat4(1.0);
    desc->meshes[0].material = getMaterialDesc({}, {});
    desc->meshes[0].attributes[VAO_POSITION_INDEX].data = vao_pos;
    desc->meshes[0].attributes[VAO_POSITION_INDEX].elementType = DATA_Float3;
    desc->meshes[0].attributes[VAO_POSITION_INDEX].elementCount = vao_pos->size() / sizeof(glm::vec3);
    desc->meshes[0].attributes[VAO_NORMAL_INDEX].data = vao_norm;
    desc->meshes[0].attributes[VAO_NORMAL_INDEX].elementType = DATA_Float3;
    desc->meshes[0].attributes[VAO_NORMAL_INDEX].elementCount = vao_norm->size() / sizeof(glm::vec3);
    desc->meshes[0].attributes[VAO_TEXCOORD_INDEX].data = vao_texc;
    desc->meshes[0].attributes[VAO_TEXCOORD_INDEX].elementType = DATA_Float2;
    desc->meshes[0].attributes[VAO_TEXCOORD_INDEX].elementCount = vao_texc->size() / sizeof(glm::vec2);
    desc->meshes[0].indices.resize(1);
    desc->meshes[0].indices[0].data = ibo;
    desc->meshes[0].indices[0].elementType = DATA_Uint;
    desc->meshes[0].indices[0].elementCount = ibo->size() / sizeof(int);
    desc->meshes[0].indices[0].indexTypeBites = 32;
    desc->meshes[0].indices[0].primitiveType = PRIM_Triangles;
    
    return true;
}

bool loadModelFile(const std::string& path, ModelDesc* desc)
{
    //return loadHelloTriangle(desc);
    
    std::string extension = std::filesystem::path(path).extension().string();
    
    if (extension == ".gltf" || extension == ".glb")
    {
        return loadModelFile_glTF(path, desc);
    }
    
    return false;
}
