cbuffer vpBuff {
    float4x4 vp;
};

cbuffer modelBuff {
    float4x4 model;
};

struct VS_IN {
    float4 pos : POSITION;
    float3 norm : NORMAL;
    float2 tex : TEXCOORD;
};

struct VS_OUT {
    float4 pos : SV_POSITION;
    float3 norm : NORMAL;
    float2 tex : TEXCOORD;

    float3 pixCoord : PIXCOORD;
};

VS_OUT main(VS_IN input) {
    VS_OUT output;
    output.pos = mul(mul(vp, model), input.pos);
    output.norm = mul((float3x3)model, input.norm);
    output.tex = input.tex;
    output.pixCoord = (float3)mul(model, input.pos);

    return output;
}