#include "Loading.h"
#include "Information.h"

using namespace CWE;


LoadingExec::LoadingExec(int priority):
	TaskBase(priority, TaskName::Loading), mCamera(), mTime()
{
	mLoadingImage.Load(L"Resource\\Loading.png", FileFormat::Picture);
}

LoadingExec::~LoadingExec()
{
	End();
}


void LoadingExec::Update(TaskExec::List& taskList)
{
	Information& info =Information::GetInfo();

	if(!mTime.GetStarted())
	{
		mTime.Start();
	}

	if(info.mLoading)
	{
		mTime.Update();
	}

	mDraw =info.mLoading && mLoadingImage.LoadResult() == Task::Success;
}


void LoadingExec::Draw()
{
	Information& info =Information::GetInfo();
	using namespace math;
	Graphic& manager =Graphic::GetInstance();

	manager.OMSetDepthBuffer(false);

	//Viewport setting.
	mCamera.Set2D();

	info.mUIShader.EnableConstantBuffer();

	//Begin the raymarching shader.
	info.mUIShader.Begin(UIShader::PS_Loading);

	//Draw the background.
	info.mUIShader.UpdateProjection();

	World2DTp(&info.mUIShader.GetCBData()->mWorld, {0.4f, -0.33f, 0.0f});
	double time{};
	mTime.GetTime(time);
	info.mUIShader.GetCBData()->mTime =static_cast<float>(time);
	info.mUIShader.mCB.Map();
	CopyMemory(info.mUIShader.mCB.Access(), info.mUIShader.GetCBData(), sizeof(*info.mUIShader.GetCBData()));
	info.mUIShader.mCB.Unmap();

	mLoadingImage.Draw(true);

	info.mUIShader.End();
}


void LoadingExec::End()
{
	mLoadingImage.Release();
}