precision highp float;

varying vec2 vTexCoord;
varying vec4 vColor;

uniform sampler2D texture;

void main() {
    vec4 baseColor = texture2D(texture, vTexCoord);
    gl_FragColor = vColor * baseColor;
}
