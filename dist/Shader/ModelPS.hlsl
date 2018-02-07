

#define PI 3.1415926535f



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



struct PSInput
{
	float4 mPos : SV_POSITION; //頂点座標(透視変換座標)
	float2 mUV : TEXCOORD0; //テクスチャ座標
	float3 mNorm : TEXCOORD1; //法線ベクトル
	float3 mViewDir : TEXCOORD2; //視線ベクトル
	float3 mShadowCoord : TEXCOORD3; //シャドウマップ座標
};


struct PSOutput
{
	float4 mColor : SV_TARGET;
};


// テクスチャ
Texture2D Texture : register(t0);
SamplerState Sampler : register(s0);
Texture2D ShadowMap : register(t1);
SamplerComparisonState CmpSampler : register(s1);



// ピクセルシェーダ関数
PSOutput ModelPS(PSInput ip)
{
	PSOutput op;
	float4 diffuse =Texture.Sample(Sampler, ip.mUV);
	float NL =max(dot(ip.mNorm, LDir), 0.0f); //法線とライトベクトルとの内積
	float bright =NL*(1.0f/PI) *LStrength +Ambient; //ランバート拡散照明 *光強度 +アンビエント
	bright +=max(dot(normalize((LDir-2.0f)*NL*ip.mNorm), normalize(ip.mViewDir)), 0.0f) *Specular; //+スペキュラ光

	//影描画
	float shadowThreshold =ShadowMap.SampleCmpLevelZero(CmpSampler, ip.mShadowCoord.xy, ip.mShadowCoord.z-0.001f);
	float shadow =lerp(0.0f, 1.0f, shadowThreshold);
	shadow =shadow+Ambient;
	shadow =saturate(shadow);

	//ディフューズ *明るさ *光色 *影の明るさ
	op.mColor.rgb =diffuse.rgb *bright *LColor.rgb *shadow;
	op.mColor.a =diffuse.a;

	return op;
}