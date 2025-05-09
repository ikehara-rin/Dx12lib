cbuffer ConstantBuffer : register(b0)
{
    float4x4 matWVP; // ワールド・ビュー・プロジェクション行列
};

struct VSInput
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
};

struct PSInput
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
};

PSInput main(VSInput input)
{
    PSInput output;
    output.pos = mul(float4(input.pos, 1.0f), matWVP);
    output.uv = input.uv;
    return output;
}
