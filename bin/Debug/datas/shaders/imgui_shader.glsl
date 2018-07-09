:params
sampler2D FontAtlas

:vertex
#version 430 core

uniform mat4 Projection;

in vec3 position;
in vec2 uv;
in vec4 color;

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

uniform sampler2D FontAtlas;

in vec4 fragColor;
in vec2 fragUV;
out vec4 FragColor;

void main()
{
    float mipmapLevel = textureQueryLod(FontAtlas, fragUV).x;
    FragColor = textureLod( FontAtlas, fragUV, mipmapLevel ) * fragColor;
}
