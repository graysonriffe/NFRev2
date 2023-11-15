struct VS_IN {
    float4 pos : POSITION;
    float2 tex : TEXCOORD;
};

struct VS_OUT {
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD;
};

VS_OUT main(VS_IN input) {
    VS_OUT output;
    output.pos = input.pos;
    output.tex = input.tex;

    return output;
}