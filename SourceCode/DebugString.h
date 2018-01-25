#pragma once


#include "Task.h"
#include "../CWE/CWEMath.h"


// デバッグ文字列出力
class DebugString : public TaskBase
{
private:
	static unsigned smCacheCount;

	const float mMagnifX,
				mMagnifY;

	CWE::math::Float3 mPos;

	int GetIndex(char aChar);
	
public:
	static void Output(float x, float y, const char* string, ...);

	DebugString();
	~DebugString();

	void Update(TaskExec::List& taskList) override;
	void Draw() override;
	void End() override;
};