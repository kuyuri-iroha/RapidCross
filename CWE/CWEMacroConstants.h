#pragma once

#define CWE_DEFAULT_WINDOW_SIZE_X (1920) //デフォルトの画面のX座標の解像度
#define CWE_DEFAULT_WINDOW_SIZE_Y (1080) //デフォルトの画面のY座標の解像度

#define CWE_WINDOW_MODE_STYLE (WS_MINIMIZEBOX | WS_POPUP | WS_SYSMENU | WS_CAPTION) //ウインドウモード時のウインドウスタイル
#define CWE_FULLSCREEN_MODE_STYLE (WS_POPUP) //フルスクリーンモード時のウインドウスタイル

#define CWE_GAMEPAD_AXIS_MIN -32768
#define CWE_GAMEPAD_AXIS_MAX 32767




// =================コンパイル時に値を変更して動作を変える ================

//ゲームパッドの最大数
#define CWE_GAMEPAD_MAX 2


//ビューポートの最大数
#define CWE_VIEWPORT_MAX 1

