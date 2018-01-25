#pragma once

#define WIN32_LEAN_AND_MEAN
#pragma warning(disable:4005) //DirectX系マクロの再定義警告を非表示
#pragma warning(disable:4838) //XNAMathの縮小変換警告を非表示

#include <Windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <xnamath.h>
#include <dxerr.h>
#include <dinput.h>
#include <Xinput.h>
#include <XAudio2.h>


//ライブラリのリンク
//#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment( lib, "dxerr.lib" )
#pragma comment( lib, "dxgi.lib" )
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "xinput.lib")
#pragma comment(lib, "dxguid.lib")


#ifdef _DEBUG
#pragma comment(lib, "d3dx11d.lib")
#else
#pragma comment(lib, "d3dx11.lib")
#endif


