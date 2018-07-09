:vertex
#version 430 core

uniform mat4 View;
uniform mat4 World;
uniform mat4 Projection;

in vec3 position;
in vec4 color;

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
