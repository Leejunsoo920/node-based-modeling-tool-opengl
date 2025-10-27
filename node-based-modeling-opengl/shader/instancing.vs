#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec3 aColor;
layout (location = 3) in mat4 modelM;


out vec3 ourColor;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    gl_Position = projection * view * modelM * vec4(aPos, 1.0); 
    ourColor = aColor;
}
