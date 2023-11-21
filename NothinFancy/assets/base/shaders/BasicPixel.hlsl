cbuffer lights {
    float3 lPos;
    float3 lColor;
};

struct PS_IN {
    float4 pos : SV_POSITION;
    float3 norm : NORMAL;
    float2 tex : TEXCOORD;

    float3 pixCoord : PIXCOORD;
};

Texture2D tex;
SamplerState samp;

float4 main(PS_IN input) : SV_TARGET
{
    float3 color;

    float3 lightPos = lPos;
    float3 lightColor = lColor;
    float3 diffColor = tex.Sample(samp, input.tex).rgb;

    float3 ambient = 0.1f * lightColor;

    float3 norm = normalize(input.norm);
    float3 lightDir = normalize(lightPos - input.pixCoord);
    float diff = max(dot(norm, lightDir), 0.0f);
    float3 diffuse = diff * lightColor;

    color = (ambient + diffuse) * diffColor;

    return float4(color, 1.0f);
}