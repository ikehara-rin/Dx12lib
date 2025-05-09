Texture2D tex0 : register(t0);
SamplerState smp0 : register(s0);

float4 main(float2 uv : TEXCOORD) : SV_TARGET
{
    return tex0.Sample(smp0, uv);
}
