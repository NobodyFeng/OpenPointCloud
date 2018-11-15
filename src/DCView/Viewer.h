#ifndef MPVIEWER_VIEWER_H
#define MPVIEWER_VIEWER_H

#include "ViewDLL.h"

#include <mutex>

//DDCore
#include "DCCore/BasicTypes.h"

//View
#include "DCView/View.h"
#include "DCView/ViewBase.h"


//DcGp
#include "DCGp/GpBasicDevice.h"
#include "DCGp/GpDrawContext.h"
#include "DCGp/Custom/GpTools.h"
#include "DCGp/Custom/GpInteractor.h"

//MPutil
#include "DCUtil/Catcher.h"
#include "UnitManager.h"


namespace DcGp
{
	class DcGpEntity;
	class DcGpPolyline;
}

namespace MPGp
{
	class DistanceLine;
}

namespace DCView
{
	class GUIEventHandle;
}

//���Ӵ������

namespace MPViewer
{

	class DC_VIEW_API Viewer : public DCView::View, public DcGp::DcGpBasicDevice, public ViewBase
	{
		Q_OBJECT
	public:
		explicit Viewer(QWidget* pParent = nullptr, 
			const QGLFormat& format = QGLFormat::defaultFormat(), 
			QGLWidget* pSharedWidget = nullptr);

		//! ���ݴ�����ļ�����ʵ����һ��viewer
		explicit Viewer(QString fileName);

		~Viewer();

		//����ͼԪ
		void SetSceneRoot(DcGp::DcGpEntity* pEntity);
		//��ȡͼԪ
		DcGp::DcGpEntity* GetSceneRoot() const;

		//�Ƿ�׽������ת���ĵ�
		bool IsRCenterPicked(const DCCore::Point_2& point2D,
			DCCore::Point_3& point3D);

		//��Ļ���굽��������
		virtual bool ScreenToWorld(const DCCore::Point_2& point2D,
			DCCore::Point_3& point3D);

		//��̬������ת����״̬
		void SetDynamicPivotState(bool state);
		bool GetDynamicPivotState();

		//������ʾȫ������
		void CenterAll();
		//������ʾѡ������
		void CenterSelected(DcGp::DcGpEntity* pEntity);

		//�������ĺ����ű���
		void UpdateBoundingBox();

		//! ���µ�ǰ�����б�
		void UpdateActiveItemsList(int x, int y, bool extendToSelectedLabels = false);

		void SetPickedWorldPoint(QString pickedWorldPoint);
		void SetCursorText(QString cursorText);

		//void SetDrawPickedPointState(bool state);

		//! ��ȡview�����Ĳ�׽��
		MPUtil::Catcher* GetCatcher();

		void SetPressPointState(bool state);

		//! ���ݵ�ǰ����¼������㲢�����ɵ�hits��¼�����ȥ
		MPUtil::HitList GenHitsByEvent(QMouseEvent* event);

		//! ���һ���¼�������
		void AddEventHandle(DCCore::GUIEventHandle* eventHandle);

		void UpdateEventhandleValidState(QStringList handles, bool state);

		//! ����ָ�����ֻ�ȡ�¼�������
		DCCore::GUIEventHandle* GetEventHandleByClassName(QString uniqueName);

		//! ��ȡ�����¼�������
		std::vector<DCCore::GUIEventHandle* > GetEventHandles();

		//! ��ȡ�Ӵ���ͼ
		QImage FrameBufferSnapshot();

		virtual void EventTraversal();

		virtual void UpdateTraversal();

		virtual void RenderingTraversals();

		//OpenGL����
		virtual void paintGL();

	private:

		//! ��ʼ��viewer��������
		void Init();
		//�ر��¼�
		void closeEvent(QCloseEvent* event);

		void DrawSnapSymbol();
signals:
		//����ʰȡ
		void EntityPicked(DcGp::DcGpEntity* pEntity);
		//�㱻ʰȡ
		void PointPicked(const DCCore::Point_3& point);
		//����ͼԪ
		void EntityGenerated(DcGp::DcGpEntity* pEntity);

		//�Ƴ���ͼԪ
		void RemoveRootEntity(DcGp::DcGpEntity* pEntity);

		//ʰȡ����
		void IdIndexPicked(unsigned selectedID, unsigned subSelectedID);

		//! ʵ��ѡ��仯���������objID
		void EntitySelectionChanged(int uinqueID);

		//! ���ص�ǰ���λ�ô���hits
		void RayHits(MPUtil::HitList hits);
		private slots:
			//�ı�ͶӰ����
			void ChangeProjectionMatrix(const double* pMatrix);
			//�ı�ģ����ͼ����
			void ChangeModelViewMatrix(const double* pMatrix);
			//�ı��ӿھ���
			void ChangeViewportMatrix(const int* pMatrix);

			//�ı䵱ǰ���ű���
			void ChangedCurrentScale(double scale);

	protected:
		//��갴��
		void mousePressEvent(QMouseEvent* event);
		//����ƶ�
		void mouseMoveEvent(QMouseEvent* event);
		//���̰����¼�
		void keyPressEvent(QKeyEvent* event);

		void mouseReleaseEvent(QMouseEvent* event);
		void mouseDoubleClickEvent(QMouseEvent* event);

	private:
		void CreateConnections();

		//����������
		DcGp::DcGpDrawContext GetContext();

	private:
		//ʰȡģʽ
		enum PickingMode
		{ 
			eNoPicking					=	0	,                    //! û��ʵ�岶׽
			eEntityPicking						,					 //! ����ʵ�岶׽��һ��������ͷ��м���
			eEntityRectPicking					,                    //! ����ѡ����ʵ��
			eFastPicking						,                    //! ����ʵ�岶׽
			ePointPicking						,
			eTrianglePicking					,
			eAutoPointPicking					,
			eDoubleClickedPicking               ,
			eElementPicking                     ,
			eDefaultPicking						
		};
		//ѡȡ
		int Pick(PickingMode mode, const DCCore::Point_2& point, 
			int* subID = nullptr, int width = 5, int height = 5, bool drawSample = false);

		virtual void DrawWithNames(const QPoint& point);
		virtual void PostPicking(const QPoint& point) { Q_UNUSED(point); }

	private:
		virtual void Resize(int iWidth, int iHeight);
		virtual void Paint2D();
		virtual void Paint3D();

		virtual void FastDraw3D();
		virtual BoundBox BoundingBox() const;


	private:
		//-------------------------------�Ӵ��Ϲ����gp����---------------------------------------------------------
		//�������ڵ㣬����ڵ�
		DcGp::DcGpEntity* m_pSceneRoot;

		

		//-----------------------------------------------------------------------------------------

		//ʰȡ������
		static const int s_c_pickBufferSize = 65536;
		GLuint m_pickBuffer[s_c_pickBufferSize];

		//��̬������ת����
		bool m_bUpdatingPivot;

		//ͶӰ����
		double m_projectionMatrix[16];
		//ģ����ͼ����
		double m_modelViewMatrix[16];

		//OpenGL������С
		GLuint m_glWidth;
		GLuint m_glHeight;

		/********************************************************************************/

		//-----------------�����Ƿ�ʹ�õ�ǰѡ�����ı߽�С����ŵ���ǰ����ʹ�á�-----------------------------------------------------------------
		bool m_useSelectBoundbox;
		BoundBox m_selectBoundbox;

		//--------------------------------------------------------------------------------------------
		//! ��ǰ��������б�����ƽ����ת����
		std::list<DcGp::DcGpInteractor* > m_activeItems;

		//! ��ǰ��괦��������ֵ
		QString m_pickedWorldPoint;

		//! ��ǰ��괦��Ҫ��ʾ���Ƶ�����(��������ֵ������һ��ľ���ֵ��)
		QString m_cursorText;

		QPoint m_cursorPos;

		//**************��λ����********************/
		QString m_unit;

		double m_scale;

		UnitManager* m_unitMan;

		//ʹ�ù��߲�׽����ͨ���ⲿ���²�׽�����������ز�ͬ��hits
		MPUtil::Catcher* m_catcher;

		//! �Ƿ񼤻�������������߲�׽�ź�
		//bool m_isGetLeftPressedPoint;

		//! �¼�������
		std::vector<DCCore::GUIEventHandle* > m_eventHandles;

		

	public:
		//��λ����
		virtual QString GetUnit();
		
		virtual QString GetENUnit();

		virtual void SetUnit(QString unit);

		virtual double GetScale();

		virtual void SetScale(double scale);
		//ˢ����ʾ
		virtual void Redraw(){DCView::View::Refresh();}

		//
		virtual void ToBeRefreshed();

		//
		virtual void Refresh(){DCView::View::Refresh();}

		//ʹ��ǰ�ӿ�������Ч
		/**����һ���ػ�ʱ�����¼����ӿڲ���**/
		virtual void InvalidateViewport(){}

		//��ͼ���л�ȡ��ͼ��ID
		virtual unsigned GetTextureID(const QImage& image){return 0;}

		//�ӻ����������ͷ�Texture
		virtual void ReleaseTexture(unsigned texID){}

		//��������
		virtual const QFont& GetDisplayedTextFont(){return QFont();}

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
			const QFont* font = 0);

		//��ָ������ά���괦��ʾ�ı�
		//�����������ά�Ӵ���ʹ��
		//����һ��Ҫ��ʾ���ı�
		//���������ı���λ�ã�������
		//����������ɫ
		//�����ģ�����
		virtual void Display3DLabel(const QString& str, 
			const DCVector3D& pos3D,
			const unsigned char* rgbColor = 0, 
			const QFont& font = QFont());

		//�����Ƿ�֧�ָ����汾��OpenGL
		//����ΪQGLFormat::OpenGLVersionFlag
		virtual bool SupportOpenGLVersion(unsigned openGLVersionFlag){return true;}

		//���ص�ǰ��ģ����ͼ����(GL_MODELVIEW)
		virtual const double* GetModelViewMatrixd();

		//���ص�ǰ��ͶӰ����(GL_PROJECTION)
		virtual const double* GetProjectionMatrixd();

		//���ص�ǰ���ӿ�����(GL_VIEWPORT)
		virtual void GetViewportArray(int viewport[/*4*/]);

		virtual QString GetWindowTitle() const{return "";}
		virtual void SetWindowTitle(QString title){}
	};

}

#endif