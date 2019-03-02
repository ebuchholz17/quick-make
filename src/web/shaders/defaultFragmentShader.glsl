precision highp float;

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
