#include "CWEInputManager.h"
#include "CWEKeyboard.h"
#include "CWEMouse.h"
#include "CWEGraphicManager.h"
#include "CWEError.h"
#include "CWEUsefullyMacro.h"



namespace CWE
{


//コンストラクタ
Input::Input():
mpDevice(0), mPadsNum(0), mUsingPadsNum(0), mSettingPadNumber(0),
mKeyboard(Keyboard::GetInstance()),
mMouse(Mouse::GetInstance())
{
}

//デストラクタ
Input::~Input()
{
}

//インスタンスの取得
Input& Input::GetInstance()
{
	static Input instance;
	return instance;
}


//初期化
bool Input::Init()
{
	//DirectInputの初期化
	if(FAILED(DirectInput8Create(Graphic::GetInstance().mWindows.mInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (LPVOID*)&mpDevice, NULL)))
	{
		(L"DirectInputの初期化に失敗しました。");
		return false;
	}
	//キーボードの初期化
	if(FAILED(mpDevice->CreateDevice(GUID_SysKeyboard, &mKeyboard.mpDevice, NULL)))
	{
		FatalError::GetInstance().Outbreak(L"キーボードデバイスの初期化に失敗しました。\nキーボードが正しく接続されているか確認して下さい。");
		return false;
	}
	if(!mKeyboard.Init()) {return false;}
	//マウスの初期化
	if(FAILED(mpDevice->CreateDevice(GUID_SysMouse, &mMouse.mpDevice, NULL)))
	{
		FatalError::GetInstance().Outbreak(L"マウスデバイスの初期化に失敗しました。\nマウスが正しく接続されているか確認して下さい。");
		return false;
	}
	if(!mMouse.Init(Graphic::GetInstance().mWindows.mMainWindow)) {return false;}
	//ゲームパッドの登録
	GamepadRegister();

	return true;
}


//ゲームパッドの登録
void Input::GamepadRegister()
{
	//一応全てのパッドの情報を抹消してから再設定する
	mPadsNum =0;
	for(i=0; i<CWE_GAMEPAD_MAX; ++i)
	{
		mGamePads[i].Clear();
	}

	//デバイスの初期化
	for(i=0; i<CWE_GAMEPAD_MAX; ++i)
	{
		mpDevice->EnumDevices(DI8DEVCLASS_GAMECTRL, callbacks::EnumGamePads, NULL, DIEDFL_ATTACHEDONLY);		
	}
}


//終了処理
void Input::End()
{
	//ゲームパッド
	for(i=0; i<CWE_GAMEPAD_MAX; ++i)
	{
		mGamePads[i].End();
	}
	//マウス
	mMouse.End();
	//キーボード
	mKeyboard.End();

	SAFE_RELEASE(mpDevice);
}


//更新処理
void Input::Update()
{
	static Windows& core =Graphic::GetInstance().mWindows;
	
	//アクティブ状態なら処理を行う
	if(core.mWinMsgState.mActive)
	{
		mKeyboard.Update();
		mMouse.Update();

		//個々のゲームパッドの更新
		for(i=0; i<CWE_GAMEPAD_MAX; ++i)
		{
			mGamePads[i].Update();
		}
	}
}



//ゲームパッドクラスの情報を提供する関数
Gamepad* const Input::GetGamepad()
{
	++mUsingPadsNum;
	CWE_ASSERT(mUsingPadsNum-1 < CWE_GAMEPAD_MAX && "ゲームパッドの数が多すぎます");
	return &mGamePads[mUsingPadsNum-1];
}


//取得したゲームパッドを手放す
void Input::ReleaseGamepad(Gamepad* pGamePad)
{
	if(pGamePad)
	{
		--mUsingPadsNum;
	}
	pGamePad =0;
}


//デバイスの接続状態に変更があったことを知らせる
//true=変更あり false=変更なし
bool Input::GetChangeConnection()
{
	if(mIsChangeConnection)
	{
		mIsChangeConnection =false;
		return true;
	}

	return false;
}



}//CWE