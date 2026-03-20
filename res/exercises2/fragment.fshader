#version 330 core

in vec3 ourColour;
in vec4 vertexPosition;

out vec4 fragColour;

void main(){
    // fragColour = vec4(ourColour, 1.0f);
    fragColour = vertexPosition;
}