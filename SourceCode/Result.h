#pragma once

#include "Task.h"
#include "Camera.h"
#include "Information.h"
#include "../CWE/CWELib.h"



// Task of the result scene.
class ResultExec : public TaskBase
{
private:
	CWE::Polygon mBackGround;
	std::array<CWE::Polygon, Information::NumberOfPlayer> mWinnerImages;
	CWE::Polygon mPressA;
	CWE::Sound mDeterm;
	enum BGM
	{
		BGM_Winner,
		BGM_Draw,
		NumberOfBGM,
		BGMUnknown,
	};
	std::array<CWE::Sound, NumberOfBGM> mBGM;
	BGM mToPlay;

	Camera mCamera;
	CWE::Timer mTime;

	bool mTitleEnded;
	bool mPlayedBGM;
	bool mIdling;
	bool mEnded;

	bool Loaded() const;

public:
	ResultExec();
	~ResultExec();

	void Update(TaskExec::List& taskList) override;
	void Draw() override;
	void End() override;

};