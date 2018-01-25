#include "CWEInputManager.h"
#include "CWEGamepad.h"
#include "CWEGraphicManager.h"
#include "CWEError.h"
#include "CWEUsefullyMacro.h"


namespace CWE
{


//ゲームパッド用のコールバック関数の宣言
extern BOOL CALLBACK callbacks::EnumGamePads(const DIDEVICEINSTANCE* pDIDInstance, VOID* pContext);
extern BOOL CALLBACK callbacks::EnumGamePadsFunction(const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext);
extern BOOL CALLBACK callbacks::EnumGamePadsFunctionForSetDeadZone(const DIDEVICEOBJECTINSTANCE* pDIDObInstance, VOID* pContext);


//========================== 個々のゲームパッド =============================


//コンストラクタ
Gamepad::Gamepad():
mNumber(-1),
mCanUse(false), mIsXInput(false), mCanUseEffect(false),
mpDevice(0), mpDIState(0), mpDIEffect(0), mpDIEffectState(0)
{
	mDeadZone.mValue =XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
	mDeadZone.mPercentage =3000;
	mXIEffect.mState.wLeftMotorSpeed =65535;
	mXIEffect.mState.wRightMotorSpeed =65535;
	mXIEffect.mRunning =false;
}

//デストラクタ
Gamepad::~Gamepad()
{
}


//DirectInput時の初期化
bool Gamepad::DIInit()
{
	//フォーマットのセット
	if(FAILED(mpDevice->SetDataFormat(&c_dfDIJoystick)))
	{
		return false;
	}
	//協調レベルのセット
	if(FAILED(mpDevice->SetCooperativeLevel(CWE::Graphic::GetInstance().mWindows.mMainWindow, (DISCL_EXCLUSIVE|DISCL_FOREGROUND))))
	{
		return false;
	}
	//軸の最大値・最小値のセット
	if(FAILED(mpDevice->EnumObjects(callbacks::EnumGamePadsFunction, CWE::Graphic::GetInstance().mWindows.mMainWindow, DIDFT_AXIS)))
	{
		return false;
	}
	//設定するゲームパッドの番号をセット
	CWE::Input::GetInstance().mSettingPadNumber =mNumber;
	//デッドゾーンのセット
	if(FAILED(mpDevice->EnumObjects(callbacks::EnumGamePadsFunctionForSetDeadZone, CWE::Graphic::GetInstance().mWindows.mMainWindow, DIDFT_AXIS)))
	{
		return false;
	}

	//エフェクトの作成
	mpDIEffectState =new DIEFFECT;

	DWORD axes[2] ={DIJOFS_X, DIJOFS_Y};
	LONG direction[2] ={0, 0};
	DICONSTANTFORCE cf;

	cf.lMagnitude =10000;
	mpDIEffectState->dwSize                  =sizeof( DIEFFECT );
	mpDIEffectState->dwFlags                 =DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
	mpDIEffectState->dwDuration              =INFINITE;
	mpDIEffectState->dwSamplePeriod          =0;
	mpDIEffectState->dwGain                  =DI_FFNOMINALMAX;
	mpDIEffectState->dwTriggerButton         =DIEB_NOTRIGGER;
	mpDIEffectState->dwTriggerRepeatInterval =0;
	mpDIEffectState->cAxes                   =2;
	mpDIEffectState->rgdwAxes                =axes;
	mpDIEffectState->rglDirection            =direction;
	mpDIEffectState->lpEnvelope              =0;
	mpDIEffectState->cbTypeSpecificParams    =sizeof(DICONSTANTFORCE);
	mpDIEffectState->lpvTypeSpecificParams   =&cf;
	mpDIEffectState->dwStartDelay            =0;

	mpDevice->CreateEffect(GUID_ConstantForce, mpDIEffectState, &mpDIEffect, NULL);
	if(mpDIEffect != NULL) {mCanUseEffect =true;}
	
	mCanUse =true;
	mIsXInput =false;
	mpDIState =new DIJOYSTATE;

	mpDevice->Acquire();

	return true;
}


//XInput時の初期化
bool Gamepad::XIInit(const unsigned& padNum)
{
	if(XInputGetState(padNum, &mXIState) == ERROR_SUCCESS)
	{
		mIsXInput =true;
		mCanUse =true;
		mCanUseEffect =true;
		return true;
	}
	else
	{
		mIsXInput =false;
		mCanUse =false;
		mCanUseEffect =false;
		return false;
	}
}


//内部情報抹消処理
void Gamepad::Clear()
{
	ZeroMemory(&mIdentifier, sizeof(mIdentifier));
	ZeroMemory(&mXIState, sizeof(mXIState));
	mNumber =-1;
	mCanUse =false;
	mIsXInput =false;

	End();
}


//終了処理
void Gamepad::End()
{
	SAFE_DELETE(mpDIEffectState);
	SAFE_DELETE(mpDIState);
	SAFE_UNACQUIRE(mpDevice);
	SAFE_RELEASE(mpDevice);
}


//更新処理
void Gamepad::Update()
{
	//使用可能なら
	if(mCanUse)
	{
		//XInputなら
		if(mIsXInput)
		{
			//状態の取得
			XInputGetState(mNumber, &mXIState);
			//デッドゾーン
			if( (mXIState.Gamepad.sThumbLX < mDeadZone.mValue && mXIState.Gamepad.sThumbLX > -mDeadZone.mValue)
				&& (mXIState.Gamepad.sThumbLY < mDeadZone.mValue && mXIState.Gamepad.sThumbLY > -mDeadZone.mValue) )
			{	
				mXIState.Gamepad.sThumbLX = 0;
				mXIState.Gamepad.sThumbLY = 0;
			}
			if((mXIState.Gamepad.sThumbRX < mDeadZone.mValue && mXIState.Gamepad.sThumbRX > -mDeadZone.mValue)
				&& (mXIState.Gamepad.sThumbRY < mDeadZone.mValue && mXIState.Gamepad.sThumbRY > -mDeadZone.mValue) ) 
			{
				mXIState.Gamepad.sThumbRX = 0;
				mXIState.Gamepad.sThumbRY = 0;
			}
			//状態を保存
			mState.mAxis[0] =mXIState.Gamepad.sThumbLX;
			mState.mAxis[1] =mXIState.Gamepad.sThumbLY*-1;
			mState.mAxis[2] =mXIState.Gamepad.sThumbRX;
			mState.mAxis[3] =mXIState.Gamepad.sThumbRY*-1;
			mState.mLeftTrigger =(64 < mXIState.Gamepad.bLeftTrigger);
			mState.mRightTrigger =(64 < mXIState.Gamepad.bRightTrigger);
			mState.mPOV_Up =(mXIState.Gamepad.wButtons & 0x0001) != 0;
			mState.mPOV_Down =(mXIState.Gamepad.wButtons & 0x0002) != 0;
			mState.mPOV_Left =(mXIState.Gamepad.wButtons & 0x0004) != 0;
			mState.mPOV_Right =(mXIState.Gamepad.wButtons & 0x0008) != 0;

			//エフェクトの自動終了
			if(mXIEffect.mRunning)
			{
				mEffectTimer.Update();
				if(mEffectTimer.TimeIsPassed())
				{
					EffectStop();
				}
			}
		}
		//DirectInputなら
		else
		{
			//状態の取得
			mpDevice->Poll();
			if(FAILED(mpDevice->GetDeviceState(sizeof(DIJOYSTATE), mpDIState)))
			{
				mpDevice->Acquire();
				mpDevice->GetDeviceState(sizeof(DIJOYSTATE), mpDIState);
			}
			//状態の保存
			mState.mAxis[0] =mpDIState->lX;
			mState.mAxis[1] =mpDIState->lY;
			mState.mAxis[2] =mpDIState->lZ;
			mState.mAxis[3] =mpDIState->lRx;
			mState.mAxis[4] =mpDIState->lRy;
			mState.mAxis[5] =mpDIState->lRz;
			//方向コントローラーフラグの初期化
			mState.mPOV_Up =false;
			mState.mPOV_Down =false;
			mState.mPOV_Left =false;
			mState.mPOV_Right =false;
			//方向コントローラーフラグ
			switch(mpDIState->rgdwPOV[0])
			{
			case 0:
				mState.mPOV_Up =true;
				break;

			case 4500:
				mState.mPOV_Up =true;
				mState.mPOV_Right =true;
				break;

			case 9000:
				mState.mPOV_Right =true;
				break;

			case 13500:
				mState.mPOV_Right =true;
				mState.mPOV_Down =true;
				break;

			case 18000:
				mState.mPOV_Down =true;
				break;

			case 22500:
				mState.mPOV_Down =true;
				mState.mPOV_Left =true;
				break;

			case 27000:
				mState.mPOV_Left =true;
				break;

			case 31500:
				mState.mPOV_Left =true;
				mState.mPOV_Up =true;
				break;
			}
		}
	}
}


//コントローラーのデッドゾーンを設定する
//percentage: 0～10000
//5000なら50%
void Gamepad::ResetDeadZone(unsigned percentage)
{
	if(percentage < 0) {percentage =0;}
	if(10000 < percentage) {percentage =10000;}
	mDeadZone.mValue =static_cast<int>(CWE_GAMEPAD_AXIS_MAX*(percentage*0.0001));
	mDeadZone.mPercentage =percentage;
	//DirectInputなら
	if(!mIsXInput)
	{
		//設定するゲームパッドの番号をセット
		CWE::Input::GetInstance().mSettingPadNumber =mNumber;
		//デッドゾーンのセット
		mpDevice->EnumObjects(callbacks::EnumGamePadsFunctionForSetDeadZone, CWE::Graphic::GetInstance().mWindows.mMainWindow, DIDFT_AXIS);
	}
}


//コントローラーの振動エフェクトを設定する
//true=成功 false=失敗
//デバイスが対応していない場合はfalse
bool Gamepad::ResetEffect(const GamepadEffect& effect)
{
	//振動エフェクトが使えない場合や値が無効なときは問答無用でfalse
	if((!mCanUseEffect) || (effect.mDuration<=0.0) || ((effect.mMagnitude<0)||(10000<effect.mMagnitude))) {return false;}

	//XInputなら
	if(mIsXInput)
	{
		mXIEffect.mState.wLeftMotorSpeed =static_cast<WORD>(65535*(effect.mMagnitude*0.0001));
		mXIEffect.mState.wRightMotorSpeed =static_cast<WORD>(65535*(effect.mMagnitude*0.0001));
		mEffectTimer.Set(effect.mDuration);

		if(mXIEffect.mRunning) {XInputSetState(mNumber, &mXIEffect.mState);}
	}
	//DirectInputなら
	else
	{
		DICONSTANTFORCE cf;

		cf.lMagnitude =effect.mMagnitude;
		mpDIEffectState->dwDuration =effect.mInfinite ? INFINITE : (DWORD)(effect.mDuration*DI_SECONDS);
		mpDIEffectState->cbTypeSpecificParams =sizeof(DICONSTANTFORCE);
		mpDIEffectState->lpvTypeSpecificParams =&cf;

		if(FAILED(mpDIEffect->SetParameters(mpDIEffectState, (DIEP_DURATION|DIEP_TYPESPECIFICPARAMS))))
		{
			return false;
		}
	}


	return true;
}


//未加工のXInputの情報を取得する
void Gamepad::GetRawXIState(XIState& state)
{
	state =mXIState;
}


//未加工のDirectInputの情報を取得する
//true=取得成功 false=取得失敗
bool Gamepad::GetRawDIState(DIState& state)
{
	if(mpDIState)
	{
		state =*mpDIState;
		return true;
	}

	return false;
}


//任意のボタン情報の取得(XInput)
//true=押されている false=押されていない
bool Gamepad::Button(const XIPadButton& button)
{
	//使用可能なら
	if(mCanUse)
	{
		//XInputなら
		if(mIsXInput)
		{
			return (mXIState.Gamepad.wButtons & button) != 0;
		}
	}

	return false;
}


//任意のボタン情報の取得(DirectInput)
//true=押されている false=押されていない
bool Gamepad::Button(const DIPadButton& button)
{
	//使用可能なら
	if(mCanUse)
	{
		//DirectInputなら
		if(!mIsXInput)
		{
			return (mpDIState->rgbButtons[button] & 0x80) != 0;
		}
	}

	return false;
	
}


//GetInputButtonNum関数で取得した値でボタンの情報を取得
bool Gamepad::Button(const InputCode& button)
{
	//使用可能なら
	if(mCanUse)
	{
		//XInputなら
		if(mIsXInput)
		{
			return (mXIState.Gamepad.wButtons & button.mInputCode) != 0;
		}
		//DirectInputなら
		else
		{
			CWE_ASSERT(button.mInputCode<32);
			return (mpDIState->rgbButtons[button.mInputCode] & 0x80) != 0;
		}
	}

	return false;
}


//押されたボタンの番号を取得
//true=取得成功 false=取得失敗
bool Gamepad::GetInputButtonNum(InputCode& buttonNum)
{
	//使用可能なら
	if(mCanUse)
	{
		//XInputなら
		if(mIsXInput)
		{
			InputCode i;
			i.mInputCode =GamePad_START;
			for(; i.mInputCode<=GamePad_Y; i.mInputCode<<=1)
			{
				if(Button(i))
				{
					//方向コントローラーの入力は無視する
					if(!((i.mInputCode==1) | (i.mInputCode==2) | (i.mInputCode==4) | (i.mInputCode==8)))
					{
						buttonNum =i;
						return true;
					}
				}
			}
		}
		//DirectInputなら
		else
		{
			for(InputCode i; i.mInputCode<128; ++i.mInputCode)
			{
				if(Button(i))
				{
					buttonNum =i;
					return true;
				}
			}
		}
	}

	return false;
}


//エフェクトの開始
void Gamepad::EffectStart()
{
	if(mCanUseEffect)
	{
		if(mIsXInput)
		{
			XInputSetState(mNumber, &mXIEffect.mState);
			mEffectTimer.Start();
			mXIEffect.mRunning =true;
		}
		else
		{
			mpDIEffect->Start(1, 0);
		}
	}
}


//エフェクトの終了
void Gamepad::EffectStop()
{
	if(mCanUseEffect)
	{
		if(mIsXInput)
		{
			XINPUT_VIBRATION xief;
			ZeroMemory(&xief, sizeof(xief));
			XInputSetState(mNumber, &xief);
			mXIEffect.mRunning =false;
		}
		else
		{
			mpDIEffect->Stop();
		}
	}
}


}//CWE