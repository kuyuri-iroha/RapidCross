#include "Gumi.h"
#include "Stage.h"
#include "Information.h"
#include <algorithm>



// ================================== Gumi ================================================

using namespace CWE;

// 重量
const float Gumi::smWeight =50.0f;
// ジャンプ力
const float Gumi::smJumpingAbility1 =3.0f;
const float Gumi::smJumpingAbility2 =3.0f;


const wchar_t gGumi1[36] =L"Resource\\m_GUMI_V3_201306\\GUMI1.pmx";
const wchar_t gGumi2[36] =L"Resource\\m_GUMI_V3_201306\\GUMI2.pmx";

// GUMIの色決定
auto ColorNumToFile(unsigned colNum)
{
	switch(colNum)
	{
	case 0:
		return gGumi1;

	case 1:
		return gGumi2;
	}

	return L"";
}

// コンストラクタ
Gumi::Gumi(bool left, Controller* controller, unsigned colorNumber):
	PlayerBase(left, controller, ColorNumToFile(colorNumber)),
	mInterFrom(MotionType::Idling), mInterTo(MotionType::NoMotion),
	mNowMotionFrame(0.0f), mMotionEnded(false),
	mNowMotionType(MotionType::Idling), mPrevMotionType(MotionType::Idling),
	mMoveSpeedX(), mMoveSpeedY(), mJumpedTwice(), mMoveBack(false)
{
	//モーションデータのロード
	mMotions[mInd_Idling].Load(L"Resource\\Motion\\Idling.vmd", FileFormat::VMD);
	mMotions[mInd_Walk].Load(L"Resource\\Motion\\Walk.vmd", FileFormat::VMD);
	mMotions[mInd_TurnAround].Load(L"Resource\\Motion\\TurnAround.vmd", FileFormat::VMD);
	mMotions[mInd_Dash].Load(L"Resource\\Motion\\Dash.vmd", FileFormat::VMD);
	mMotions[mInd_ToDash1].Load(L"Resource\\Motion\\ToDash1.vmd", FileFormat::VMD);
	mMotions[mInd_Brake1].Load(L"Resource\\Motion\\Brake1.vmd", FileFormat::VMD);
	mMotions[mInd_Brake2].Load(L"Resource\\Motion\\Brake2.vmd", FileFormat::VMD);
	mMotions[mInd_Brake2ToIdling].Load(L"Resource\\Motion\\Brake2ToIdling.vmd", FileFormat::VMD);
	mMotions[mInd_ToDash2].Load(L"Resource\\Motion\\ToDash2.vmd", FileFormat::VMD);
	mMotions[mInd_JumpPre].Load(L"Resource\\Motion\\JumpPre.vmd", FileFormat::VMD);
	mMotions[mInd_Jump1].Load(L"Resource\\Motion\\Jump1.vmd", FileFormat::VMD);
	mMotions[mInd_Jump2].Load(L"Resource\\Motion\\Jump2.vmd", FileFormat::VMD);
	mMotions[mInd_Air].Load(L"Resource\\Motion\\Air.vmd", FileFormat::VMD);
	mMotions[mInd_Landing].Load(L"Resource\\Motion\\Landing.vmd", FileFormat::VMD);
	mMotions[mInd_SideWeakPunch1].Load(L"Resource\\Motion\\Side_Weak_Punch1.vmd", FileFormat::VMD);
	mMotions[mInd_SideWeakPunch1ToIdling].Load(L"Resource\\Motion\\Side_Weak_Punch1_To_Idling.vmd", FileFormat::VMD);
	mMotions[mInd_SideWeakPunch2].Load(L"Resource\\Motion\\Side_Weak_Punch2.vmd", FileFormat::VMD);
	mMotions[mInd_SideWeakPunch2ToIdling].Load(L"Resource\\Motion\\Side_Weak_Punch2_To_Idling.vmd", FileFormat::VMD);
	mMotions[mInd_DashAttack1].Load(L"Resource\\Motion\\DashAttack1.vmd", FileFormat::VMD);
	mMotions[mInd_DashAttackImpact].Load(L"Resource\\Motion\\DashAttackImpact.vmd", FileFormat::VMD);
	mMotions[mInd_DashAttack2].Load(L"Resource\\Motion\\DashAttack2.vmd", FileFormat::VMD);
	mMotions[mInd_DashAttackToIdling].Load(L"Resource\\Motion\\DashAttackToIdling.vmd", FileFormat::VMD);
	mMotions[mInd_SideAttack1].Load(L"Resource\\Motion\\SideAttack1.vmd", FileFormat::VMD);
	mMotions[mInd_SideAttackImpact1].Load(L"Resource\\Motion\\SideAttackImpact1.vmd", FileFormat::VMD);
	mMotions[mInd_SideAttack2].Load(L"Resource\\Motion\\SideAttack2.vmd", FileFormat::VMD);
	mMotions[mInd_SideAttackImpact2].Load(L"Resource\\Motion\\SideAttackImpact2.vmd", FileFormat::VMD);
	mMotions[mInd_SideAttack3].Load(L"Resource\\Motion\\SideAttack3.vmd", FileFormat::VMD);
	mMotions[mInd_UpperAttack].Load(L"Resource\\Motion\\UpperAttack.vmd", FileFormat::VMD);
	mMotions[mInd_AerialSideWeakAttack].Load(L"Resource\\Motion\\AerialSideWeakAttack.vmd", FileFormat::VMD);
	mMotions[mInd_AerialLowerAttack].Load(L"Resource\\Motion\\AerialLowerAttack.vmd", FileFormat::VMD);
	mMotions[mInd_LowerAttackBackStep].Load(L"Resource\\Motion\\LowerAttackBackStep.vmd", FileFormat::VMD);
	mMotions[mInd_Guard].Load(L"Resource\\Motion\\Guard.vmd", FileFormat::VMD);
	mMotions[mInd_Damaged].Load(L"Resource\\Motion\\Damaged.vmd", FileFormat::VMD);
	mMotions[mInd_GreatDamaged].Load(L"Resource\\Motion\\GreatDamaged.vmd", FileFormat::VMD);

	//サウンドデータのロード
	mSoundEff[seInd_Landing].Load(L"Resource\\Sound\\GUMI\\landing.wav", FileFormat::WAVE);
	mSoundEff[seInd_Attack].Load(L"Resource\\Sound\\GUMI\\attack.wav", FileFormat::WAVE);
	mSoundEff[seInd_Damage].Load(L"Resource\\Sound\\GUMI\\damage.wav", FileFormat::WAVE);
	mSoundEff[seInd_GreatDamage].Load(L"Resource\\Sound\\GUMI\\greateDamage.wav", FileFormat::WAVE);
	mSoundEff[seInd_Jump].Load(L"Resource\\Sound\\GUMI\\jump.wav", FileFormat::WAVE);
	mSoundEff[seInd_UpperKick].Load(L"Resource\\Sound\\GUMI\\upperKick.wav", FileFormat::WAVE);
	mSoundEff[seInd_LegDrop].Load(L"Resource\\Sound\\GUMI\\legDrop.wav", FileFormat::WAVE);
	mSoundEff[seInd_Guard].Load(L"Resource\\Sound\\GUMI\\guard.wav", FileFormat::WAVE);
	mSoundEff[seInd_Footsteps].Load(L"Resource\\Sound\\GUMI\\footsteps.wav", FileFormat::WAVE);
	mSoundEff[seInd_Brake].Load(L"Resource\\Sound\\GUMI\\brake.wav", FileFormat::WAVE);

	//コリジョンの設定
	mUpperBody.mRadius =2.5f;
	mLowerBody.mRadius =2.5f;
	mUpperBody.mAggression =false;
	mLowerBody.mAggression =false;

	mAggressors.resize(agInd_TheNumber);
	mAggressors[agInd_RightArm].mRadius =1.5f;
	mAggressors[agInd_RightArm].mAggression =false;
	mAggressors[agInd_LeftArm].mRadius =1.5f;
	mAggressors[agInd_LeftArm].mAggression =false;
	mAggressors[agInd_RightLeg].mRadius =3.5f;
	mAggressors[agInd_RightLeg].mAggression =false;
	mAggressors[agInd_LeftLeg].mRadius =3.5f;
	mAggressors[agInd_LeftLeg].mAggression =false;
}


// デストラクタ
Gumi::~Gumi()
{
	for(auto& se : mSoundEff)
	{
		se.Release();
	}
	for(auto& motion : mMotions)
	{
		motion.Release();
	}
}


// 当たり判定の更新
void Gumi::CollisionUpdate()
{
	using namespace math;
	auto gumiBones =mModel.GetBoneData(nullptr);

	Float3 pose{};
	QuaternionA qRotate;
	if(!mIsRight)
	{
		QuaternionRotationRollPitchYaw(&qRotate, 0.0f, ConvertToRadians(180.0f), 0.0f);
	}

	//被ダメージ
		//頭
	VectorConvert(&pose, gumiBones[90].mPose(3));
	Vector3Rotate(&pose, pose, qRotate);
	VectorAdd(&mUpperBody.mSegment.mP1, pose, mPos);

		//上半身
	VectorConvert(&pose, gumiBones[27].mPose(3));
	Vector3Rotate(&pose, pose, qRotate);
	VectorAdd(&mUpperBody.mSegment.mP2, pose, mPos);

		//下半身
	VectorConvert(&pose, gumiBones[4].mPose(3));
	Vector3Rotate(&pose, pose, qRotate);
	VectorAdd(&mLowerBody.mSegment.mP1, pose, mPos);

		//全ての親
	VectorConvert(&pose, gumiBones[0].mPose(3));
	Vector3Rotate(&pose, pose, qRotate);
	mLowerBody.mSegment.mP2 =mPos;

	//与ダメージ
		//右腕
	VectorConvert(&pose, gumiBones[32].mPose(3));
	Vector3Rotate(&pose, pose, qRotate);
	VectorAdd(&mAggressors[agInd_RightArm].mSegment.mP1, pose, mPos);

		//右手首
	VectorConvert(&pose, gumiBones[42].mPose(3));
	Vector3Rotate(&pose, pose, qRotate);
	VectorAdd(&mAggressors[agInd_RightArm].mSegment.mP2, pose, mPos);
	
		//左腕
	VectorConvert(&pose, gumiBones[61].mPose(3));
	Vector3Rotate(&pose, pose, qRotate);
	VectorAdd(&mAggressors[agInd_LeftArm].mSegment.mP1, pose, mPos);

		//左手首
	VectorConvert(&pose, gumiBones[71].mPose(3));
	Vector3Rotate(&pose, pose, qRotate);
	VectorAdd(&mAggressors[agInd_LeftArm].mSegment.mP2, pose, mPos);

		//右足
	VectorConvert(&pose, gumiBones[20].mPose(3));
	Vector3Rotate(&pose, pose, qRotate);
	VectorAdd(&mAggressors[agInd_RightLeg].mSegment.mP1, pose, mPos);

		//右足首
	VectorConvert(&pose, gumiBones[22].mPose(3));
	Vector3Rotate(&pose, pose, qRotate);
	VectorAdd(&mAggressors[agInd_RightLeg].mSegment.mP2, pose, mPos);

		//左足
	VectorConvert(&pose, gumiBones[24].mPose(3));
	Vector3Rotate(&pose, pose, qRotate);
	VectorAdd(&mAggressors[agInd_LeftLeg].mSegment.mP1, pose, mPos);

		//左足首
	VectorConvert(&pose, gumiBones[26].mPose(3));
	Vector3Rotate(&pose, pose, qRotate);
	VectorAdd(&mAggressors[agInd_LeftLeg].mSegment.mP2, pose, mPos);
}


// 衝突への反応
void Gumi::CollideReaction()
{
#define LOW_IMPACT 1.5f
#define HIGH_IMPACT 3.0f

	if(mPhnomenon.mDamaged)
	{
		if(mNowMotionType == MotionType::Guard)
		{
			mMoveSpeedX =mPhnomenon.mDamagedFromRight ? mIsRight?-LOW_IMPACT:LOW_IMPACT : mIsRight?LOW_IMPACT:-LOW_IMPACT;
			if(mMoveSpeedX <= 0.0f) {mMoveBack =true;}
			mPhnomenon.mDamaged =false;
			mSoundEff[seInd_Guard].Start(false, 0, 0);
		}
	}
	else if(mPhnomenon.mGreatDamaged)
	{
		if(mNowMotionType == MotionType::Guard)
		{
			mMoveSpeedX =mPhnomenon.mDamagedFromRight ? mIsRight?-HIGH_IMPACT:HIGH_IMPACT : mIsRight?HIGH_IMPACT:-HIGH_IMPACT;
			if(mMoveSpeedX <= 0.0f) {mMoveBack =true;}
			mPhnomenon.mGreatDamaged =false;
			mSoundEff[seInd_Guard].Start(false, 0, 0);
		}
	}
}


// モーションタイプから対応するモーションインデックスを取得
const Gumi::MotionIndex Gumi::GetMotionIndex(const MotionType mType) const
{
	MotionIndex result;

	switch(mType)
	{
	case MotionType::Idling:
		result =mInd_Idling;
		break;

	case MotionType::Walk:
		result =mInd_Walk;
		break;

	case MotionType::TurnAround:
		result =mInd_TurnAround;
		break;

	case MotionType::Dash:
		result =mInd_Dash;
		break;

	case MotionType::ToDash1:
		result =mInd_ToDash1;
		break;

	case MotionType::Brake1:
		result =mInd_Brake1;
		break;

	case MotionType::Brake2:
		result =mInd_Brake2;
		break;

	case MotionType::Brake2ToIdling:
		result =mInd_Brake2ToIdling;
		break;

	case MotionType::ToDash2:
		result =mInd_ToDash2;
		break;

	case MotionType::JumpPre:
		result =mInd_JumpPre;
		break;

	case MotionType::Jump1:
		result =mInd_Jump1;
		break;

	case MotionType::Jump2:
		result =mInd_Jump2;
		break;

	case MotionType::Air:
		result =mInd_Air;
		break;

	case MotionType::Landing:
		result =mInd_Landing;
		break;

	case MotionType::SideWeakPunch1:
		result =mInd_SideWeakPunch1;
		break;

	case MotionType::SideWeakPunch1ToIdling:
		result =mInd_SideWeakPunch1ToIdling;
		break;

	case MotionType::SideWeakPunch2:
		result =mInd_SideWeakPunch2;
		break;

	case MotionType::SideWeakPunch2ToIdling:
		result =mInd_SideWeakPunch2ToIdling;
		break;

	case MotionType::DashAttack1:
		result =mInd_DashAttack1;
		break;

	case MotionType::DashAttackImpact:
		result =mInd_DashAttackImpact;
		break;

	case MotionType::DashAttack2:
		result =mInd_DashAttack2;
		break;

	case MotionType::DashAttackToIdling:
		result =mInd_DashAttackToIdling;
		break;

	case MotionType::SideAttack1:
		result =mInd_SideAttack1;
		break;

	case MotionType::SideAttackImpact1:
		result =mInd_SideAttackImpact1;
		break;

	case MotionType::SideAttack2:
		result =mInd_SideAttack2;
		break;

	case MotionType::SideAttackImpact2:
		result =mInd_SideAttackImpact2;
		break;

	case MotionType::SideAttack3:
		result =mInd_SideAttack3;
		break;

	case MotionType::UpperAttack:
		result =mInd_UpperAttack;
		break;

	case MotionType::Guard:
		result =mInd_Guard;
		break;

	case MotionType::Damaged:
		result =mInd_Damaged;
		break;
		
	case MotionType::GreatDamaged:
		result =mInd_GreatDamaged;
		break;

	default:
		result =mInd_Unknown;
		break;
	}

	return result;
}


// 補間モーションの設定
void Gumi::SetInterMotion(MotionType from, MotionType to, unsigned frame, MotionType interType)
{
	CWE_ASSERT(interType == MotionType::Inter || interType == MotionType::InterNoIncpt);

	mInterFrom =(GetMotionIndex(from) != mInd_Unknown) ? from : mInterTo;
	mInterTo =to;
	mInter.Reset(frame);
	mNowMotionType =interType;
}


// モーションの再生
void Gumi::PlayMotion(CWE::Motion& motion)
{
	if(mNowMotionType != mPrevMotionType) //変更されていたら
	{
		motion.Restart();
	}

	if(!mModel.MotionUpdate(motion, mNowMotionFrame)) //再生が終了したら
	{
		if(mNowMotionType == MotionType::Idling ||
			mNowMotionType == MotionType::Dash ||
			mNowMotionType == MotionType::Walk)
		{
			motion.Restart();
			mNowMotionFrame =0.0f;
		}
		mMotionEnded =true;
	}
}


// 補間の再生
void Gumi::PlayInter(CWE::Motion& from, CWE::Motion& to)
{
	if(!mInter.Exec(mModel, from, to)) //再生が終了したら
	{
		mMotionEnded =true;
	}
}


// 入力に対するモーションの変更
void Gumi::InputReaction(RequestMotion& request)
{
	// モーションの変更
	if(GetController()->LowerAttack()) {request =RequestMotion::LowerAttack;}
	else if(GetController()->UpperAttack()) {request =RequestMotion::UpperAttack;}
	else if(GetController()->SideAttack(mIsRight)) {request =RequestMotion::SideAttack;}
	else if(GetController()->SideWeakAttack()) {request =RequestMotion::SideWeakAttack;}
	else if(GetController()->Guard()) {request =RequestMotion::Guard;}
	else if(GetController()->Jump()) {request =RequestMotion::Jump;}
	else if(GetController()->Reverse(mIsRight)) {request =RequestMotion::TurnAround;}
	else if(GetController()->Dash(mIsRight)) {request =RequestMotion::Dash;}
	else if(GetController()->Walk(mIsRight)) {request =RequestMotion::Walk;}
	else {request =RequestMotion::ToNext;}
}


// 受動的に起こるモーションの変更
void Gumi::PassiveReaction(PassiveRequest& request)
{
	if(mPhnomenon.mDamaged)
	{
		request =PassiveRequest::Damaged;
		mHP -=smDamage;
		mPhnomenon.mDamaged =false;
	}
	else if(mPhnomenon.mGreatDamaged)
	{
		request =PassiveRequest::GreatDamaged;
		mHP -=smDamage *7;
		mPhnomenon.mGreatDamaged =false;
	}
	else if((mNowMotionType == MotionType::Jump1 || mNowMotionType == MotionType::Jump2 || mNowMotionType == MotionType::Air || mNowMotionType == MotionType::AerialLowerAttack) &&
		mPos.y() <= 0.0f)
	{
		request =PassiveRequest::Landing;
	}
}


#define WALK_SPEED 0.15f
#define DASH_SPEED 2.00f
#define BRAKE_FRICTION 1.5f
#define WEIGHT_REF_VAL 30.0f

// モーションの更新
void Gumi::MotionUpdate(bool& motionMoveX, bool& motionMoveY, float& friction)
{
	if(mNowMotionType != mPrevMotionType) //モーションが切り替わった場合
	{
		mNowMotionFrame =0.0f;
	}
	else
	{
		mNowMotionFrame +=0.5f;
	}

	//モーションの再生
	switch(mNowMotionType)
	{
	//アイドリング
	case MotionType::Idling:
		PlayMotion(mMotions[mInd_Idling]);
		break;

	//歩行
	case MotionType::Walk:
		PlayMotion(mMotions[mInd_Walk]);
		if(math::NearlyEqual(13.000, mMotions[mInd_Walk].GetNowFrame()))
		{
			mSoundEff[seInd_Footsteps].Start(false, 0, 0);
		}
		else if(math::NearlyEqual(28.000, mMotions[mInd_Walk].GetNowFrame()))
		{
			mSoundEff[seInd_Footsteps].Start(false, 0, 0);
		}
		mMoveSpeedX =WALK_SPEED;
		motionMoveX =true;
		break;

	//振り向き
	case MotionType::TurnAround:
		PlayMotion(mMotions[mInd_TurnAround]);
		break;

	//走行
	case MotionType::Dash:
		PlayMotion(mMotions[mInd_Dash]);
		if(math::NearlyEqual(6.000, mMotions[mInd_Dash].GetNowFrame()))
		{
			mSoundEff[seInd_Footsteps].Start(false, 0, 0);
		}
		else if(math::NearlyEqual(14.000, mMotions[mInd_Dash].GetNowFrame()))
		{
			mSoundEff[seInd_Footsteps].Start(false, 0, 0);
		}
		mMoveSpeedX =DASH_SPEED;
		motionMoveX =true;
		break;

	//走行前の蹴り出し1
	case MotionType::ToDash1:
		PlayMotion(mMotions[mInd_ToDash1]);
		mMoveSpeedX =DASH_SPEED +1.0f;
		motionMoveX =true;
		break;

	//その場ブレーキ
	case MotionType::Brake1:
		if(!(mMoveSpeedX <= 0.0f)) //停止していなかったら
		{
			mNowMotionFrame =0.0f;
		}
		PlayMotion(mMotions[mInd_Brake1]);
		friction =BRAKE_FRICTION;
		break;

	//振り向きブレーキ
	case MotionType::Brake2:
		if(9.0f <= mNowMotionFrame && !(mMoveSpeedX <= 0.0f)) //特定のフレームに達していて停止していなかったら
		{
			mNowMotionFrame -=0.5; //モーションを停止
		}
		PlayMotion(mMotions[mInd_Brake2]);
		friction =BRAKE_FRICTION;
		break;

	//振り向きブレーキからアイドリング
	case MotionType::Brake2ToIdling:
		PlayMotion(mMotions[mInd_Brake2ToIdling]);
		break;

	//走行前の蹴り出し2
	case MotionType::ToDash2:
		PlayMotion(mMotions[mInd_ToDash2]);
		mMoveSpeedX =DASH_SPEED +1.0f;
		motionMoveX =true;
		break;

	//ジャンプの予備動作
	case MotionType::JumpPre:
		PlayMotion(mMotions[mInd_JumpPre]);
		break;

	//ジャンプ1段
	case MotionType::Jump1:
		PlayMotion(mMotions[mInd_Jump1]);
		break;

	//ジャンプ2段
	case MotionType::Jump2:
		PlayMotion(mMotions[mInd_Jump2]);
		break;

	//空中
	case MotionType::Air:
		if(mPos.y()) //着地していなかったら
		{
			mNowMotionFrame =0.0f;
		}
		PlayMotion(mMotions[mInd_Air]);
		break;

	//着地
	case MotionType::Landing:
		PlayMotion(mMotions[mInd_Landing]);
		friction =BRAKE_FRICTION;
		break;

	//横弱攻撃1
	case MotionType::SideWeakPunch1:
		PlayMotion(mMotions[mInd_SideWeakPunch1]);
		mAggressors[agInd_LeftArm].mAggression =true;
		break;

	//横弱攻撃1補間
	case MotionType::SideWeakPunch1ToIdling:
		PlayMotion(mMotions[mInd_SideWeakPunch1ToIdling]);
		break;

	//横弱攻撃2
	case MotionType::SideWeakPunch2:
		PlayMotion(mMotions[mInd_SideWeakPunch2]);
		mAggressors[agInd_RightArm].mAggression =true;
		break;

	//横弱攻撃2補間
	case MotionType::SideWeakPunch2ToIdling:
		PlayMotion(mMotions[mInd_SideWeakPunch2ToIdling]);
		break;

	//ダッシュ攻撃1
	case MotionType::DashAttack1:
		PlayMotion(mMotions[mInd_DashAttack1]);
		friction =0.4f;
		break;

	//ダッシュ攻撃インパクト
	case MotionType::DashAttackImpact:
		PlayMotion(mMotions[mInd_DashAttackImpact]);
		mAggressors[agInd_RightArm].mAggression =true;
		friction =0.7f;
		break;

	//ダッシュ攻撃2
	case MotionType::DashAttack2:
		PlayMotion(mMotions[mInd_DashAttack2]);
		friction =1.5f;
		break;

	//ダッシュ攻撃補間
	case MotionType::DashAttackToIdling:
		PlayMotion(mMotions[mInd_DashAttackToIdling]);
		break;

	//横攻撃1
	case MotionType::SideAttack1:
		PlayMotion(mMotions[mInd_SideAttack1]);
		break;

	//横攻撃インパクト1
	case MotionType::SideAttackImpact1:
		PlayMotion(mMotions[mInd_SideAttack1]);
		mAggressors[agInd_RightArm].mAggression =true;
		break;

	//横攻撃2
	case MotionType::SideAttack2:
		PlayMotion(mMotions[mInd_SideAttack2]);
		break;

	//横攻撃インパクト2
	case MotionType::SideAttackImpact2:
		PlayMotion(mMotions[mInd_SideAttackImpact2]);
		mAggressors[agInd_LeftArm].mAggression =true;
		break;

	//横攻撃2
	case MotionType::SideAttack3:
		PlayMotion(mMotions[mInd_SideAttack3]);
		break;

	//上攻撃
	case MotionType::UpperAttack:
		PlayMotion(mMotions[mInd_UpperAttack]);
		if(math::NearlyEqual(mMotions[mInd_UpperAttack].GetNowFrame(), 2.5f)) //1回目の上昇
		{
			mMoveSpeedY =3.0f;
			mSoundEff[seInd_UpperKick].Start(false, 0, 0);
		}
		else if(math::NearlyEqual(mMotions[mInd_UpperAttack].GetNowFrame(), 5.5f)) //2回目の上昇
		{
			mMoveSpeedY =5.5f;
			mSoundEff[seInd_UpperKick].Start(false, 0, 0);
		}
		else
		{
			if(mMoveSpeedY <= 0.0f)
			{
				mMoveSpeedY =0.0f;
			}
		}
		if(3.5f <= mMotions[mInd_UpperAttack].GetNowFrame() && mMotions[mInd_UpperAttack].GetNowFrame() <= 4.5f) //1回目のインパクト
		{
			mAggressors[agInd_LeftLeg].mAggression =true;
		}
		else if(6.0f <= mMotions[mInd_UpperAttack].GetNowFrame() && mMotions[mInd_UpperAttack].GetNowFrame() <= 7.0f) //2回目のインパクト
		{
			mAggressors[agInd_RightLeg].mAggression =true;
		}
		break;

	//空中下攻撃
	case MotionType::AerialLowerAttack:
		PlayMotion(mMotions[mInd_AerialLowerAttack]);
		if(mMotions[mInd_AerialLowerAttack].GetNowFrame() <= 2.0f)
		{
			mMoveSpeedY =1.0f;
		}
		else
		{
			 mMoveSpeedY =-5.0f;
			 mAggressors[agInd_RightLeg].mAggression =true;
		}
		break;

	//空中下攻撃バックステップ
	case MotionType::LowerAttackBackStep:
		PlayMotion(mMotions[mInd_LowerAttackBackStep]);
		if(2.0f <= mMotions[mInd_LowerAttackBackStep].GetNowFrame())
		{
			mMoveSpeedX =-0.7f;
			motionMoveX =true;
		}
		break;

	//ガード
	case MotionType::Guard:
		PlayMotion(mMotions[mInd_Guard]);
		break;

	//ダメージ
	case MotionType::Damaged:
		PlayMotion(mMotions[mInd_Damaged]);
		break;

	//大ダメージ
	case MotionType::GreatDamaged:
		PlayMotion(mMotions[mInd_GreatDamaged]);
		break;

	//補間系統
	case MotionType::Inter:
	case MotionType::InterNoIncpt:
		PlayInter(mMotions[GetMotionIndex(mInterFrom)], mMotions[GetMotionIndex(mInterTo)]);
		break;
	}
}


#define INTER_FRAME 5

//モーションの移行
void Gumi::MotionChange(RequestMotion requested, PassiveRequest psRequested)
{
#define D_LOW_IMPACT 0.5f
#define D_HIGH_IMPACT 4.0f

	//変更要求の実行
	switch(psRequested)
	{
	//着地
	case PassiveRequest::Landing:
		if(mNowMotionType == MotionType::AerialLowerAttack)
		{
			mNowMotionType =MotionType::LowerAttackBackStep;
			mSoundEff[seInd_LegDrop].Start(false, 0, 0);
			mMotionEnded =false;
		}
		break;

	//ダメージ
	case PassiveRequest::Damaged:
		if(mNowMotionType != MotionType::Damaged &&
			mNowMotionType != MotionType::GreatDamaged &&
			mNowMotionType != MotionType::Guard)
		{
			mNowMotionType =MotionType::Damaged;
			mSoundEff[seInd_Damage].Start(false, 0, 0);
			if(mPhnomenon.mDamagedFromRight ? mIsRight?false:true : mIsRight?true:false)
			{
				mIsRight =!mIsRight;
			}
			mMoveSpeedX =-D_LOW_IMPACT;
			if(mMoveSpeedX <= 0.0f) {mMoveBack =true;}
		}
		break;

	//大ダメージ
	case PassiveRequest::GreatDamaged:
		if(mNowMotionType != MotionType::Damaged &&
			mNowMotionType != MotionType::GreatDamaged &&
			mNowMotionType != MotionType::Guard)
		{
			mNowMotionType =MotionType::GreatDamaged;
			mSoundEff[seInd_GreatDamage].Start(false, 0, 0);
			if(mPhnomenon.mDamagedFromRight ? mIsRight?false:true : mIsRight?true:false)
			{
				mIsRight =!mIsRight;
			}
			mMoveSpeedX =-D_HIGH_IMPACT;
			if(mMoveSpeedX <= 0.0f) {mMoveBack =true;}
		}
		break;
	}

	if(0 <= static_cast<int>(mNowMotionType)) //変更可能なモーションであれば
	{
		if(mPos.y() <= 0.0f) //地上にいたら
		{
			//受動的モーションリクエストへの移行
			switch(psRequested)
			{
			//着地
			case PassiveRequest::Landing:
				if(mNowMotionType == MotionType::AerialLowerAttack)
				{
					mNowMotionType =MotionType::LowerAttackBackStep;
				}
				else
				{
					mNowMotionType =MotionType::Landing;
					mSoundEff[seInd_Landing].Start(false, 0, 0);
				}
				break;

			default:
				//入力リクエスト処理
				GroundMotionRespond(requested, psRequested);
				break;
			}
		}
		else
		{
			//受動的モーションリクエストへの移行
			switch(psRequested)
			{
			default:
				//入力リクエスト処理
				AerialMotionRespond(requested);
				break;
			}
		}
	}

	if(mMotionEnded)
	{
		//モーションが終了した時の自動移行
		switch(mNowMotionType)
		{
		//割り込み禁止の補間
		case MotionType::InterNoIncpt:
			mNowMotionType =mInterTo; //補間先へそのまま移行
			mInterFrom =mInterTo;
			break;

		//補間
		case MotionType::Inter:
			mNowMotionType =mInterTo; //補間先へそのまま移行
			mInterFrom =mInterTo;
			break;

		//振り向き
		case MotionType::TurnAround:
			mNowMotionType =MotionType::Idling;
			break;

		//走行前の蹴り出し1
		case MotionType::ToDash1:
			mNowMotionType =MotionType::Dash;
			break;

		//その場ブレーキ
		case MotionType::Brake1:
			SetInterMotion(mPrevMotionType, MotionType::Idling, 5, MotionType::Inter);
			break;

		//振り向きブレーキ
		case MotionType::Brake2:
			if(requested == RequestMotion::TurnAround) //逆方向要求が継続的にされていたら
			{
				mNowMotionType =MotionType::ToDash2;
			}
			else
			{
				mNowMotionType =MotionType::Brake2ToIdling;
			}
			mIsRight =!mIsRight;
			break;

		//振り向きブレーキからアイドリング
		case MotionType::Brake2ToIdling:
			mNowMotionType =MotionType::Idling;
			break;

		//走行前の蹴り出し2
		case MotionType::ToDash2:
			mNowMotionType =MotionType::Dash;
			break;

		//ジャンプの予備動作
		case MotionType::JumpPre:
			mMoveSpeedY =smJumpingAbility1 *(smWeight /WEIGHT_REF_VAL);
			mNowMotionType =MotionType::Jump1;
			mSoundEff[seInd_Jump].Start(false, 0, 0);
			break;

		//ジャンプ1段目
		case MotionType::Jump1:
			mNowMotionType =MotionType::Air;
			break;

		//ジャンプ2段目
		case MotionType::Jump2:
			mNowMotionType =MotionType::Air;
			break;

		//着地
		case MotionType::Landing:
			SetInterMotion(MotionType::Landing, MotionType::Idling, 5, MotionType::Inter);
			break;

		//横弱攻撃1
		case MotionType::SideWeakPunch1:
			mNowMotionType =MotionType::SideWeakPunch1ToIdling;
			mSoundEff[seInd_Attack].Start(false, 0, 0);
			break;

		//横弱攻撃1補間
		case MotionType::SideWeakPunch1ToIdling:
			mNowMotionType =MotionType::Idling;
			break;

		//横弱攻撃2
		case MotionType::SideWeakPunch2:
			mNowMotionType =MotionType::SideWeakPunch2ToIdling;
			mSoundEff[seInd_Attack].Start(false, 0, 0);
			break;

		//横弱攻撃2補間
		case MotionType::SideWeakPunch2ToIdling:
			mNowMotionType =MotionType::Idling;
			break;

		//ダッシュ攻撃1
		case MotionType::DashAttack1:
			mNowMotionType =MotionType::DashAttackImpact;
			mSoundEff[seInd_Attack].Start(false, 0, 0);
			break;

		//ダッシュ攻撃インパクト
		case MotionType::DashAttackImpact:
			mNowMotionType =MotionType::DashAttack2;
			break;

		//ダッシュ攻撃2
		case MotionType::DashAttack2:
			mNowMotionType =MotionType::DashAttackToIdling;
			break;

		//ダッシュ攻撃補間
		case MotionType::DashAttackToIdling:
			mNowMotionType =MotionType::Idling;
			break;

		//横攻撃1
		case MotionType::SideAttack1:
			mNowMotionType =MotionType::SideAttackImpact1;
			mSoundEff[seInd_Attack].Start(false, 0, 0);
			break;

		//横攻撃インパクト1
		case MotionType::SideAttackImpact1:
			mNowMotionType =MotionType::SideAttack2;
			break;

		//横攻撃2
		case MotionType::SideAttack2:
			mNowMotionType =MotionType::SideAttackImpact2;
			mSoundEff[seInd_Attack].Start(false, 0, 0);
			break;

		//横攻撃インパクト2
		case MotionType::SideAttackImpact2:
			mNowMotionType =MotionType::SideAttack3;
			break;

		//横攻撃2
		case MotionType::SideAttack3:
			SetInterMotion(MotionType::SideAttack3, MotionType::Idling, 5, MotionType::Inter);
			break;

		//上攻撃
		case MotionType::UpperAttack:
			SetInterMotion(MotionType::UpperAttack, MotionType::Air, 10, MotionType::Inter);
			mJumpedTwice =true;
			mAerialAttacked =true;
			break;

		//空中下攻撃
		case MotionType::AerialLowerAttack:
			//着地を待機
			break;

		//空中下攻撃バックステップ
		case MotionType::LowerAttackBackStep:
			SetInterMotion(MotionType::LowerAttackBackStep, MotionType::Idling, 10, MotionType::Inter);
			break;

		//ガード
		case MotionType::Guard:
			SetInterMotion(mNowMotionType, MotionType::Idling, 20, MotionType::Inter);
			break;

		//ダメージ
		case MotionType::Damaged:
			SetInterMotion(mNowMotionType, MotionType::Idling, 10, MotionType::Inter);
			break;

		//大ダメージ
		case MotionType::GreatDamaged:
			SetInterMotion(mNowMotionType, MotionType::Idling, 10, MotionType::Inter);
			break;

		//特に指定がなければ何もしない
		default:
			break;
		}
	}

	mMotionEnded =false;
}


// 地上でのモーションの移行要求処理
void Gumi::GroundMotionRespond(RequestMotion requested, PassiveRequest psReqested)
{
	//変更時に行う処理
	switch(requested)
	{
	//上攻撃
	case RequestMotion::UpperAttack:
		if(!mUpperAttacked)
		{
			SetInterMotion(mNowMotionType, MotionType::UpperAttack, 2, MotionType::InterNoIncpt);
		}
		break;

	//横攻撃
	case RequestMotion::SideAttack:
		mNowMotionType =MotionType::SideAttack1;
		break;

	//横弱攻撃
	case RequestMotion::SideWeakAttack:
		if(mNowMotionType == MotionType::Dash) //ダッシュ攻撃
		{
			mNowMotionType =MotionType::DashAttack1;
		}
		else if(mNowMotionType == MotionType::SideWeakPunch1ToIdling) //2発目
		{
			mNowMotionType =MotionType::SideWeakPunch2;
		}
		else
		{
			mNowMotionType =MotionType::SideWeakPunch1;
		}
		break;

	//ガード
	case RequestMotion::Guard:
		if(!(mNowMotionType == MotionType::Inter && mInterFrom == MotionType::Guard))
		{
			mNowMotionType =MotionType::Guard;
		}
		break;

	//歩行
	case RequestMotion::Walk:
		if(mNowMotionType == MotionType::Idling)
		{
			mNowMotionType =MotionType::Walk;
		}
		break;

	//振り向き
	case RequestMotion::TurnAround:
		if(WALK_SPEED < mMoveSpeedX) //走っていたら
		{
			//振り向きブレーキへ
			SetInterMotion(mPrevMotionType, MotionType::Brake2, 3, MotionType::InterNoIncpt);
			mSoundEff[seInd_Brake].Start(false, 0, 0);
		}
		else
		{
			mNowMotionType =MotionType::TurnAround;
			mIsRight =!mIsRight;
		}
		break;

	//ダッシュ
	case RequestMotion::Dash:
		if(mNowMotionType != MotionType::Dash &&
			mNowMotionType != MotionType::ToDash1)
		{
			mNowMotionType =MotionType::ToDash1;
		}
		else if(mNowMotionType == MotionType::Brake2ToIdling)
		{
			mNowMotionType =MotionType::ToDash2;
		}
		else //psReqested == PassiveRequest::Landing
		{
			mNowMotionType =MotionType::Dash;
		}
		break;

	//ジャンプ
	case RequestMotion::Jump:
		if(mNowMotionType != MotionType::JumpPre &&
			(mNowMotionType != MotionType::Inter) ? mInterTo != MotionType::JumpPre : true)
		{
			SetInterMotion(mPrevMotionType, MotionType::JumpPre, 1, MotionType::InterNoIncpt);
		}
		break;

	//次のモーションへ
	case RequestMotion::ToNext:
		//次のモーションの選択
		switch(mPrevMotionType)
		{
		//歩行からなら
		case MotionType::Walk:
			SetInterMotion(mPrevMotionType, MotionType::Idling, 5, MotionType::Inter);
			break;

		//走行からなら
		case MotionType::Dash:
			SetInterMotion(mPrevMotionType, MotionType::Brake1, 4, MotionType::Inter);
			mSoundEff[seInd_Brake].Start(false, 0, 0);
			break;

		//無入力の時に遷移するモーションがない場合
		default:
			mNowMotionType =mPrevMotionType;
			break;
		}
		break;

	//何もなかったら何もしない
	default:
		break;
	}
}


// 空中でのモーションの移行要求処理
void Gumi::AerialMotionRespond(RequestMotion requested)
{
	//変更時に行う処理
	switch(requested)
	{
	//下攻撃
	case RequestMotion::LowerAttack:
		mNowMotionType =MotionType::AerialLowerAttack;
		break;

	//ジャンプ
	case RequestMotion::Jump:
		if(!mJumpedTwice)
		{
			mNowMotionType =MotionType::Jump2;
			mMoveSpeedY =smJumpingAbility2 *(smWeight /WEIGHT_REF_VAL);
			mSoundEff[seInd_Jump].Start(false, 0, 0);
		}
		mJumpedTwice =true;
		break;

	//次のモーションへ
	case RequestMotion::ToNext:
		switch(mPrevMotionType)
		{
		//無入力の時に遷移するモーションがない場合
		default:
			mNowMotionType =mPrevMotionType;
			break;
		}
		break;

	//何もなかったら何もしない
	default:
		break;
	}
}


// X軸移動
void Gumi::MoveX(bool motionMove, float friction)
{
	//加速度の算出
	if(mPos.y() <= 0.0f) //地上にいたら
	{
		if(!motionMove)
		{
			if(mMoveBack)
			{
				mMoveSpeedX +=(Stage::smGroundFriction *friction) *(smWeight /WEIGHT_REF_VAL);

				if(0.0f <= mMoveSpeedX)
				{
					mMoveSpeedX =0.0f;

					mMoveBack =false;
				}
			}
			else
			{
				mMoveSpeedX -=(Stage::smGroundFriction *friction) *(smWeight /WEIGHT_REF_VAL);

				if(mMoveSpeedX <= 0.0f)
				{
					mMoveSpeedX =0.0f;
				}
			}
		}
	}
	else
	{
		if(GetController()->Dash(mIsRight))
		{
			mMoveSpeedX =DASH_SPEED /1.5f;
		}
		else if(GetController()->Walk(mIsRight))
		{
			mMoveSpeedX =WALK_SPEED /1.5f;
		}
		else if(GetController()->Back(mIsRight))
		{
			mMoveSpeedX =-DASH_SPEED /1.5f;
		}
		else if(GetController()->SlightlyBack(mIsRight))
		{
			mMoveSpeedX =-WALK_SPEED /1.5f;
		}
		else if(mMoveSpeedX <= 0.0f)
		{
			mMoveSpeedX +=(Stage::smGroundFriction *0.5f) *(smWeight /WEIGHT_REF_VAL);
		}
		else
		{
			mMoveSpeedX -=(Stage::smGroundFriction *0.5f) *(smWeight /WEIGHT_REF_VAL);
		}
	}

	//速度の適用
	if(mIsRight)
	{
		mPos.x() +=mMoveSpeedX;
	}
	else
	{
		mPos.x() -=mMoveSpeedX;
	}
}


#define Y_MOVESPEED_LIMIT 4.0f

// Y軸移動
void Gumi::MoveY()
{
	mMoveSpeedY -=Stage::smGravityAcceleration;
	if(GetController()->Down())
	{
		mPos.y() -=1.0f;
	}

	if(Y_MOVESPEED_LIMIT <= mMoveSpeedY)
	{
		mPos.y() +=Y_MOVESPEED_LIMIT;
	}
	else if(mMoveSpeedY <= -Y_MOVESPEED_LIMIT)
	{
		mPos.y() -=Y_MOVESPEED_LIMIT;
	}
	else
	{
		mPos.y() +=mMoveSpeedY;
	}

	if(mPos.y() < 0.0f)
	{
		mPos.y() =0.0f;
	}
}

// ロードの完了を通知
bool Gumi::LoadSuccess() const
{
	//失敗判定
	if(mModel.LoadResult() == Task::Failed)
	{
		FatalError::GetInstance().Outbreak(smFailedModelMs);
		return false;
	}
	for(const auto& motion : mMotions)
	{
		if(motion.LoadResult() == Task::Failed)
		{
			FatalError::GetInstance().Outbreak(smFailedMotionMs);
			return false;
		}
	}

	//成功判定
	if(mModel.LoadResult() != Task::Success)
	{
		return false;
	}

	for(const auto& motion : mMotions)
	{
		if(motion.LoadResult() != Task::Success)
		{
			return false;
		}
	}

	for(const auto& se : mSoundEff)
	{
		if(se.LoadResult() != Task::Success)
		{
			return false;
		}
	}

	return true;
}


// 現在のモーションが強攻撃かどうか
bool Gumi::IsHighImpact() const
{
	bool result =false;

	switch(mNowMotionType)
	{
	case MotionType::DashAttackImpact:
		result =true;
		break;

	case MotionType::AerialLowerAttack:
		result =true;
		break;

	case MotionType::SideAttackImpact2:
		result =true;
		break;

	case MotionType::UpperAttack:
		result =true;
		break;
	}

	return result;
}


// 更新
void Gumi::Update()
{
	CWE_ASSERT(GetController());

	//初期化
	RequestMotion request;
	PassiveRequest psRequest;
	bool motionMoveX =false,
		 motionMoveY =false;
	float friction =1.0f;
	if(mPos.y()<=0.0f)
	{
		mJumpedTwice =false; //2段ジャンプフラグのリセット
		mAerialAttacked =false; //空中で攻撃フラグのリセット
		mUpperAttacked =false; //上攻撃フラグのリセット
	}
	for(auto& aggressor : mAggressors)
	{
		aggressor.mAggression =false;
	}

	//衝突への反応
	CollideReaction();

	//入力に対するモーションの変更
	InputReaction(request);

	//受動的に起こるモーションの変更
	PassiveReaction(psRequest);

	//モーションの移行
	MotionChange(request, psRequest);

	//モーションの更新
	MotionUpdate(motionMoveX, motionMoveY, friction);

	//移動処理
	MoveY();
	MoveX(motionMoveX, friction);

	//後処理
	mPrevMotionType =mNowMotionType;

	//描画用計算
	using namespace math;
	QuaternionA qPose;
	MatrixIdentity(mLocal);

	if(!mIsRight)
	{
		QuaternionRotationRollPitchYaw(&qPose, 0.0f, ConvertToRadians(180.0f), 0.0f);
	}
	MatrixTranslationR(&mLocal, mPos, qPose);

	//当たり判定の更新
	CollisionUpdate();
}