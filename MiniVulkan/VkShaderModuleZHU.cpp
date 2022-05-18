#include "VkShaderModuleZHU.h"
#include "VkCommonZHU.h"

VkShaderModuleZHU::VkShaderModuleZHU(const VkShaderModuleCreateInfoZHU& info)
    : _info(info)
{
    std::vector<uint8_t> spirv;

    if (endsWith(_info.sourcePath, ".glsl"))
    {
        compile();
    }

    if (endsWith(_info.sourcePath, ".spv"))
    {
        reflect();
    }
	
	loadSPIRV();
	loadReflection();

    assert(!_spirv.empty());
	
    VkShaderModuleCreateInfo moduleInfo = {};
    moduleInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    moduleInfo.pNext = nullptr;
    moduleInfo.flags = 0;
    moduleInfo.pCode = (const uint32_t*)_spirv.data();
    moduleInfo.codeSize = _spirv.size();

    VkResult result = vkCreateShaderModule(_info.device, &moduleInfo, nullptr, &_shaderModule);
    VK_CHECK(result);
}

VkShaderModuleZHU::~VkShaderModuleZHU()
{
    if (_shaderModule)
    {
        vkDestroyShaderModule(_info.device, _shaderModule, nullptr);
        _shaderModule = VK_NULL_HANDLE;
    }
}

std::string concatIncludes(const std::vector<std::string>& searchPath)
{
    std::string str;

    for (const auto& path : searchPath)
    {
        str += " -I" + path + " ";
    }

    return str;
}

std::string concatMacros(const std::unordered_map<std::string, std::string>& macros)
{
    std::string str;

    for (const auto& [name, value] : macros)
    {
        str += " -D" + name + (value.empty() ? "" : ("=" + value)) + " ";
    }

    return str;
}

void printErrors(const std::string& path)
{
    std::string content;
    if (!std::filesystem::exists(path) ||
        !loadTextFile(path, &content) ||
        content.find("ERROR:") == std::string::npos)
    {
        return;
    }

    printf("%s\n", content.c_str());
}

void VkShaderModuleZHU::compile()
{
	std::string spvPath = replaceFileExtension(_info.sourcePath, ".spv");
    std::string logPath = replaceFileExtension(_info.sourcePath, ".log");
    std::string cmdLine = "glslangValidator.exe ";
    
    cmdLine += " --target-env vulkan1.1 --client vulkan100 ";
    cmdLine += " --auto-map-bindings ";
    cmdLine += " -l "; // link all input files together to form a single module
    cmdLine += " -R "; // allowing the use of gl_VertexID and gl_InstanceID keywords
    cmdLine += " -o " + spvPath + " ";
    cmdLine += concatIncludes(_info.searchPath);
    cmdLine += concatMacros(_info.macros);
    cmdLine += " " + _info.sourcePath + " ";
    cmdLine += " > " + logPath;

    system(cmdLine.c_str());
    printErrors(logPath);

    _info.sourcePath = spvPath;
}

void VkShaderModuleZHU::reflect()
{
    std::string jsonPath = replaceFileExtension(_info.sourcePath, ".json");
    std::string logPath = replaceFileExtension(_info.sourcePath, ".log");
    std::string cmdLine = "spirv-cross.exe";

    cmdLine += " --reflect";
    cmdLine += " --output " + jsonPath + " ";
    cmdLine += " " + _info.sourcePath + " ";
    cmdLine += " > " + logPath;

    system(cmdLine.c_str());
    printErrors(logPath);

    _info.reflectionPath = jsonPath;
}

void VkShaderModuleZHU::loadSPIRV()
{
	if (endsWith(_info.sourcePath, ".vert.spv"))
	{
        _stage = VK_SHADER_STAGE_VERTEX_BIT;
	}
	
    if (endsWith(_info.sourcePath, ".frag.spv"))
    {
		_stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	}
	
    if (!loadFile(_info.sourcePath, &_spirv))
    {
        printf("Failed to load SPIRV binary\n");
        assert(false);
    }
}

void VkShaderModuleZHU::loadReflection()
{
    if (!_reflection.loadFromFile(_info.reflectionPath))
    {
        printf("Failed to load reflection\n");
        assert(false);
    }
}








/*
#include "shaderc/shaderc.h"

struct ShaderCompiler
{
    std::vector<uint8_t> spirv;
    std::map<std::string, std::string> macros;
    std::list<std::string> searchPath = {
        std::filesystem::current_path().string(),
        std::filesystem::current_path().append("Shaders").string(),
    };

    bool compile(const std::string& path)
    {
        std::string source;
        loadTextFile(path.c_str(), &source);

        if (source.empty())
        {
            printf("Failed to load shader source: %s\n", path);
            return false;
        }

        auto compiler = shaderc_compiler_initialize();
        auto options = shaderc_compile_options_initialize();
        auto kind = getShaderKind(path);

        auto absPath = std::filesystem::absolute(path);
        auto filename = absPath.filename().string();
        searchPath.push_front(absPath.parent_path().string());

        shaderc_compile_options_set_source_language(options, shaderc_source_language_glsl);
        shaderc_compile_options_set_optimization_level(options, shaderc_optimization_level_zero);;
        shaderc_compile_options_set_auto_map_locations(options, true);
        shaderc_compile_options_set_auto_bind_uniforms(options, true);
        shaderc_compile_options_set_include_callbacks(options, resolveIncludeFile, releaseIncludeResult, this);
        shaderc_compile_options_set_generate_debug_info(options);

        for (const auto& [name, value] : macros)
        {
            shaderc_compile_options_add_macro_definition(options, name.c_str(), name.size(), value.c_str(), value.size());
        }

        auto result = shaderc_compile_into_spv(compiler, source.c_str(), source.size(), kind, filename.c_str(), "main", options);
        auto status = shaderc_result_get_compilation_status(result);

        if (status == shaderc_compilation_status_success)
        {
            auto length = shaderc_result_get_length(result);
            auto bytes = shaderc_result_get_bytes(result);

            spirv.resize(length);
            memcpy(spirv.data(), bytes, length);
        }
        else
        {
            handleCompileError(status, result);
        }

        searchPath.pop_front();
        shaderc_result_release(result);
        shaderc_compile_options_release(options);
        shaderc_compiler_release(compiler);

        return status == shaderc_compilation_status_success;
    }

private:
    static shaderc_include_result*
        resolveIncludeFile(void* user_data,
                           const char* requested_source,
                           int type,
                           const char* requesting_source,
                           size_t include_depth)
    {
        shaderc_include_result* result = new shaderc_include_result();
        memset(result, 0, sizeof(shaderc_include_result));

        auto compiler = (ShaderCompiler*)user_data;
        const char* foundPath = nullptr;

        for (const auto& path : compiler->searchPath)
        {
            auto iterator = std::filesystem::directory_iterator(path);

            for (const auto& entry : iterator)
            {
                if (entry.path().filename().string() == requested_source)
                {
                    foundPath = strdup(entry.path().string().c_str());
                    break;
                }
            }

            if (foundPath)
            {
                break;
            }
        }

        if (foundPath)
        {
            std::string content;
            loadTextFile(foundPath, &content);
            assert(!content.empty());
            const char* dup = strdup(content.c_str());

            result->source_name = foundPath;
            result->source_name_length = strlen(foundPath);
            result->content = dup;
            result->content_length = strlen(dup);
            result->user_data = user_data;
        }

        return result;
    }

    static void releaseIncludeResult(void* user_data, shaderc_include_result* include_result)
    {
        if (include_result)
        {
            if (include_result->source_name)
                free((void*)include_result->source_name);

            if (include_result->content)
                free((void*)include_result->content);

            delete include_result;
        }
    }

    static void loadTextFile(const char* path, std::string* text)
    {
        FILE* file = fopen(path, "rb");

        if (file == nullptr)
            return;

        fseek(file, 0, SEEK_END);
        long size = ftell(file);
        fseek(file, 0, SEEK_SET);

        text->resize(size);

        long num = fread(text->data(), 1, size, file);
        assert(num == size);

        fclose(file);
    }

    static shaderc_shader_kind getShaderKind(const std::string& path)
    {
        if (path.find(".vert") != std::string::npos ||
            path.find(".vert.glsl") != std::string::npos)
        {
            return shaderc_vertex_shader;
        }

        if (path.find(".frag") != std::string::npos ||
            path.find(".frag.glsl") != std::string::npos)
        {
            return shaderc_fragment_shader;
        }

        assert(false);

        return shaderc_vertex_shader;
    }

    static VkShaderStageFlagBits getShaderStage(shaderc_shader_kind kind)
    {
        switch (kind)
        {
            case shaderc_vertex_shader:
                return VK_SHADER_STAGE_VERTEX_BIT;
            case shaderc_fragment_shader:
                return VK_SHADER_STAGE_FRAGMENT_BIT;
            default:
                assert(false);
                return VK_SHADER_STAGE_VERTEX_BIT;
        }
    }

    static void handleCompileError(shaderc_compilation_status status,
                                   shaderc_compilation_result_t result)
    {
        switch (status)
        {
            case shaderc_compilation_status_invalid_stage:
                printf("Failed to compile shader: invalid stage\n");
                break;
            case shaderc_compilation_status_compilation_error:
                printf("Failed to compile shader: compilation error\n");
                break;
            case shaderc_compilation_status_internal_error:
                printf("Failed to compile shader: internal error\n");
                break;
            case shaderc_compilation_status_null_result_object:
                printf("Failed to compile shader: null result object\n");
                break;
            case shaderc_compilation_status_invalid_assembly:
                printf("Failed to compile shader: invalid assembly\n");
                break;
            case shaderc_compilation_status_validation_error:
                printf("Failed to compile shader: validation error\n");
                break;
            case shaderc_compilation_status_transformation_error:
                printf("Failed to compile shader: transformation error\n");
                break;
            case shaderc_compilation_status_configuration_error:
                printf("Failed to compile shader: configuration error\n");
                break;
            default: break;
        }

        int errors = shaderc_result_get_num_errors(result);
        const char* message = shaderc_result_get_error_message(result);

        printf("Number of Errors: %d\n", errors);
        printf("%s\n", message);
    }
};
*/