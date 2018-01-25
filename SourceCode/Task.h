#pragma once


#include <list>
#include <unordered_map>



class TaskBase;


// タスク名
enum class TaskName
{
	Battle,
	FPS,
	DebugString,
	Title,
	Loading,
	Result,
	Unnamed
};


// タスクの実行クラス
class TaskExec
{
public:
	//外部からタスクを制御する為のオブジェクト
	class List final
	{
		friend class TaskExec;
	private:
		std::list<TaskBase*> mAllTask;
		std::unordered_map<TaskName, TaskBase*> mTaskMap;

	public:
		List();
		~List();

		//タスクの追加
		//(true: タスクマップ追加成功,  false: タスクマップ追加失敗)
		bool Append(TaskBase* pTask, const TaskName& name);
		//タスクの情報を取得
		//(失敗したらnullptrを返す)
		const TaskBase* const GetTaskData(const TaskName& name) const;

		//タスクのクリア
		void Clear();
	};

private:

protected:
	using ListIterator =std::list<TaskBase*>::iterator;

public:
	List mTaskList;

	TaskExec();
	virtual ~TaskExec();

	void AllTaskUpdate();
	void AllTaskDraw();

	void AllTaskEnd() {mTaskList.Clear();}

};


// タスクシステムベース
class TaskBase
{
private:

protected:
	bool mUpdate; //更新を行うか
	bool mDraw; //描画を行うか
	bool mEnable; //タスクが有効かどうか
	const int mPriority; //優先度
	const TaskName mName;

public:
	TaskBase(): mUpdate(true), mDraw(true), mEnable(true), mPriority(0), mName(TaskName::Unnamed) {}
	TaskBase(int priority, TaskName name): mUpdate(true), mDraw(true), mEnable(true), mPriority(priority), mName(name) {}
	virtual ~TaskBase(){}

	bool IsUpdate() const {return mUpdate;}
	bool IsDraw() const {return mDraw;}
	bool IsEnable() const {return mEnable;}
	int GetPriority() const {return mPriority;}
	TaskName GetName() const {return mName;}

	virtual void Update(TaskExec::List& taskList) =0;
	virtual void Draw() =0;
	virtual void End() =0;

};