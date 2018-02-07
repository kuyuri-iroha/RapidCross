


cbuffer g_cb : register(b0)
{
    matrix World;
    float Param;
	float3 dammy;
}


// テクスチャ
Texture2D Texture : register(t0);
SamplerState Sampler : register(s0);


// ピクセルシェーダの入力データ
struct PSInput
{
    float4 mPos : SV_POSITION; //頂点座標
    float2 mUV : TEXCOORD0; //UV座標
};


// ピクセルシェーダの出力データ
struct PSOutput
{
    float4 mColor : SV_TARGET;
};


// ピクセルシェーダ関数
PSOutput ParamPS(PSInput input)
{
    PSOutput op;
	op.mColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

	float mask =input.mUV.x;
    if (mask <= Param)
    {
        //draw
        op.mColor.rgb =Texture.Sample(Sampler, input.mUV).rgb;
        op.mColor.a =1.0f;
    }
    else
    {
		op.mColor =float4(0.5f, 0.5f, 0.5f, 0.7f);
    }

    return op;
}