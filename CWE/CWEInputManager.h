#pragma once

#include "CWEIncludedAPI.h"
#include "CWEGamePad.h"
#include "CWEMacroConstants.h"


namespace CWE
{
namespace callbacks//使用禁止
{
LRESULT CALLBACK CWE_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK EnumGamePads(const DIDEVICEINSTANCE* pDIDInstance, VOID* pContext);
BOOL CALLBACK EnumGamePadsFunction(const DIDEVICEOBJECTINSTANCE* pDIDObInstance, VOID* pContext);
BOOL CALLBACK EnumGamePadsFunctionForSetDeadZone(const DIDEVICEOBJECTINSTANCE* pDIDObInstance, VOID* pContext);
}

class Keyboard;
class Mouse;
class Gamepad;


//入力モジュール管理クラス
class Input
{
private:
	//フレンド
	friend bool Init(unsigned width, unsigned height, bool windowMode, const wchar_t windowTitle[128]);
	friend bool Update();
	friend void End();
	friend class Keyboard;
	friend class Mouse;
	friend class Gamepad;

	//参照
	Keyboard& mKeyboard;
	Mouse& mMouse;


	//DirectInputのデバイス
	LPDIRECTINPUT8 mpDevice;

//========================== ゲームパッド管理用==================================================

	friend LRESULT CALLBACK callbacks::CWE_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);//(呼び出し禁止)
	friend BOOL CALLBACK callbacks::EnumGamePads(const DIDEVICEINSTANCE* pDIDInstance, VOID* pContext);//(呼び出し禁止)
	friend BOOL CALLBACK callbacks::EnumGamePadsFunction(const DIDEVICEOBJECTINSTANCE* pDIDObInstance, VOID* pContext);//(呼び出し禁止)
	friend BOOL CALLBACK callbacks::EnumGamePadsFunctionForSetDeadZone(const DIDEVICEOBJECTINSTANCE* pDIDObInstance, VOID* pContext);//(呼び出し禁止)


	//ゲームパッドのデバイス
	Gamepad mGamePads[CWE_GAMEPAD_MAX];

	//接続中のゲームパッド数
	unsigned mPadsNum;
	//利用中のゲームパッド数
	unsigned mUsingPadsNum;
	//設定中のゲームパッド番号
	unsigned mSettingPadNumber;
	//デバイスの接続状態に変更があったか
	bool mIsChangeConnection;

	//内部利用の汎用変数
	unsigned char i;

	
	//ゲームパッドの登録
	void GamepadRegister();

//========================================================

	//初期化
	bool Init();
	//終了処理
	void End();
	//更新処理
	void Update();
	
	Input();
	~Input();
	Input(const Input&) =delete;
	Input& operator=(const Input&) =delete;

public:
	//シングルトンパターンの実装
	static Input& GetInstance();


	//ゲームパッドクラスの情報を提供する関数
	//必ずCGamePad*型の変数に格納して使うこと
	Gamepad* const GetGamepad();

	//取得したゲームパッドを手放す
	void ReleaseGamepad(Gamepad* pGamepad);

	//ゲームパッドデバイスの接続状態に変更があったことを知らせる
	//true=変更あり false=変更なし
	bool GetChangeConnection();

};



}// CWE

