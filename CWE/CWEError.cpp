#include "CWEError.h"
#include "CWEGraphicManager.h"
#include "CWEIncludedAPI.h"
#include <tchar.h>



namespace CWE
{


bool OutputDebugStringF(const wchar_t* formatString, ...)
{
	va_list argl;
	wchar_t buf[512];
	va_start(argl, formatString);
	_vstprintf_s(buf, 512, formatString, argl);
	va_end(argl);
	OutputDebugString(buf);
	return true;
}

#if defined(_DEBUG) || defined(DEBUG)
bool Exit()
{
	OutputDebugStringF(L"***************************************************************************\n");
	OutputDebugStringF(L"=================================== End ===================================\n");
	OutputDebugStringF(L"***************************************************************************\n\n");

	::exit(1);

	return true;
}
#endif



// ===================== FatalError ========================

const wchar_t FatalError::smFileLoadErrorMS[69] =L"Failed to load data.\nThere is a possibility that the file is broken.";
const wchar_t FatalError::smDeviceCreationErrorMS[57] =L"Device creation failed.\nThe device may not be supported.";

FatalError::FatalError():
	mIsFatalError(false), mErrorMessage(L"Fatal error is happened.\nCan not continue processing.")
{
}

FatalError::~FatalError()
{
	if(mIsFatalError)
	{
		MessageBox(NULL, mErrorMessage.c_str(), L"Fatal error", (MB_OK|MB_ICONERROR));
	}
}


FatalError& FatalError::GetInstance()
{
	static FatalError instance;
	return instance;
}


void FatalError::Outbreak(const wchar_t errorMessage[])
{
	CWE::Graphic& graphic =CWE::Graphic::GetInstance();
	if(errorMessage != nullptr)
	{
		mErrorMessage =errorMessage;
	}
	mIsFatalError =true;

	graphic.ResetWindowMode(true);
	graphic.mWindows.EndRequest();
}

bool FatalError::NoInfoOutbreak() const
{
	CWE::Graphic& graphic =CWE::Graphic::GetInstance();
	graphic.ResetWindowMode(true);
	graphic.mWindows.EndRequest();
	return true;
}




}