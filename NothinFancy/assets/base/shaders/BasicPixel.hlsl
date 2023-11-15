struct PS_IN {
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD;
};

Texture2D tex;
SamplerState samp;

float4 main(PS_IN input) : SV_TARGET
{
    return tex.Sample(samp, input.tex);
}