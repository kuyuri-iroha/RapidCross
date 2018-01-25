#include "CWEViewPort.h"
#include "CWEGraphicManager.h"
#include <algorithm>
#include "CWEError.h"



namespace CWE
{

// ========= 目オブジェクト ==============

// デフォルトコンストラクタ
Eye::Eye():
mPos(0.0f, 0.0f, 10.0f), mFocusPos(), mUpDir(), mNormalizedPos(0.0f, 0.0f, 1.0f), mNormalizedPosIs(false), mDistance(10.0f)
{
}

// コンストラクタ(ベクトルから)
Eye::Eye(const math::Float3& pos, const math::Float3& focusPos, const math::Float3& upDir):
mPos(pos), mFocusPos(focusPos), mUpDir(upDir), mNormalizedPos(0.0f, 0.0f, 1.0f), mNormalizedPosIs(false), mDistance(10.0f)
{
}

// デストラクタ
Eye::~Eye()
{
}


// -------------- 視点座標移動関数 ----------------

//現在位置からの移動
void Eye::PosMove(float x, float y, float z)
{
	mPos.x() +=x;
	mPos.y() +=y;
	mPos.z() +=z;
	mNormalizedPosIs =false;
}

// 位置の変更
void Eye::PosReset(float x, float y, float z)
{
	mPos.x() =x;
	mPos.y() =y;
	mPos.z() =z;
	mNormalizedPosIs =false;
}

// 角度を用いて現在位置から焦点を中心に回転移動
// ( 0<r が有効値)
void Eye::PosMoveByAngle(float xAngle, float yAngle, float r)
{
	//回転半径の変更
	if(0<r)
	{
		mDistance =r;
	}

	// 正規化されていなければ正規化
	if(!mNormalizedPosIs)
	{
		mNormalizedPos.x() =mPos.x() -mFocusPos.x();
		mNormalizedPos.y() =mPos.y() -mFocusPos.y();
		mNormalizedPos.z() =mPos.z() -mFocusPos.z();
		math::VectorNormalize(&mNormalizedPos, mNormalizedPos);
		mNormalizedPosIs =true;
	}
	
	//クォータニオンによる回転
	if(xAngle!=0.0f) {math::QuaternionRotationY(&mNormalizedPos, xAngle);}
	if(yAngle!=0.0f) {math::QuaternionRotationX(&mNormalizedPos, yAngle);}
	
	//正規化ベクトルから視点位置を算出
	mPos.x() =mNormalizedPos.x() *mDistance;
	mPos.y() =mNormalizedPos.y() *mDistance;
	mPos.z() =mNormalizedPos.z() *mDistance;
	math::VectorAdd(&mPos, mPos, mFocusPos);
}



// -------------- 焦点座標移動関数 ----------------

// 現在位置からの移動
void Eye::FocusMove(float x, float y, float z)
{
	mFocusPos.x() +=x;
	mFocusPos.y() +=y;
	mFocusPos.z() +=z;
	mNormalizedPosIs =false;
}

// 位置の変更
void Eye::FocusReset(float x, float y, float z)
{
	mFocusPos.x() =x;
	mFocusPos.y() =y;
	mFocusPos.z() =z;
	mNormalizedPosIs =false;
}

// 視点位置と共に現在位置からの移動
void Eye::FocusMoveWithPos(float x, float y, float z)
{
	mPos.x() +=x;
	mPos.y() +=y;
	mPos.z() +=z;
	mFocusPos.x() +=x;
	mFocusPos.y() +=y;
	mFocusPos.z() +=z;
}


// 視点位置と共に位置の変更
void Eye::FocusResetWithPos(float x, float y, float z)
{
	mPos.x() =x +(mPos.x() -mFocusPos.x());
	mPos.y() =y +(mPos.y() -mFocusPos.y());
	mPos.z() =z +(mPos.z() -mFocusPos.z());
	mFocusPos.x() =x;
	mFocusPos.y() =y;
	mFocusPos.z() =z;
}


// --------- 上方向設定関数 ---------------

// 現在の角度を基準に変更
/*
void Eye::UpDirMove(float rad)
{
	mUpDir.z +=rad;

	//角度を0<=θ<360に調整
	if((CWE_PI_F*2) <= mUpDir.z) {mUpDir.z -=(CWE_PI_F*2);}
	if(mUpDir.z < 0.0f) {mUpDir.z +=(CWE_PI_F*2);}

	//sinf, cosfの仕様上の問題への対処
	if((4.71228409f < mUpDir.z && mUpDir.z < 4.71249390f) || (1.57069170f < mUpDir.z && mUpDir.z < 1.57090104f))
	{
		mUpDir.x =0.0f;
	}
	else
	{
		mUpDir.x =cosf(mUpDir.z);
	}

	if((3.14148808f < mUpDir.z && mUpDir.z < 3.14169741f) || (mUpDir.z < 0.0001047197f))
	{
		mUpDir.y =0.0f;
	}
	else
	{
		mUpDir.y =mUpDir.z;
	}
}
*/


// 上方向の変更
void Eye::UpDirReset(float x, float y)
{
	mUpDir.x() =x;
	mUpDir.y() =y;
}


// ビュー行列作成
void Eye::LookAtLHTp(math::Float4x4* view)
{
	math::LookAtLHTp(view, mPos, mFocusPos, mUpDir);
}


// 視点と焦点の距離を取得
float Eye::GetDistance()const
{
	return mNormalizedPosIs ? mDistance : math::VectorDistance(mPos, mFocusPos);
}


// 視点と焦点の距離の2乗を取得
float Eye::GetSqrDistance()const
{
	return mNormalizedPosIs ? mDistance*mDistance : math::VectorDistanceSq(mPos, mFocusPos);
}


}// CWE