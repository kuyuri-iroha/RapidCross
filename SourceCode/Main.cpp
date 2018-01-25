#include "../CWE/CWELib.h"
#include "Game.h"
#include "Information.h"



int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	// 基礎処理機構の初期化
#if defined(_DEBUG) || defined(DEBUG)
	const bool initalized =CWE::Init(1920, 1080, true, L"Rapid Cross");
#else
	const bool initalized =CWE::Init(1920, 1080, false, L"Rapid Cross");
#endif

	// 初期化に失敗したら終了
	if(!initalized)
	{
		CWE::End();
		CoUninitialize();
		return 0;
	}

	CWE::Mouse::GetInstance().ResetPointerMode(false);

	// ゲームプログラム&ゲームデータの初期化
	Game game;
	Information& info =Information::GetInfo();

	// メインループ
    while(CWE::Update() && !(info.mEnded))
    {
		// ゲーム側の終了リクエストがあったら終了
		if(game.Run())
		{
			break;
		}
    }

	game.End();

	CWE::End();

	CoUninitialize();

	return 0;
}