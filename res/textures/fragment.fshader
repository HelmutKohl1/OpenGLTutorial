#version 330 core

in vec3 ourColour;
in vec2 TexCoord;

out vec4 fragColour;

// Samplers are the in-built GLSL way to pass the texture object to
// the fragment shader
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float fraction;

void main(){
    // fragColour = vec4(ourColour, 1.0f);

    // third param means take 80% of param1, 20% of param2.
    fragColour = mix(texture(texture1, TexCoord), texture(texture2, vec2(TexCoord.x, TexCoord.y)), fraction);
}