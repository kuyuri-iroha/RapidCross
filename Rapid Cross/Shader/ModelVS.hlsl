


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

cbuffer g_bone : register(b1)
{
	float4x4 BoneMat[145];
}

cbuffer g_shadow : register(b2)
{
	matrix SView;
	matrix SProjection;
}


struct VSInput
{
	float3 mPos : POSITION; //頂点座標(モデル座標系)
	float2 mUV : TEXCOORD0; //テクスチャ座標
	float3 mNorm : NORMAL; //法線ベクトル
	uint4 mBoneIndx : BLENDINDICES0;//ボーンインデックス
	float4 mWeight : BLENDWEIGHT0;//ボーンウェイト
};


struct VSOutput
{
	float4 mPos : SV_POSITION; //頂点座標(透視変換座標)
	float2 mUV : TEXCOORD0; //テクスチャ座標
	float3 mNorm : TEXCOORD1; //法線ベクトル
	float3 mViewDir : TEXCOORD2; //視線ベクトル
	float3 mShadowCoord : TEXCOORD3; //シャドウマップ座標
};



// ボーンスキニング
float4 BoneSkinning(float4 pos, uint4 bidx, float4 bwgt)
{
	float3 bPos =bwgt.x*mul( pos, BoneMat[bidx.x]);
	bPos +=bwgt.y*mul(pos, BoneMat[bidx.y]);
	bPos +=bwgt.z*mul(pos, BoneMat[bidx.z]);
	bPos +=bwgt.w*mul(pos, BoneMat[bidx.w]);
	return float4(bPos, 1.0f);
}


// ボーンスキニング法線変換
float3 BoneSkinningNorm(float3 nor, uint4 bidx, float4 bwgt)
{
	float3 bnor =bwgt.x*mul(nor, (float3x3)BoneMat[bidx.x]);
	bnor.xyz +=bwgt.y*mul(nor, (float3x3)BoneMat[bidx.y]);
	bnor.xyz +=bwgt.z*mul(nor, (float3x3)BoneMat[bidx.z]);
	bnor.xyz +=bwgt.w*mul(nor, (float3x3)BoneMat[bidx.w]);
	// 単純な回転＆均一スケールの場合のみ有効
	// 正確な法線の変換は逆転置行列で
	return normalize(bnor);
}


// シャドウマップ描画用頂点シェーダ関数
float4 ModelShadowVS(VSInput ip) : SV_POSITION
{
	// スキニング
	float4 pos =BoneSkinning(float4(ip.mPos, 1.0f), ip.mBoneIndx, ip.mWeight);

	// 頂点座標変換
	pos =mul(pos, World);
    pos =mul(pos, View);
	pos =mul(pos, Projection);

	return pos;
}


// 頂点シェーダ関数
VSOutput ModelVS(VSInput ip)
{
	VSOutput op;

	// スキニング
	float4 pos =BoneSkinning(float4(ip.mPos, 1.0f), ip.mBoneIndx, ip.mWeight);

	// 頂点座標変換
	op.mPos =mul(pos, World);
	op.mViewDir =pos.xyz -EyePos.xyz; //視点ベクトルの作成
    op.mPos =mul(op.mPos, View);
	op.mPos =mul(op.mPos, Projection);

	// テクスチャ座標の受け渡し
	op.mUV =ip.mUV;

	// 法線ベクトル変換
	float3 norm =BoneSkinningNorm(ip.mNorm, ip.mBoneIndx, ip.mWeight);
	op.mNorm =mul(norm, (float3x3)World);
	op.mNorm =mul(op.mNorm, (float3x3)View);

	// シャドウマッピング用ベクトルの算出
	pos =mul(pos, World);
	pos =mul(pos, SView);
	pos =mul(pos, SProjection);
	pos.xyz =pos.xyz /pos.w;
	op.mShadowCoord.x =(pos.x +1.0f) /2.0f;
	op.mShadowCoord.y =(-pos.y +1.0f) /2.0f;
	op.mShadowCoord.z =pos.z;

	return op;
}