// テクスチャ
Texture2D Texture : register(t0);
SamplerState Sampler : register(s0);


// ピクセルシェーダの入力データ
struct PSInput
{
	float4 mPos : SV_POSITION; //頂点座標
	float2 mUV : TEXCOORD0; //テクスチャ座標
};


// ピクセルシェーダの出力データ
struct PSOutput
{
	float4 mColor : SV_TARGET;
};


// ピクセルシェーダ関数
PSOutput DomePS(PSInput input)
{
	PSOutput op;
	op.mColor =Texture.Sample(Sampler, input.mUV);
	op.mColor.a =op.mColor.a;
	return op;
}