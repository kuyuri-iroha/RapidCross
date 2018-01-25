#include "Title.h"
#include "Information.h"
#include "Controller.h"


using namespace CWE;


TitleExec::TitleExec():
	TaskBase(-1, TaskName::Title), mPressingFrame(), mPlayedBGM(), mScaleOfMenuBar(), mScaleOfPressA(1.0f), mPreparingMenuBar(), mPressAAnimTime(), mMenuBarAnimTime(), mSelector(), mDisplayingCredit(), mEnding()
{
	mTitleImage.Load(L"Resource\\Title.png", FileFormat::Picture);
	mTitle.Load(L"Resource\\SoftName.png", FileFormat::Picture);
	mPressA.Load(L"Resource\\PressA.png", FileFormat::Picture);
	mMenuBar.Load(L"Resource\\TitleMenuBar.png", FileFormat::Picture);
	mMenuBattle.Load(L"Resource\\TitleMenuBattle.png", FileFormat::Picture);
	mMenuCredit.Load(L"Resource\\TitleMenuCredit.png", FileFormat::Picture);
	mCredit.Load(L"Resource\\Credit.png", FileFormat::Picture);
	mEndParam.Create(L"Resource\\Circle filter.png", L"Resource\\Paramater\\Circle.pmx");
	mBGM.Load(L"Resource\\Sound\\Title\\ÉCÉRÅ[ÉãÉ[Éç.wav", FileFormat::WAVE);
	mDeterm.Load(L"Resource\\Sound\\Title\\button30.wav", FileFormat::WAVE);
	mSelect.Load(L"Resource\\Sound\\Title\\cancel1.wav", FileFormat::WAVE);
}

TitleExec::~TitleExec()
{
	End();
}


// Check this class members were loaded.
bool TitleExec::MembersLoaded() const
{
	auto failed =[](auto obj) {return obj.LoadResult() == Task::Failed;};
	auto working =[](auto obj) {return obj.LoadResult() == Task::Working;};

	if(failed(mTitleImage) ||
		failed(mTitle) ||
		failed(mPressA) ||
		failed(mMenuBar) ||
		failed(mMenuBattle) ||
		failed(mMenuCredit) ||
		failed(mCredit) ||
		failed(mBGM) ||
		failed(mDeterm) ||
		failed(mSelect))
	{
		FatalError::GetInstance().Outbreak(FatalError::smFileLoadErrorMS);
		return false;
	}

	if(working(mTitleImage) ||
		working(mTitle) ||
		working(mPressA) ||
		working(mMenuBar) ||
		working(mMenuBattle) ||
		working(mMenuCredit) ||
		working(mCredit) ||
		working(mBGM) ||
		working(mDeterm) ||
		working(mSelect))
	{
		return false;
	}

	return true;
}


// Check all of the resources were loaded.
bool TitleExec::LoadSuccess() const
{
	return MembersLoaded() &&
		mEndParam.LoadSucceeded();
}


// Has ended animation when return true.
bool TitleExec::MenuBarDisplayAnim()
{
	using namespace math;

	mPressAAnimTime +=1.0f /20.0f;
	mScaleOfPressA =1.0f -CubicBezier(0.6f, 0.04f, 0.98f, 0.335f, mPressAAnimTime);
	if(1.0f < mPressAAnimTime)
	{
		mScaleOfPressA =0.0f;

		mMenuBarAnimTime +=1.0f /20.0f;
		mScaleOfMenuBar =CubicBezier(0.075f, 0.82f, 0.165f, 1.0f, mMenuBarAnimTime);
		if(1.0f < mMenuBarAnimTime)
		{
			mScaleOfMenuBar =1.0f;
			return true;
		}
	}

	return false;
}


void TitleExec::Update(TaskExec::List& taskList)
{
	Information& info =Information::GetInfo();

	if(LoadSuccess())
	{
		//Start processing
		mDraw =true;
		if(!mPlayedBGM)
		{
			mBGM.Start(true, static_cast<unsigned>(44100 *21.33), static_cast<unsigned>(44100 *(42.68 -21.33)));
			mPlayedBGM =true;
		}
		if(!mTime.GetStarted())
		{
			mTime.Start();
		}

		//The data update processing
		gController1.InputUpdate();
		gController2.InputUpdate();
		mTime.Update();

		//Input process
		if(!mEnding)
		{
			if(gController1.SideWeakAttack())
			{
				if(!mPreparingMenuBar && math::NearlyEqual(mScaleOfMenuBar, 0.0f)) //Displaying the PressA.
				{
					mDeterm.Start(false, 0, 0);
					mPreparingMenuBar =true;
				}
				else if(!mPreparingMenuBar) //Displaying the menu bar.
				{
					switch(mSelector)
					{
					case Selec_Battle:
						mDeterm.Start(false, 0, 0);
						mEnding =true;
						break;

					case Selec_Credit:
						mDisplayingCredit =!mDisplayingCredit;
						mDeterm.Start(false, 0, 0);
						break;
	
					default:
						break;
					}
				}
			}
		}

		if(!mDisplayingCredit && math::NearlyEqual(mScaleOfMenuBar, 1.0f))
		{
			if(gController1.Jump())
			{
				mSelect.Start(false, 0, 0);
				--mSelector;
				if(mSelector < 0)
				{
					mSelector =0;
				}
			}
			if(gController1.InstDown())
			{
				mSelect.Start(false, 0, 0);
				++mSelector;
				if(NumberOfSelec <= mSelector)
				{
					mSelector =NumberOfSelec -1;
				}
			}
		}

		if(gController1.BackLong())
		{
			if(smEndingPressFrame <= mPressingFrame)
			{
				Information::GetInfo().mEnded =true;
			}
			++mPressingFrame;
		}
		else
		{
			mPressingFrame =0;
		}

		//Animation process
		if(mPreparingMenuBar)
		{
			if(MenuBarDisplayAnim())
			{
				mPreparingMenuBar =false;
			}
		}

		if(mEnding)
		{
			if(mDeterm.Ended())
			{
				info.mRuningScene =TaskName::Battle;
				mEnable =false;
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


void TitleExec::Draw()
{
	Information& info =Information::GetInfo();
	using namespace math;
	Graphic& manager =Graphic::GetInstance();

	manager.OMSetDepthBuffer(false);

	//Viewport setting.
	mCamera.Set2D();

	info.mUIShader.EnableConstantBuffer();

	//Begin the raymarching shader.
	info.mUIShader.Begin(UIShader::PS_Title);

	//Draw the background.
	info.mUIShader.UpdateProjection();

	World2DTp(&info.mUIShader.GetCBData()->mWorld, {0.0f, 0.0f, 0.0f});
	double time{};
	mTime.GetTime(time);
	info.mUIShader.GetCBData()->mTime =static_cast<float>(time);
	info.mUIShader.mCB.Map();
	CopyMemory(info.mUIShader.mCB.Access(), info.mUIShader.GetCBData(), sizeof(*info.mUIShader.GetCBData()));
	info.mUIShader.mCB.Unmap();

	mTitleImage.Draw(true);

	//Begin the default shader.
	info.mUIShader.Begin();

	//Draw the title.
	World2DTp(&info.mUIShader.GetCBData()->mWorld, {0.0f, 0.15f, 0.0f});
	info.mUIShader.mCB.Map();
	CopyMemory(info.mUIShader.mCB.Access(), info.mUIShader.GetCBData(), sizeof(*info.mUIShader.GetCBData()));
	info.mUIShader.mCB.Unmap();

	mTitle.Draw(true);

	//Draw the PressA.
	float y =-0.25f;
	if(!NearlyEqual(mScaleOfPressA, 0.0f))
	{
		auto scale =XMMatrixScaling(1.0f, mScaleOfPressA, 1.0f);
		auto trans =XMMatrixTranslation(0.0f, y, 0.0f);
		auto world =MatrixMultiply(scale, trans);
		world =MatrixTranspose(world);
		StoreMatrix(&info.mUIShader.GetCBData()->mWorld, world);

		info.mUIShader.mCB.Map();
		CopyMemory(info.mUIShader.mCB.Access(), info.mUIShader.GetCBData(), sizeof(*info.mUIShader.GetCBData()));
		info.mUIShader.mCB.Unmap();

		mPressA.Draw(true);
	}
	//Draw the menu bar.
	if(!NearlyEqual(mScaleOfMenuBar, 0.0f))
	{
		auto scale =XMMatrixScaling(1.0f, mScaleOfMenuBar, 1.0f);
		auto trans =XMMatrixTranslation(0.0f, y, 0.0f);
		auto world =MatrixMultiply(scale, trans);
		world =MatrixTranspose(world);
		StoreMatrix(&info.mUIShader.GetCBData()->mWorld, world);

		info.mUIShader.mCB.Map();
		CopyMemory(info.mUIShader.mCB.Access(), info.mUIShader.GetCBData(), sizeof(*info.mUIShader.GetCBData()));
		info.mUIShader.mCB.Unmap();

		mMenuBar.Draw(true);
	}

	//Draw the menu contents.
	float distance =0.045f;
	if(!mPreparingMenuBar && math::NearlyEqual(mScaleOfMenuBar, 1.0f))
	{
		static const float sNot =0.2f;
		auto selected =[this](SelectMask mask) {return mSelector == mask;};

		info.mUIShader.GetCBData()->mAlpha =selected(Selec_Battle) ? 1.0f : sNot;
		World2DTp(&info.mUIShader.GetCBData()->mWorld, {0.0f, y +distance, 0.0f});
		info.mUIShader.mCB.Map();
		CopyMemory(info.mUIShader.mCB.Access(), info.mUIShader.GetCBData(), sizeof(*info.mUIShader.GetCBData()));
		info.mUIShader.mCB.Unmap();

		mMenuBattle.Draw(true);

		info.mUIShader.GetCBData()->mAlpha =selected(Selec_Credit) ? 1.0f : sNot;
		World2DTp(&info.mUIShader.GetCBData()->mWorld, {0.0f, y -distance, 0.0f});
		info.mUIShader.mCB.Map();
		CopyMemory(info.mUIShader.mCB.Access(), info.mUIShader.GetCBData(), sizeof(*info.mUIShader.GetCBData()));
		info.mUIShader.mCB.Unmap();

		mMenuCredit.Draw(true);

		info.mUIShader.GetCBData()->mAlpha =1.0f;
	}

	//Draw the credit.
	if(mDisplayingCredit)
	{
		World2DTp(&info.mUIShader.GetCBData()->mWorld, {0.0f, 0.0f, 0.0f});
		info.mUIShader.mCB.Map();
		CopyMemory(info.mUIShader.mCB.Access(), info.mUIShader.GetCBData(), sizeof(*info.mUIShader.GetCBData()));
		info.mUIShader.mCB.Unmap();

		mCredit.Draw(true);
	}

	//End the shader path.
	info.mUIShader.End();


	if(mPressingFrame !=0)
	{
		//Start the shader path.
		info.mEndParamShader.Begin();

		info.mEndParamShader.EnableConstantBuffer();

		//Update the projection matrix.
		if(info.mEndParamShader.GetStaticCBData()->mProjection != Polygon::Get2DProjection())
		{
			info.mEndParamShader.GetStaticCBData()->mProjection =Polygon::Get2DProjection();
			info.mEndParamShader.mStaticCB.Map();
			CopyMemory(info.mEndParamShader.mStaticCB.Access(), &Polygon::Get2DProjection(), sizeof(Float4x4));
			info.mEndParamShader.mStaticCB.Unmap();
		}

		manager.OMSetDepthBuffer(false);

		float x =-0.48f;
		float y =0.465f;
		World2DTp(&info.mEndParamShader.GetCBData()->mWorld, {x, y, 0.0f});
		info.mEndParamShader.GetCBData()->mParam =static_cast<float>(mPressingFrame) / static_cast<float>(smEndingPressFrame);

		info.mEndParamShader.mCB.Map();
		CopyMemory(info.mEndParamShader.mCB.Access(), info.mEndParamShader.GetCBData(), sizeof(*info.mEndParamShader.GetCBData()));
		info.mEndParamShader.mCB.Unmap();

		mEndParam.Draw();

		info.mEndParamShader.End();
	}

	manager.OMSetDepthBuffer(true);
}


void TitleExec::End()
{
	mBGM.Release();
	mEndParam.End();
	mCredit.Release();
	mMenuCredit.Release();
	mMenuBattle.Release();
	mMenuBar.Release();
	mPressA.Release();
	mTitle.Release();
	mTitleImage.Release();
}