#include "CWEIncludedAPI.h"
#include "CWEThread.h"
#include <algorithm>



namespace CWE
{



// ========================= WorkerThread::Thread =============================================

// コンストラクタ
WorkerThread::Thread::Thread(const bool& threadAble):
	mTaskBuffer(), mThread(Activity, std::ref(*this), std::ref(threadAble))
{

}

// コピーコンストラクタ
WorkerThread::Thread::Thread(Thread& thread):
	mTaskBuffer(thread.mTaskBuffer)
{
	mThread.swap(thread.mThread);
}

// デストラクタ
WorkerThread::Thread::~Thread()
{
	mThread.join();
}


// スレッド処理
void WorkerThread::Thread::Activity(Thread& thisData, const volatile bool& threadAble)
{
	std::function<void()> task;

	//タスク待ち用ループ
	while(threadAble)
	{
		Sleep(1); //TODO: ループが早すぎると起こるハングアップ(「mTaskBuffer」のメモリ破壊)の原因究明と解決

		//タスクがあれば
		if(!thisData.mTaskBuffer.empty())
		{
			task =thisData.mTaskBuffer.front().mTask;

			if(!threadAble) {return;}
			task();

			thisData.mMutex.lock();

			thisData.mTaskBuffer.pop_front();

			thisData.mMutex.unlock();
		}
	}
}



// =============================== WorkerThread ===============================================

#define THREAD_NUM 4

// コンストラクタ
WorkerThread::WorkerThread():
	m_Able(true)
{
	mThreadGroup.reserve(THREAD_NUM); //TODO: 最適化
	m_NextID.mNumber =INT_MIN;
}

// デストラクタ
WorkerThread::~WorkerThread()
{
	Deactivation();
}


// スレッドの起動
void WorkerThread::Activation()
{
	for(unsigned i=0; i<THREAD_NUM; ++i)
	{
		mThreadGroup.emplace_back(m_Able);
	}
}


// スレッドの終了
void WorkerThread::Deactivation()
{
	m_Able =false;
	mThreadGroup.clear();
}


// スレッドが起動しているか
bool WorkerThread::Activated() const
{
	return !mThreadGroup.empty();
}


// タスクが全て終了しているか
bool WorkerThread::AllTaskEnded() const
{
	for(unsigned i=0; i<mThreadGroup.size(); ++i)
	{
		if(!mThreadGroup[i].mTaskBuffer.empty()) {return false;}
	}

	if(!mTaskContainer.empty()) {return false;}

	return true;
}


// タスクコンテナの内容を全て消去
void WorkerThread::TaskClear()
{
	mTaskContainer.clear();
}


// 各スレッドにタスクの振り分け
void WorkerThread::Update()
{
	if(mTaskContainer.empty() || mThreadGroup.empty()) {return;}
	const int containerSize =mTaskContainer.size();
	int divideTaskNum =containerSize /mThreadGroup.size();
	if(int(divideTaskNum*mThreadGroup.size()) < containerSize) {++divideTaskNum;} //切捨て時に起こる誤差の修正

	int cnt=0, actuallyDivNum=0;
	for(unsigned i=0; i<mThreadGroup.size(); ++i)
	{
		if(containerSize <= cnt) {return;} //タスクの振り分けが終わっている

		if(containerSize <= cnt+divideTaskNum) //このループで振り分けが終わるのなら
		{
			actuallyDivNum =(cnt+divideTaskNum) -containerSize;
			actuallyDivNum =divideTaskNum -actuallyDivNum +cnt;
		}
		else
		{
			actuallyDivNum =divideTaskNum +cnt;
		}

		mThreadGroup[i].mMutex.lock();

		//タスクの分配
		for(; cnt<actuallyDivNum; ++cnt)
		{
			mThreadGroup[i].mTaskBuffer.emplace_back(mTaskContainer[cnt]);
		}

		mThreadGroup[i].mMutex.unlock();
	}

	mTaskContainer.clear();
}


// タスクのリクエスト
const WorkerThread::TaskID WorkerThread::TaskRequest(const std::function<void()>& task)
{
	mTaskContainer.emplace_back(task, m_NextID);

	TaskID result(m_NextID);
	++m_NextID.mNumber;

	return result;
}


// タスクが終了しているか
bool WorkerThread::TaskEnded(const TaskID& id) const
{
	for(unsigned i=0; i<mThreadGroup.size(); ++i)
	{
		for(const Task& task : mThreadGroup[i].mTaskBuffer)
		{
			if(id.mNumber == task.mID.mNumber)
			{
				return false;
			}
		}
	}

	for(const Task& task : mTaskContainer)
	{
		if(id.mNumber == task.mID.mNumber)
		{
			return false;
		}
	}

	return true;
}


}