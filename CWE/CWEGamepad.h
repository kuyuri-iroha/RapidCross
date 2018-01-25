#pragma once

#include "CWETimer.h"
#include "CWETypeDefinition.h"
#include <array>



namespace CWE
{
namespace callbacks
{
BOOL CALLBACK EnumGamePads(const DIDEVICEINSTANCE* pDIDInstance, VOID* pContext);
BOOL CALLBACK EnumGamePadsFunction(const DIDEVICEOBJECTINSTANCE* pDIDObInstance, VOID* pContext);
BOOL CALLBACK EnumGamePadsFunctionForSetDeadZone(const DIDEVICEOBJECTINSTANCE* pDIDObInstance, VOID* pContext);
}
class Input;


enum XIPadButton
{
	/*
	GamePad_DPAD_UP		=XINPUT_GAMEPAD_DPAD_UP,
	GamePad_DPAD_DOWN	=XINPUT_GAMEPAD_DPAD_DOWN,
	GamePad_DPAD_LEFT	=XINPUT_GAMEPAD_DPAD_LEFT,
	GamePad_DPAD_RIGHT	=XINPUT_GAMEPAD_DPAD_RIGHT,
	*/

	GamePad_START		=XINPUT_GAMEPAD_START,
	GamePad_BACK		=XINPUT_GAMEPAD_BACK,
	GamePad_LTHUMB		=XINPUT_GAMEPAD_LEFT_THUMB,
	GamePad_RTHUMB		=XINPUT_GAMEPAD_RIGHT_THUMB,
	GamePad_LSHOULDER	=XINPUT_GAMEPAD_LEFT_SHOULDER,
	GamePad_RSHOULDER	=XINPUT_GAMEPAD_RIGHT_SHOULDER,
	GamePad_A			=XINPUT_GAMEPAD_A,
	GamePad_B			=XINPUT_GAMEPAD_B,
	GamePad_X			=XINPUT_GAMEPAD_X,
	GamePad_Y			=XINPUT_GAMEPAD_Y,
};


enum DIPadButton
{
	GamePad_0  =0x00,
	GamePad_1  =0x01,
	GamePad_2  =0x02,
	GamePad_3  =0x03,
	GamePad_4  =0x04,
	GamePad_5  =0x05,
	GamePad_6  =0x06,
	GamePad_7  =0x07,
	GamePad_8  =0x08,
	GamePad_9  =0x09,
	GamePad_10 =0x0A,
	GamePad_11 =0x0B,
};


struct GamepadState
{
	int mAxis[6]; //-32768～32767
	bool mPOV_Up;
	bool mPOV_Down;
	bool mPOV_Left;
	bool mPOV_Right;
	bool mLeftTrigger; //XInput
	bool mRightTrigger; //XInput
};


struct GamepadEffect
{
	//0～10000
	int mMagnitude;
	//Second
	double mDuration;
	bool mInfinite;
};


class Gamepad
{
private:
	friend class CWE::Input;
	friend BOOL CALLBACK callbacks::EnumGamePads(const DIDEVICEINSTANCE* pDIDInstance, VOID* pContext);
	friend BOOL CALLBACK callbacks::EnumGamePadsFunction(const DIDEVICEOBJECTINSTANCE* pDIDObInstance, VOID* pContext);
	friend BOOL CALLBACK callbacks::EnumGamePadsFunctionForSetDeadZone(const DIDEVICEOBJECTINSTANCE* pDIDObInstance, VOID* pContext);


	LPDIRECTINPUTDEVICE8W mpDevice;
	LPDIRECTINPUTEFFECT mpDIEffect;

	DIJOYSTATE* mpDIState;
	XINPUT_STATE mXIState;
	GamepadState mState;
	DIEFFECT* mpDIEffectState;
	struct XIEffect
	{
		XINPUT_VIBRATION mState;
		bool mRunning;
	};
	XIEffect mXIEffect;

	Timer mEffectTimer;

	//識別番号
	int mNumber;
	GUID mIdentifier;

	bool mCanUse;
	bool mIsXInput;
	bool mCanUseEffect;

	struct DeadZone
	{
		int mValue;
		unsigned int mPercentage; //デフォルトは3000
	};
	DeadZone mDeadZone;


	Gamepad();
	~Gamepad();

	bool DIInit();
	bool XIInit(const unsigned& padNum);
	void Clear();
	void End();
	void Update();

public:
	Gamepad(const Gamepad&) =delete;
	Gamepad& operator=(const Gamepad&) =delete;

	//percentage: 0～10000
	//5000なら50%
	void ResetDeadZone(unsigned percentage);

	//コントローラーの振動エフェクトを設定する
	//true=成功 false=失敗
	//デバイスが対応していない場合はfalse
	bool ResetEffect(const GamepadEffect& effect);

	//現在このゲームパッドが利用可能かどうかの取得
	inline const bool& GetCanUse(){return mCanUse;}

	//このゲームパッドの取得方法がXInputかDirectInputを取得する
	//true=XInput false=DirectInput
	inline const bool& GetIsXInput(){return mIsXInput;}

	//未加工のXInputの情報を取得する
	void GetRawXIState(XIState& state);

	//未加工のDirectInputの情報を取得する
	//true=取得成功 false=取得失敗
	bool GetRawDIState(DIState& state);

	//ゲームパッドのButton関数で取得できない情報を取得する
	inline void GetState(GamepadState& state) {state =mState;}


	//任意のボタン情報の取得(XInput)
	//true=押されている false=押されていない
	bool Button(const XIPadButton& button);
	//任意のボタン情報の取得(DirectInput)
	//true=押されている false=押されていない
	bool Button(const DIPadButton& button);
	//GetInputButtonNum関数で取得した値でボタンの情報を取得
	bool Button(const InputCode& button);

	//押されたボタンの番号を取得
	//true=取得成功 false=取得失敗
	bool GetInputButtonNum(InputCode& buttonNum);

	//エフェクトの開始
	void EffectStart();
	//エフェクトの終了
	void EffectStop();

};



}//CWE

