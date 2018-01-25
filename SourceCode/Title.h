#pragma once


#include "Parameter.h"
#include "Task.h"
#include "Camera.h"
#include "../CWE/CWELib.h"



// Task of the title scene.
class TitleExec : public TaskBase
{
private:
	CWE::Polygon mTitleImage;
	CWE::Polygon mTitle;
	CWE::Polygon mPressA;
	CWE::Polygon mMenuBar;
	CWE::Polygon mMenuBattle;
	CWE::Polygon mMenuCredit;
	CWE::Polygon mCredit;
	Parameter mEndParam;
	CWE::Sound mBGM;
	CWE::Sound mDeterm;
	CWE::Sound mSelect;

	Camera mCamera;
	CWE::Timer mTime;

	bool mPlayedBGM;
	float mScaleOfMenuBar;
	float mScaleOfPressA;
	bool mPreparingMenuBar;
	float mPressAAnimTime;
	float mMenuBarAnimTime;
	int mSelector;
	bool mDisplayingCredit;
	bool mEnding;
	enum SelectMask
	{
		Selec_Battle,
		Selec_Credit,
		NumberOfSelec,
		SelecUnknown,
	};

	static const unsigned smEndingPressFrame =120;
	unsigned mPressingFrame;

	bool MembersLoaded() const;
	bool LoadSuccess() const;
	bool MenuBarDisplayAnim();

public:
	TitleExec();
	~TitleExec();

	void Update(TaskExec::List& taskList) override;
	void Draw() override;
	void End() override;

};