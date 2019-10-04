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
    mat4 boneMatrix0 = boneTransforms[int(boneIndices.x)];
    mat4 boneMatrix1 = boneTransforms[int(boneIndices.y)];
    mat4 boneMatrix2 = boneTransforms[int(boneIndices.z)];
    mat4 boneMatrix3 = boneTransforms[int(boneIndices.w)];
    mat4 boneMatrix = boneWeights.x * boneMatrix0 + 
                      boneWeights.y * boneMatrix1 + 
                      boneWeights.z * boneMatrix2 + 
                      boneWeights.w * boneMatrix3;
    gl_Position = projMatrix * viewMatrix * modelMatrix * boneMatrix * vec4(position, 1.0); 
    vTexCoord = texCoord;
    vec4 newNormal = normalize(modelMatrix * boneMatrix * vec4(normal, 0.0));

    //gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(position, 1.0); 
    //vTexCoord = texCoord;
    //vec4 newNormal = normalize(modelMatrix * vec4(normal, 0.0));

    vec3 lightDir = normalize(vec3(0.0, 1.0, 0.0));
    float lighting = dot(newNormal, vec4(lightDir, 0.0));
    vLighting = lighting;
}
