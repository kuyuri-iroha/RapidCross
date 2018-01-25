#include "CWEAll.h"
#include "CWEGraphicManager.h"
#include "CWEInputManager.h"
#include "CWEGraphicResource.h"
#include "CWEError.h"




namespace CWE
{


bool Init(unsigned width, unsigned height, bool windowMode, const wchar_t windowTitle[128])
{
	if(!windowTitle) {return false;}

	if(XMVerifyCPUSupport() != TRUE)
	{
		FatalError::GetInstance().Outbreak(L"The application cannot be started.\nThe CPU may not be compatible.");
		return false;
	}

	if(!Graphic::GetInstance().mWindows.Init(width, height, windowTitle)){End(); return false;}
	if(!Graphic::GetInstance().Init()) {End(); return false;}
	if(!Input::GetInstance().Init()) {End(); return false;}
	if(!MasterSound::GetInstance().Init()) {End(); return false;}
	Graphic::GetInstance().ResetDisplayMode(width, height);
	Graphic::GetInstance().ResetWindowMode(windowMode);

#if defined(_DEBUG) || defined(DEBUG)
	OutputDebugStringF(L"***************************************************************************\n");
	OutputDebugStringF(L"================================== Start ==================================\n");
	OutputDebugStringF(L"***************************************************************************\n");
#endif
	
	return true;
}


bool Update()
{
	static Graphic &rGraphic =Graphic::GetInstance();
	static Resource &rGResource =Resource::GetInstance();
	static Input &rInput =Input::GetInstance();

	//非アクティブ状態での処理負荷軽減
	do
	{
		rGraphic.mWindows.ProcessMessage();
		rGraphic.ChackDeviceLost();
		rInput.Update();
		if(!rGraphic.mWindows.mWinMsgState.mActive) {Sleep(1);}
	}while(!rGraphic.mWindows.mWinMsgState.mActive && !rGraphic.mWindows.mWinMsgState.mQuitMessage);

	rGResource.LoadExecution(rGraphic.mWindows.mIsEndRequest);
	rGraphic.Update();

	return !rGraphic.mWindows.mIsEndRequest;
}


void End()
{
	Resource::GetInstance().End();
	Input::GetInstance().End();
	MasterSound::GetInstance().End();
	Graphic::GetInstance().End();
	Graphic::GetInstance().mWindows.End();

#if defined(_DEBUG) || defined(DEBUG)
	OutputDebugStringF(L"***************************************************************************\n");
	OutputDebugStringF(L"=================================== End ===================================\n");
	OutputDebugStringF(L"***************************************************************************\n\n");
#endif
}



}// CWE