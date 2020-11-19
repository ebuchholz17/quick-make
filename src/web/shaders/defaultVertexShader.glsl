attribute vec3 position;
attribute vec2 texCoord;
attribute vec3 normal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

varying vec2 vTexCoord;
varying float vLighting;
varying float vFogAmount;

void main() {
    vec4 cameraPos = viewMatrix * modelMatrix * vec4(position, 1.0); 
    gl_Position = projMatrix * cameraPos;
    vTexCoord = texCoord;
    vec4 newNormal = normalize(modelMatrix * vec4(normal, 0.0));
    vec3 lightDir = normalize(vec3(0.2, 0.5, 0.9));
    float lighting = dot(newNormal, vec4(lightDir, 0.0));
    vLighting = lighting;

    vFogAmount = clamp((-100.0 - cameraPos.z) / (-100.0 - (-50.0)), 0.0, 1.0);
}

