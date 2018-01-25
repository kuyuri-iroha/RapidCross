#include "CWEWindows.h"
#include "CWEError.h"
#include "CWEStatic.h"
#include "CWEMacroConstants.h"


namespace CWE
{


// コンストラクタ
Windows::Windows()
{
	mIsWindowMode =true;
}

// デストラクタ
Windows::~Windows()
{
}


// 初期化
bool Windows::Init(const unsigned& width, const unsigned& height, const wchar_t windowTitle[128])
{
	// 初期化に使用するデータのセット
	mInstance =GetModuleHandle(NULL);
	lstrcpy(mWindowTitle, windowTitle);
	lstrcpy(mClassName, mWindowTitle);
	mWindowSizeX =width;
	mWindowSizeY =height;
	mInitialWindowSizeX =mWindowSizeX;
	mInitialWindowSizeY =mWindowSizeY;
	mWindowSizeExRateX =mWindowSizeX/mInitialWindowSizeX;
	mWindowSizeExRateY =mWindowSizeY/mInitialWindowSizeY;

	//多重起動の防止
	if(FindWindow(mClassName, mWindowTitle) != NULL)
	{
		return false;
	}

	//スクリーンセーバー無効化
	SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, FALSE, NULL, SPIF_SENDWININICHANGE);

	WNDCLASSEX wcx;

	wcx.cbSize =sizeof(wcx);
	wcx.style =(CS_HREDRAW | CS_VREDRAW);
	wcx.lpfnWndProc =callbacks::CWE_WndProc;
	wcx.cbClsExtra =0;
	wcx.cbWndExtra =0;
	wcx.hInstance =mInstance;
	wcx.hIcon =static_cast<HICON>(LoadImage(mInstance, (mIconID != 0) ? MAKEINTRESOURCE(mIconID) : IDI_APPLICATION, IMAGE_ICON, 0, 0, (LR_DEFAULTSIZE|LR_SHARED)));
	wcx.hCursor =static_cast<HCURSOR>(LoadImage(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, (LR_DEFAULTSIZE|LR_SHARED)));
	wcx.hbrBackground =static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
	wcx.lpszMenuName =NULL;
	wcx.lpszClassName =mClassName;
	wcx.hIconSm =static_cast<HICON>(LoadImage(mInstance, (mIconID != 0) ? MAKEINTRESOURCE(mIconID) : IDI_APPLICATION, IMAGE_ICON, 0, 0, (LR_DEFAULTSIZE|LR_SHARED)));

	if(RegisterClassEx(&wcx) == 0)
	{
		FatalError::GetInstance().Outbreak(L"ウィンドウクラスの登録に失敗しました。");
		return false;
	}



	//マウスカーソルを表示or非表示
	mIsMouseVisible =ResetMouseVisible(mIsWindowMode?true:false);

	//ウインドウの初期位置の確定
	int windowPlaceX =0,
		windowPlaceY =0;
	int monitorSizeX =GetSystemMetrics(SM_CXSCREEN),
		monitorSizeY =GetSystemMetrics(SM_CYSCREEN);
	if((mWindowSizeX <= monitorSizeX) && (mWindowSizeY <= monitorSizeY))
	{
		windowPlaceX =(monitorSizeX -mWindowSizeX) /2;
		windowPlaceY =(monitorSizeY -mWindowSizeY) /2;
	}

	//ウインドウの作成
	mMainWindow =CreateWindow(
						mClassName,
						mWindowTitle,
						CWE_WINDOW_MODE_STYLE, 
						windowPlaceX, 
						windowPlaceY, 
						mWindowSizeX, 
						mWindowSizeY,
						NULL,
						NULL, 
						mInstance, 
						NULL);

	//ウィンドウサイズの調整
	RECT client;
	GetClientRect(mMainWindow, &client);
	MoveWindow(mMainWindow, windowPlaceX, windowPlaceY, mWindowSizeX+(mWindowSizeX-client.right), mWindowSizeY+(mWindowSizeY-client.bottom), TRUE);
	


	if(mMainWindow == NULL)
	{
		FatalError::GetInstance().Outbreak(L"ウィンドウの作成に失敗しました。");
		return false;
	}

	ShowWindow(mMainWindow, SW_SHOW);
	UpdateWindow(mMainWindow);

	return true;
}


// 終了処理
void Windows::End()
{
	//スクリーンセーバー有効化
	SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, TRUE, NULL, SPIF_SENDWININICHANGE);

	//マウスを可視化しておく
	ResetMouseVisible(true);
}


// メッセージ処理
void Windows::ProcessMessage()
{
	if(PeekMessage(&m_WinMessage, NULL, 0, 0, PM_REMOVE))
	{
		//WM_QUITメッセージが発行されたら終了
		if(mWinMsgState.mQuitMessage) 
		{
			EndRequest();
		}

		TranslateMessage(&m_WinMessage);//メッセージを変換
		DispatchMessage(&m_WinMessage);//メッセージを送出
	}
}



}// CWE