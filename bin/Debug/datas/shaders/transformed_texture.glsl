:params
sampler2D Albedo

:vertex
#version 430 core

layout (location = 0) uniform mat4 View;
layout (location = 1) uniform mat4 World;
layout (location = 2) uniform mat4 Projection;

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;

out vec2 fragUV;

void main()
{
    gl_Position = Projection * View * World * vec4( position, 1.0 );
    fragUV = uv;
}

:fragment
#version 430 core

layout (binding = 0, location = 3) uniform sampler2D Albedo;

in vec2 fragUV;
out vec4 FragColor;

void main()
{
    float mipmapLevel = textureQueryLod(Albedo, fragUV).x;
    FragColor = textureLod( Albedo, fragUV, mipmapLevel );
}
