#include "Task.h"
#include "../CWE/CWEError.h"
#include "../CWE/CWEUsefullyMacro.h"




// =========== TaskExec::List ====================

// コンストラクタ
TaskExec::List::List()
{

}


// デストラクタ
TaskExec::List::~List()
{
	for(auto& task : mAllTask)
	{
		SAFE_DELETE(task);
	}
	mAllTask.clear();
}


// タスクの追加
bool TaskExec::List::Append(TaskBase* pTask, const TaskName& name)
{
	CWE_ASSERT(pTask);

	if(mTaskMap.find(name) != mTaskMap.end()) //すでに同じものがあったら
	{
		return false;
	}

	//優先度順に追加
	if(mAllTask.empty()) //タスクリストが空だったら
	{
		mAllTask.push_front(pTask);
	}
	else
	{
		TaskExec::ListIterator itr =mAllTask.begin();

		for(auto& task : mAllTask)
		{
			CWE_ASSERT(task);

			if(task->GetPriority() < pTask->GetPriority())
			{
				itr =mAllTask.insert(itr, pTask);
				break;
			}
			++itr;
		}

		if(itr == mAllTask.end()) //優先度が1番小さければ
		{
			mAllTask.push_back(pTask);
		}
	}

	//タスクマップに登録
	mTaskMap[name] =pTask;
	return true;
}


//タスクの情報を取得
const TaskBase* const TaskExec::List::GetTaskData(const TaskName& name) const
{
	if(mTaskMap.find(name) == mTaskMap.end())
	{
		return nullptr;
	}

	return mTaskMap.at(name);
}


// タスクのクリア
void TaskExec::List::Clear()
{
	for(const auto& task : mAllTask)
	{
		CWE_ASSERT(task);
		task->End();
	}
	mAllTask.clear();
	mTaskMap.clear();
}




// =========== TaskExec =======================


// コンストラクタ
TaskExec::TaskExec()
{

}


// デストラクタ
TaskExec::~TaskExec()
{
	AllTaskEnd();
}


// 更新
void TaskExec::AllTaskUpdate()
{
	for(ListIterator itr=mTaskList.mAllTask.begin(); itr!=mTaskList.mAllTask.end();)
	{
		CWE_ASSERT(*itr);

		//個々のタスクの更新
		if((*itr)->IsUpdate())
		{
			(*itr)->Update(mTaskList);
		}

		//タスクが終了しているか
		if(!(*itr)->IsEnable())
		{
			mTaskList.mTaskMap.erase((*itr)->GetName());
			SAFE_DELETE(*itr);
			itr =mTaskList.mAllTask.erase(itr);
		}
		else
		{
			++itr;
		}
	}
}


// 描画
void TaskExec::AllTaskDraw()
{
	for(const auto& task : mTaskList.mAllTask)
	{
		CWE_ASSERT(task);

		if(task->IsDraw())
		{
			task->Draw();
		}
	}
}