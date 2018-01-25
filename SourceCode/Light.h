#pragma once

#include "../CWE/CWELib.h"


// ƒ‰ƒCƒg
class Light
{
private:
	CWE::Eye mEye;

public:
	Light();
	~Light();

	void Update();

	inline const CWE::Eye& GetEye() const {return mEye;}

};