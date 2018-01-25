#pragma once

#include "../CWE/CWELib.h"



// モーション
class Motion final
{
private:
	CWE::Motion mMotion;
	CWE::math::Float4x4 mOldPos;
	float mNowFrame;

public:
	Motion(): mMotion(), mNowFrame(0.0f) {}
	~Motion() {}

	inline const CWE::Motion& GetMotion() const {return mMotion;}
	inline CWE::Motion& GetMotion() {return mMotion;}
	inline float GetNowFrame() const {return mNowFrame;}

	bool Play(CWE::Polygon& applied, bool order, float speed, CWE::math::Matrix& movement);
	bool Apply(CWE::Polygon& applied, float frame, CWE::math::Matrix& movement);
	void Restart();

};


// モーション補間
class MotionInter final
{
private:
	unsigned mNowCount; //現在のカウント
	unsigned mInterCount; //補間カウント

public:
	MotionInter();
	~MotionInter();

	void Reset(unsigned interCount);
	//result: true=補間中, false=補間完了
	bool Exec(CWE::Polygon& model, CWE::Motion& from, CWE::Motion& to);

};