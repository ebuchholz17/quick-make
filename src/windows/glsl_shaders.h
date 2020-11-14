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
    varying float vFogAmount;

    void main() {
        vec4 cameraPos = viewMatrix * modelMatrix * vec4(position, 1.0); 
        gl_Position = projMatrix * cameraPos;
        vTexCoord = texCoord;
        vec4 newNormal = normalize(modelMatrix * vec4(normal, 0.0));
        vec3 lightDir = normalize(vec3(0.2, 0.5, 0.9));
        float lighting = dot(newNormal, vec4(lightDir, 0.0));
        vLighting = lighting;

        vFogAmount = saturate((-100.0 - cameraPos.z) / (-100.0 - (-50.0)));
    }
)shader";

char *animatedModelVertexShaderSource = R"shader(
    attribute vec3 position;
    attribute vec2 texCoord;
    attribute vec3 normal;
    attribute vec4 boneIndices;
    attribute vec4 boneWeights;

    uniform mat4 modelMatrix;
    uniform mat4 viewMatrix;
    uniform mat4 projMatrix;

    uniform mat4 boneTransforms[24];

    varying vec2 vTexCoord;
    varying float vLighting;

    void main() {
        mat4 boneMatrix0 = boneTransforms[int(boneIndices[0])];
        mat4 boneMatrix1 = boneTransforms[int(boneIndices[1])];
        mat4 boneMatrix2 = boneTransforms[int(boneIndices[2])];
        mat4 boneMatrix3 = boneTransforms[int(boneIndices[3])];
        mat4 boneMatrix = boneWeights[0] * boneMatrix0 + 
                          boneWeights[1] * boneMatrix1 + 
                          boneWeights[2] * boneMatrix2 + 
                          boneWeights[3] * boneMatrix3;
        gl_Position = projMatrix * viewMatrix * modelMatrix * boneMatrix * vec4(position, 1.0); 
        vTexCoord = texCoord;
        vec4 newNormal = normalize(modelMatrix * boneMatrix * vec4(normal, 0.0));
        vec3 lightDir = normalize(vec3(0.0, 1.0, 0.0));
        float lighting = dot(newNormal, vec4(lightDir, 0.0));
        vLighting = lighting;
    }
)shader";

char *defaultFragmentShaderSource = R"shader(
    varying vec2 vTexCoord;
    varying float vLighting;
    varying float vFogAmount;

    uniform sampler2D texture;

    void main() {
        vec2 sampleCoord;
        sampleCoord.x = vTexCoord.x;
        sampleCoord.y = vTexCoord.y;
        vec3 textureColor = texture2D(texture, sampleCoord).rgb;

        vec3 ambientLighting = vec3(0.3, 0.6, 1.0) * 0.15;
        vec3 directLighting = vec3(1.0, 0.9, 0.6) * vLighting * textureColor;
        vec3 litColor = clamp(vec3(ambientLighting + directLighting), vec3(0.0, 0.0, 0.0), vec3(1.0, 1.0, 1.0));
        vec3 fogColor = vec3(0.0, 0.7, 0.8);
        vec3 foggedColor = vFogAmount * litColor + (1.0 - vFogAmount) * fogColor;
        gl_FragColor = vec4(foggedColor, 1.0);
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
// sdf visualization

uniform float screenWidth;
uniform float screenHeight;
uniform float t;

float sdf (vec3 pos) {
    vec3 repetition = vec3(4.0, 4.0, 4.0);
    float distToArea = length(pos) - 5.0;
    vec3 q = mod(pos - vec3(0.0, 0.0, 0.0 + t*4.0), repetition) - 0.5 * repetition;
    vec3 d = abs(q) - vec3(0.7, 0.7, 0.7);
    float cube = length(max(d, 0.0))  + min(max(d.x,max(d.y,d.z)),0.0);
    float sphere = length(q) - 0.9;
    return max(cube, sphere);
    //return cube - 0.3;
    //return (length(q) - 1.0);
}


vec3 sdfNormal (vec3 point) {
    return normalize(vec3(
        sdf(vec3(point.x + 0.001, point.y, point.z)) - sdf(vec3(point.x - 0.001, point.y, point.z)),
        sdf(vec3(point.x, point.y + 0.001, point.z)) - sdf(vec3(point.x, point.y - 0.001, point.z)),
        sdf(vec3(point.x, point.y, point.z + 0.001)) - sdf(vec3(point.x, point.y, point.z - 0.001))
    ));

}

float doRay (vec3 origin, vec3 rayDir, out float depth, out vec3 normal) {
    depth = 0.0;
    normal = vec3(0.0, 0.0, 0.0);
    for (int i = 0; i < 128; ++i) {
        float dist = sdf(origin + depth * rayDir);
        if (dist < 0.0001) {
            normal = sdfNormal(origin + depth * rayDir);
            return dist;
        }
        depth += dist;
        if (depth >= 75.0) {
            return 75.0;
        }
    }
    return 75.0;
}

void main() {
    vec2 windowDim = vec2(screenWidth, screenHeight);
    vec2 rayXY = gl_FragCoord.xy - (windowDim / 2.0);
    float fovy = 45.0;
    float z = screenHeight / tan(radians(fovy) / 2.0);
    vec3 rayDir = normalize(vec3(rayXY, -z));

    vec3 cameraTarget = vec3(0.0, 0.0, 3.5);
    vec3 cameraPos = vec3(10.0*sin(t*0.15), 1.0*sin(t*0.25), cameraTarget.z + 10.0 * cos(t*0.15));
    vec3 cameraDir = normalize(cameraTarget - cameraPos);
    //vec3 cameraDir = vec3(0.0, 0.0, -1.0);

    vec3 up = vec3(0.0, 1.0, 0.0);
    vec3 right = normalize(cross(cameraDir, up));
    vec3 cameraUp = cross(right, cameraDir);

    mat3 cameraMat = mat3(1.0);
    cameraMat[0] = right;
    cameraMat[1] = cameraUp;
    cameraMat[2] = -cameraDir;

    rayDir = cameraMat * rayDir;

    float depth = 0.0;
    vec3 normal;
    float dist = doRay(cameraPos, rayDir, depth, normal);
    vec4 color = vec4(0.0, 0.1, 0.2, 1.0);
    if (dist <= 0.0001) {
        vec3 lightDir = normalize(vec3(0.9, 0.9, 0.9));
        vec3 diffuseColor = dot(normal, vec3(lightDir)) * vec3(0.0, 0.3, 0.6);
        vec3 reflectDir = reflect(-lightDir, normal);
        vec3 point = cameraPos + dist * rayDir;
        vec3 toCamera = normalize(cameraPos - point);
        float specFactor = pow(clamp(dot(reflectDir, toCamera), 0.0, 1.0), 10.0);
        vec3 specColor = vec3(0.5) * specFactor;
        float fogAmount = (depth/60.0);
        color = vec4(mix(vec3(0.0, 0.1, 0.2) + diffuseColor + specColor, vec3(0.0, 0.1, 0.2), fogAmount), 1.0);
        //color = vec4(vec3(0.0, 0.1, 0.2) + vec3(0.0, 0.5, 0.5)*diffuseColor, 1.0);
    }

    gl_FragColor = color;
}

// mandelbrot
//
//uniform float screenWidth;
//uniform float screenHeight;
//uniform float t;
//
//void main() {
//    vec2 windowDim = vec2(screenWidth, screenHeight);
//    vec2 coord = ((gl_FragCoord.xy - (windowDim / 2.0)) / windowDim) * 6.0;
//    vec2 zoomTarget = vec2(-0.4945, -0.6125);
//    float zoom = 1.0 + exp(t);
//    coord /= zoom;
//    coord += zoomTarget;
//    //coord -= zoom * zoomTarget;
//
//    float x = 0.0;
//    float y = 0.0;
//    float numIterations = 0.0;
//    for (int i = 0; i <= 200; ++i) {
//        numIterations = float(i);
//        float prevX = x;
//        x = (x * x) + -1.0 * (y * y) + coord.x;
//        y = 2.0 * prevX * y + coord.y;
//        if (x > 2.0) { break; }
//    }
//    
//    vec4 color = vec4(0.0, 0.0, 0.0, 1.0);
//    if (x > 2.0 || numIterations > 200.0) {
//        float multiplier = (200.0 - numIterations) / 200.0;
//        color = vec4(vec3(1.0, 1.0, 1.0) * multiplier, 1.0);
//    }
//    
//    gl_FragColor = color;
//}

)shader";

#endif
