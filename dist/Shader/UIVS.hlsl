


// 定数バッファ
cbuffer g_cb : register(b0)
{
	matrix World;
	float Param;
	float3 dammy;
}

cbuffer g_StaticCB : register(b1)
{
	matrix Projection;
};


// 頂点シェーダの入力データ
struct VSInput
{
	float3 mPos : POSITION; //頂点座標
	float2 mUV : TEXCOORD0; //テクスチャ座標
};


// 頂点シェーダの出力データ
struct VSOutput
{
	float4 mPos : SV_POSITION; //頂点座標
	float2 mUV : TEXCOORD0; //テクスチャ座標
};



// 頂点シェーダ関数
VSOutput UIVS(VSInput input)
{
	VSOutput output;
	// 頂点座標
    output.mPos =float4(input.mPos.xy, 0.0f, 1.0f);
    output.mPos =mul(output.mPos, World);
	output.mPos =mul(output.mPos, Projection);

	// テクスチャ座標の受け渡し
	output.mUV =input.mUV;
	
	// 出力
	return output;
}