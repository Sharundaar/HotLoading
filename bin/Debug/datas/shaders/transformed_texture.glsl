:params
sampler2D Albedo

:vertex
#version 430 core

uniform mat4 View;
uniform mat4 World;
uniform mat4 Projection;

in vec3 position;
in vec2 uv;

out vec2 fragUV;

void main()
{
    gl_Position = Projection * View * World * vec4( position, 1.0 );
    fragUV = uv;
}

:fragment
#version 430 core

uniform sampler2D Albedo;

in vec2 fragUV;
out vec4 FragColor;

void main()
{
    float mipmapLevel = textureQueryLod(Albedo, fragUV).x;
    FragColor = textureLod( Albedo, fragUV, mipmapLevel );
}
