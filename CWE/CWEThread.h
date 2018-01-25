#pragma once

#include <vector>
#include <deque>
#include <list>
#include <thread>
#include <functional>
#include <mutex>



namespace CWE
{


// ワーカースレッド
class WorkerThread
{
public:
	struct TaskID
	{
		int mNumber;
	};

private:
	struct Task
	{
		std::function<void()> mTask;
		TaskID mID;

		Task(const std::function<void()>& task, const TaskID& id): mTask(task), mID(id) {}
		~Task() {}
	};

	struct Thread
	{
		std::thread mThread;
		std::deque<Task> mTaskBuffer;
		std::mutex mMutex;
		static void Activity(Thread& thisData, const volatile bool& able);

		Thread(const bool& threadAble);
		Thread(Thread& thread);
		~Thread();
	};

private:
	std::vector<Thread> mThreadGroup;
	bool m_Able;
	std::vector<Task> mTaskContainer;
	TaskID m_NextID;

public:
	WorkerThread();
	~WorkerThread();

	// スレッドの起動
	void Activation();
	// スレッドの終了
	void Deactivation();
	// スレッドが起動しているか
	bool Activated() const;
	// タスクが全て終了しているか
	bool AllTaskEnded() const;
	// タスクコンテナの内容を全て消去
	void TaskClear();
	// 各スレッドにタスクの振り分け
	void Update();
	// タスクのリクエスト
	const TaskID TaskRequest(const std::function<void()>& task);
	// タスクが終了しているか
	bool TaskEnded(const TaskID& id) const;

};



}