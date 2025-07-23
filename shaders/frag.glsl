#version 330 core
in vec2 screenPos;
out vec4 FragColor;

uniform sampler2D uScene;
uniform vec2 uLight; 
uniform float uRadius;

void main() {
    vec4 sceneCol = texture(uScene, screenPos);
    float d = length(screenPos - uLight);
    float intensity = 1.0 - smoothstep(uRadius*0.8, uRadius, d);
    FragColor = sceneCol * intensity;
}