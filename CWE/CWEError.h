#pragma once

#include <string>


namespace CWE
{

bool OutputDebugStringF(const wchar_t* formatString, ...);


class FatalError
{
private:
	friend void End();

	bool mIsFatalError;
	std::wstring mErrorMessage;


	FatalError();
	~FatalError();
	FatalError(const FatalError&) =delete;
	FatalError& operator=(const FatalError&) =delete;

	inline bool GetIs() {return mIsFatalError;}

public:
	static const wchar_t smFileLoadErrorMS[69];
	static const wchar_t smDeviceCreationErrorMS[57];
	static FatalError& GetInstance();

	//Fatal errorÇÃê∂ê¨
	void Outbreak(const wchar_t errorMessage[] =nullptr);
	bool NoInfoOutbreak() const;

};


// Assertion
#if defined(_DEBUG) || defined(DEBUG)

bool Exit();

#define CWE_ASSERT(expression, ...)\
			(!(expression) && CWE::OutputDebugStringF(L"%s(%d): Assert has failed. : ("#expression")\n", __FILEW__, __LINE__, __VA_ARGS__) && CWE::Exit()) 
#else
#define CWE_ASSERT(...)
#endif


}// CWE
