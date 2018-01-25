#pragma once

#include "Player.h"


// GUMI
class Gumi final  : public PlayerBase
{
public:
	// モーションのリクエスト
	enum class RequestMotion : int
	{
		Walk,					//歩行
		TurnAround,				//振り向き
		Dash,					//走行
		Jump,					//ジャンプ
		Guard,					//ガード

		SideWeakAttack,			//横弱攻撃
		SideAttack,				//横攻撃
		UpperAttack,			//上攻撃
		LowerAttack,			//下攻撃

		ToNext,					//次へ
	};

	// モーションの受動的なリクエスト
	enum class PassiveRequest : int
	{
		Landing,				//着地
		Damaged,				//ダメージ
		GreatDamaged,			//大ダメージ

	};

	// モーションの種類
	enum class MotionType : int
	{
		InterNoIncpt =-INT_MIN,	//割り込み禁止の補間
		JumpPre,				//ジャンプの予備動作
		Brake1,					//ブレーキ
		Brake2,					//振り向きブレーキ
		ToDash2,				//走行前の蹴り出し2
		TurnAround,				//振り向き
		SideWeakPunch1,			//横弱攻撃1
		SideWeakPunch2,			//横弱攻撃2
		DashAttack1,			//ダッシュ攻撃1
		DashAttackImpact,		//ダッシュ攻撃インパクト
		DashAttack2,			//ダッシュ攻撃2
		SideAttack1,			//横攻撃1
		SideAttackImpact1,		//横攻撃インパクト1
		SideAttack2,			//横攻撃2
		SideAttackImpact2,		//横攻撃インパクト2
		SideAttack3,			//横攻撃3
		UpperAttack,			//上攻撃
		AerialLowerAttack,		//空中下攻撃
		LowerAttackBackStep,	//下攻撃バックステップ
		Guard,					//ガード
		Damaged,				//ダメージ
		GreatDamaged,			//大ダメージ

		NoMotion =0,			//モーションなし

		Idling,					//アイドリング
		Walk,					//歩行
		Dash,					//走行
		ToDash1,				//走行前の蹴り出し1
		Brake2ToIdling,			//振り向きブレーキからアイドリング
		Jump1,					//ジャンプ1
		Jump2,					//ジャンプ2
		Air,					//空中
		Landing,				//着地

		SideWeakPunch1ToIdling,	//横弱攻撃1からアイドリングへ
		SideWeakPunch2ToIdling,	//横弱攻撃2からアイドリングへ
		DashAttackToIdling,		//ダッシュ攻撃からアイドリングへ
		Inter,					//補間
	};

	// モーション配列のインデックス
	enum MotionIndex : unsigned
	{
		mInd_Idling,
		mInd_Walk,
		mInd_TurnAround,
		mInd_Dash,
		mInd_ToDash1,
		mInd_Brake1,
		mInd_Brake2,
		mInd_Brake2ToIdling,
		mInd_ToDash2,
		mInd_JumpPre,
		mInd_Jump1,
		mInd_Jump2,
		mInd_Air,
		mInd_Landing,

		mInd_SideWeakPunch1,
		mInd_SideWeakPunch1ToIdling,
		mInd_SideWeakPunch2,
		mInd_SideWeakPunch2ToIdling,
		mInd_DashAttack1,
		mInd_DashAttackImpact,
		mInd_DashAttack2,
		mInd_DashAttackToIdling,
		mInd_SideAttack1,
		mInd_SideAttackImpact1,
		mInd_SideAttack2,
		mInd_SideAttackImpact2,
		mInd_SideAttack3,
		mInd_UpperAttack,
		mInd_AerialSideWeakAttack,
		mInd_AerialLowerAttack,
		mInd_LowerAttackBackStep,
		mInd_Guard,
		mInd_Damaged,
		mInd_GreatDamaged,

		mInd_TheNumber,
		mInd_Unknown
	};

	// 与ダメージコリジョン配列インデックス
	enum AggressorIndex : unsigned
	{
		agInd_RightArm,
		agInd_LeftArm,
		agInd_RightLeg,
		agInd_LeftLeg,
		agInd_TheNumber,
		agInd_Unknown
	};

	// 効果音配列インデックス
	enum SoundEffIndex : unsigned
	{
		seInd_Landing,
		seInd_Attack,
		seInd_Damage,
		seInd_GreatDamage,
		seInd_Jump,
		seInd_UpperKick,
		seInd_LegDrop,
		seInd_Guard,
		seInd_Footsteps,
		seInd_Brake,
		seInd_TheNumber,
		seInd_Unknown
	};

private:
	// キャラクターの物理情報
	static const float smWeight,
					   smJumpingAbility1,
					   smJumpingAbility2;
	static const int smDamage =5;

	// モーションデータ
	std::array<CWE::Motion, mInd_TheNumber> mMotions;
	// モーション補間オブジェクト
	MotionType mInterFrom, //補間元モーション
			   mInterTo; //補間先モーション

	// サウンドデータ
	std::array<CWE::Sound, seInd_TheNumber> mSoundEff;

	float mNowMotionFrame; //現在のモーションフレーム
	MotionType mNowMotionType, //現フレームのモーション
			   mPrevMotionType; //前フレームのモーション
	bool mMotionEnded; //再生中のモーションが終了したか
	float mMoveSpeedX, //移動速度(X座標)
		  mMoveSpeedY; //移動速度(Y座標)
	bool mMoveBack;	//後方への移動か
	bool mJumpedTwice; //2段ジャンプしたか
	bool mAerialAttacked; //空中で攻撃したか
	bool mUpperAttacked; //上攻撃をしたか

	// 当たり判定の更新
	void CollisionUpdate();
	// 衝突への反応
	void CollideReaction();

	// モーションタイプから対応するモーションインデックスを取得
	const MotionIndex GetMotionIndex(const MotionType mType) const;
	// 補間モーションの設定
	void SetInterMotion(MotionType from, MotionType to, unsigned frame, MotionType interType);
	// モーションの再生
	void PlayMotion(CWE::Motion& motion);
	// 補間の再生
	void PlayInter(CWE::Motion& from, CWE::Motion& to);

	// 入力に対するモーションの変更
	void InputReaction(RequestMotion& request);
	// 受動的に起こるモーションの変更
	void PassiveReaction(PassiveRequest& request);
	// モーションの更新
	void MotionUpdate(bool& motionMoveX, bool& motionMoveY, float& friction);
	// モーションの移行
	void MotionChange(RequestMotion requested, PassiveRequest psReqested);
	// 地上でのモーションの移行要求処理
	void GroundMotionRespond(RequestMotion requested, PassiveRequest psReqested);
	// 空中でのモーションの移行要求処理
	void AerialMotionRespond(RequestMotion requested);
	// result: (0: 移動なし, 1: 入力あり移動, 2: 入力なし移動)
	void MoveX(bool motionMove, float friction);
	void MoveY();

public:
	Gumi(bool left, Controller* controller, unsigned colorNumber);
	~Gumi();

	bool LoadSuccess() const override;
	bool IsHighImpact() const override;
	void Update() override;

};