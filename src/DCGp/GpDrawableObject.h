#ifndef DCGRAPHICOBJECT_H
#define DCGRAPHICOBJECT_H

#include "DCGp/DcGpDLL.h"

//Qt
#include "QObject"
#include "QtGui/QColor"
#include "QString"
#include "QFile"

#include "Eigen/Dense"

class QPushButton;

//#include "DCCore/DCGL.h"
#include "DCGp/GpBasicTypes.h"
#include "GpObject.h"
//#include "DCGp/GpBasicDevice.h"
//#include "DCGp/GpDrawContext.h"

namespace DcGp
{
	class DcGpBoundBox;
	class DcGpDrawContext;
	class glDrawParameters;
	class DcGpBasicDevice;
	class DcGpMatrix;
	class DcGpColorRampShader;

	DECLARE_SMARTPTR(DcGpDrawableObject)
	DECLARE_SMARTPTR(DcGpDrawableObjectImpl)

	typedef Eigen::Matrix4d EigenMatrix4d;

	class DCGP_API DcGpDrawableObject : public QObject, public DcGpObject
	{
		Q_OBJECT

		DECLARE_IMPLEMENT(DcGpDrawableObject)
		GET_CLASSNAME(DcGpDrawableObject)

	public:
		//����������
		enum ScalarFieldType
		{
			eScalarField		=	0	,
			eDistanceField				,
			eHeightField
		};

		DcGpDrawableObject(QString name);
		virtual ~DcGpDrawableObject();

		//����ͼ��
		virtual void Draw(DcGpDrawContext& context) = 0;

		 //���ر߽��
		virtual DcGpBoundBox GetBoundBox(bool relative=true, 
										bool withGLfeatures=false, 
										const DcGpBasicDevice* window=0) = 0;

		//��ȡ��ɫ
		virtual QColor GetColor();
		//��ʾ����
		virtual void ShowProperties(QTableWidget*) {}

		//
		virtual DcGpBoundBox GetFitBoundBox(DcGpMatrix& trans);

		//!
		virtual void DrawBoundBox(const ColorType col[]);

		//���Ӵ���
		virtual void LinkWindow(DcGpBasicDevice* window) = 0;

		//�ػ�
		void Refresh();
		DcGpBasicDevice* GetAssociatedWindow() const;
		void SetAssociatedWindow(DcGpBasicDevice* window);

		//��ȡ���Ʋ���
		virtual void GetDrawingParameters(glDrawParameters& params) const;

		//��ʱ��ɫ

		//��ǰ��ɫ�Ƿ���ʱ��ɫ��д
		virtual bool IsColorOverriden() const;

		//! Ψһɫ
		virtual const RGBColor GetTempColor() const;

		//���õ�ǰ����ʱ��ɫ
		//����һ����ɫֵ
		//���������Ƿ��Զ�������ʱ��ɫ
		virtual void SetTempColor(const RGBColor color, bool autoActivate = true);

		//�Ƿ�����ʹ����ʱ��ɫ
		virtual void EnableTempColor(bool state);

		//�Ƿ�ʹ����ɫ
		virtual bool HasColors() const;

		//�Ƿ���ʾ��ɫ
		virtual bool ColorsShown() const;

		//������ɫ����ʾ״̬
		virtual void ShowColors(bool state);

		//�л���ɫ����ʾ״̬
		virtual void ToggleColors();

		//�Ƿ�ʹ�÷���
		virtual bool HasNormals() const;

		//�Ƿ���ʾ����
		virtual bool NormalsShown() const;

		//���÷��ߵ���ʾ״̬
		virtual void ShowNormals(bool state);

		//�л����ߵ�״̬
		virtual void ToggleNormals();

		//�Ƿ��б�������ʵ����
		virtual bool HasScalarFields() const;

		//���ü���ı���������ʾ״̬
		virtual void ShowScalarField(bool state);

		//�л�����������ʾ״̬
		virtual void ToggleScalarField();

		//����ı������Ƿ���ʾ
		virtual bool ScalarFieldShown() const;

		//���ñ�ѡ��
		void SetSelected(bool status);

		//�Ƿ�ѡ��
		bool IsSelected() const;

		//
		void PrepareDisplayForRefresh();

		//�Ƿ�ɼ�
		bool IsVisible() const;

		//���ÿɼ���
		void SetVisible(bool status);

		//�ɼ����Ƿ�����
		bool IsVisibilityLocked() const;

		//�����ɼ���
		void LockVisibility(bool status);

		//�Ƿ���ʾ����
		bool IsShownName() const;

		//��ʾ����
		void ShowName(bool status);

		//��ȡ��ɫģʽ
		ColorMode GetColorMode() const;
		//������ɫģʽ
		void SetColorMode(ColorMode mode);

		//����ѡȡ��ɫ
		virtual void RefreshChoosedColor() {}

		//���ı���������ɫ��
		virtual void ChangeScalarFieldColorScale(QString name) {}

		//��ȡ������������
		virtual unsigned ScalarFieldCount() const;

		//��ȡ������������
		virtual QStringList ScalarFieldNames() const;

		virtual int GetCurrentScalarFieldIndex() {return -1;}
		//���õ�ǰ������
		virtual void SetCurrentScalarField(unsigned index, unsigned setps, ScalarFieldType type = eScalarField){}
	
		//ת������
		DcGpMatrix GetTransMat() const;
		void SetTransMat(DcGpMatrix mat);

		virtual void ApplyTransform(EigenMatrix4d& transMatrix);
		virtual void ApplyTransform(DcGpMatrix& transMatrix);

		//
		bool IsTransformed() const;
		void SetTransformStatus(bool status);

		//!brief ���ø�����ɫ������ʾ״̬
		virtual void SetSfColorBarVisibility(bool status /*!<[in] �����״ֵ̬ */);

		//!\brief ������ɫ���Ƿ�ɼ�״ֵ̬
		//!\retval ������ɫ��״ֵ̬
		bool IsSfColorbarVisible() const;

	protected:
		DcGpDrawableObject();

	private slots:
	
	};

}
#endif // DCGRAPHICOBJECT_H
