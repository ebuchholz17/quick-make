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
