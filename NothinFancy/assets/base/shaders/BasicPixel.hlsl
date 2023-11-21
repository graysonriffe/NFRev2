cbuffer psBuff {
    float3 lPos;
    float3 lColor;
    float3 cPos;
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

    float3 cameraPos = cPos;

    float3 lightPos = lPos;
    float3 lightColor = lColor;
    float3 diffColor = tex.Sample(samp, input.tex).rgb;

    float3 ambient = 0.1f * lightColor;

    float3 norm = normalize(input.norm);
    float3 lightDir = normalize(lightPos - input.pixCoord);
    float diff = max(dot(norm, lightDir), 0.0f);
    float3 diffuse = diff * lightColor;

    float3 viewDir = normalize(cameraPos - input.pixCoord);
    float3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    float3 specular = 0.5f * spec * lightColor;

    color = (ambient + diffuse + specular) * diffColor;

    return float4(color, 1.0f);
}