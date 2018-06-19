:custom
sampler2D FontAtlas

:vertex
#version 430 core

layout (location = 0) uniform mat4 Projection;

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec4 color;

out vec2 fragUV;
out vec4 fragColor;

void main()
{
    gl_Position = Projection * vec4( position, 1.0 );
    fragColor = color;
    fragUV = uv;
}

:fragment
#version 430 core

layout (binding = 0, location = 3) uniform sampler2D FontAtlas;

in vec4 fragColor;
in vec2 fragUV;
out vec4 FragColor;

void main()
{
    float mipmapLevel = textureQueryLod(FontAtlas, fragUV).x;
    FragColor = textureLod( FontAtlas, fragUV, mipmapLevel ) * fragColor;
}
