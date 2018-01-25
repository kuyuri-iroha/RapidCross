#include "Motion.h"




// モーションを再生しつつ、移動/回転量を取得
bool Motion::Play(CWE::Polygon& applied, bool order, float speed, CWE::math::Matrix& movement)
{
	using namespace CWE::math;

	//モーションの適用処理
	const bool result =applied.MotionUpdate(mMotion, mNowFrame);
	std::vector<CWE::Bone>& bones =applied.GetBoneData(nullptr);
	Matrix inv;
	if(!CWE::math::MatrixIsIdentity(mOldPos))
	{
		inv =MatrixInverse(LoadMatrix(mOldPos));
		movement =MatrixMultiply(inv, LoadMatrix(applied.GetBone(0).mPose));
		mOldPos =bones[0].mPose;
	}
	else
	{
		mOldPos =mOldPos =bones[0].mPose;
		movement =MatrixIdentity();
	}

	//全ての親ボーンの変更を無効化
	MatrixIdentity(bones[0].mPose);

	//変更を全てのボーンに適用
	applied.FKApply();

	//再生中フレームを更新
	if(result)
	{
		mNowFrame +=order?speed:-speed;
	}

	return result;
}


// モーションを適用しつつ、移動/回転量を取得
bool Motion::Apply(CWE::Polygon& applied, float frame, CWE::math::Matrix& movement)
{
	using namespace CWE::math;

	//再生中フレームを更新
	mNowFrame =frame;

	//モーションの適用処理
	const bool result =applied.MotionUpdate(mMotion, mNowFrame);
	std::vector<CWE::Bone>& bones =applied.GetBoneData(nullptr);
	Matrix inv;
	if(!CWE::math::MatrixIsIdentity(mOldPos))
	{
		inv =MatrixInverse(LoadMatrix(mOldPos));
		movement =MatrixMultiply(inv, LoadMatrix(applied.GetBone(0).mPose));
		mOldPos =bones[0].mPose;
	}
	else
	{
		mOldPos =mOldPos =bones[0].mPose;
		movement =MatrixIdentity();
	}

	//全ての親ボーンの変更を無効化
	MatrixIdentity(bones[0].mPose);

	//変更を全てのボーンに適用
	applied.FKApply();

	return result;
}


// リスタート
void Motion::Restart()
{
	mMotion.Restart();
	mNowFrame =0.0f;
	CWE::math::MatrixIdentity(mOldPos);
}



// ============================ MotionInter ======================================

// コンストラクタ
MotionInter::MotionInter():
	mNowCount(0), mInterCount(0)
{

}

// デストラクタ
MotionInter::~MotionInter()
{

}


// リセット
void MotionInter::Reset(unsigned interCount)
{
	if(interCount == 0) {interCount =1;} //0フレームは1フレームと同義に
	mInterCount =interCount+1; //始まりと終わりを含まない為
	mNowCount =mInterCount-1;
}


// 実行
bool MotionInter::Exec(CWE::Polygon& model, CWE::Motion& from, CWE::Motion& to)
{
	model.MotionLerp(to, from, float(mNowCount)/float(mInterCount));
	--mNowCount;
	if(mNowCount == 0) {return false;} //補間終了

	return true;
}