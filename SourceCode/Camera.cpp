#include "Camera.h"
#include "Information.h"



// コンストラクタ
Camera::Camera()
{
	//ビューポートの作成
	CWE::Graphic& manager =CWE::Graphic::GetInstance();
	mViewport.Width =static_cast<float>(manager.mWindows.GetWindowSizeX());
	mViewport.Height =static_cast<float>(manager.mWindows.GetWindowSizeY());
	mViewport.TopLeftX =0.0f;
	mViewport.TopLeftY =0.0f;
	mViewport.MinDepth =0.0f;
	mViewport.MaxDepth =1.0f;

	//各種情報のセット
	CWE::math::PerspectiveFovLHTp(&mProjection, CWE::math::ConvertToRadians(50.0f), 16.0f/9.0f, 1.0f, 1000.0f);
	
	mEye.PosReset(0.0f, 50.0f, -160.0f);
	mEye.FocusReset(0.0f, 20.0f, 0.0f);
	mEye.UpDirReset(0.0f, 1.0f);
}


// デストラクタ
Camera::~Camera()
{

}


// 更新
void Camera::Update()
{

}


// 視点にセット
void Camera::Set()
{
	Information& info =Information::GetInfo();

	CWE_ASSERT(info.mModelShader.GetCBData());

	//ビュー変換
	CWE::math::LookAtLHTp(&info.mModelShader.GetCBData()->mView, mEye.GetPos(), mEye.GetFocusPos(), mEye.GetUpDir());
	info.mModelShader.GetCBData()->mEyePos.x() =mEye.GetPos().x();
	info.mModelShader.GetCBData()->mEyePos.y() =mEye.GetPos().y();
	info.mModelShader.GetCBData()->mEyePos.z() =mEye.GetPos().z();
	info.mModelShader.GetCBData()->mEyePos.w() =0.0f;

	//プロジェクション変換
	info.mModelShader.GetCBData()->mProjection =mProjection;

	Set2D();
}


// 2D視点にセット
void Camera::Set2D()
{
	//ビューポートのセット
	CWE::Graphic::GetInstance().DeviceContext().RSSetViewports(1, &mViewport);
}