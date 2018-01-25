#include "Battle.h"
#include "Player.h"
#include "Gumi.h"
#include "Information.h"
#include "Controller.h"
#include "DebugString.h"


using namespace CWE;


const float BattleExec::smMovementLimit =125.0f;
const int BattleExec::smTimeLimit =60 *60;
const float BattleExec::smDefBGMVol =1.0f;
const unsigned BattleExec::smStartUpFrame =60;
const unsigned BattleExec::smSlowMotionInter =5;
const unsigned BattleExec::smEndProcFinishFr =3 *60;
const unsigned BattleExec::smGameSetAnimFr =15;
const unsigned BattleExec::smReadyFinishFr =2 *60;

// コンストラクタ
BattleExec::BattleExec(PlayerCharacter player1, PlayerCharacter player2, BattleStage stage):
	TaskBase(-1, TaskName::Battle), mStage(stage), mStarted(), mEnded(), mStartingAnimEnded(), mTimedUp(), mPlayedBGM(), mExistInterMenu(), mEndingFromMenu(), mScaleOfStarting(), mAlphaOfStarting(1.0f), mScaleOfGameSet(), mPosOfGameSet(1.5f), mElapsedOfStarting(), mElapsedOfEnding(), mRemainingTime(smTimeLimit)
{
	switch(player1)
	{
	case PlayerCharacter::Gumi:
		mPlayer1 =new Gumi(true, &gController1, 0);
		break;

	case PlayerCharacter::Gumi2:
		mPlayer1 =new Gumi(true, &gController1, 1);
		break;

	case PlayerCharacter::Unknown:
		CWE_ASSERT(false); //不明な列挙子
		break;

	default:
		CWE_ASSERT(false); //未実装
		break;
	}

	switch(player2)
	{
	case PlayerCharacter::Gumi:
		mPlayer2 =new Gumi(false, &gController2, 0);
		break;

	case PlayerCharacter::Gumi2:
		mPlayer2 =new Gumi(false, &gController2, 1);
		break;

	case PlayerCharacter::Unknown:
		CWE_ASSERT(false); //不明な列挙子
		break;

	default:
		CWE_ASSERT(false); //未実装
		break;
	}

	//BGM
	mBGM.Load(L"Resource\\Sound\\Starry Ruins\\Sound_Wave.wav", FileFormat::WAVE);
	//SE
	mDeterm.Load(L"Resource\\Sound\\Title\\button30.wav", FileFormat::WAVE);

	//HP
	mHitPoint.Create(L"Resource\\Rectangle filter.png", L"Resource\\Paramater\\Rectangle.pmx");
	mHPCase1.Load(L"Resource\\HPCase1.png", FileFormat::Picture);
	mHPCase2.Load(L"Resource\\HPCase2.png", FileFormat::Picture);

	//Icon
	mIcon1.Load(L"Resource\\1PIcon.png", FileFormat::Picture);
	mIcon2.Load(L"Resource\\2PIcon.png", FileFormat::Picture);

	//Time
	mTimeCase.Load(L"Resource\\TimeCase.png", FileFormat::Picture);

	//Ready & Go
	mReady.Load(L"Resource\\Ready.png", FileFormat::Picture);
	mGo.Load(L"Resource\\GO.png", FileFormat::Picture);

	//Game set
	mGameSet.Load(L"Resource\\GameSet.png", FileFormat::Picture);
	mTimeUp.Load(L"Resource\\TimeUp.png", FileFormat::Picture);
	mBar.Load(L"Resource\\GameSetBar.png", FileFormat::Picture);

	//Pause
	mPause.Load(L"Resource\\Pause.png", FileFormat::Picture);

	//シャドウマップとその描画
	mShadow.Create();
}


// デストラクタ
BattleExec::~BattleExec()
{
	End();
}


bool BattleExec::MembersLoadSuccess() const
{
	auto failed =[](auto obj) {return obj.LoadResult() == Task::Failed;};
	auto working =[](auto obj) {return obj.LoadResult() == Task::Working;};

	if(failed(mHPCase1) ||
		failed(mHPCase2) ||
		failed(mIcon1) ||
		failed(mIcon2) ||
		failed(mTimeCase) ||
		failed(mPause) ||
		failed(mReady) ||
		failed(mGo) ||
		failed(mGameSet) ||
		failed(mTimeUp) ||
		failed(mBar) ||
		failed(mBGM) ||
		failed(mDeterm))
	{
		FatalError::GetInstance().Outbreak(FatalError::smFileLoadErrorMS);
		return false;
	}
	if(working(mHPCase1) ||
		working(mHPCase2) ||
		working(mIcon1) ||
		working(mIcon2) ||
		working(mTimeCase) ||
		working(mPause) ||
		working(mReady) ||
		working(mGo) ||
		working(mGameSet) ||
		working(mTimeUp) ||
		working(mBar) ||
		working(mBGM) ||
		working(mDeterm))
	{
		return false;
	}

	return true;
}


// ロードの完了を通知
bool BattleExec::LoadSuccess() const
{
	Information& info =Information::GetInfo();

	info.mModelShader.EnableConstantBuffer(); //定数バッファを有効化

	return (mPlayer1->LoadSuccess()) &&
		(mPlayer2->LoadSuccess()) &&
		(mStage.LoadSuccess()) &&
		mHitPoint.LoadSucceeded() &&
		MembersLoadSuccess();
}


//衝突のチェック
void BattleExec::CollideCheck()
{
	//1P当たり判定
	for(const auto& aggressor : mPlayer2->mAggressors)
	{
		if(aggressor.mAggression)
		{
			if(Collide(mPlayer1->mUpperBody, aggressor) ||
				Collide(mPlayer1->mLowerBody, aggressor))
			{
				if(mPlayer2->IsHighImpact()) {mPlayer1->mPhnomenon.mGreatDamaged =true;}
				else {mPlayer1->mPhnomenon.mDamaged =true;}
			}
		}
	}

	//2P当たり判定
	for(const auto& aggressor : mPlayer1->mAggressors)
	{
		if(aggressor.mAggression)
		{
			if(Collide(mPlayer2->mUpperBody, aggressor) ||
				Collide(mPlayer2->mLowerBody, aggressor))
			{
				if(mPlayer1->IsHighImpact()) {mPlayer2->mPhnomenon.mGreatDamaged =true;}
				else {mPlayer2->mPhnomenon.mDamaged =true;}
			}
		}
	}
}


// Draw the battle state
void BattleExec::DrawBattle()
{
	Information& info =Information::GetInfo();
	using namespace math;

	info.mModelShader.EnableConstantBuffer();

	//パス1(シャドウマップ描画)
		//ビュー変換行列の作成
	LookAtLHTp(&info.mModelShader.GetCBData()->mView, mLight.GetEye().GetPos(), mLight.GetEye().GetFocusPos(), mLight.GetEye().GetUpDir());
		//プロジェクション行列の作成
	ShadowMap::GetProjection(info.mModelShader.GetCBData()->mProjection);
	
	mShadow.Begin();

	info.mModelShader.BeginShadow();
	mPlayer1->Draw(mStage.Local());
	mPlayer2->Draw(mStage.Local());
	info.mModelShader.End();

	info.mModelShader.BeginStageShadow();
	mStage.DrawStage(info);
	info.mModelShader.End();

	mShadow.End();

		//シャドウマップビューとプロジェクションの設定
	info.mModelShader.GetShadowCBData()->mView =info.mModelShader.GetCBData()->mView;
	info.mModelShader.GetShadowCBData()->mProjection =info.mModelShader.GetCBData()->mProjection;
	info.mModelShader.mShadowCB.Map();
	CopyMemory(info.mModelShader.mShadowCB.Access(), info.mModelShader.GetShadowCBData(), sizeof(*info.mModelShader.GetShadowCBData()));
	info.mModelShader.mShadowCB.Unmap();
	
	//パス2(本描画)
	mCamera.Set();
	info.mModelShader.Begin(mShadow.GetSRV());
	mPlayer1->Draw(mStage.Local());
	mPlayer2->Draw(mStage.Local());
	info.mModelShader.End();

	info.mModelShader.BeginStage(mShadow.GetSRV());
	mStage.DrawStage(info);
	info.mModelShader.End();

	info.mModelShader.BeginDome();
	mStage.DrawSkyDome(info);
	info.mModelShader.End();

	info.mUIShader.EnableConstantBuffer();
	info.mUIShader.Begin();
	math::World2DTp(&info.mUIShader.GetCBData()->mWorld, {-0.4f, -0.3f, 0.0f}, WORLD_SCALE);
	info.mUIShader.mCB.Map();
	CopyMemory(info.mUIShader.mCB.Access(), info.mUIShader.GetCBData(), sizeof(*info.mUIShader.GetCBData()));
	info.mUIShader.mCB.Unmap();

	info.mUIShader.End();
}


// Draw the parameters.
void BattleExec::DrawParam()
{
	Graphic& manager =Graphic::GetInstance();
	Information& info =Information::GetInfo();
	using namespace math;
	manager.OMSetDepthBuffer(false);

	info.mHPParamShader.Begin();
	info.mHPParamShader.EnableConstantBuffer();

	//Update the projective transformation matrix.
	if(info.mHPParamShader.GetStaticCBData()->mProjection != Polygon::Get2DProjection())
	{
		info.mHPParamShader.GetStaticCBData()->mProjection =Polygon::Get2DProjection();
		info.mHPParamShader.mStaticCB.Map();
		CopyMemory(info.mHPParamShader.mStaticCB.Access(), &Polygon::Get2DProjection(), sizeof(math::Float4x4));
		info.mHPParamShader.mStaticCB.Unmap();
	}

	float x =0.31f;
	float y =0.43f;
	float paramX =0.03f;
	//Draw the HP parameter of the Player 1.
	World2DTp(&info.mHPParamShader.GetCBData()->mWorld, {-x +paramX, y, 0.0f});
	info.mHPParamShader.GetCBData()->mParam =static_cast<float>(mPlayer1->mHP) /static_cast<float>(mPlayer1->smMaxHP);
	info.mHPParamShader.mCB.Map();
	CopyMemory(info.mHPParamShader.mCB.Access(), info.mHPParamShader.GetCBData(), sizeof(*info.mHPParamShader.GetCBData()));
	info.mHPParamShader.mCB.Unmap();

	mHitPoint.Draw();

	//Draw the HP parameter of the Player 2.
	auto rota =MatrixRotationRollPitchYaw(0.0f, 0.0f, ConvertToRadians(180.0f));
	auto trans =XMMatrixTranslation(x -paramX, y, 0.0f);
	auto world =MatrixMultiply(rota, trans);
	world =MatrixTranspose(world);
	StoreMatrix(&info.mHPParamShader.GetCBData()->mWorld, world);
	info.mHPParamShader.GetCBData()->mParam =static_cast<float>(mPlayer2->mHP) /static_cast<float>(mPlayer2->smMaxHP);
	info.mHPParamShader.mCB.Map();
	CopyMemory(info.mHPParamShader.mCB.Access(), info.mHPParamShader.GetCBData(), sizeof(*info.mHPParamShader.GetCBData()));
	info.mHPParamShader.mCB.Unmap();

	mHitPoint.Draw();

	info.mHPParamShader.End();


	info.mUIShader.Begin();
	info.mUIShader.EnableConstantBuffer();

	//Update the projective transformation matrix.
	info.mUIShader.UpdateProjection();

	math::World2DTp(&info.mUIShader.GetCBData()->mWorld, {-x, y, 0.0f});
	info.mUIShader.mCB.Map();
	CopyMemory(info.mUIShader.mCB.Access(), info.mUIShader.GetCBData(), sizeof(*info.mUIShader.GetCBData()));
	info.mUIShader.mCB.Unmap();

	mHPCase1.Draw(true);

	trans =XMMatrixTranslation(x, y, 0.0f);
	world =MatrixTranspose(trans);
	StoreMatrix(&info.mUIShader.GetCBData()->mWorld, world);
	info.mUIShader.mCB.Map();
	CopyMemory(info.mUIShader.mCB.Access(), info.mUIShader.GetCBData(), sizeof(*info.mUIShader.GetCBData()));
	info.mUIShader.mCB.Unmap();

	mHPCase2.Draw(true);

	//Draw the time case.
	math::World2DTp(&info.mUIShader.GetCBData()->mWorld, {0.0f, y, 0.0f});
	info.mUIShader.mCB.Map();
	CopyMemory(info.mUIShader.mCB.Access(), info.mUIShader.GetCBData(), sizeof(*info.mUIShader.GetCBData()));
	info.mUIShader.mCB.Unmap();

	mTimeCase.Draw(true);

	//Draw the Icons.
	if(!mStarted)
	{
		float iconX =0.253f;
		float iconY =0.1f;
		float iconScale =1.0f;
		auto scale =XMMatrixScaling(iconScale, iconScale, 1.0f);
		trans =XMMatrixTranslation(-iconX, iconY, 0.0f);
		world =MatrixMultiply(scale, trans);
		world =MatrixTranspose(world);
		StoreMatrix(&info.mUIShader.GetCBData()->mWorld, world);
		info.mUIShader.mCB.Map();
		CopyMemory(info.mUIShader.mCB.Access(), info.mUIShader.GetCBData(), sizeof(*info.mUIShader.GetCBData()));
		info.mUIShader.mCB.Unmap();

		mIcon1.Draw(true);

		scale =XMMatrixScaling(iconScale, iconScale, 1.0f);
		trans =XMMatrixTranslation(iconX, iconY, 0.0f);
		world =MatrixMultiply(scale, trans);
		world =MatrixTranspose(world);
		StoreMatrix(&info.mUIShader.GetCBData()->mWorld, world);
		info.mUIShader.mCB.Map();
		CopyMemory(info.mUIShader.mCB.Access(), info.mUIShader.GetCBData(), sizeof(*info.mUIShader.GetCBData()));
		info.mUIShader.mCB.Unmap();

		mIcon2.Draw(true);
	}

	info.mUIShader.End();
	manager.OMSetDepthBuffer(true);

	//Draw the remaining time.
	float remTimeSec =static_cast<float>(mRemainingTime) /60.0f;
	float timeLimX =remTimeSec < 100.0f ? remTimeSec < 10.0f ?  0.0f : -0.015f /(1.0f /1920.0f) *0.5f : -0.015f /(1.0f /1920.0f);
	DebugString::Output(timeLimX, 465, "%d", static_cast<int>(remTimeSec));
}


// Draw the starting.
void BattleExec::DrawStarting()
{
	Graphic& manager =Graphic::GetInstance();
	Information& info =Information::GetInfo();
	using namespace math;
	manager.OMSetDepthBuffer(false);

	info.mUIShader.EnableConstantBuffer();
	info.mUIShader.Begin();

	auto scale =XMMatrixScaling(1.0f, mScaleOfStarting, 1.0f);
	auto trans =XMMatrixTranslation(0.0f, 0.0f, 0.0f);
	auto world =MatrixMultiply(scale, trans);
	world =MatrixTranspose(world);
	StoreMatrix(&info.mUIShader.GetCBData()->mWorld, world);
	info.mUIShader.mCB.Map();
	CopyMemory(info.mUIShader.mCB.Access(), info.mUIShader.GetCBData(), sizeof(*info.mUIShader.GetCBData()));
	info.mUIShader.mCB.Unmap();

	mBar.Draw(true);

	if(!mStarted)
	{
		info.mUIShader.GetCBData()->mAlpha =mAlphaOfStarting;
		info.mUIShader.mCB.Map();
		CopyMemory(info.mUIShader.mCB.Access(), info.mUIShader.GetCBData(), sizeof(*info.mUIShader.GetCBData()));
		info.mUIShader.mCB.Unmap();

		mReady.Draw(true);

		info.mUIShader.GetCBData()->mAlpha =1.0f;
	}
	else
	{
		mGo.Draw(true);
	}

	info.mUIShader.End();
	manager.OMSetDepthBuffer(true);
}


// Draw the game set.
void BattleExec::DrawGameSet()
{
	Graphic& manager =Graphic::GetInstance();
	Information& info =Information::GetInfo();
	using namespace math;
	manager.OMSetDepthBuffer(false);

	//Begin the shader path.
	info.mUIShader.Begin();
	info.mUIShader.EnableConstantBuffer();
	info.mUIShader.UpdateProjection();
	
	auto scale =XMMatrixScaling(1.0f, mScaleOfGameSet, 1.0f);
	auto trans =XMMatrixTranslation(0.0f, 0.0f, 0.0f);
	auto world =MatrixMultiply(scale, trans);
	world =MatrixTranspose(world);
	StoreMatrix(&info.mUIShader.GetCBData()->mWorld, world);
	info.mUIShader.mCB.Map();
	CopyMemory(info.mUIShader.mCB.Access(), info.mUIShader.GetCBData(), sizeof(*info.mUIShader.GetCBData()));
	info.mUIShader.mCB.Unmap();

	mBar.Draw(true);

	if(mTimedUp)
	{
		mTimeUp.Draw(true);
	}
	else
	{
		scale =XMMatrixScaling(1.0f, mScaleOfGameSet, 1.0f);
		trans =XMMatrixTranslation(mPosOfGameSet, 0.0f, 0.0f);
		world =MatrixMultiply(scale, trans);
		world =MatrixTranspose(world);
		StoreMatrix(&info.mUIShader.GetCBData()->mWorld, world);
		info.mUIShader.mCB.Map();
		CopyMemory(info.mUIShader.mCB.Access(), info.mUIShader.GetCBData(), sizeof(*info.mUIShader.GetCBData()));
		info.mUIShader.mCB.Unmap();

		mGameSet.Draw(true);
	}

	info.mUIShader.End();
	manager.OMSetDepthBuffer(true);
}


// Draw the pause screen.
void BattleExec::DrawPause()
{
	Graphic& manager =Graphic::GetInstance();
	Information& info =Information::GetInfo();
	using namespace math;
	manager.OMSetDepthBuffer(false);

	//Begin the shader path.
	info.mUIShader.Begin();

	info.mUIShader.EnableConstantBuffer();

	info.mUIShader.UpdateProjection();

	World2DTp(&info.mUIShader.GetCBData()->mWorld, {0.0f, 0.0f, 0.0f});
	info.mUIShader.mCB.Map();
	CopyMemory(info.mUIShader.mCB.Access(), info.mUIShader.GetCBData(), sizeof(*info.mUIShader.GetCBData()));
	info.mUIShader.mCB.Unmap();

	mPause.Draw(true);

	info.mUIShader.End();
	manager.OMSetDepthBuffer(true);
}


void BattleExec::GamingUpdate(TaskExec::List& taskList)
{
	Information& info =Information::GetInfo();
	Graphic& graphic =Graphic::GetInstance();
	using namespace math;

	//BGM
	if(!mPlayedBGM && !mEnded)
	{
		mBGM.Start(true, 0, 0);
		mPlayedBGM =true;
	}
	else if(mEnded)
	{
		mBGM.Stop();
		mPlayedBGM =false;
	}

	//Model motion
	auto fightingProc =[&](){
		//Damage from
		if(mPlayer1->mPos.x() <= mPlayer2->mPos.x())
		{
			mPlayer1->mPhnomenon.mDamagedFromRight =true;
			mPlayer2->mPhnomenon.mDamagedFromRight =false;
		}
		else
		{
			mPlayer1->mPhnomenon.mDamagedFromRight =false;
			mPlayer2->mPhnomenon.mDamagedFromRight =true;
		}
	
		//Items update
		if(info.mRuningScene != TaskName::Title)
		{
			mLight.Update();
			mCamera.Update();

			mPlayer1->Update();
			mPlayer2->Update();
			mStage.Update();

			CollideCheck();
		}

		//Movement limit
		if(mPlayer1->mPos.x() <= -smMovementLimit)
		{
			mPlayer1->mPos.x() =-smMovementLimit;
		}
		else if(smMovementLimit <=mPlayer1->mPos.x())
		{
			mPlayer1->mPos.x() =smMovementLimit;
		}
		if(mPlayer2->mPos.x() <= -smMovementLimit)
		{
			mPlayer2->mPos.x() =-smMovementLimit;
		}
		else if(smMovementLimit <=mPlayer2->mPos.x())
		{
			mPlayer2->mPos.x() =smMovementLimit;
		}
	};
	if(!mEnded)
	{
		fightingProc();
	}
	else
	{
		if(mElapsedOfEnding %smSlowMotionInter == 0)
		{
			fightingProc();
		}
	}

	//Time limit
	if(mStarted && !mEnded)
	{
		if(60 < mRemainingTime)
		{
			--mRemainingTime;
		}
		else
		{
			mRemainingTime =0;
			if(mPlayer2->mHP < mPlayer1->mHP)
			{
				info.mWinner =Information::Player_1;
			}
			else if(mPlayer1->mHP < mPlayer2->mHP)
			{
				info.mWinner =Information::Player_2;
			}
			else
			{
				info.mWinner =Information::Player_None;
			}
			mTimedUp =true;
		}
	}

	//Starting
	if(!mStartingAnimEnded)
	{
		++mElapsedOfStarting;
		if(mElapsedOfStarting < smGameSetAnimFr)
		{
			mScaleOfStarting =CubicBezier(0.075f, 0.82f, 0.165f, 1.0f, static_cast<float>(mElapsedOfStarting) /static_cast<float>(smGameSetAnimFr));
		}
		else
		{
			mScaleOfStarting =1.0f;
			mAlphaOfStarting =1.0f -static_cast<float>(mElapsedOfStarting -smGameSetAnimFr) /static_cast<float>(smReadyFinishFr -smGameSetAnimFr);

			if(smReadyFinishFr <= mElapsedOfStarting)
			{
				mAlphaOfStarting =0.0f;
				mStarted =true;

				const unsigned waitFrame =30;
				if(smReadyFinishFr +waitFrame <= mElapsedOfStarting)
				{
					if(mElapsedOfStarting < smReadyFinishFr +waitFrame +smGameSetAnimFr)
					{
						mScaleOfStarting =1.0f -CubicBezier(0.95f, 0.05f, 0.795f, 0.035f, static_cast<float>(mElapsedOfStarting -(smReadyFinishFr +waitFrame)) /static_cast<float>(smReadyFinishFr +waitFrame +smGameSetAnimFr -(smReadyFinishFr +waitFrame)));
					}
					else
					{
						mScaleOfStarting =0.0f;
						mStartingAnimEnded =true;
					}
				}
			}
		}
	}

	//Ending
	auto endingProc =[&](){
		mEnded =true;
		info.mRuningScene =TaskName::Result;
		++mElapsedOfEnding;
		EndingAnim();
		if(smEndProcFinishFr <= mElapsedOfEnding)
		{
			auto resScene =taskList.GetTaskData(TaskName::Result);
			if(resScene != nullptr)
			{
				if(resScene->IsDraw())
				{
					mEnable =false;
				}
			}
		}
	};

	if(mPlayer1->mHP <= 0)
	{
		mPlayer1->mHP =0;
		info.mWinner =Information::Player_2;
		endingProc();
	}
	else if(mPlayer2->mHP <= 0)
	{
		mPlayer2->mHP =0;
		info.mWinner =Information::Player_1;
		endingProc();
	}
	else if(mTimedUp)
	{
		endingProc();
	}

	//InterMenu
	if(mStartingAnimEnded && !mEnded)
	{
		if(gController1.Menu())
		{
			mDeterm.Start(false, 0, 0);
			mExistInterMenu =true;
		}
	}
}


//Update the interruption screen.
void BattleExec::InterMenuUpdate()
{
	mBGM.SetVolume(smDefBGMVol /2.0f);

	if(gController1.Menu() && !mEndingFromMenu)
	{
		mBGM.SetVolume(smDefBGMVol);
		mExistInterMenu =false;
	}

	if(gController1.Back())
	{
		if(!mEndingFromMenu)
		{
			mDeterm.Start(false, 0, 0);
			mEndingFromMenu =true;
		}
	}

	if(mEndingFromMenu && mDeterm.Ended())
	{
		mEnable =false;
		Information::GetInfo().mRuningScene =TaskName::Title;
	}
}


//Process of animation at ending.
void BattleExec::EndingAnim()
{
	using namespace math;

	if(mElapsedOfEnding < smGameSetAnimFr)
	{
		float t =CubicBezier(0.075f, 0.82f, 0.165f, 1.0f, static_cast<float>(mElapsedOfEnding) /static_cast<float>(smGameSetAnimFr));

		mScaleOfGameSet =t;
		mPosOfGameSet =(1.001f -t) *1.5f;
	}
	else
	{
		mScaleOfGameSet =1.0f;
		mPosOfGameSet -=0.0002f;
	}
}


void BattleExec::Update(TaskExec::List& taskList)
{
	if(LoadSuccess())
	{
		mDraw =true;

		//Controller
		if(!mEnded && mStarted)
		{
			gController1.InputUpdate();
			gController2.InputUpdate();
		}
		else
		{
			gController1.ClearFlags();
			gController2.ClearFlags();
		}

		if(!mExistInterMenu)
		{
			GamingUpdate(taskList);
		}
		else
		{
			InterMenuUpdate();
		}
		
	}
	else
	{
		mDraw =false;
	}
}


// 描画
void BattleExec::Draw()
{
	DrawBattle();

	DrawParam();

	if(!mStartingAnimEnded)
	{
		DrawStarting();
	}
	if(mEnded)
	{
		DrawGameSet();
	}
	if(mExistInterMenu)
	{
		DrawPause();
	}
}


void BattleExec::End()
{
	mBar.Release();
	mTimeUp.Release();
	mGameSet.Release();
	mGo.Release();
	mReady.Release();
	mPause.Release();
	mTimeCase.Release();
	mIcon2.Release();
	mIcon1.Release();
	mHPCase2.Release();
	mHPCase1.Release();
	mHitPoint.End();
	mDeterm.Release();
	mBGM.Release();
	SAFE_DELETE(mPlayer2);
	SAFE_DELETE(mPlayer1);
	Information::GetInfo().mModelShader.DisableConstantBuffer();
}