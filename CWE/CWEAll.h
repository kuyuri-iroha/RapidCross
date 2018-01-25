#pragma once


namespace CWE
{


//CWEの初期化処理
bool Init(unsigned width, unsigned height, bool windowMode, const wchar_t windowTitle[128]);

//CWEの更新処理
bool Update();

//CWEの終了処理
void End();



}// CWE
