precision highp float;

uniform float screenWidth;
uniform float screenHeight;
uniform float t;

float sdf (vec3 pos) {
    vec3 repetition = vec3(4.0, 4.0, 4.0);
    vec3 q = mod(pos - vec3(0.0, 0.0, t*2.0), repetition) - 0.5 * repetition;
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
    vec2 rayXY = gl_FragCoord.xy - (windowDim / 2.0);
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
