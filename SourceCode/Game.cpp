#include "Game.h"
#include "Information.h"
#include "Battle.h"
#include "FPS.h"
#include "DebugString.h"
#include "Loading.h"
#include "Result.h"
#include "Controller.h"
#include "Title.h"




// コンストラクタ
Game::Game()
{
//	mTask.mTaskList.Append(new FPS, TaskName::FPS); //要らなければコメントアウト
	mTask.mTaskList.Append(new LoadingExec(-1000), TaskName::Loading);
	mTask.mTaskList.Append(new DebugString, TaskName::DebugString);
	gController1.SetGamepad();
	gController2.SetGamepad();
}


// デストラクタ
Game::~Game()
{
	gController1.RemoveGamepad();
	gController2.RemoveGamepad();
}


// 更新
bool Game::Run()
{
	Information& info =Information::GetInfo();

	switch(info.mRuningScene)
	{
	case TaskName::Title:
		if(mTask.mTaskList.GetTaskData(TaskName::Title) == nullptr)
		{
			mTask.mTaskList.Append(new TitleExec, TaskName::Title);
		}
		break;

	case TaskName::Battle:
		if(mTask.mTaskList.GetTaskData(TaskName::Battle) == nullptr)
		{
			mTask.mTaskList.Append(new BattleExec(PlayerCharacter::Gumi, PlayerCharacter::Gumi2, BattleStage::StarryRuins), TaskName::Battle);
		}
		break;

	case TaskName::Result:
		if(mTask.mTaskList.GetTaskData(TaskName::Result) == nullptr)
		{
			mTask.mTaskList.Append(new ResultExec, TaskName::Result);
		}
		break;
	}

	auto loading =[&](TaskName name) {
		if(mTask.mTaskList.GetTaskData(name) == nullptr)
		{
			return false;
		}
		return mTask.mTaskList.GetTaskData(name)->IsUpdate() && !mTask.mTaskList.GetTaskData(name)->IsDraw();
	};
	info.mLoading =loading(TaskName::Title) || loading(TaskName::Battle);

	mTask.AllTaskUpdate();
	mTask.AllTaskDraw();

	return info.mEndRequest;
}


void Game::End()
{
	gController1.RemoveGamepad();
	gController2.RemoveGamepad();
	mTask.AllTaskEnd();
}