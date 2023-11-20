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
    float3 lightPos = float3(5.0f, 5.0f, -5.0f);

    float3 diffColor = tex.Sample(samp, input.tex).rgb;
    float3 norm = normalize(input.norm);
    float3 lightDir = normalize(lightPos - input.pixCoord);
    float diff = max(dot(norm, lightDir), 0.0f);
    float3 diffuse = (0.1f + diff) * diffColor;

    return float4(diffuse, 1.0f);
}