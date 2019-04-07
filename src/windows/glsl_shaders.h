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

char *spriteVertexShaderSource = R"shader(
    attribute vec2 position;
    attribute vec2 texCoord;
    attribute vec4 color;

    uniform float screenWidth;
    uniform float screenHeight;

    varying vec2 vTexCoord;
    varying vec4 vColor;

    void main() {
        vec4 pos = vec4(position.x, position.y, -1.0, 1.0);
        pos.x = pos.x * (1.0 / screenWidth) * 2.0 - 1.0;
        pos.y = (screenHeight - pos.y) * (1.0 / screenHeight) * 2.0 - 1.0;
        gl_Position = pos;
        vTexCoord = texCoord;
        vColor = color;
    }
)shader";

//char *spriteFragmentShaderSource = R"shader(
//    varying vec2 vTexCoord;
//    varying vec4 vColor;
//
//    uniform sampler2D texture;
//    uniform float textureWidth;
//    uniform float textureHeight;
//
//    vec2 saturate (vec2 value) {
//        return clamp(value, 0.0, 1.0);
//    }
//
//    void main() {
//        vec2 screenDimensions = vec2(textureWidth, textureHeight);
//        vec2 texCoord = vTexCoord;
//        texCoord *= screenDimensions;
//        texCoord = (saturate(fract(texCoord) / saturate(fwidth(texCoord))) + floor(texCoord) - 0.5) / screenDimensions;
//
//        vec4 baseColor = textureGrad(texture, texCoord, dFdx(texCoord), dFdy(texCoord));
//
//        gl_FragColor = vColor * baseColor;
//    }
//)shader";

char *spriteFragmentShaderSource = R"shader(
    varying vec2 vTexCoord;
    varying vec4 vColor;

    uniform sampler2D texture;

    void main() {
        vec4 baseColor = texture2D(texture, vTexCoord);
        gl_FragColor = vColor * baseColor;
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

char *backgroundVisualizationVertexShaderSource = R"shader(
    attribute vec2 position;

    void main() {
        gl_Position = vec4(position.x, position.y, -1.0, 1.0);    
    }
)shader";

char *backgroundVisualizationFragmentShaderSource = R"shader(
    uniform float screenWidth;
    uniform float screenHeight;
    uniform float t;

    float sdf (vec3 pos) {
        vec3 repetition = vec3(4.0, 4.0, 4.0);
        vec3 q = mod(pos - vec3(0.0, 0.0, t), repetition) - 0.5 * repetition;
        // sphere
        return length(q) - 0.5;
    }

    float doRay (vec3 rayDir) {
        vec3 origin = vec3(0.0, 0.0, 15.0);
        float depth = 0.0;
        for (int i = 0; i < 255; ++i) {
            float dist = sdf(origin + depth * rayDir);
            if (dist < 0.0001) {
                return dist;
            }
            depth += dist;
            if (depth >= 50.0) {
                return 50.0;
            }
        }
        return 50.0;
    }

    void main() {
        vec2 windowDim = vec2(screenWidth, screenHeight);
        vec2 rayXY = gl_FragCoord.xy - (windowDim / 2);
        float fovy = 45.0;
        float z = screenHeight / tan(radians(fovy) / 2.0);
        vec3 rayDir = normalize(vec3(rayXY, -z));

        float dist = doRay(rayDir);
        vec4 color = vec4(0.0, 0.0, 0.0, 1.0);
        if (dist <= 0.0001) {
            color = vec4(1.0, 0.0, 1.0, 1.0);
        }

        gl_FragColor = color;
    }
)shader";

#endif
