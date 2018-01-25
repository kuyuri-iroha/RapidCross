#include "Player.h"
#include "Information.h"
#include "DebugString.h"
#include "Controller.h"
#include "../CWE/CWEMath.h"




// スタティック定数
const wchar_t* const PlayerBase::smFailedModelMs =L"Failed to load model data.\nMay file is corrupted.";
const wchar_t* const PlayerBase::smFailedMotionMs =L"Failed to load motion data.\nMay file is corrupted.";


// コンストラクタ
PlayerBase::PlayerBase(bool left, Controller* pController, const wchar_t* const filePath):
	mpController(pController), mIsRight(left), mHP(smMaxHP)
{
	mModel.Load(filePath, CWE::FileFormat::PMX);

	mPos.x() =left ? -70.0f : 70.0f;
}


// デストラクタ
PlayerBase::~PlayerBase()
{
	mModel.Release();
}


// 描画
void PlayerBase::Draw(const CWE::math::Float4x4& stage) const
{
	Information& info =Information::GetInfo();
	CWE_ASSERT(info.mModelShader.GetCBData() && info.mModelShader.GetBoneCBData());

	//ワールド変換
	CWE::math::MatrixMultiplyTp(&info.mModelShader.GetCBData()->mWorld, mLocal, stage);

	//ボーン情報の適用
	const std::vector<CWE::Bone>& boneData =mModel.GetBoneData(nullptr);
	for(unsigned i=0; i<boneData.size(); ++i)
	{
		if(MODEL_BONE_MAX <= i) {break;}
		CWE::math::MatrixTranspose(&info.mModelShader.GetBoneCBData()->mBoneMat[i], boneData[i].mMatrix);
	}

	//スペキュラなし
	info.mModelShader.GetCBData()->mSpecular =0.0f;

	//コンスタントバッファデータの適用
	info.mModelShader.mCB.Map();
	CopyMemory(info.mModelShader.mCB.Access(), info.mModelShader.GetCBData(), sizeof(*info.mModelShader.GetCBData()));
	info.mModelShader.mCB.Unmap();
	info.mModelShader.mBoneCB.Map();
	CopyMemory(info.mModelShader.mBoneCB.Access(), info.mModelShader.GetBoneCBData(), sizeof(*(info.mModelShader.GetBoneCBData())));
	info.mModelShader.mBoneCB.Unmap();

	//描画
	mModel.Draw(false);
}