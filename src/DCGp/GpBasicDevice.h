#ifndef DCGP_BASIC_DEVICE_H
#define DCGP_BASIC_DEVICE_H

#include "DCGp/DcGpDLL.h"

//Qt��׼��
#include "QtGui/QFont"
class QString;
class QImage;

//DCCore
#include "DCCore/BasicTypes.h"

#include "DCGp/GpMatrix.h"

namespace DcGp
{

//OpenGL�ӿ�ģ�͵ı�׼����
	DECLARE_SMARTPTR(DcGpViewportParameters)
	DECLARE_SMARTPTR(DcGpViewportParametersImpl)

class DCGP_API DcGpViewportParameters
{
	DECLARE_IMPLEMENT(DcGpViewportParameters)

public:
	//Ĭ�Ϲ��캯��
	DcGpViewportParameters();

	//��ֵ���캯��
	DcGpViewportParameters(const DcGpViewportParameters& params);

	//���ش�С
	float& PixeSize();

	//���ű���
	float& Zoom();

	//��ͼ�������ת����
	DcGpMatrix& ViewRotateMatrix();

	//���С
	float& PointSize();

	//�߿�
	float& LineWidth();

	//ͶӰ״̬
	bool& Perspective();

	bool& ObjCenterd();

	//��ת����
	DCVector3D& PivotPosition();

	//���λ��
	DCVector3D& CameraPosition();

	//
	float& Fov();

	//
	float& Aspect();

	void SaveNear(double near);
	void SaveFar(double far);
	double GetNear();
	double GetFar();
};

//GL��ʾ����
class DcGpBasicDevice
{
public:
	virtual ~DcGpBasicDevice()
	{

	}

    //ˢ����ʾ
	virtual void Redraw() = 0;

	//
    virtual void ToBeRefreshed() = 0;

    //
    virtual void Refresh() = 0;

    //ʹ��ǰ�ӿ�������Ч
    /**����һ���ػ�ʱ�����¼����ӿڲ���**/
    virtual void InvalidateViewport() = 0;

    //��ͼ���л�ȡ��ͼ��ID
    virtual unsigned GetTextureID(const QImage& image) = 0;

    //�ӻ����������ͷ�Texture
    virtual void ReleaseTexture(unsigned texID) = 0;

	//��������
	virtual const QFont& GetDisplayedTextFont() = 0;

	//�ı��Ķ��뷽ʽ
	enum TextAlign
	{ 
		eLeftAlign			=	1	,
		eHMidAlign			=	2	,
		eRightAlign			=	4	,
		eTopAlign			=	8	,
		eVMidAlign			=	16	,
		eBottomAlign		=	32	,
		eDefaultAlign		=	1 | 8
	};

    //��ָ���Ķ�ά���괦��ʾ�ַ���
	//�����ڶ�ά�Ӵ�ʹ��
	//����һ����ʾ���ı�
	//�������������ı���X��Y����
	//�����ģ��ı��Ķ��뷽ʽ
	//�����壺�ı���͸����
	//���������ı�����ɫ
	//�����ߣ��ı�������
    virtual void Display2DLabel(QString text, int x, int y, 
								unsigned char align = eDefaultAlign, 
								float bkgAlpha = 0, 
								const unsigned char* rgbColor = 0, 
								const QFont* font = 0) = 0;

	//��ָ������ά���괦��ʾ�ı�
    //�����������ά�Ӵ���ʹ��
    //����һ��Ҫ��ʾ���ı�
    //���������ı���λ�ã�������
	//����������ɫ
	//�����ģ�����
	virtual void Display3DLabel(const QString& str, 
								const DCVector3D& pos3D,
								const unsigned char* rgbColor = 0, 
								const QFont& font = QFont()) = 0;

	//�����Ƿ�֧�ָ����汾��OpenGL
	//����ΪQGLFormat::OpenGLVersionFlag
	virtual bool SupportOpenGLVersion(unsigned openGLVersionFlag) = 0;

	//���ص�ǰ��ģ����ͼ����(GL_MODELVIEW)
	virtual const double* GetModelViewMatrixd() = 0;

	//���ص�ǰ��ͶӰ����(GL_PROJECTION)
    virtual const double* GetProjectionMatrixd() = 0;

	//���ص�ǰ���ӿ�����(GL_VIEWPORT)
    virtual void GetViewportArray(int viewport[/*4*/]) = 0;

	virtual QString GetWindowTitle() const = 0;
	virtual void SetWindowTitle(QString title) = 0;
};

}
#endif