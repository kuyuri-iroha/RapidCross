#pragma once

#include "CWEIncludedAPI.h"

namespace CWE
{

// タスクの状態
enum class Task
{
	Success,
	Failed,
	Working,
};


// 対応しているファイルフォーマット
enum class FileFormat
{
	PMX,
	Picture,
	VMD,
	WAVE,
	Unknown,
};


// ポリゴンのハンドルデータを格納
struct ResourceHandle
{
private:
	friend class Resource;
	int mHandle;

public:
	ResourceHandle():mHandle(-1){}
	ResourceHandle(const ResourceHandle &original): mHandle(original.mHandle){}
	~ResourceHandle() {}
};


// 入力コード格納用構造体
struct InputCode
{
private:
	friend class Keyboard;
	friend class Mouse;
	friend class Gamepad;
	unsigned int mInputCode;

public:
	InputCode():mInputCode(0){}
	~InputCode(){}
};

//未加工のXInputのデータをあつかうための構造体
typedef XINPUT_STATE XIState;

//未加工のDirectInputのデータをあつかうための構造体
typedef DIJOYSTATE DIState;


}//CWE
