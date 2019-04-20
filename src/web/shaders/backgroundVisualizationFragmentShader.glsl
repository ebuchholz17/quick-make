precision highp float;

uniform float screenWidth;
uniform float screenHeight;
uniform float t;

float sdf (vec3 pos) {
    vec3 repetition = vec3(4.0, 4.0, 4.0);
    float distToArea = length(pos) - 5.0;
    vec3 q = mod(pos - vec3(0.0, 0.0, 0.0), repetition) - 0.5 * repetition;
    vec3 d = abs(q) - vec3(0.4, 0.4, 0.4);
    float cube = length(max(d, 0.0))  + min(max(d.x,max(d.y,d.z)),0.0);
    //float sphere = length(q) - 0.6;
    //return max(cube, -sphere);
    return cube - 0.2;
}


float doRay (vec3 origin, vec3 rayDir, out float depth) {
    depth = 0.0;
    for (int i = 0; i < 255; ++i) {
        float dist = sdf(origin + depth * rayDir);
        if (dist < 0.0001) {
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

    vec3 cameraTarget = vec3(0.0, 0.0, -t*3.5);
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
    float dist = doRay(cameraPos, rayDir, depth);
    vec4 color = vec4(0.0, 0.0, 0.0, 1.0);
    if (dist <= 0.0001) {
        float fogAmount = (depth/60.0);
        color = vec4(mix(vec3(0.0, 1.0, 1.0), vec3(0.0, 0.1, 0.2), fogAmount), 1.0);
    }

    gl_FragColor = color;
}
