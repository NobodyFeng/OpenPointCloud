#ifndef DC_VIEW_VIEWER_H
#define DC_VIEW_VIEWER_H

#include "ViewDLL.h"

#include <array>
//Boost
#include "boost/shared_ptr.hpp"

//Qt
#include "DCCore/DCGL.h"
#include <QPoint>

class QColor;
class QLinearGradient;

#include "Common.h"
#include "ViewerParams.h"

#define DCLWVIEWER_VERSION 0x020101

namespace DcGp
{
	class DcGpEntity;
	class DcGpPolyline;
}

namespace DCCore
{
	class GUIEventHandle;
}

namespace DCView
{
	class GLFilter;
	typedef boost::shared_ptr<GLFilter> GLFilterPtr;
	class GUIEventHandle;
	class Camera;

	enum MatrixPiple
	{
		eOpenGL						=	0,
		eCamera						=	1
	};

	/*
		*
		* ���ӻ�����
		* �̳��Լ� QGLWidget
		* 
	*/
	class DC_VIEW_API View : public QGLWidget
	{
		Q_OBJECT
	public:
		
		// ��ͼģʽ
		enum ViewMode
		{
			eTopViewMode				=	0x00000010	,	//����ͼģʽ
			eBottomViewMode				=	0x00000020	,	//����ͼģʽ
			eFrontViewMode				=	0x00000040	,	//ǰ��ͼģʽ
			eBackViewMode				=	0x00000080	,	//����ͼģʽ
			eLeftViewMode				=	0x00000100	,	//����ͼģʽ
			eRightViewMode				=	0x00000200	,	//����ͼģʽ
		};

		// ��ά���ģʽ
		enum InterActionMode
		{
			eNoInter			=	0, // �޽���
			ePan				=	1, // ƽ��
			eRotate				=	2, // ��ת
			eZoom				=	3, // ����
			e2DOnly				=	4, // ����ά
			eOrther				=	5  // ���� 
		};

		//��Χ��
		struct BoundBox
		{
			Point_3 center;
			Point_3 minCorner;
			Point_3 maxCorner;
		};

	public:
		// ��ȡ��ǰ����
		static View* CurrentViewer();

		// �����ڹر�ʱ����յ�ǰ���ڶ���
		static void NullCurrentViewer();

		// ��Ϊ��ǰ
		void SetCurrent();

	public:
		explicit View(QWidget* pParent = nullptr, 
			const QGLFormat& format = QGLFormat::defaultFormat(), 
			QGLWidget* pSharedWidget = nullptr);

		virtual ~View();

		//������ʾȫ������
		virtual void CenterAll() = 0;

		// ��ȡͶӰģʽ
		ProjectionMode GetProjection() const;
		
		// ����ͶӰģʽ
		void SetProjection(ProjectionMode mode);

		// ������ͼ
		void SetView(ViewMode mode);
		
		// ������ͼ
		void ResetView();

		// ��ȡ���λ�õ�Զ�����������
		void Get3DRayUnderMouse(const Point_2& point2D, Point_3* v1, Point_3* v2);

		// ��Ļ(��ά)���굽����(��ά)����
		virtual bool ScreenToWorld(const Point_2& point2D, Point_3& point3D);
		
		// ����(��ά)���굽��Ļ(��ά)����
		virtual bool WorldToScreen(const Point_3& point, Point_2& point2D);

		// ����GLFilter
		void SetGLFilter(GLFilterPtr pFilter);
		
		// ��ȡGLFilter
		GLFilterPtr GetGLFilter() const;

		// �ػ�
		void Refresh();

		// �����Ƿ�����LOD
		void SetApplyLODState(bool state);

		// ������ά����ģʽ
		void SetInterActionMode(InterActionMode iaMode);

		// ��ʾ��Ϣ
		void DisplayMessage(const QString& message, int delay = 2000);

		// ��������
		void DrawText(int x, int y, const QString& text, const QFont& fnt = QFont());

		// ��������Ч������״̬
		void SetSunLightState(bool state);
		
		// ��ȡ����Ч������״̬
		bool GetSunLightState();

		//������귽��
		Vector_3 MouseOrientation(unsigned iX, unsigned iY);

		// �ƶ����
		void MoveCamera(const Vector_3& v);

		//  ���һ��������
		void AddManipulatorHandle(DCCore::GUIEventHandle* manipulator);

		// ��ת��ͼ
		void RotateView(const Matrix_4_4& matrix);

		// ������ת����
		void EnablePivot(bool bState);

		// ת������
		void RotatingWheel(float fDegree);

		//����
		void Scale(float fScale);

		// ��ȡ�Ӵ�������camera
		Camera* GetCamera() const;
		
		// camera--����camera�࣬�����������gl����
		MatrixPiple GetMatrixPiple() const;

		void SetMatrixPiple(MatrixPiple mpType);

		//ʹͶӰ������Ч
		void DisableProjection();
		//ʹģ����ͼ��Ч
		void DisableModelView();

		//�������λ��
		void UpdateCameraPos(const DCVector3D& pos);
		void UpdateRotateView(const Matrix_4_4& rm);

		bool IsKeyPressed(Qt::Key key);

	//-----------------------------��̬֡����----------------------------
	public:
		void SetRefreshRate( int msec );
		int GetRefreshRate();

		void SetContinuousUpdate(bool continuous);
	signals:
		//��갴��
		void MousePressed(Qt::MouseButton button, int x, int y);
		//����ɿ�
		void MouseReleased(Qt::MouseButton button, int x, int y);
		//�ƶ����
		void MouseMoving(Qt::MouseButtons buttons, int x, int y);

		//ͶӰ�������仯
		void ProjectionMatrixChanged(const double* pMatrix);
		//ģ����ͼ�������仯
		void ModelViewMatrixChanged(const double* pMatrix);
		//�ӿھ������仯
		void ViewportMatrixChanged(const int* pMatrix);

		//��ǰ���ű��������仯,ֻ������ͶӰ����
		void CurrentScaleChanged(double scale);

	protected:
// 			enum Module
// 			{
// 				eGlewModule					=	0x00000010	,	//glewģ��
// 				eShaderModule				=	0x00000020	,	//Shaderģ��
// 				eFBOModule					=	0x00000040		//FBOģ��
// 			};
// 			typedef Module Modules;
// 			//����ģ��
// 			void Enable(Modules modules);
	protected:
		//�����
		virtual void mousePressEvent(QMouseEvent* event);
		//����ɿ�
		virtual void mouseReleaseEvent(QMouseEvent* event);
		//����ƶ�
		virtual void mouseMoveEvent(QMouseEvent* event);
		//ת������
		virtual void wheelEvent(QWheelEvent* event);

		//���̰������
		virtual void keyPressEvent(QKeyEvent* event);

		virtual void keyReleaseEvent(QKeyEvent * event);
	public:
		//���´�С
		virtual void Resize(int iWidth, int iHeight){}
		//����
		virtual void Paint3D() = 0;
		virtual void Paint2D() = 0;
		virtual void FastDraw3D() {}

		//pick��post
		virtual void DrawWithNames(const QPoint& point) {}
		virtual void PostPicking(const QPoint& point) { Q_UNUSED(point); }

		
		virtual BoundBox BoundingBox() const = 0;

		//�������Ĳ��ƶ����
		void UpdateCenterAndZoom(const BoundBox& box);

		//������ͶӰ
		void EnableOrthoProjection();

		//������ת����
		void UpdatePivot(const Point_3& ptPivot);

		//������Ļʵ�����ش�С
		float ComputeActualPixelSize();

		//��ȡ�Ӿ������������
		std::array<std::array<float, 4> ,6> GetFrustum();

		virtual const ViewerParams& GetViewportParameters() const;

		//OpenGL����
		void Render();
	private:
		//��ʼ��OpenGL����
		virtual void initializeGL();
		//��С�仯
		virtual void resizeGL(int iWidth, int iHeight);
		
	
	 //������Ϣ�ӿ�
	public:
		virtual QString HelpString() const;

		virtual QString MouseString() const;
		virtual QString KeyboardString() const;

	//hud��������
	public:
		virtual void SetHudtext(QString text);

		//-----------------------------------InnerRoot[��Ҫ����������ʵ�������ѡȡ�����еĶ����]-----------------------------------------------

		//! ��ȡ�ڲ����ڵ�
		virtual DcGp::DcGpEntity* GetInnerRoot();

		//! ���ʵ�����innerRoot��
		virtual void AddtoInnerRoot(DcGp::DcGpEntity* obj2D, bool noDependency = true);

		//! �Ƴ�ָ���Ķ���
		virtual void RemoveFormInnerRoot(DcGp::DcGpEntity* obj2D);

		//----------------------------------------------------------------------------------
		
	public slots:
		virtual void Help();
		void SetKeyDescription(int key, QString description);
	private:
		class ViewerImpl;
		friend class ViewerImpl;
		ViewerImpl* m_pImpl;
	};
}

#endif // VIEWER_H
