#version 330 core

in vec3 ourColour;
in vec2 TexCoord;

out vec4 fragColour;

// Samplers are the in-built GLSL way to pass the texture object to
// the fragment shader
uniform sampler2D ourTexture;

void main(){
    // fragColour = vec4(ourColour, 1.0f);
    fragColour = texture(ourTexture, TexCoord) * vec4(ourColour, 1.0f);
}