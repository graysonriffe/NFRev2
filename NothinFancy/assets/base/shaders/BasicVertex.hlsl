cbuffer vpBuff {
    float4x4 vp;
};

cbuffer modelBuff {
    float4x4 model;
};

struct VS_IN {
    float4 pos : POSITION;
    float2 tex : TEXCOORD;
    float3 norm : NORMAL;
    float3 tan : TANGENT;
};

struct VS_OUT {
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD;
    float3 norm : NORMAL;

    float3 pixCoord : PIXCOORD;
    float3x3 tbn : TBN;
};

VS_OUT main(VS_IN input) {
    VS_OUT output;
    output.pos = mul(mul(vp, model), input.pos);

    float3x3 normMatrix = (float3x3)model;
    float3 N = normalize(mul(normMatrix, input.norm));
    output.norm = N;
    float3 T = normalize(mul(normMatrix, input.tan));
    float3 B = normalize(cross(T, N));
    float3x3 tbn = transpose(float3x3(T, B, N));
    output.tbn = tbn;

    output.tex = input.tex;
    output.pixCoord = (float3)mul(model, input.pos);

    return output;
}