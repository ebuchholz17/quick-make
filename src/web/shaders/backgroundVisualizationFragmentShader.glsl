// sdf visualization
precision highp float;

uniform float screenWidth;
uniform float screenHeight;
uniform float t;

float sdf (vec3 pos) {
    vec3 repetition = vec3(4.0, 4.0, 4.0);
    float distToArea = length(pos) - 5.0;
    vec3 q = mod(pos - vec3(0.0, 0.0, 0.0 + t*4.0), repetition) - 0.5 * repetition;
    vec3 d = abs(q) - vec3(0.3, 0.3, 0.3);
    float cube = length(max(d, 0.0))  + min(max(d.x,max(d.y,d.z)),0.0);
    //float sphere = length(q) - 0.6;
    //return max(cube, -sphere);
    return cube - 0.3;
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
    for (int i = 0; i < 255; ++i) {
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
        vec3 lightDir = normalize(vec3(0.0, 0.5, 0.9));
        vec3 diffuseColor = dot(normal, vec3(lightDir)) * vec3(0.0, 0.3, 0.6);
        //vec3 reflectDir = reflect(-lightDir, normal);
        //vec3 point = cameraPos + dist * rayDir;
        //vec3 toCamera = normalize(cameraPos - point);
        //float specFactor = pow(max(dot(reflectDir, toCamera), 0.0), 2.0);
        vec3 specColor = vec3(0.0);
        float fogAmount = (depth/60.0);
        color = vec4(mix(vec3(0.0, 0.1, 0.2) + diffuseColor + specColor, vec3(0.0, 0.1, 0.2), fogAmount), 1.0);
        //color = vec4(vec3(0.0, 0.1, 0.2) + vec3(0.0, 0.5, 0.5)*diffuseColor, 1.0);
    }

    gl_FragColor = color;
}

// mandelbrot
//precision highp float;
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

