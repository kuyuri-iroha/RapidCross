#pragma once


#include "CWEGraphicResource.h"
#include "CWETypeDefinition.h"


namespace CWE
{


// モーション動作管理クラス
class Motion
{
private:
	ResourceHandle mHandle;
	// リソースクラスにアクセスする為の参照
	Resource& mResource;
	// 再生中のフレーム
	float mOldFrame;

	/// キーフレームのリセット
///	void ResetKeyFrames(MotionData& data);
	// VMD用ベジェ補間
	float BezierInterpolation(float x1, float y1, float x2, float y2, float x);
	// ボーンへの適用
	bool Apply(Bone& bone, float frame, MotionData& data);

public:
	Motion();
	Motion(const Motion& original);
	Motion& operator=(const Motion& original);
	~Motion();


	// ファイルのロード(重複なし)
	void Load(const wchar_t *filePath, const FileFormat &format);

	// ハンドルの参照先が存在するか
	bool Is() const {return mResource.IsMotion(mHandle);}

	// 現在のハンドルの参照先が使用可能かどうか
	const Task& LoadResult()const;

	// データのコピー
	void Copy(const Motion& original);

	// データの開放
	void Release();

	// 再生位置を初期状態に戻す
	void Restart();

	// 再生
	// 戻り値：true=再生中, false=終了
	bool Update(Bone& bone, float frame);

	// ボーン座標と回転を取得
	void GetNowPose(const Bone& bone, math::Float3& pos, math::Quaternion& rotation);

	// 再生中のフレームを取得
	inline float GetNowFrame()const {return mOldFrame;}

	// 最終キーフレームを取得
	unsigned GetLastKeyFrame() const;

};



}
