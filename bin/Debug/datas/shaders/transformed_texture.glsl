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
    vec2 adjustedUV = floor( fragUV * 16.0 ) / 16.0;
    FragColor = texture( Albedo, adjustedUV );
}
