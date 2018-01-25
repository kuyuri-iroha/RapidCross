#pragma once


#include "Task.h"



// ゲーム実行クラス
class Game
{
private:
	TaskExec mTask;

public:
	Game();
	~Game();

	bool Run();
	void End();

};