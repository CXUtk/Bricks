#version 330 core
/*
    Input: [vec2] texture coords from vertex shader
    Output: [vec4] color of text render by textColor
*/

in vec2 texCoords;
out vec4 color;

uniform vec3 uColor;

void main(){
    vec2 vDis = texCoords - vec2(0.5, 0.5);
    if (length(vDis) > 0.5) {
        color = vec4(0, 0, 0, 0);
    }
    else {
         color = vec4(uColor, 1);
    }
}