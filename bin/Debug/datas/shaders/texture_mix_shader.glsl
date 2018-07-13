:params
sampler2D Albedo1
sampler2D Albedo2
float amount

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

uniform sampler2D Albedo1;
uniform sampler2D Albedo2;
uniform float     amount;

in vec2 fragUV;
out vec4 FragColor;

void main()
{
    float albedo1_mipmap = textureQueryLod(Albedo1, fragUV).x;
    float albedo2_mipmap = textureQueryLod(Albedo2, fragUV).x;
    FragColor = mix( textureLod( Albedo1, fragUV, albedo1_mipmap ), textureLod( Albedo2, fragUV, albedo2_mipmap ), amount );
}
