#include "Parameter.h"
#include "Information.h"

using namespace CWE;

Parameter::~Parameter()
{
	End();
}


bool Parameter::LoadSucceeded() const
{
	if(mMask.LoadResult() == Task::Failed ||
		mShape.LoadResult() == Task::Failed)
	{
		FatalError::GetInstance().Outbreak(L"Failed to load data.\nMay file is corrupted.");
		return false;
	}

	return (mMask.LoadResult() == Task::Success &&
		mShape.LoadResult() == Task::Success);
}


// 作成
void Parameter::Create(const wchar_t* maskPath, const wchar_t* shapePath)
{
	//マスク
	mMask.Load(maskPath, FileFormat::Picture);

	//形状
	mShape.Load(shapePath, FileFormat::PMX);
}


// 描画
void Parameter::Draw()
{
	Graphic& gm =Graphic::GetInstance();
	bool suc =false;
	auto mesh =mMask.GetMeshData(&suc);

	gm.PSSetShaderResources(1, 1, &mesh[0].mpTexture);
	mShape.Draw(true);
}


// 終了
void Parameter::End()
{
	mShape.Release();
	mMask.Release();
}