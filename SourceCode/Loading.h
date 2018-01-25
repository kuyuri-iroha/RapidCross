#pragma once


#include "Task.h"
#include "Camera.h"
#include "../CWE/CWELib.h"



// Task of the loading scene.
class LoadingExec : public TaskBase
{
private:
	CWE::Polygon mLoadingImage;

	Camera mCamera;
	CWE::Timer mTime;

public:
	LoadingExec(int priority);
	~LoadingExec();

	void Update(TaskExec::List& taskList) override;
	void Draw() override;
	void End() override;

};