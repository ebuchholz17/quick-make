#include <metal_stdlib>
using namespace metal;

vertex float4 basic_vertex (const device packed_float3 *vertex_array [[buffer(0)]], 
                            unsigned int vid [[vertex_id]]) 
{
    return float4(vertex_array[vid], 1.0);
}

fragment half4 basic_fragment () {
    return half4(1.0);
}

struct sprite_fragment_data {
    float4 position [[position]];
    float4 color;
    float2 texCoord;
};

struct sprite_uniforms {
    float screenWidth;
    float screenHeight;
};

vertex sprite_fragment_data 
spriteVertexShader (const device packed_float2 *positions [[buffer(0)]],
                    const device packed_float2 *texCoords [[buffer(1)]],
                    const device packed_float4 *colors [[buffer(2)]],
                    constant sprite_uniforms &uniforms [[buffer(3)]],
                    unsigned int vid [[vertex_id]])
{
    sprite_fragment_data result;
    float2 inputPosition = positions[vid];
    float4 outputPosition = float4(inputPosition.x, inputPosition.y, 0.0, 1.0);
    outputPosition.x = outputPosition.x * (1.0 / uniforms.screenWidth) * 2.0 - 1.0;
    outputPosition.y = (uniforms.screenHeight - outputPosition.y) * (1.0 / uniforms.screenHeight) * 2.0 - 1.0;

    result.position = outputPosition;
    result.color = colors[vid];
    result.texCoord = texCoords[vid];

    return result;
}

fragment float4 spriteFragmentShader (sprite_fragment_data fragmentData [[stage_in]],
                                      constant sprite_uniforms &uniforms [[buffer(0)]],
                                      texture2d<float> spriteTexture [[texture(0)]],
                                      sampler textureSampler [[sampler(0)]])
{
    float4 baseColor = spriteTexture.sample(textureSampler, fragmentData.texCoord);
    return baseColor * fragmentData.color;
}
