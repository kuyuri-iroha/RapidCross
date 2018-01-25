#include "FPS.h"
#include "Information.h"
#include <cmath>



// コンストラクタ
FPS::FPS():
	TaskBase(INT_MIN, TaskName::FPS),
	m2DMagnifX(1.0f /float(CWE_DEFAULT_WINDOW_SIZE_X)), m2DMagnifY(1.0f /float(CWE_DEFAULT_WINDOW_SIZE_Y)),
	mTime(1.0)
{
	float offsetX =-944*m2DMagnifX,
		  offsetY =524*m2DMagnifY;
	mPos.x() =offsetX;
	mPos.y() =offsetY;
}


// デストラクタ
FPS::~FPS()
{
	End();
}


// ロードの完了を通知
bool FPS::LoadResult() const
{
	return Information::GetInfo().DebugCharsLoaded();
}


// FPSを算出
void FPS::Calculation()
{
	if(mTimer.GetStarted())
	{
		mTimer.Update();
		mTimer.GetTime(mTime); //経過時間を取得
	}

	if(mTime < 1.0)
	{
		++mPassingFrameCnt;
	}
	else
	{
		mFPS =mPassingFrameCnt;
		mPassingFrameCnt =0;
		mTimer.Start();
	}
}


// 更新
void FPS::Update(TaskExec::List& taskList)
{
	if(LoadResult())
	{
		mDraw =true;

		Calculation();
	}
	else
	{
		mDraw =false;
	}
}


// 描画
void FPS::Draw()
{
	using namespace CWE;
	Information& info =Information::GetInfo();
	Graphic& manager =Graphic::GetInstance();

	//シェーダのセット
	info.mUIShader.Begin();

	//定数バッファの有効化
	info.mUIShader.EnableConstantBuffer();

	info.mUIShader.UpdateProjection();

	//FPSの桁数回描画
	int index;
	float adding =0;
	manager.OMSetDepthBuffer(false);
	for(int i=int(log10f(float(mFPS)))+1; 0<i; --i)
	{
		index =mFPS;

		//描画する桁の数を算出
		for(int j=i-1; 0<j; --j) {index /=10;}
		index %=10;

		//定数バッファの更新
		mPos.x() -=adding;
		math::World2DTp(&info.mUIShader.GetCBData()->mWorld, mPos);
		mPos.x() +=adding;
		info.mUIShader.mCB.Map();
		CopyMemory(info.mUIShader.mCB.Access(), info.mUIShader.GetCBData(), sizeof(*info.mUIShader.GetCBData()));
		info.mUIShader.mCB.Unmap();

		//描画
		info.DrawDebugChar(index);

		//次の桁の位置を算出
		adding -=mPos.x()+0.501f;
	}
	manager.OMSetDepthBuffer(true);
}


void FPS::End()
{
	Information::GetInfo().mUIShader.DisableConstantBuffer();
}