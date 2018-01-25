#pragma once

#include "CWEIncludedAPI.h"


namespace CWE
{

//コールバック関数分離用名前空間
namespace callbacks//使用禁止
{
LRESULT CALLBACK CWE_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
}


class Mouse;
class Keyboard;
class Gamepad;




// Windows用情報クラス(ライブラリ外部からの生成不可)
class Windows
{
private:
	Windows();
	~Windows();
	Windows(const Windows&) =delete;
	Windows& operator=(const Windows&) =delete;

	friend LRESULT CALLBACK callbacks::CWE_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);//(呼び出し禁止)
	friend class Graphic;
	friend class Input;
	friend bool Init(unsigned width, unsigned height, bool windowMode, const wchar_t windowTitle[128]);
	friend bool Update();
	friend void End();
	friend class Mouse;
	friend class Keyboard;
	friend class Gamepad;


	HINSTANCE mInstance; //ソフトのインスタンスハンドル
	HWND mMainWindow; //メインウインドウハンドル
	MSG m_WinMessage; //ウインドウメッセージ
	wchar_t mClassName[128]; //メインウインドウのクラス名
	wchar_t mWindowTitle[128]; //ウインドウタイトル
	WORD mIconID; //使用するアイコンのID
	int mInitialWindowSizeX, //ウィンドウ作成時のウィンドウのX座標のサイズ
		mInitialWindowSizeY; //ウィンドウ作成時のウィンドウのY座標のサイズ
	int mWindowSizeX, //ウィンドウのX座標のサイズ
		mWindowSizeY; //ウィンドウのY座標のサイズ
	double mWindowSizeExRateX, //ウィンドウの初期サイズに対する現在のウィンドウサイズの比率X
		   mWindowSizeExRateY; //ウィンドウの初期サイズに対する現在のウィンドウサイズの比率Y
	POINT mClientPos; //ウインドウのクライアント領域の左上の座標
	bool mIsWindowMode; //ウインドウモードフラグ

	bool mIsMouseVisible; //マウスポインタの表示フラグ
	bool mIsEndRequest; //ソフトの終了が要求されたかどうか

		
	struct WinMsgState
	{
		bool mQuitMessage; //WM_QUITメッセージが送られてきたかどうか
		bool mActive; //WM_ACTIVATE時にウインドウがアクティブになったかどうか

	}mWinMsgState;


	//初期化
	//返り値: true=成功, false=失敗
	bool Init(const unsigned& width, const unsigned& height, const wchar_t windowTitle[128]);

	//終了処理
	void End();

	//メッセージ処理
	void ProcessMessage();

public:
	//ソフトの終了要求をする
	inline void EndRequest() {mIsEndRequest =true;}

	//ソフトアイコンのセット
	inline void SetIconImage(WORD iconHandle) {mIconID =iconHandle;}

	// 現在のウインドウサイズを取得
	inline int GetWindowSizeX() {return mWindowSizeX;}
	inline int GetWindowSizeY() {return mWindowSizeY;}

};


}