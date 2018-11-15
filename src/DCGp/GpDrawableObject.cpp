#include "GpDrawableObject.h"

//Qt
#include "QtGui/QPixmap"
#include "QtWidgets/QColorDialog"
#include "QtWidgets/QLabel"
#include "QString"
#include "QtGui/QIcon"

//common
#include "DCCore/Logger.h"

#include "DCGp/Custom/GpBoundBox.h"
#include "DCGp/GpBasicDevice.h"
#include "DCGp/GpDrawContext.h"

namespace DcGp
{
	DECLARE_SMARTPTR(DcGpDrawableObjectImpl)

	class DcGpDrawableObjectImpl
	{
		DECLARE_PHONY_CONSTRUCTOR(DcGpDrawableObjectImpl)

	protected:
		DcGpDrawableObjectImpl()
		{}

	private:
		friend class DcGpDrawableObject;

		//�����Ĵ���
		DcGpBasicDevice* m_associatedWindow;
		//��ɫ
		QColor m_color;
		//��ɫ���ð�ť
		QPushButton* m_pbtnColor;

		//��ɫģʽ
		ColorMode m_colorMode;

		//
		bool m_visible;

		//�����Ƿ�ѡ��
		bool m_selected;

		//�Ƿ�������ʾ���������򲻿ɸ��ģ�
		bool m_lockedVisibility;


		//�Ƿ���ʾ��ɫ
		bool m_colorsDisplayed;
		//�Ƿ���ʾ����
		bool m_normalsDisplayed;
		//�Ƿ���ʾ������
		bool m_scalarFieldDisplayed;

		//��ʱ��ɫ
		RGBColor m_tempColor;
		//��ʱ��ɫ�ļ���״̬
		bool m_colorIsOverriden;

		//
		DcGpMatrix m_glTrans;

		//
		bool m_glTransEnabled;

		//�Ƿ���ʾ����
		bool m_showName;

		//�Ƿ���ʾ��ɫ��
		bool m_sfColorBar;
	};
}

DcGp::DcGpDrawableObject::DcGpDrawableObject()
	: DcGpObject()
{
	m_pDcGpDrawableObjectImpl = DcGpDrawableObjectImpl::CreateObject();

	m_pDcGpDrawableObjectImpl->m_color = QColor(255, 255, 255);
	m_pDcGpDrawableObjectImpl->m_pbtnColor = nullptr;
	m_pDcGpDrawableObjectImpl->m_selected = false;
	m_pDcGpDrawableObjectImpl->m_visible = true;
	m_pDcGpDrawableObjectImpl->m_colorsDisplayed = false;
	m_pDcGpDrawableObjectImpl->m_lockedVisibility = false;
	m_pDcGpDrawableObjectImpl->m_normalsDisplayed = false;
	m_pDcGpDrawableObjectImpl->m_scalarFieldDisplayed = false;
	m_pDcGpDrawableObjectImpl->m_associatedWindow = nullptr;
	m_pDcGpDrawableObjectImpl->m_colorIsOverriden = false;
	m_pDcGpDrawableObjectImpl->m_showName = false;
	m_pDcGpDrawableObjectImpl->m_colorMode = eTrueLinearColor;

	//����Ĭ�ϵ���ʱ��ɫ
	m_pDcGpDrawableObjectImpl->m_tempColor[0] = 255;
	m_pDcGpDrawableObjectImpl->m_tempColor[1] = 255;
	m_pDcGpDrawableObjectImpl->m_tempColor[2] = 255;
}

DcGp::DcGpDrawableObject::DcGpDrawableObject(QString name)
	: DcGpObject(name)
{
	m_pDcGpDrawableObjectImpl = DcGpDrawableObjectImpl::CreateObject();

	m_pDcGpDrawableObjectImpl->m_color = QColor(255, 255, 255);
	m_pDcGpDrawableObjectImpl->m_pbtnColor = nullptr;
	m_pDcGpDrawableObjectImpl->m_selected = false;
	m_pDcGpDrawableObjectImpl->m_visible = true;
	m_pDcGpDrawableObjectImpl->m_colorsDisplayed = false;
	m_pDcGpDrawableObjectImpl->m_lockedVisibility = false;
	m_pDcGpDrawableObjectImpl->m_normalsDisplayed = false;
	m_pDcGpDrawableObjectImpl->m_scalarFieldDisplayed = false;
	m_pDcGpDrawableObjectImpl->m_associatedWindow = nullptr;
	m_pDcGpDrawableObjectImpl->m_colorIsOverriden = false;
	m_pDcGpDrawableObjectImpl->m_showName = false;
	m_pDcGpDrawableObjectImpl->m_colorMode = eTrueLinearColor;

	//����Ĭ�ϵ���ʱ��ɫ
	m_pDcGpDrawableObjectImpl->m_tempColor[0] = 255;
	m_pDcGpDrawableObjectImpl->m_tempColor[1] = 255;
	m_pDcGpDrawableObjectImpl->m_tempColor[2] = 255;
}

DcGp::DcGpDrawableObject::~DcGpDrawableObject()
{
	//delete m_pDcGpDrawableObjectImpl.get();
}

DcGp::ColorMode DcGp::DcGpDrawableObject::GetColorMode() const
{
	return m_pDcGpDrawableObjectImpl->m_colorMode;
}

void DcGp::DcGpDrawableObject::SetColorMode(ColorMode mode)
{
	m_pDcGpDrawableObjectImpl->m_colorMode = mode;
}

QColor DcGp::DcGpDrawableObject::GetColor()
{
	return m_pDcGpDrawableObjectImpl->m_color;
}

DcGp::DcGpBasicDevice* DcGp::DcGpDrawableObject::GetAssociatedWindow() const
{
	return m_pDcGpDrawableObjectImpl->m_associatedWindow;
}

//
DcGp::DcGpBoundBox DcGp::DcGpDrawableObject::GetBoundBox(bool relative/* =true */, bool withGLfeatures/* =false */, const DcGpBasicDevice* window/* =0 */)
{
	DcGpBoundBox box;
	return box;
}

DcGp::DcGpBoundBox DcGp::DcGpDrawableObject::GetFitBoundBox(DcGpMatrix& trans)
{
	DcGpBoundBox box;
	return box;
}

void DcGp::DcGpDrawableObject::DrawBoundBox(const ColorType col[])
{
	GetBoundBox(true, false, m_pDcGpDrawableObjectImpl->m_associatedWindow).Draw(col);
}

//������ʱ��ɫ
void DcGp::DcGpDrawableObject::SetTempColor(const RGBColor color, bool autoActivate /*= true*/)
{
	m_pDcGpDrawableObjectImpl->m_tempColor = color;

	if (autoActivate)
		EnableTempColor(true);
}

//�Ƿ�������ʱ��ɫ
void DcGp::DcGpDrawableObject::EnableTempColor(bool state)
{
	m_pDcGpDrawableObjectImpl->m_colorIsOverriden = state;
	if (state)
	{
		m_pDcGpDrawableObjectImpl->m_colorMode = eSingleColor;
	}
}

//�Ƿ�����ʹ����ʱ��ɫ
bool DcGp::DcGpDrawableObject::IsColorOverriden() const
{
	return m_pDcGpDrawableObjectImpl->m_colorIsOverriden;
}

//��ȡ��ʱ��ɫ
const RGBColor DcGp::DcGpDrawableObject::GetTempColor() const
{
	return m_pDcGpDrawableObjectImpl->m_tempColor;
}

//��ʾ��ɫ
void DcGp::DcGpDrawableObject::ShowColors(bool state)
{
	m_pDcGpDrawableObjectImpl->m_colorsDisplayed = state;
	EnableTempColor(!state);
}

//�л���ɫ��ʾ״̬
void DcGp::DcGpDrawableObject::ToggleColors()
{
	ShowColors(!ColorsShown());
}

//��ɫ����ʾ״̬
bool DcGp::DcGpDrawableObject::ColorsShown() const
{
	return m_pDcGpDrawableObjectImpl->m_colorsDisplayed;
}

//�Ƿ���ʾ��ɫ
bool DcGp::DcGpDrawableObject::HasColors() const
{
	return ColorsShown();
}

//��ʾ����
void DcGp::DcGpDrawableObject::ShowNormals(bool state)
{
	m_pDcGpDrawableObjectImpl->m_normalsDisplayed = state;
}

//�л�������ʾ״̬
void DcGp::DcGpDrawableObject::ToggleNormals()
{
	ShowNormals(!NormalsShown());
}

//���ߵ���ʾ״̬
bool DcGp::DcGpDrawableObject::NormalsShown() const
{
	return m_pDcGpDrawableObjectImpl->m_normalsDisplayed;
}

//�Ƿ���ʾ����
bool DcGp::DcGpDrawableObject::HasNormals() const
{
	return false;
}

//�Ƿ���ʾ������
bool DcGp::DcGpDrawableObject::HasScalarFields() const
{
	return false;
}

//��ʾ������
void DcGp::DcGpDrawableObject::ShowScalarField(bool state)
{
	m_pDcGpDrawableObjectImpl->m_scalarFieldDisplayed = state;
}

//�л�������
void DcGp::DcGpDrawableObject::ToggleScalarField()
{
	ShowScalarField(!ScalarFieldShown());
}

void DcGp::DcGpDrawableObject::Refresh()
{
	if (m_pDcGpDrawableObjectImpl->m_associatedWindow)
	{
		m_pDcGpDrawableObjectImpl->m_associatedWindow->Redraw();
	}
}

//����������ʾ״̬
bool DcGp::DcGpDrawableObject::ScalarFieldShown() const
{
	return m_pDcGpDrawableObjectImpl->m_scalarFieldDisplayed;
}

//��ȡ��ʾ����
void DcGp::DcGpDrawableObject::GetDrawingParameters(glDrawParameters& params) const
{
	//��ɫ��д
	if (IsColorOverriden())
	{
		params.showColors=true;
		params.showNormals=false;
		params.showScalarField=false;
	}
	else
	{
		//a scalar field must have been selected for display!
		params.showScalarField = HasScalarFields() && ScalarFieldShown();
		params.showNormals = HasNormals() &&  NormalsShown();
		//colors are not displayed if scalar field is displayed
		params.showColors = !params.showScalarField && HasColors() && ColorsShown();
	}
}

//���ñ�ѡ��
void DcGp::DcGpDrawableObject::SetSelected(bool status)
{
	m_pDcGpDrawableObjectImpl->m_selected = status;
}

//�Ƿ�ѡ��
bool DcGp::DcGpDrawableObject::IsSelected() const
{
	return m_pDcGpDrawableObjectImpl->m_selected;
}

//
void DcGp::DcGpDrawableObject::PrepareDisplayForRefresh()
{
	if (m_pDcGpDrawableObjectImpl->m_associatedWindow)
	{
		m_pDcGpDrawableObjectImpl->m_associatedWindow->ToBeRefreshed();
	}
}

//�Ƿ�ɼ�
bool DcGp::DcGpDrawableObject::IsVisible() const
{
	return m_pDcGpDrawableObjectImpl->m_visible;
}

//���ÿɼ���
void DcGp::DcGpDrawableObject::SetVisible(bool status)
{
	m_pDcGpDrawableObjectImpl->m_visible = status;
}

//�ɼ����Ƿ�����
bool DcGp::DcGpDrawableObject::IsVisibilityLocked() const
{
	return m_pDcGpDrawableObjectImpl->m_lockedVisibility;
}

//�����ɼ���
void DcGp::DcGpDrawableObject::LockVisibility(bool status)
{
	m_pDcGpDrawableObjectImpl->m_lockedVisibility = status;
}

//�Ƿ���ʾ����
bool DcGp::DcGpDrawableObject::IsShownName() const
{
	return m_pDcGpDrawableObjectImpl->m_showName;
}

//��ʾ����
void DcGp::DcGpDrawableObject::ShowName(bool status)
{
	m_pDcGpDrawableObjectImpl->m_showName = status;
}

//
DcGp::DcGpMatrix DcGp::DcGpDrawableObject::GetTransMat() const
{
	return m_pDcGpDrawableObjectImpl->m_glTrans;
}

void DcGp::DcGpDrawableObject::SetTransMat(DcGpMatrix mat)
{
	m_pDcGpDrawableObjectImpl->m_glTrans = mat;
}

bool DcGp::DcGpDrawableObject::IsTransformed() const
{
	return m_pDcGpDrawableObjectImpl->m_glTransEnabled;
}

void DcGp::DcGpDrawableObject::SetTransformStatus(bool status)
{
	m_pDcGpDrawableObjectImpl->m_glTransEnabled = status;
}

void DcGp::DcGpDrawableObject::SetAssociatedWindow(DcGpBasicDevice* window)
{
	m_pDcGpDrawableObjectImpl->m_associatedWindow = window;
}

//��ʾ��ɫ��
void DcGp::DcGpDrawableObject::SetSfColorBarVisibility(bool state)
{
	m_pDcGpDrawableObjectImpl->m_sfColorBar = state;
}

bool DcGp::DcGpDrawableObject::IsSfColorbarVisible() const
{
	return m_pDcGpDrawableObjectImpl->m_sfColorBar;
}

unsigned DcGp::DcGpDrawableObject::ScalarFieldCount() const
{
	return 0;
}

QStringList DcGp::DcGpDrawableObject::ScalarFieldNames() const
{
	return QStringList();
}

void DcGp::DcGpDrawableObject::ApplyTransform(DcGpMatrix& transMatrix)
{
	DCCore::Logger::Warning(tr("Transform Invalid!"));
}

void DcGp::DcGpDrawableObject::ApplyTransform(EigenMatrix4d& transMatrix)
{
	DCCore::Logger::Warning(tr("Transform Invalid!"));
}