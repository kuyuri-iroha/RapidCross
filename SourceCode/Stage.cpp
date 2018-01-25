#include "Stage.h"

using namespace CWE;


// 重力値
const float Stage::smGravityAcceleration =(9.80665f /2) /15;
// 摩擦
const float Stage::smGroundFriction =0.05f;


// コンストラクタ
Stage::Stage(BattleStage stage)
{
	switch(stage)
	{
	case BattleStage::StarryRuins:
		mModel.Load(L"Resource\\stage37\\月齢ステージ_遺跡風(改変).pmx", FileFormat::PMX);
		mSkyDome.Load(L"Resource\\stage37\\Dome_CC901(ゴリ押し).pmx", FileFormat::PMX);
		break;

	case BattleStage::Unknown:
		CWE_ASSERT(false); //不明な列挙子
		break;

	default:
		CWE_ASSERT(false); //未実装
		break;
	}
}


// デストラクタ
Stage::~Stage()
{
	mModel.Release();
	mSkyDome.Release();
}


// 素材全体のロード結果を表示
bool Stage::AllResourceLoadResult(Task equal) const
{
	return (mModel.LoadResult() == equal || 
		mSkyDome.LoadResult() == equal);
}


// ロードの状況を通知
bool Stage::LoadSuccess() const
{
	if(AllResourceLoadResult(Task::Failed))
	{
		FatalError::GetInstance().Outbreak(L"Failed to load model data.\nMay file is corrupted.");

		return false;
	}
	else if(AllResourceLoadResult(Task::Working))
	{
		return false;
	}

	return true;
}


// 更新
void Stage::Update()
{

}


// ステージモデルの描画
void Stage::DrawStage(Information& info) const
{
	CWE_ASSERT(info.mModelShader.GetCBData());

	// ワールド変換
	CWE::math::MatrixTranspose(&info.mModelShader.GetCBData()->mWorld, mLocal);

	info.mModelShader.GetCBData()->mSpecular =0.5f;

	// コンスタントバッファデータの適用
	info.mModelShader.mCB.Map();
	CopyMemory(info.mModelShader.mCB.Access(), info.mModelShader.GetCBData(), sizeof(*info.mModelShader.GetCBData()));
	info.mModelShader.mCB.Unmap();

	// 描画
	mModel.Draw(true);
}


// スカイドームモデルの描画
void Stage::DrawSkyDome(Information& info) const
{
	CWE_ASSERT(info.mModelShader.GetCBData());

	// ワールド変換
	CWE::math::WorldTp(&info.mModelShader.GetCBData()->mWorld, {0.0f, 0.0f, 0.0f}, WORLD_SCALE);

	// コンスタントバッファデータの適用
	info.mModelShader.mCB.Map();
	CopyMemory(info.mModelShader.mCB.Access(), info.mModelShader.GetCBData(), sizeof(*info.mModelShader.GetCBData()));
	info.mModelShader.mCB.Unmap();

	// 描画
	mSkyDome.Draw(false);
}