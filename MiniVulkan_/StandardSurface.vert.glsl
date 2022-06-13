#version 450

layout(location = 0) in  vec3 in_position;
layout(location = 1) in  vec3 in_normal;
layout(location = 2) in  vec2 in_texcoord;
layout(location = 0) out vec3 out_world_pos;
layout(location = 1) out vec3 out_world_normal;
layout(location = 2) out vec2 out_texcoord;
layout(location = 3) out vec3 out_sun_vec;
layout(location = 4) out vec3 out_camera_vec;

layout(set = 0, binding = 0) uniform Config
{ 
    mat4 model_matrix;
    mat4 view_matrix;
    mat4 projection_matrix;
    vec3 sun_pos;
    vec3 camera_pos;
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