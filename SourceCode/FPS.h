#pragma once


#include "Task.h"
#include "../CWE/CWEPolygon.h"
#include "../CWE/CWETimer.h"
#include <array>
#include <string>



// FPS•\Ž¦
class FPS : public TaskBase
{
private:
	CWE::math::Float3 mPos;
	float m2DMagnifX,
		  m2DMagnifY;
	CWE::Timer mTimer;
	double mTime;
	int mPassingFrameCnt;
	int mFPS;

	bool LoadResult() const;
	void Calculation();

public:
	FPS();
	~FPS();

	void Update(TaskExec::List& taskList) override;
	void Draw() override;
	void End() override;

};