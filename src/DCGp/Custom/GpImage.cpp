//Qt
#include "QtGui/QImage"
#include "QFileInfo"

#include "atlimage.h"

#include "DCCore/DCGL.h"
#include "DCGp/GpDrawContext.h"

#include "GpImage.h"

namespace DcGp
{
	DECLARE_SMARTPTR(DcGpImageImpl)

	class DcGpImageImpl
	{
		DECLARE_PHONY_CONSTRUCTOR(DcGpImageImpl)

		public:
			~DcGpImageImpl()
			{

			}

		protected:
			DcGpImageImpl()
			{

			}
		private:
			friend class DcGpImage;

			//Ӱ��
			QImage m_image;

			//Ŀ������
			GLuint m_targetTex;

			//bitmap
			BITMAP m_bm;
	};
}

DcGp::DcGpImage::DcGpImage()
	: DcGpEntity()
{
	m_pDcGpImageImpl = DcGpImageImpl::CreateObject();
}

DcGp::DcGpImage::DcGpImage(QString name)
	: DcGpEntity(name)
{
	m_pDcGpImageImpl = DcGpImageImpl::CreateObject();
}

DcGp::DcGpImage::~DcGpImage()
{

}

void DcGp::DcGpImage::DrawMyselfOnly(DcGpDrawContext& context)
{
	//����������أ��򷵻��ϲ�����
	if (!IsVisible())
	{
		return;
	}

	if(!glIsTexture(m_pDcGpImageImpl->m_targetTex))
	{
		//��������
		SetTarget(m_pDcGpImageImpl->m_image);
	}
	

	//�ж��Ƿ��ǻ�����ά����
	if (MACRO_Draw3D(context))
	{
		//glClear(GL_DEPTH_BUFFER_BIT);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, m_pDcGpImageImpl->m_targetTex);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f);	//first point
			glVertex3f(-493.713f, 93.723f, -482.14084f);
			glTexCoord2f(1.0f, 0.0f);	//second point
			glVertex3f(620.877f, 22.880f, -618.419f);
			glTexCoord2f(1.0f, 0.5f);	//third point
			glVertex3f(623.558f, 260.611f, -690.228f);
			glTexCoord2f(0.0f, 0.7f);	//fourth point
			glVertex3f(-478.968f, 333.700f, -545.807f);
		glEnd();

		//glBindTexture(GL_TEXTURE_2D, 0);
		glDisable(GL_TEXTURE_2D);
	}
	auto a =2;
}

bool DcGp::DcGpImage::LoadFromFile(const QString& fileName,
	                               bool coordinatesShiftEnabled,
	                               double* coordinatesShift)
{

	QFileInfo fi(fileName);
	if( fi.suffix().toLower()=="png" || fi.suffix().toLower()=="xpm" || fi.suffix().toLower()=="jpg")
	{
		m_pDcGpImageImpl->m_image = QImage(fileName);
		auto a = 3;
	}
	return true;
}

void DcGp::DcGpImage::SetTarget(QImage& image)
{
	if (m_pDcGpImageImpl->m_image.isNull())
		return;
	if (m_pDcGpImageImpl->m_targetTex) 
	{
		glDeleteTextures(1, &m_pDcGpImageImpl->m_targetTex);
		m_pDcGpImageImpl->m_targetTex = 0;
	}

	// create texture
	glGenTextures(1, &m_pDcGpImageImpl->m_targetTex);
	QImage tximg = QGLWidget::convertToGLFormat(image);
	glBindTexture(GL_TEXTURE_2D, m_pDcGpImageImpl->m_targetTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, tximg.width(), tximg.height(),
		0, GL_RGBA, GL_UNSIGNED_BYTE, tximg.bits());

	glBindTexture(GL_TEXTURE_2D, 0);
} 
