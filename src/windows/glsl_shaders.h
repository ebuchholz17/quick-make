#ifndef GLSL_SHADERS_H
#define GLSL_SHADERS_H

char *defaultVertexShaderSource = R"shader(
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
)shader";

char *defaultFragmentShaderSource = R"shader(
    varying vec2 vTexCoord;
    varying float vLighting;

    uniform sampler2D texture;

    void main() {
        vec2 sampleCoord;
        sampleCoord.x = vTexCoord.x;
        sampleCoord.y = vTexCoord.y;
        vec3 textureColor = texture2D(texture, sampleCoord).rgb;

        vec3 ambientLighting = vec3(0.3, 0.6, 1.0) * 0.15;
        vec3 directLighting = vec3(1.0, 0.9, 0.6) * vLighting * textureColor;
        gl_FragColor = clamp(vec4(ambientLighting + directLighting, 1.0), vec4(0.0, 0.0, 0.0, 0.0), vec4(1.0, 1.0, 1.0, 1.0));
    }
)shader";


char *lineVertexShaderSource = R"shader(
    attribute vec3 position;

    uniform mat4 viewMatrix;
    uniform mat4 projMatrix;

    void main() {
        gl_Position = projMatrix * viewMatrix * vec4(position, 1.0); 
    }
)shader";

char *lineFragmentShaderSource = R"shader(
    void main() {
        gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
    }
)shader";

#endif
