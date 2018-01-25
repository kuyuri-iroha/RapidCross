#pragma once


#include "Task.h"
#include "Static.h"
#include <memory>

#include "Camera.h"
#include "Light.h"
#include "Stage.h"
#include "Shadow.h"
#include "BattleEnums.h"
#include "Parameter.h"


// 前方宣言
class PlayerBase;



// バトルシーンタスク
class BattleExec : public TaskBase
{
private:
	Parameter mHitPoint;
	CWE::Polygon mHPCase1;
	CWE::Polygon mHPCase2;
	CWE::Polygon mIcon1;
	CWE::Polygon mIcon2;
	CWE::Polygon mTimeCase;
	CWE::Polygon mPause;
	CWE::Polygon mReady;
	CWE::Polygon mGo;
	CWE::Polygon mGameSet;
	CWE::Polygon mTimeUp;
	CWE::Polygon mBar;
	CWE::Sound mBGM;
	CWE::Sound mDeterm;
	Light mLight;
	Camera mCamera;
	PlayerBase* mPlayer1;
	PlayerBase*	mPlayer2;
	Stage mStage;
	ShadowMap mShadow;
	bool mPlayedBGM;
	bool mExistInterMenu;
	bool mEndingFromMenu;
	bool mStarted;
	bool mStartingAnimEnded;
	bool mEnded;
	bool mTimedUp;
	static const float smMovementLimit; 
	static const int smTimeLimit;
	static const float smDefBGMVol;
	static const unsigned smStartUpFrame;
	static const unsigned smSlowMotionInter;
	static const unsigned smEndProcFinishFr;
	static const unsigned smGameSetAnimFr;
	static const unsigned smReadyFinishFr;
	float mScaleOfStarting;
	float mAlphaOfStarting;
	float mScaleOfGameSet;
	float mPosOfGameSet;
	unsigned mElapsedOfStarting;
	unsigned mElapsedOfEnding;
	int mRemainingTime;

	bool MembersLoadSuccess() const;
	bool LoadSuccess() const;
	void CollideCheck();
	void DrawBattle();
	void DrawParam();
	void DrawStarting();
	void DrawGameSet();
	void DrawPause();
	void GamingUpdate(TaskExec::List& taskList);
	void InterMenuUpdate();
	void EndingAnim();

public:
	BattleExec() =delete;
	BattleExec(PlayerCharacter player1, PlayerCharacter player2, BattleStage stage);
	~BattleExec();

	void Update(TaskExec::List& taskList) override;
	void Draw() override;
	void End() override;

};