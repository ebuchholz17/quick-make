attribute vec3 position;
attribute vec2 texCoord;
attribute vec3 normal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

varying vec2 vTexCoord;
varying float vLighting;

void main() {
    gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(position, 1.0);
    vTexCoord = texCoord;
    vec4 newNormal = normalize(modelMatrix * vec4(normal, 0.0));
    vec3 lightDir = normalize(vec3(0.0, 0.5, 0.9));
    float lighting = dot(newNormal, vec4(lightDir, 0.0));
    vLighting = lighting;
}

