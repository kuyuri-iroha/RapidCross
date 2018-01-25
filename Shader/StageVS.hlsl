


// 定数バッファ
cbuffer g_cb : register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;
	float4 EyePos;

	float Ambient;
	float3 LDir;
	float4 LColor;
	float LStrength;
	float Specular;
	float2 Dammy;
}

cbuffer g_shadow : register(b1)
{
	matrix SView;
	matrix SProjection;
}


struct VSInput
{
	float3 mPos : POSITION; //頂点座標(モデル座標系)
	float2 mUV : TEXCOORD0; //テクスチャ座標
	float3 mNorm : NORMAL; //法線ベクトル
};


struct VSOutput
{
	float4 mPos : SV_POSITION; //頂点座標(透視変換座標)
	float2 mUV : TEXCOORD0; //テクスチャ座標
	float3 mNorm : TEXCOORD1; //法線ベクトル
	float3 mViewDir : TEXCOORD2; //視線ベクトル
	float3 mShadowCoord : TEXCOORD3; //シャドウマップ座標
};


// シャドウマップ描画用頂点シェーダ関数
float4 StageShadowVS(VSInput ip) : SV_POSITION
{
	// スキニング
	float4 pos =float4(ip.mPos, 1.0f);

	// 頂点座標変換
	pos =mul(pos, World);
	pos =mul(pos, View);
	pos =mul(pos, Projection);

	return pos;
}


// 頂点シェーダ関数
VSOutput StageVS(VSInput ip)
{
	VSOutput op;

	// スキニング
	float4 pos =float4(ip.mPos, 1.0f);

	// 頂点座標変換
	op.mPos =mul(pos, World);
	op.mViewDir =op.mPos.xyz -EyePos.xyz; //視点ベクトルの作成
    op.mPos =mul(op.mPos, View);
	op.mPos =mul(op.mPos, Projection);

	// テクスチャ座標の受け渡し
	op.mUV =ip.mUV;

	// 法線ベクトル変換
	op.mNorm =ip.mNorm;

	// シャドウマッピング用の算出
	pos =mul(pos, World);
	pos =mul(pos, SView);
	pos =mul(pos, SProjection);
	pos.xyz =pos.xyz /pos.w;
	op.mShadowCoord.x =(pos.x +1.0f) /2.0f;
	op.mShadowCoord.y =(-pos.y +1.0f) /2.0f;
	op.mShadowCoord.z =pos.z;

	return op;
}