#version 330 core
/**
* Default Shader
*/
layout (location = 0) in vec4 vertex;

uniform mat4 projection;
uniform mat4 model;

void main(){
    gl_Position = projection * model * vertex;
}

