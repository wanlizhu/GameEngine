#version 450

#define TECHNIQUE_BLINN_PHONG 0
#define TECHNIQUE_PBR 1

#define MAX_NUM_LIGHT 16
#define LIGHT_TYPE_DIRECTIONAL 0
#define LIGHT_TYPE_POINT 1
#define LIGHT_TYPE_SPOT 2
#define SUN_LIGHT_COLOR vec3(1.0)

#define IMAGE_FLAG_AMBIENT  1
#define IMAGE_FLAG_DIFFUSE  2
#define IMAGE_FLAG_SPECULAR 4
#define IMAGE_FLAG_BASE_COLOR 8
#define IMAGE_FLAG_METALLIC_ROUGHNESS 16
#define IMAGE_FLAG_NORMAL 32
#define IMAGE_FLAG_AO 64

layout(location = 0) in  vec3 in_world_pos;
layout(location = 1) in  vec3 in_world_normal;
layout(location = 2) in  vec2 in_texcoord;
layout(location = 3) in  vec3 in_sun_vec;
layout(location = 4) in  vec3 in_camera_vec;
layout(location = 0) out vec4 out_color;

layout(set = 0, binding = 1) uniform Config
{
    uint technique;

    // Blinn-Phong parameters
    uint  phone_image_mask;
    vec3  phong_ambient;
    vec3  phong_diffuse;
    vec3  phong_specular;
    float phong_gloss; // default: 20

    // PBR parameters
    uint  pbr_image_mask;
    vec4  pbr_basecolor;
    float pbr_metallic;
    float pbr_roughness;
} config;

layout(set = 0, binding = 2) uniform Light
{
    uint count;
    uint types[MAX_NUM_LIGHT];
    vec3 positions[MAX_NUM_LIGHT];
    vec3 directions[MAX_NUM_LIGHT];
    vec4 colors[MAX_NUM_LIGHT];
    vec3 attenuations[MAX_NUM_LIGHT]; // [quadratic, linear, constant]
    vec2 cutoffs[MAX_NUM_LIGHT];      // [outer_cutoff, inner_cutoff] for spot lights
} light;





void main_blinn_phong()
{
    // ambient color
    vec3 ambient = config.phong_ambient;

    // diffuse color
    vec3 N = normalize(in_worldNormal);
    vec3 L = normalize(in_sunVec);
    vec3 diffuse = SUN_LIGHT_COLOR * config.phong_diffuse * max(dot(N, L), 0.0);

    // specular color
    vec3 V = normalize(in_cameraVec);
    vec3 H = normalize(L + V); //normalize(reflect(-L, N)); // Phong
    vec3 specular = SUN_LIGHT_COLOR * config.phong_specular * pow(max(dot(H, N), 0.0), config.phong_gloss); 

    out_color = vec4(ambient + diffuse + specular, 1.0);
}

void main_pbr()
{}

void main()
{
    if (config.technique == TECHNIQUE_BLINN_PHONG)
    {
        main_blinn_phong();
    }
    else if (config.technique == TECHNIQUE_PBR)
    {
        main_pbr();
    }
    else
    {
        out_color = vec4(1, 0, 0, 1);
    }
}