#pragma once

#include "CWEIncludedAPI.h"
#include "CWEMath.h"


namespace CWE
{


//目オブジェクト
struct Eye
{
private:
	math::Float3 mPos; //視点の位置
	math::Float3 mFocusPos; //焦点座標
	math::Float3 mUpDir; //上方向座標
	math::Float3 mNormalizedPos; //正規化された視点の位置
	float mDistance; //視点から焦点までの距離
	bool mNormalizedPosIs; //正規化されているか

public:


	//コンストラクタ&デストラクタ
	Eye();
	Eye(const math::Float3& pos, const math::Float3& focusPos, const math::Float3& upDir);
	~Eye();


	// 視点座標移動関数

	//現在位置からの移動
	void PosMove(float x, float y, float z);
	//位置の変更
	void PosReset(float x, float y, float z);
	//角度を用いて現在位置から焦点を中心に回転移動
	// (r値のみ代入で0.0fにするとエラー)
	void PosMoveByAngle(float xAngle, float yAngle, float r);
	//角度を用いて位置の変更
//	void PosResetByRad(float xRad, float yRad, float r);


	// 焦点座標移動関数

	//現在位置からの移動
	void FocusMove(float x, float y, float z);
	//位置の変更
	void FocusReset(float x, float y, float z);
	//視点位置と共に現在位置からの移動
	void FocusMoveWithPos(float x, float y, float z);
	//視点位置と共に位置の変更
	void FocusResetWithPos(float x, float y, float z);


	// 上方向設定関数

	//現在の角度を基準に変更
	// 未実装
//	void UpDirMove(float rad);

	//上方向の変更
	void UpDirReset(float x, float y);

	// ビュー変換行列作成
	void LookAtLHTp(math::Float4x4* view);


	// 内部情報への参照
	const math::Float3& GetPos()const {return mPos;}
	const math::Float3& GetFocusPos()const {return mFocusPos;}
	const math::Float3& GetUpDir()const {return mUpDir;}
	bool GetNormalized()const {return mNormalizedPosIs;}
	// 視点と焦点間の距離を取得
	//(回転関数のr値を設定済みだと高速)
	float GetDistance()const;
	// 視点と焦点の距離の2乗を取得
	float GetSqrDistance()const;
};


}//CWE
