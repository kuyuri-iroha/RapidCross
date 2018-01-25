#include "CWEMouse.h"
#include "CWEGraphicManager.h"
#include "CWEStatic.h"
#include "CWEError.h"
#include "CWEUsefullyMacro.h"



namespace CWE
{


//コンストラクタ
Mouse::Mouse():
mpDevice(0), mPointerMode(Graphic::GetInstance().mWindows.mIsMouseVisible)
{
}

//デストラクタ
Mouse::~Mouse()
{
}

//インスタンス
Mouse& Mouse::GetInstance()
{
	static Mouse instance;
	return instance;
}


//初期化処理
bool Mouse::Init(const HWND window)
{
	//マウスデバイスのセット
	if(FAILED(mpDevice->SetDataFormat(&c_dfDIMouse2)))
	{
		CWE::FatalError::GetInstance().Outbreak(L"マウスデバイスの初期化に失敗しました。\nマウスが正しくつながれているか確認して下さい。");
		return false;
	}
	if(FAILED(mpDevice->SetCooperativeLevel(window, (DISCL_NONEXCLUSIVE|DISCL_FOREGROUND))))
	{
		CWE::FatalError::GetInstance().Outbreak(L"マウスデバイスの初期化に失敗しました。\nマウスが正しくつながれているか確認して下さい。");
		return false;
	}

	//マウスのヘッダー
	DIPROPDWORD aDIProp;
	aDIProp.diph.dwSize =sizeof(DIPROPDWORD);
	aDIProp.diph.dwHeaderSize =sizeof(DIPROPHEADER);
	aDIProp.diph.dwObj =0;
	aDIProp.diph.dwHow =DIPH_DEVICE;
	aDIProp.dwData =DIPROPAXISMODE_REL;
	if(FAILED(mpDevice->SetProperty(DIPROP_AXISMODE, &aDIProp.diph)))
	{
		CWE::FatalError::GetInstance().Outbreak(L"マウスデバイスの初期化に失敗しました。\nマウスが正しくつながれているか確認して下さい。");
		return false;
	}

	return true;
}


//終了処理
void Mouse::End()
{
	SAFE_UNACQUIRE(mpDevice);
	SAFE_RELEASE(mpDevice);
}


//更新処理
void Mouse::Update()
{
	static CWE::Windows& core =CWE::Graphic::GetInstance().mWindows;

	//マウス情報の取得
	if(FAILED(mpDevice->GetDeviceState(sizeof(DIMOUSESTATE2), &mState)))
	{
		mpDevice->Acquire();
		mpDevice->GetDeviceState(sizeof(DIMOUSESTATE2), &mState);
	}

	//マウスカーソルを使わないときの処理
	if(!mPointerMode)
	{
		//座標データの代入
		mX =mState.lX;
		mY =mState.lY;
	}
	//それ以外
	else
	{
		// スクリーン上でのカーソルの位置を取得
		GetCursorPos(&mMousePos);

		//ウインドウモードの場合、座標をクライアント領域に合わせる
		if(core.mIsWindowMode)
		{
			mMousePos.x -=core.mClientPos.x;
			mMousePos.y -=core.mClientPos.y;

			
			mMousePos.x =(int)(mMousePos.x /core.mWindowSizeExRateX);
			mMousePos.y =(int)(mMousePos.y /core.mWindowSizeExRateY);
		}

		mX =mMousePos.x;
		mY =mMousePos.y;
	}

}


//マウスカーソルを使った処理を有効にするかどうか
//デフォルトは「使用する」
bool Mouse::ResetPointerMode(const bool& usePointer)
{
	mPointerMode =usePointer;
	CWE::Graphic::GetInstance().mWindows.mIsMouseVisible =ResetMouseVisible(usePointer);

	//マウスの強調レベルのリセット
	SAFE_UNACQUIRE(mpDevice);
	if(FAILED(mpDevice->SetCooperativeLevel(CWE::Graphic::GetInstance().mWindows.mMainWindow, ((usePointer ? DISCL_NONEXCLUSIVE:DISCL_EXCLUSIVE)|DISCL_FOREGROUND))))
	{
		return false;
	}
	mpDevice->Acquire();
	return true;
}


//押されたボタンの番号を取得
//true=取得成功 false=取得失敗
bool Mouse::GetInputButtonNum(InputCode& buttonNum)
{
	for(InputCode i; i.mInputCode<8; ++i.mInputCode)
	{
		if(Button(i))
		{
			buttonNum =i;
			return true;
		}
	}
	return false;
}


}//CWE