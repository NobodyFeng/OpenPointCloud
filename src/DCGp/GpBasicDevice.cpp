#include "DCGp/GpBasicDevice.h"

//Qt
#include "QString"
#include "QDataStream"

//Common
#include "DCCore/Const.h"

namespace DcGp
{
	DECLARE_SMARTPTR(DcGpViewportParametersImpl)
	
	class DcGpViewportParametersImpl
	{
		DECLARE_PHONY_CONSTRUCTOR(DcGpViewportParametersImpl)

	protected:
		DcGpViewportParametersImpl()
			: m_bPerspective(true)
		{}

	private:
		friend class DcGpViewportParameters;

		float m_pixeSize;

		//��ǰ�����ű���
		float m_zoom;

		//��ͼ���󣬽���ת����R
		DcGpMatrix m_viewRotateMat;

		//���С
		float m_pointSize;

		//ֱ�ߵĿ��
		float m_lineWidth;

		//͸��ͶӰ״̬
		bool m_bPerspective;
		bool m_bObjectCentered;

		//��ת���ĵ� (for object-centered view modes)
		DCVector3D m_pivotPos;

		//������ģ�����͸��ģʽ��
		DCVector3D m_cameraPos;

		//! Camera F.O.V. (field of view - for perspective mode only)
		float m_fov;
		//��������ű���(��͸��ģʽ)
		float m_aspect;

		//! Theoretical perspective 'zNear' relative position
		double m_zNearCoef;
		//! Actual perspective 'zNear' value
		double m_zNear;
		//! Actual perspective 'zFar' value
		double m_zFar;
	};
}

DcGp::DcGpViewportParameters::DcGpViewportParameters()
{
	m_pDcGpViewportParametersImpl = DcGpViewportParametersImpl::CreateObject();

	m_pDcGpViewportParametersImpl->m_pixeSize = 1.0f;
	m_pDcGpViewportParametersImpl->m_zoom = 1.0f;
	m_pDcGpViewportParametersImpl->m_pointSize = 1.0f;
	m_pDcGpViewportParametersImpl->m_lineWidth = 1.0f;
	m_pDcGpViewportParametersImpl->m_bPerspective = true;
	m_pDcGpViewportParametersImpl->m_bObjectCentered = true;
	m_pDcGpViewportParametersImpl->m_pivotPos = 0.0f;
	m_pDcGpViewportParametersImpl->m_cameraPos = 0.0f;
	m_pDcGpViewportParametersImpl->m_fov = 30.0f;
	m_pDcGpViewportParametersImpl->m_aspect = 1.0f;
	m_pDcGpViewportParametersImpl->m_viewRotateMat.ToIdentity();
	m_pDcGpViewportParametersImpl->m_zNearCoef = 0.001;
	m_pDcGpViewportParametersImpl->m_zNear = 0;
	m_pDcGpViewportParametersImpl->m_zFar = 0;
}

DcGp::DcGpViewportParameters::DcGpViewportParameters(const DcGpViewportParameters& params)
{
	m_pDcGpViewportParametersImpl = DcGpViewportParametersImpl::CreateObject();

	m_pDcGpViewportParametersImpl->m_pixeSize = params.m_pDcGpViewportParametersImpl->m_pixeSize;
	m_pDcGpViewportParametersImpl->m_zoom = params.m_pDcGpViewportParametersImpl->m_zoom;
	m_pDcGpViewportParametersImpl->m_pointSize = params.m_pDcGpViewportParametersImpl->m_pointSize;
	m_pDcGpViewportParametersImpl->m_lineWidth = params.m_pDcGpViewportParametersImpl->m_lineWidth;
	m_pDcGpViewportParametersImpl->m_bPerspective = params.m_pDcGpViewportParametersImpl->m_bPerspective;
	m_pDcGpViewportParametersImpl->m_bObjectCentered = params.m_pDcGpViewportParametersImpl->m_bObjectCentered;
	m_pDcGpViewportParametersImpl->m_pivotPos = params.m_pDcGpViewportParametersImpl->m_pivotPos;
	m_pDcGpViewportParametersImpl->m_cameraPos = params.m_pDcGpViewportParametersImpl->m_cameraPos;
	m_pDcGpViewportParametersImpl->m_fov = params.m_pDcGpViewportParametersImpl->m_fov;
	m_pDcGpViewportParametersImpl->m_aspect = params.m_pDcGpViewportParametersImpl->m_aspect;
	m_pDcGpViewportParametersImpl->m_viewRotateMat = params.m_pDcGpViewportParametersImpl->m_viewRotateMat;
}


//���ش�С
float& DcGp::DcGpViewportParameters::PixeSize()
{
	return m_pDcGpViewportParametersImpl->m_pixeSize;
}

//���ű���
float& DcGp::DcGpViewportParameters::Zoom()
{
	return m_pDcGpViewportParametersImpl->m_zoom;
}

//��ͼ�������ת����
DcGp::DcGpMatrix& DcGp::DcGpViewportParameters::ViewRotateMatrix()
{
	return m_pDcGpViewportParametersImpl->m_viewRotateMat;
}

//���С
float& DcGp::DcGpViewportParameters::PointSize()
{
	return m_pDcGpViewportParametersImpl->m_pointSize;
}

//�߿�
float& DcGp::DcGpViewportParameters::LineWidth()
{
	return m_pDcGpViewportParametersImpl->m_lineWidth;
}

//ͶӰ״̬
bool& DcGp::DcGpViewportParameters::Perspective()
{
	return m_pDcGpViewportParametersImpl->m_bPerspective;
}

bool& DcGp::DcGpViewportParameters::ObjCenterd()
{
	return m_pDcGpViewportParametersImpl->m_bObjectCentered;
}

//��ת����
DCVector3D& DcGp::DcGpViewportParameters::PivotPosition()
{
	return m_pDcGpViewportParametersImpl->m_pivotPos;
}

//���λ��
DCVector3D& DcGp::DcGpViewportParameters::CameraPosition()
{
	return m_pDcGpViewportParametersImpl->m_cameraPos;
}

//
float& DcGp::DcGpViewportParameters::Fov()
{
	return m_pDcGpViewportParametersImpl->m_fov;
}

//
float& DcGp::DcGpViewportParameters::Aspect()
{
	return m_pDcGpViewportParametersImpl->m_aspect;
}

void DcGp::DcGpViewportParameters::SaveFar(double far)
{
	m_pDcGpViewportParametersImpl->m_zFar = far;
}

void DcGp::DcGpViewportParameters::SaveNear(double near)
{
	m_pDcGpViewportParametersImpl->m_zNear = near;
}

double DcGp::DcGpViewportParameters::GetNear()
{
	return m_pDcGpViewportParametersImpl->m_zNear;
}

double DcGp::DcGpViewportParameters::GetFar()
{
	return m_pDcGpViewportParametersImpl->m_zFar;
}