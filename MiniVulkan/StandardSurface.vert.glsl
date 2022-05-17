#version 450

layout(location = 0) in  vec3 in_position;
layout(location = 1) in  vec3 in_normal;
layout(location = 2) in  vec2 in_texcoord;
layout(location = 0) out vec3 out_worldPos;
layout(location = 1) out vec3 out_worldNormal;
layout(location = 2) out vec2 out_texcoord;
layout(location = 3) out vec3 out_sunVec;
layout(location = 4) out vec3 out_cameraVec;

layout(set = 0, binding = 0) uniform Config
{ 
    mat4 modelMatrix;
    mat4 viewMatrix;
    mat4 projectionMatrix;
    vec3 sunPos;
    vec3 cameraPos;
} config;





void main()
{
    vec3 worldPos = vec3(config.modelMatrix * vec4(in_position, 1.0));
    gl_Position = config.projectionMatrix * config.viewMatrix * config.modelMatrix * vec4(in_position, 1.0);

    mat3 matrixNormal = mat3(inverse(transpose(config.modelMatrix)));
    vec3 normal = matrixNormal * in_normal;

    out_worldPos = worldPos;
    out_worldNormal = normal;
    out_texcoord = in_texcoord;
    out_sunVec = config.sunPos - worldPos;
    out_cameraVec = config.cameraPos - worldPos;
}