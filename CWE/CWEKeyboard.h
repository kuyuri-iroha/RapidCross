#pragma once

#include "CWETypeDefinition.h"
#include "CWEError.h"


namespace CWE
{
class Input;

//キーコードの列挙
enum KeyCode
{
	Key_ESCAPE			=DIK_ESCAPE, //「Esc」キー
	Key_1				=DIK_1, //「1」キー
	Key_2				=DIK_2, //「2」キー
	Key_3				=DIK_3, //「3」キー
	Key_4				=DIK_4, //「4」キー
	Key_5				=DIK_5, //「5」キー
	Key_6				=DIK_6, //「6」キー
	Key_7				=DIK_7, //「7」キー
	Key_8				=DIK_8, //「8」キー
	Key_9				=DIK_9, //「9」キー
	Key_0				=DIK_0, //「0」キー
	Key_MINUS			=DIK_MINUS,  //「-」キー
	Key_EQUALS			=DIK_EQUALS, //「=」キー
	Key_BACKSPACE		=DIK_BACK, //「Backspace」キー
	Key_TAB				=DIK_TAB, //「Tab」キー
	Key_Q				=DIK_Q, //「Q」キー
	Key_W				=DIK_W, //「W」キー
	Key_E				=DIK_E, //「E」キー
	Key_R				=DIK_R, //「R」キー
	Key_T				=DIK_T, //「T」キー
	Key_Y				=DIK_Y, //「Y」キー
	Key_U				=DIK_U, //「U」キー
	Key_I				=DIK_I, //「I」キー
	Key_O				=DIK_O, //「O」キー
	Key_P				=DIK_P, //「P」キー
	Key_LBRACKET		=DIK_LBRACKET, //「 [ 」キー
	Key_RBRACKET		=DIK_RBRACKET, //「 ] 」キー
	Key_ENTER			=DIK_RETURN, //「Enter」キー
	Key_LCONTROL		=DIK_LCONTROL, //「Ctrl」キー（左）
	Key_A				=DIK_A, //「A」キー
	Key_S				=DIK_S, //「S」キー
	Key_D				=DIK_D, //「D」キー
	Key_F				=DIK_F, //「F」キー
	Key_G				=DIK_G, //「G」キー
	Key_H				=DIK_H, //「H」キー
	Key_J				=DIK_J, //「J」キー
	Key_K				=DIK_K, //「K」キー
	Key_L				=DIK_L, //「L」キー
	Key_SEMICOLON		=DIK_SEMICOLON, //「;」キー
	Key_APOSTROPHE		=DIK_APOSTROPHE, //「 ' 」キー
	Key_GRAVE			=DIK_GRAVE, //「 ` 」キー
	Key_LSHIFT			=DIK_LSHIFT, //「Shift」キー（左）
	Key_BACKSLASH		=DIK_BACKSLASH, //「\」キー
	Key_Z				=DIK_Z, //「Z」キー
	Key_X				=DIK_X, //「X」キー
	Key_C				=DIK_C, //「C」キー
	Key_V				=DIK_V, //「V」キー
	Key_B				=DIK_B, //「B」キー
	Key_N				=DIK_N, //「N」キー
	Key_M				=DIK_M, //「M」キー
	Key_COMMA			=DIK_COMMA, //「 , 」キー
	Key_PERIOD			=DIK_PERIOD, //「 . 」キー
	Key_SLASH			=DIK_SLASH, //「/」キー
	Key_RSHIFT			=DIK_RSHIFT, //「Shift」キー（右）
	Key_MULTIPLY		=DIK_MULTIPLY, //「*」キー（Numpad）
	Key_LALT			=DIK_LMENU, //「Alt」キー（左）
	Key_SPACE			=DIK_SPACE, //「Space」キー
	Key_CAPSLOCK		=DIK_CAPITAL, //「Caps Lock」キー
	Key_F1				=DIK_F1, //「F1」キー
	Key_F2				=DIK_F2, //「F2」キー
	Key_F3				=DIK_F3, //「F3」キー
	Key_F4				=DIK_F4, //「F4」キー
	Key_F5				=DIK_F5, //「F5」キー
	Key_F6				=DIK_F6, //「F6」キー
	Key_F7				=DIK_F7, //「F7」キー
	Key_F8				=DIK_F8, //「F8」キー
	Key_F9				=DIK_F9, //「F9」キー
	Key_F10				=DIK_F10, //「F10」キー
	Key_NUMLOCK			=DIK_NUMLOCK, //「Num Lock」キー
	Key_SCROLL			=DIK_SCROLL, //「Scroll Lock」キー
	Key_NUMPAD7			=DIK_NUMPAD7, //「7」キー（Numpad）
	Key_NUMPAD8			=DIK_NUMPAD8, //「8」キー（Numpad）
	Key_NUMPAD9			=DIK_NUMPAD9, //「9」キー（Numpad）
	Key_SUBTRACT		=DIK_SUBTRACT, //「-」キー（Numpad）
	Key_NUMPAD4			=DIK_NUMPAD4, //「4」キー（Numpad）
	Key_NUMPAD5			=DIK_NUMPAD5, //「5」キー（Numpad）
	Key_NUMPAD6			=DIK_NUMPAD6, //「6」キー（Numpad）
	Key_ADD				=DIK_ADD, //「+」キー（Numpad）
	Key_NUMPAD1			=DIK_NUMPAD1, //「1」キー（Numpad）
	Key_NUMPAD2			=DIK_NUMPAD2, //「2」キー（Numpad）
	Key_NUMPAD3			=DIK_NUMPAD3, //「3」キー（Numpad）
	Key_NUMPAD0			=DIK_NUMPAD0, //「0」キー（Numpad）
	Key_DECIMAL			=DIK_DECIMAL, //「.」キー（Numpad）
	Key_F11				=DIK_F11, //「F11」キー
	Key_F12				=DIK_F12, //「F12」キー
	Key_F13				=DIK_F13, //「F13」キー
	Key_F14				=DIK_F14, //「F14」キー
	Key_F15				=DIK_F15, //「F15」キー
	Key_KANA			=DIK_KANA, //「カナ」キー（日本語キーボード）
	Key_CONVERT			=DIK_CONVERT, //「変換」キー（日本語キーボード）
	Key_NOCONVERT		=DIK_NOCONVERT, //「無変換」キー（日本語キーボード）
	Key_YEN				=DIK_YEN, //「\」キー（日本語キーボード）
	Key_NUMPADEQUALS	=DIK_NUMPADEQUALS, //「=」キー（Numpad）
	Key_CIRCUMFLEX		=DIK_CIRCUMFLEX, //「^」キー（日本語キーボード）
	Key_AT				=DIK_AT, //「@」キー
	Key_COLON			=DIK_COLON, //「:」キー
	Key_UNDERLINE		=DIK_UNDERLINE, //「 _ 」キー
	Key_KANJI			=DIK_KANJI, //「半角/全角」キー
	Key_STOP			=DIK_STOP, //「Stop」キー
	Key_NUMPADENTER		=DIK_NUMPADENTER, //「Enter」キー（Numpad）
	Key_RCONTROL		=DIK_RCONTROL, //「Ctrl」キー（右）
	Key_NUMPADCOMMA		=DIK_NUMPADCOMMA, //「 , 」キー（Numpad）
	Key_DIVIDE			=DIK_DIVIDE, //「/」キー（Numpad）
	Key_SYSRQ			=DIK_SYSRQ, //「Sys Rq」キー
	Key_RALT			=DIK_RMENU, //「Alt」キー（右）
	Key_PAUSE			=DIK_PAUSE, //「Pause」キー
	Key_HOME			=DIK_HOME, //「Home」キー
	Key_UP				=DIK_UP, //「↑」キー
	Key_PAGEUP			=DIK_PRIOR, //「Page Up」キー
	Key_LEFT			=DIK_LEFT, //「←」キー
	Key_RIGHT			=DIK_RIGHT, //「→」キー
	Key_END				=DIK_END, //「End」キー
	Key_DOWN			=DIK_DOWN, //「↓」キー
	Key_PAGEDOWN		=DIK_NEXT, //「Page Down」キー
	Key_INSERT			=DIK_INSERT, //「Insert」キー
	Key_DELETE			=DIK_DELETE, //「Delete」キー
	Key_LWIN			=DIK_LWIN, //「Windows」キー（左）
	Key_RWIN			=DIK_RWIN, //「Windows」キー（右）
	Key_APPS			=DIK_APPS, //「Menu」キー
	Key_POWER			=DIK_POWER //「Power」キー
};



//キーボード入力をサポートするクラス
class Keyboard
{
private:
	//フレンド
	friend class CWE::Input;

	
	//キーボードのデバイス
	LPDIRECTINPUTDEVICE8 mpDevice;

	//キーの状態
	char mKeyState[256];


	//シングルトンパターンによる実装
	Keyboard();
	~Keyboard();

	//初期化
	bool Init();
	//終了処理
	void End();
	//更新処理
	void Update();


public:
	//シングルトンパターンによる実装
	static Keyboard& GetInstance();
	Keyboard(const Keyboard&) =delete;
	Keyboard& operator=(const Keyboard&) =delete;


	//Windowキーをボタンを有効にするかをセットする
	//true=有効 false=無効
	bool ResetWindowsKey(bool windowsKey);


	//任意のキーの情報を取得
	//true=押されている false=押されていない
	inline bool Key(const KeyCode& keyCode) {return (mKeyState[keyCode] & 0x80) != 0;}
	//GetInputKeyCode関数で取得した値でキーの情報を取得
	inline bool Key(const InputCode& keyCode) {CWE_ASSERT(keyCode.mInputCode<256); return (mKeyState[keyCode.mInputCode] & 0x80) != 0;}

	//押されたキーのキーコードを取得
	//true=取得成功 false=取得失敗
	bool GetInputKeyCode(InputCode& keyCode);

};



}//CWE