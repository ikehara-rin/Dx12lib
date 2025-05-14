// SimpleVertexShader.hlsl
cbuffer cbChangePerObject : register(b0)
{
    matrix modelViewProj; // ÉÇÉfÉãïœä∑çsóÒ
}

struct VS_INPUT
{
    float3 position : POSITION;
    float2 texCoord : TEXCOORD0;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD0;
};

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output;
    output.position = mul(float4(input.position, 1.0f), modelViewProj);
    output.texCoord = input.texCoord;
    return output;
}
