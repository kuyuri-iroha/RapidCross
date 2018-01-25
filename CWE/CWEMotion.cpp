#include "CWEMotion.h"
#include "CWEMath.h"


namespace CWE
{


// コンストラクタ
Motion::Motion():
	mHandle(), mResource(Resource::GetInstance()),
	mOldFrame(0.0f)
{
}


// コピーコンストラクタ
Motion::Motion(const Motion& original):
	mHandle(), mResource(Resource::GetInstance()),
	mOldFrame(0.0f)
{
	mHandle =mResource.CopyMotionHandle(original.mHandle);
}

Motion& Motion::operator=(const Motion& original)
{
	mHandle =mResource.CopyMotionHandle(original.mHandle);
	return *this;
}


// デストラクタ
Motion::~Motion()
{
	Release();
}


// ファイルのロード
void Motion::Load(const wchar_t *filePath, const FileFormat &format)
{
	mHandle =mResource.Create(filePath, format);
}


// 現在のハンドルの参照先が使用可能かどうか
const Task& Motion::LoadResult()const
{
	return mResource.GetMotionData(mHandle).mEnabled;
}


// データのコピー
void Motion::Copy(const Motion& original)
{
	mHandle =mResource.CopyMotionHandle(original.mHandle);
}


// データの開放
void Motion::Release()
{
	mResource.MotionRelease(mHandle);
}


// 再生位置をループの最初に戻す
void Motion::Restart()
{
	mOldFrame =0.0f;
	for(auto& a:mResource.GetMotionData(mHandle).mFrameData)
	{
		a.second.second =0;
	}
}


// 再生
bool Motion::Update(Bone& bone, float frame)
{
	MotionData& data =mResource.GetMotionData(mHandle);
	if(data.mEnabled != Task::Success) {return false;}
	// ボーンにモーションを適用
	return Apply(bone, frame, data);
}



// ======================= プライベート =========================


/// キーフレームのリセット
/*
void Motion::ResetKeyFrames(MotionData& data)
{
	unsigned low=0, high=0, mid =0;
	for(std::map<std::wstring, std::pair<std::vector<FrameData>, unsigned>>::iterator itr=data.mFrameData.begin(); itr!=data.mFrameData.end(); ++itr)
	{
		// 特殊化二分探索
		low =0;
		high =itr->second.first.size();
		while(1 < high-low) //範囲が1以下になったら完了
		{
			mid =low+((high-low)/2);
			if(mOldFrame < static_cast<double>(itr->second.first[mid].mFrameNumber)) {high =mid;}
			else if(static_cast<double>(itr->second.first[mid].mFrameNumber) < mOldFrame) {low =mid;}
			else {low =mid; break;}
		}

		// 再生位置のキーフレームインデックス
		itr->second.second =low;
	}
}
*/


// VMD用ベジェ補間
float Motion::BezierInterpolation(float x1, float y1, float x2, float y2, float x)
{
	//データを0.0～1.0に
	x1 /=127.0f;
	y1 /=127.0f;
	x2 /=127.0f;
	y2 /=127.0f;

	float param = 0.5f;
	float approx =0.0f;

	//ベジェ曲線 X関数
	auto fx =[x1, x2](float t){
		float s =1.0f -t;
		return (3 *s*s *t *x1) + (3 *s *t*t *x2) + (t*t*t);
	};
	//ベジェ曲線 Y関数
	auto fy =[y1, y2](float t){
		float s =1.0f -t;
		return (3 *s*s *t *y1) + (3 *s *t*t *y2) + (t*t*t);
	};

	//ニュートン法を使って方向点からパラメータを算出
	for (int i = 0; i < 15; i++)
	{
		approx =fx(param);

		if(math::NearlyEqual(approx, x)) {break;}

		if(approx > x)
		{
			param -= 1.0f / float(4 << i);
		}
		else //approx < x
		{
			param += 1.0f / float(4 << i);
		}
	}

	return fy(param);
}


// ボーンへの適用
bool Motion::Apply(Bone& bone, float frame, MotionData& data)
{
	if(data.mFrameData.find(bone.mName) == data.mFrameData.end()) {return false;} //指定されたボーンがなければ無視
	std::pair<std::vector<FrameData>, unsigned>& refF =data.mFrameData.at(bone.mName);

	// VMD用ベジェ補間
	float bezierX =0.0f;

	// 一時保存
	bool order =mOldFrame <= frame;
	mOldFrame =frame;
	bool result =true;
	unsigned numFrames =refF.first.size();
	unsigned nextKeyFrame =refF.second +(order?1:-1);

	// モーションが終わっているか
	if(float(data.mLastKeyFrame) <= mOldFrame)
	{
		mOldFrame =float(data.mLastKeyFrame);
		result =false;
	}
	else if(mOldFrame < 0.0f)
	{
		mOldFrame =0.0f;
		result =false;
	}

	if(order) //順再生
	{
		// キーフレームの移行処理
		while(true)
		{
			//最後のキーフレームだった場合
			if(numFrames <= refF.second+1)
			{
				nextKeyFrame =refF.second;
				break;
			}

			if((mOldFrame -refF.first[refF.second+1].mFrameNumber <= 0.0f))
			{
				break; //目的のフレームになったら抜け出す
			}
			++refF.second;
			nextKeyFrame =refF.second+1;
		}

		//ベジェ曲線のX座標を算出
		if(refF.second == nextKeyFrame) //0除算回避
		{
			bezierX =1.0f;
		}
		else
		{
			bezierX =(mOldFrame -refF.first[refF.second].mFrameNumber) /(refF.first[nextKeyFrame].mFrameNumber -refF.first[refF.second].mFrameNumber);
		}
	}
	else //逆再生
	{
		// キーフレームの移行処理
		while(true)
		{
			//最後のキーフレームだった場合
			if(refF.second <= 0)
			{
				nextKeyFrame =refF.second;
				break;
			}

			if((refF.first[refF.second].mFrameNumber -mOldFrame <= 0.0f))
			{
				break; //目的のフレームになったら抜け出す
			}
			--refF.second;
			nextKeyFrame =refF.second+1;
		}

		//ベジェ曲線のX座標を算出
		if(refF.second == nextKeyFrame) //0除算回避
		{
			bezierX =0.0f;
		}
		else
		{
			bezierX =(mOldFrame -refF.first[refF.second].mFrameNumber) /(refF.first[nextKeyFrame].mFrameNumber -refF.first[refF.second].mFrameNumber);
		}
	}

	//Clamp
	if(bezierX <= 0.0f) {bezierX =0.0f;}
	else if(1.0f <= bezierX) {bezierX =1.0f;}

	//座標補間
	math::VectorLerp(&bone.mPosition, refF.first[refF.second].mBonePos, refF.first[nextKeyFrame].mBonePos,
		BezierInterpolation(refF.first[refF.second].mXInterpolation[0], refF.first[refF.second].mXInterpolation[1], refF.first[refF.second].mXInterpolation[2], refF.first[refF.second].mXInterpolation[3], bezierX),
		BezierInterpolation(refF.first[refF.second].mYInterpolation[0], refF.first[refF.second].mYInterpolation[1], refF.first[refF.second].mYInterpolation[2], refF.first[refF.second].mYInterpolation[3], bezierX),
		BezierInterpolation(refF.first[refF.second].mZInterpolation[0], refF.first[refF.second].mZInterpolation[1], refF.first[refF.second].mZInterpolation[2], refF.first[refF.second].mZInterpolation[3], bezierX)
		);
		
	//回転補間
	math::QuaternionSlerp(&bone.mRotation, refF.first[refF.second].mBoneQuaternion, refF.first[nextKeyFrame].mBoneQuaternion,
		BezierInterpolation(refF.first[refF.second].mRInterpolation[0], refF.first[refF.second].mRInterpolation[1], refF.first[refF.second].mRInterpolation[2], refF.first[refF.second].mRInterpolation[3], bezierX)
		);

	math::MatrixTranslationR(&bone.mPose, bone.mPosition, bone.mRotation); //mPoseを一時的に流用
	math::MatrixMultiply(&bone.mRelatPose, bone.mPose, bone.mRelatPose);
	
	return result;
}


// ボーン座標と回転を取得
void Motion::GetNowPose(const Bone& bone, math::Float3& pos, math::Quaternion& rotation)
{
	MotionData& data =mResource.GetMotionData(mHandle);
	if(data.mEnabled != Task::Success) {return;}
	if(data.mFrameData.find(bone.mName) == data.mFrameData.end()) {return;} //指定されたボーンがなければ無視
	std::pair<std::vector<FrameData>, unsigned>& refF =data.mFrameData.at(bone.mName);
	float bezierX =0.0f;
	unsigned nextKeyFrame =refF.second;

	//ベジェ曲線のX座標を算出
	if(float(refF.first.size()-1) <= mOldFrame) //最終フレームかどうか
	{
		mOldFrame =static_cast<float>(refF.first.size());
		bezierX =1.0f;
	}
	else
	{
		++nextKeyFrame;
		bezierX =(mOldFrame -refF.first[refF.second].mFrameNumber) /(refF.first[nextKeyFrame].mFrameNumber -refF.first[refF.second].mFrameNumber);
	}

	if(bezierX <= 0.0f) {bezierX =0.0f;}
	else if(1.0f <= bezierX) {bezierX =1.0f;}

	//座標補間
	math::VectorLerp(&pos, refF.first[refF.second].mBonePos, refF.first[nextKeyFrame].mBonePos,
		BezierInterpolation(refF.first[refF.second].mXInterpolation[0], refF.first[refF.second].mXInterpolation[1], refF.first[refF.second].mXInterpolation[2], refF.first[refF.second].mXInterpolation[3], bezierX),
		BezierInterpolation(refF.first[refF.second].mYInterpolation[0], refF.first[refF.second].mYInterpolation[1], refF.first[refF.second].mYInterpolation[2], refF.first[refF.second].mYInterpolation[3], bezierX),
		BezierInterpolation(refF.first[refF.second].mZInterpolation[0], refF.first[refF.second].mZInterpolation[1], refF.first[refF.second].mZInterpolation[2], refF.first[refF.second].mZInterpolation[3], bezierX)
		);
		
	//回転補間
	math::QuaternionSlerp(&rotation, refF.first[refF.second].mBoneQuaternion, refF.first[nextKeyFrame].mBoneQuaternion,
		BezierInterpolation(refF.first[refF.second].mRInterpolation[0], refF.first[refF.second].mRInterpolation[1], refF.first[refF.second].mRInterpolation[2], refF.first[refF.second].mRInterpolation[3], bezierX)
		);
}


// 最終フレームを取得
unsigned Motion::GetLastKeyFrame() const
{
	MotionData& data =mResource.GetMotionData(mHandle);
	if(data.mEnabled != Task::Success) {return 0;}

	return data.mLastKeyFrame;
}



}
