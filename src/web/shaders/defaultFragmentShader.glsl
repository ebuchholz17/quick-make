precision highp float;

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
