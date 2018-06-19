:vertex
#version 430 core

layout (location = 0) uniform mat4 View;
layout (location = 1) uniform mat4 World;
layout (location = 2) uniform mat4 Projection;

layout (location = 0) in vec3 position;
layout (location = 1) in vec4 color;

out vec4 vertexColor;

void main()
{
    gl_Position = Projection * View * World * vec4( position, 1.0 );;
    vertexColor = color;
}

:fragment
#version 430 core
in vec4 vertexColor;
out vec4 FragColor;

void main()
{
    FragColor = vertexColor;
}
