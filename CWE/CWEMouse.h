#pragma once

#include "CWETypeDefinition.h"
#include "CWEError.h"



namespace CWE
{
class Input;
namespace callbacks//使用禁止
{
LRESULT CALLBACK CWE_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
}


//マウスのボタン入力用マスク
enum MouseButton
{
	Mouse_Left =0x00,
	Mouse_Right =0x01,
	Mouse_Middle =0x02
};


//マウス入力をサポートするクラス
class Mouse
{
private:
	//フレンド
	friend class CWE::Input;
	friend LRESULT CALLBACK callbacks::CWE_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);//(呼び出し禁止)


	//マウスのデバイス
	LPDIRECTINPUTDEVICE8 mpDevice;
	//現在のマウス情報
	DIMOUSESTATE2 mState;
	//クライアント領域におけるマウスの座標
	POINT mMousePos;

	//マウスカーソルを使った処理が有効かどうか
	bool mPointerMode;

	//マウスの座標情報(カーソルが有効化によって異なる)
	int mX, mY;


	//シングルトンパターンによる実装
	Mouse();
	~Mouse();

	//初期化
	bool Init(const HWND window);
	//終了処理
	void End();
	//更新処理
	void Update();


public:
	//シングルトンパターンによる実装
	static Mouse& GetInstance();
	Mouse(const Mouse&) =delete;
	Mouse& operator=(const Mouse&) =delete;

	//マウスカーソルを使った処理の使用をセット	
	//デフォルトは「使用する」
	bool ResetPointerMode(const bool& usePointer);
	//マウスの座標データの取得
	inline void GetPoint(int& x, int& y) {x =mX; y =mY;}
	//任意のマウスボタンの情報を取得
	//true=押されている false=押されていない
	inline bool Button(const MouseButton& button) {return (mState.rgbButtons[button] & 0x80) != 0;}
	//GetInputButtonNum関数で取得した値でボタンの情報を取得
	inline bool Button(const InputCode& button) {CWE_ASSERT(button.mInputCode<8); return (mState.rgbButtons[button.mInputCode] & 0x80) != 0;}

	//押されたボタンの番号を取得
	//true=取得成功 false=取得失敗
	bool GetInputButtonNum(InputCode& buttonNum);
};


}//CWE

