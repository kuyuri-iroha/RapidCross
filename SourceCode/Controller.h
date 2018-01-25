#pragma once


#include <vector>
#include <array>
#include <type_traits>
#include "../CWE/CWELib.h"



// enum型の値を根底型にキャスト
template<class tEnum>
constexpr auto ToUType(tEnum enumerator) noexcept
{
	return static_cast<std::underlying_type_t<tEnum>>(enumerator);
}


class Controller
{
protected:
	enum FlagIndex : unsigned
	{
		Index_Left =0,
		Index_Right,
		Index_Up,
		Index_Down,
		Index_SlightLeft,
		Index_SlightRight,
		Index_WeakAttack,
		Index_SrgAttack,
		Index_Guard,
		Index_Menu,
		Index_Back,
		NumberOfIndex
	};

	template<class tGamepadFunc>
	bool InputJudge(CWE::Gamepad* gamepad, tGamepadFunc func, CWE::KeyCode keyCode, FlagIndex index, bool useGamepad, bool useKeyboard, bool longPress)
	{
		using namespace CWE;
		CWE_ASSERT(useGamepad ? gamepad!=nullptr : true);

		if((useKeyboard ? Keyboard::GetInstance().Key(keyCode) : false) ||
			(useGamepad ? func(gamepad, mGPState) : false))
		{
			//長押し判定
			if(longPress) {return true;}

			//単発押し判定
			if(!mFlags[index])
			{
				mFlags[index] =true;
				return true;
			}
		}
		else
		{
			mFlags[index] =false;
		}
		return false;
	}

	virtual void UpdateGPState() =0;

	virtual bool Left(bool useGamepad, bool useKeyboard, bool longPress) =0;
	virtual bool Right(bool useGamepad, bool useKeyboard, bool longPress) =0;
	virtual bool Up(bool useGamepad, bool useKeyboard, bool longPress) =0;
	virtual bool Down(bool useGamepad, bool useKeyboard, bool longPress) =0;
	virtual bool SlightlyLeft(bool useGamepad, bool useKeyboard, bool longPress) =0;
	virtual bool SlightlyRight(bool useGamepad, bool useKeyboard, bool longPress) =0;
	virtual bool WeakAttack(bool useGamepad, bool useKeyboard, bool longPress) =0;
	virtual bool SrgAttack(bool useGamepad, bool useKeyboard, bool longPress) =0;
	virtual bool Guard(bool useGamepad, bool useKeyboard, bool longPress) =0;
	virtual bool Menu(bool useGamepad, bool useKeyboard, bool longPress) =0;
	virtual bool Back(bool useGamepad, bool useKeyboard, bool longPress) =0;

	CWE::GamepadState mGPState;
	std::vector<bool> mFlags;
	//同時押しコマンド用
	unsigned mPushFlag,
			 mLongFlag;
	//即時押しコマンド用
	unsigned mInstPushFlag,
			 mInstLongFlag;
	bool mPushed,
		 mIsLong;
	unsigned mPushPassed,
			 mLongPassed;
	unsigned mPressing;

public:
	enum class InputFlag : unsigned
	{
		None		=0,
		Left		=1 <<0,
		Right		=1 <<1,
		Up			=1 <<2,
		Down		=1 <<3,
		SlightLeft	=1 <<4,
		SlightRight	=1 <<5,
		WeakAttack	=1 <<6,
		SrgAttack	=1 <<7,
		Guard		=1 <<8,
		Menu		=1 <<9,
		Back		=1 <<10
	};

	static const int smAxisLowest =1000;
	static const int smAxisRefVal =25000;
	static const unsigned smAllowanceFrame =10;

	Controller(): mPushFlag(ToUType(InputFlag::None)), mLongFlag(ToUType(InputFlag::None)), mPressing(ToUType(InputFlag::None)) {}
	virtual ~Controller() {}
	Controller(const Controller&) =delete;
	Controller& operator=(const Controller&) =delete;

	void InputUpdate();
	void ClearFlags();

	const unsigned PushFlag() const {return mPushFlag;}
	const unsigned LongFlag() const {return mLongFlag;}

	// ゲームに使う入力判定
	bool Dash(bool isRight) const;
	bool Walk(bool isRight) const;
	bool Reverse(bool isRight) const;
	bool Jump() const;
	bool Back(bool isRight) const;
	bool SlightlyBack(bool isRight) const;
	bool Down() const;
	bool InstDown() const;
	bool SideWeakAttack() const;
	bool ALong() const;
	bool SideAttack(bool isRight) const;
	bool UpperAttack() const;
	bool LowerAttack() const;
	bool Guard() const;
	bool Menu() const;
	bool Back() const;
	bool BackLong() const;

};


// 1Pのコントローラー
class Controller1 final : public Controller
{
private:
	static CWE::Gamepad* smpGamepad1;

	void UpdateGPState() override {CWE_ASSERT(smpGamepad1); smpGamepad1->GetState(mGPState);}

	bool Left(bool useGamepad, bool useKeyboard, bool longPress) override;
	bool Right(bool useGamepad, bool useKeyboard, bool longPress) override;
	bool Up(bool useGamepad, bool useKeyboard, bool longPress) override;
	bool Down(bool useGamepad, bool useKeyboard, bool longPress) override;
	bool SlightlyLeft(bool useGamepad, bool useKeyboard, bool longPress) override;
	bool SlightlyRight(bool useGamepad, bool useKeyboard, bool longPress) override;
	bool WeakAttack(bool useGamepad, bool useKeyboard, bool longPress) override;
	bool SrgAttack(bool useGamepad, bool useKeyboard, bool longPress) override;
	bool Guard(bool useGamepad, bool useKeyboard, bool longPress) override;
	bool Menu(bool useGamepad, bool useKeyboard, bool longPress) override;
	bool Back(bool useGamepad, bool useKeyboard, bool longPress) override;

public:
	using Controller::Menu;
	using Controller::Back;
	using Controller::Jump;
	using Controller::InstDown;

	Controller1();
	~Controller1();
	Controller1(const Controller1&) =delete;
	Controller1& operator=(const Controller1&) =delete;

	static void SetGamepad();
	static void RemoveGamepad();
	inline static bool IsGamepad() {CWE_ASSERT(smpGamepad1); return smpGamepad1->GetCanUse();}

};

// グローバル変数として宣言
static Controller1 gController1;


// 2Pのコントローラー
class Controller2 final : public Controller
{
private:
	static CWE::Gamepad* smpGamepad2;

	void UpdateGPState() override {CWE_ASSERT(smpGamepad2); smpGamepad2->GetState(mGPState);}

	bool Left(bool useGamepad, bool useKeyboard, bool longPress) override;
	bool Right(bool useGamepad, bool useKeyboard, bool longPress) override;
	bool Up(bool useGamepad, bool useKeyboard, bool longPress) override;
	bool Down(bool useGamepad, bool useKeyboard, bool longPress) override;
	bool SlightlyLeft(bool useGamepad, bool useKeyboard, bool longPress) override;
	bool SlightlyRight(bool useGamepad, bool useKeyboard, bool longPress) override;
	bool WeakAttack(bool useGamepad, bool useKeyboard, bool longPress) override;
	bool SrgAttack(bool useGamepad, bool useKeyboard, bool longPress) override;
	bool Guard(bool useGamepad, bool useKeyboard, bool longPress) override;
	bool Menu(bool useGamepad, bool useKeyboard, bool longPress) override;
	bool Back(bool useGamepad, bool useKeyboard, bool longPress) override;

public:
	using Controller::Menu;
	using Controller::Back;
	using Controller::Jump;
	using Controller::InstDown;

	Controller2();
	~Controller2();
	Controller2(const Controller2&) =delete;
	Controller2& operator=(const Controller2&) =delete;

	static void SetGamepad();
	static void RemoveGamepad();
	inline static bool IsGamepad() {CWE_ASSERT(smpGamepad2); return smpGamepad2->GetCanUse();}

};

// グローバル変数として宣言
static Controller2 gController2;