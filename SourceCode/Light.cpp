#include "Light.h"
#include "Information.h"



// コンストラクタ
Light::Light():
	mEye({100.0f, 300.0f, -200.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f})
{

}


// デストラクタ
Light::~Light()
{

}


// 更新
void Light::Update()
{
	Information& info =Information::GetInfo();

	CWE_ASSERT(info.mModelShader.GetCBData());

	info.mModelShader.GetCBData()->m_Ambient =0.03f;
	CWE::math::VectorNormalize(&info.mModelShader.GetCBData()->mDiffuse, mEye.GetPos());
	info.mModelShader.GetCBData()->mDiffuseColor.x() =1.0f;
	info.mModelShader.GetCBData()->mDiffuseColor.y() =1.0f;
	info.mModelShader.GetCBData()->mDiffuseColor.z() =1.0f;
	info.mModelShader.GetCBData()->mStrength =3.0f;
}