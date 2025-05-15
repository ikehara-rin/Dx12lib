// SpriteShader.hlsl

cbuffer WorldMatrixBuffer : register(b0)
{
    matrix world;
};

Texture2D tex : register(t0);
SamplerState samp : register(s0);

struct VS_INPUT
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
};

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
};

PS_INPUT VSMain(VS_INPUT input)
{
    PS_INPUT output;
    float4 worldPos = mul(float4(input.pos, 1.0f), world);
    output.pos = worldPos;
    output.uv = input.uv;
    return output;
}

float4 PSMain(PS_INPUT input) : SV_TARGET
{
    return tex.Sample(samp, input.uv);
}
