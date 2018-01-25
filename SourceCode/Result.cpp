#include "Result.h"
#include "Controller.h"

using namespace CWE;


ResultExec::ResultExec():
	TaskBase(0, TaskName::Result), mCamera(), mTime(), mPlayedBGM(), mIdling(), mEnded()
{
	mBackGround.Load(L"Resource\\Title.png", FileFormat::Picture);
	mWinnerImages[Information::Player_1].Load(L"Resource\\1P Win.png", FileFormat::Picture);
	mWinnerImages[Information::Player_2].Load(L"Resource\\2P Win.png", FileFormat::Picture);
	mWinnerImages[Information::Player_None].Load(L"Resource\\Draw.png", FileFormat::Picture);
	mPressA.Load(L"Resource\\PressAtoTitle.png", FileFormat::Picture);
	mDeterm.Load(L"Resource\\Sound\\Title\\button30.wav", FileFormat::WAVE);
	mBGM[BGM_Winner].Load(L"Resource\\Sound\\Result\\タイムベンド.wav", FileFormat::WAVE);
	mBGM[BGM_Draw].Load(L"Resource\\Sound\\Result\\SSS.wav", FileFormat::WAVE);

	mToPlay =Information::GetInfo().mWinner == Information::Player_None ? BGM_Draw : BGM_Winner;
}

ResultExec::~ResultExec()
{
	End();
}


bool ResultExec::Loaded() const
{
	auto failed =[](auto obj) {return obj.LoadResult() == Task::Failed;};
	auto working =[](auto obj) {return obj.LoadResult() == Task::Working;};

	if(failed(mBackGround) ||
		failed(mWinnerImages[Information::Player_1]) ||
		failed(mWinnerImages[Information::Player_2]) ||
		failed(mWinnerImages[Information::Player_None]) ||
		failed(mPressA) ||
		failed(mDeterm) ||
		failed(mBGM[BGM_Winner]) ||
		failed(mBGM[BGM_Draw]))
	{
		FatalError::GetInstance().Outbreak(FatalError::smFileLoadErrorMS);
		return false;
	}

	if(working(mBackGround) ||
		working(mWinnerImages[Information::Player_1]) ||
		working(mWinnerImages[Information::Player_2]) ||
		working(mWinnerImages[Information::Player_None]) ||
		working(mPressA) ||
		working(mDeterm) ||
		working(mBGM[BGM_Winner]) ||
		working(mBGM[BGM_Draw]))
	{
		return false;
	}

	return true;
}


void ResultExec::Update(TaskExec::List& taskList)
{
	Information& info =Information::GetInfo();

	if(Loaded())
	{
		mDraw =true;

		mTitleEnded =taskList.GetTaskData(TaskName::Battle) == nullptr;
		if(mTitleEnded)
		{
			//Data update process
			if(!mTime.GetStarted())
			{
				mTime.Start();
			}
			mTime.Update();
			if(!mPlayedBGM)
			{
				mBGM[mToPlay].Start(true, 0, 0);
				mPlayedBGM =true;
			}
			gController1.InputUpdate();
		

			//Input process
			if(mIdling)
			{
				if(gController1.SideWeakAttack())
				{
					if(!mEnded)
					{
						mDeterm.Start(false, 0, 0);
					}
					mEnded =true;
				}
			}

			//Animation process
			mIdling =true;
	
			if(mEnded && mDeterm.Ended())
			{
				mEnable =false;
				info.mRuningScene =TaskName::Title;
			}
		}
	}
	else
	{
		mDraw =false;

		gController1.ClearFlags();
		gController2.ClearFlags();
	}
}


void ResultExec::Draw()
{
	Information& info =Information::GetInfo();
	using namespace math;
	Graphic& manager =Graphic::GetInstance();

	if(mTitleEnded)
	{
		manager.OMSetDepthBuffer(false);
		mCamera.Set2D();
		info.mUIShader.EnableConstantBuffer();

		//Draw the background animation.
		info.mUIShader.Begin(UIShader::PS_Result);
	
		World2DTp(&info.mUIShader.GetCBData()->mWorld, {0.0f, 0.0f, 0.0f});
		double time{};
		mTime.GetTime(time);
		info.mUIShader.GetCBData()->mTime =static_cast<float>(time);
		info.mUIShader.mCB.Map();
		CopyMemory(info.mUIShader.mCB.Access(), info.mUIShader.GetCBData(), sizeof(*info.mUIShader.GetCBData()));
		info.mUIShader.mCB.Unmap();

		mBackGround.Draw(true);

		info.mUIShader.End();

		//Draw the winner.
		info.mUIShader.Begin();

		info.mUIShader.UpdateProjection();

		World2DTp(&info.mUIShader.GetCBData()->mWorld, {0.0f, 0.05f, 0.0f});
		info.mUIShader.mCB.Map();
		CopyMemory(info.mUIShader.mCB.Access(), info.mUIShader.GetCBData(), sizeof(*info.mUIShader.GetCBData()));
		info.mUIShader.mCB.Unmap();

		switch(info.mWinner)
		{
		case Information::Player_1:
			mWinnerImages[Information::Player_1].Draw(true);
			break;

		case Information::Player_2:
			mWinnerImages[Information::Player_2].Draw(true);
			break;

		case Information::Player_None:
			mWinnerImages[Information::Player_None].Draw(true);
			break;

		default:
			//Do nothing.
			break;
		}

		//Draw the press A guide.
		World2DTp(&info.mUIShader.GetCBData()->mWorld, {0.0f, -0.4f, 0.0f});
		info.mUIShader.mCB.Map();
		CopyMemory(info.mUIShader.mCB.Access(), info.mUIShader.GetCBData(), sizeof(*info.mUIShader.GetCBData()));
		info.mUIShader.mCB.Unmap();

		mPressA.Draw(true);

		info.mUIShader.End();

		manager.OMSetDepthBuffer(true);
	}
}


void ResultExec::End()
{
	mDeterm.Release();
	mPressA.Release();
	for(auto& image : mWinnerImages)
	{
		image.Release();
	}
	mBackGround.Release();
}