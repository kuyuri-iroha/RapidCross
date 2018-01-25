#pragma once



#include "../CWE/CWELib.h"
#include "Task.h"
#include "Static.h"
#include <array>


// モデルシェーダ
struct ModelShader final
{
private:
	struct CBData
	{
		CWE::math::Float4x4 mWorld;
		CWE::math::Float4x4 mView;
		CWE::math::Float4x4 mProjection;

		CWE::math::Float4 mEyePos;

		float m_Ambient;
		CWE::math::Float3 mDiffuse;
		CWE::math::Float4 mDiffuseColor;
		float mStrength;
		float mSpecular;
		CWE::math::Float2 dammy;
	};
	struct BoneCBData
	{
		CWE::math::Float4x4 mBoneMat[MODEL_BONE_MAX];
	};
	struct ShadowCBData
	{
		CWE::math::Float4x4 mView;
		CWE::math::Float4x4 mProjection;
	};

	CBData* mpCBData;
	BoneCBData* mpBoneCBData;
	ShadowCBData* mpShadowCBData;

public:
	CWE::IAAndVS mVS;
	CWE::IAAndVS mShadowVS;
	CWE::IAAndVS mStageVS;
	CWE::IAAndVS mStageShadowVS;
	CWE::PS mPS;
	CWE::PS mDomePS;
	CWE::ConstantBuffer mCB;
	CWE::ConstantBuffer	mBoneCB;
	CWE::ConstantBuffer mShadowCB;
	CWE::SamplerState mSampler;
	CWE::SamplerState mShadowMapSampler;

	ModelShader();
	~ModelShader();

	void Create();
	void Begin(ID3D11ShaderResourceView* shadowMapTex) const;
	void End() const;
	void BeginShadow() const;
	void BeginStage(ID3D11ShaderResourceView* shadowMapTex) const;
	void BeginStageShadow() const;
	void BeginDome() const;
	void EnableConstantBuffer();
	void DisableConstantBuffer();
	inline CBData* const GetCBData() {return mpCBData;}
	inline BoneCBData* const GetBoneCBData() {return mpBoneCBData;}
	inline ShadowCBData* const GetShadowCBData() {return mpShadowCBData;}

};


// UI shader
struct UIShader
{
private:
	struct CBData
	{
		CWE::math::Float4x4 mWorld;
		float mParam;
		float mAlpha;
		float mTime;
		float dammy;

		CBData(): mWorld(), mParam(), mAlpha(1.0f), dammy() {};
		~CBData() =default;
	};
	struct StaticCBData
	{
		CWE::math::Float4x4 mProjection;
	};

	CBData* mpCBData;
	StaticCBData* mpStaticCBData;

public:
	enum PSIndex
	{
		PS_Picture,
		PS_Loading,
		PS_Title,
		PS_Result,
		NumberOfPS,
		PS_Unknown
	};
	CWE::IAAndVS mVS;
	std::array<CWE::PS, NumberOfPS> mPixShaders;
	CWE::ConstantBuffer mCB;
	CWE::ConstantBuffer mStaticCB;
	CWE::SamplerState mSampler;

	UIShader();
	~UIShader();

	void Create();
	void Begin() const;
	void Begin(PSIndex index) const;
	void End() const;
	void EnableConstantBuffer();
	void DisableConstantBuffer();
	void UpdateProjection();
	inline CBData* const GetCBData() {return mpCBData;}
	inline StaticCBData* const GetStaticCBData() {return mpStaticCBData;}

};


// Ending Parameter shader
struct EndParamShader
{
private:
	struct CBData
	{
		CWE::math::Float4x4 mWorld;
		float mParam;
		CWE::math::Float3 dammy;
	};
	struct StaticCBData
	{
		CWE::math::Float4x4 mProjection;
	};

	CBData* mpCBData;
	StaticCBData* mpStaticCBData;

public:
	CWE::IAAndVS mVS;
	CWE::PS mPS;
	CWE::ConstantBuffer mCB,
						mStaticCB;
	CWE::SamplerState mSampler;

	EndParamShader() =default;
	~EndParamShader();

	void Create();
	void Begin() const;
	void End() const;
	void EnableConstantBuffer();
	void DisableConstantBuffer();
	inline CBData* const GetCBData() {return mpCBData;}
	inline StaticCBData* const GetStaticCBData() {return mpStaticCBData;}
};


// HP Parameter Shader
struct HPParamShader
{
private:
	struct CBData
	{
		CWE::math::Float4x4 mWorld;
		float mParam;
		CWE::math::Float3 dammy;
	};
	struct StaticCBData
	{
		CWE::math::Float4x4 mProjection;
	};

	CBData* mpCBData;
	StaticCBData* mpStaticCBData;

public:
	CWE::IAAndVS mVS;
	CWE::PS mPS;
	CWE::ConstantBuffer mCB,
						mStaticCB;
	CWE::SamplerState mSampler;

	HPParamShader() =default;
	~HPParamShader();

	void Create();
	void Begin() const;
	void End() const;
	void EnableConstantBuffer();
	void DisableConstantBuffer();
	inline CBData* const GetCBData() {return mpCBData;}
	inline StaticCBData* const GetStaticCBData() {return mpStaticCBData;}
};


// ゲーム全般の情報を所持
struct Information final
{
public:
	const float mMagnifX,
				mMagnifY;
	bool mEndRequest;
	ModelShader mModelShader;
	UIShader mUIShader;
	EndParamShader mEndParamShader;
	HPParamShader mHPParamShader;
	TaskName mRuningScene;
	enum Player
	{
		Player_1,
		Player_2,
		Player_None,
		NumberOfPlayer,
		PlayerUnknown,
	};
	Player mWinner;
	bool mEnded;
	bool mLoading;


	static Information& GetInfo()
	{
		static Information info; return info;
	}

	bool DebugCharsLoaded() const;
	void DrawDebugChar(unsigned index) const;

private:
	static const unsigned DEBUG_CHAR_NUM =37;
	std::array<CWE::Polygon, DEBUG_CHAR_NUM> mDebugChars;
	CWE::math::Float3 mDebugStringPos;

	Information();
	~Information();

};