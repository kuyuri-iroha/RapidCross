#include "CWEKeyboard.h"
#include "CWEGraphicManager.h"
#include "CWEError.h"
#include "CWEUsefullyMacro.h"


namespace CWE
{


//コンストラクタ
Keyboard::Keyboard():
mpDevice(0)
{
}

//デストラクタ
Keyboard::~Keyboard()
{
}

//インスタンスの取得
Keyboard& Keyboard::GetInstance()
{
	static Keyboard instance;
	return instance;
}


//初期化処理
bool Keyboard::Init()
{
	if(FAILED(mpDevice->SetDataFormat(&c_dfDIKeyboard)))
	{
		FatalError::GetInstance().Outbreak(L"キーボードデバイスの初期化に失敗しました。\nキーボードが正しく接続されているか確認して下さい。");
		return false;
	}
	if(FAILED(mpDevice->SetCooperativeLevel(CWE::Graphic::GetInstance().mWindows.mMainWindow, (DISCL_FOREGROUND|DISCL_NONEXCLUSIVE))))
	{
		FatalError::GetInstance().Outbreak(L"キーボードデバイスの初期化に失敗しました。\nキーボードが正しく接続されているか確認して下さい。");
		return false;
	}
	ZeroMemory(mKeyState, 256);

	return true;
}


//終了処理
void Keyboard::End()
{
	SAFE_UNACQUIRE(mpDevice);
	SAFE_RELEASE(mpDevice);
}


//更新処理
void Keyboard::Update()
{
	if(FAILED(mpDevice->GetDeviceState(256, mKeyState)))
	{
		mpDevice->Acquire();
		mpDevice->GetDeviceState(256, mKeyState);
	}
}


//Windowキーをボタンを有効にするかをセットする
//返り値: true=成功 false=失敗
//引数: true=有効 false=無効
bool Keyboard::ResetWindowsKey(bool windowsKey)
{
	unsigned flags =(DISCL_FOREGROUND|DISCL_NONEXCLUSIVE);
	if(!windowsKey) {flags |=DISCL_NOWINKEY;}
	SAFE_UNACQUIRE(mpDevice);
	if(FAILED(mpDevice->SetCooperativeLevel(CWE::Graphic::GetInstance().mWindows.mMainWindow, flags)))
	{
		return false;
	}
	mpDevice->Acquire();
	return true;
}


//押されたキーのキーコードを取得
//true=取得成功 false=取得失敗
bool Keyboard::GetInputKeyCode(InputCode& keyCode)
{
	for(InputCode i; i.mInputCode<256; ++i.mInputCode)
	{
		if(Key(i))
		{
			keyCode.mInputCode =i.mInputCode;
			return true;
		}
	}
	return false;
}


}//CWE