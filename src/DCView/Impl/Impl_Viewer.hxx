#ifndef DC_VIEW_IMPL_VIEWER_H
#define DC_VIEW_IMPL_VIEWER_H

#include "DCView/View.h"
#include <array>
#include <set>
//Boost
#include "boost/shared_ptr.hpp"

//Qt
#include "QObject"
#include "QTimer"

#include "DCCore/DCConfig.h"

class QTabWidget;

namespace DcGp
{
	class DcGpEntity;
	class DcGpPolyline;
}

namespace DCCore
{
	class Transform;
	class GUIEventHandle;
	
}

namespace DCView
	{
		//FBO
		class FrameBuffer;
		typedef boost::shared_ptr<FrameBuffer> FrameBufferPtr;

		class Camera;
		class GUIEventHandle;
		//�Ӵ�ʵ����
		class View::ViewerImpl : public QObject
		{
			Q_OBJECT

		public:
			//��ȡ��ǰ�Ӵ�
			static View* CurrentViewer();
			static void NullCurrentViewer();
			//��Ϊ��ǰ
			void SetCurrent();

			
			enum KeyboardAction 
			{ 
				DRAW_AXIS, 
				DRAW_GRID, 
				DRAW_OPT_HIDING, 
				HELP,
				PROJECTMODE,
				EYESHADER,
				SNAPSHOT_TO_CLIPBOARD
			};

			enum MouseHandler 
			{ 
				CAMERA, 
				FRAME 
			};

			enum ClickAction 
			{ NO_CLICK_ACTION, ZOOM_ON_PIXEL, ZOOM_TO_FIT, SELECT, RAP_FROM_PIXEL, RAP_IS_CENTER,
				CENTER_FRAME, CENTER_SCENE, SHOW_ENTIRE_SCENE, ALIGN_FRAME, ALIGN_CAMERA 
			};
			enum MouseAction { 
				NO_MOUSE_ACTION,
				ROTATE, ZOOM, 
				TRANSLATE,
				MOVE_FORWARD,
				LOOK_AROUND, 
				MOVE_BACKWARD,
				SCREEN_ROTATE,
				ROLL, DRIVE,
				SCREEN_TRANSLATE, 
				ZOOM_ON_REGION 
			};			

		private:
			//��ǰ�Ӵ�
			static View* s_pCurrentViewer;

		public:
			explicit ViewerImpl(View* pInterface);
			~ViewerImpl();

			//��ȡͶӰģʽ
			ProjectionMode GetProjection() const;
			//����ͶӰģʽ
			void SetProjection(ProjectionMode mode);

			//��Ļ���굽��������
			bool ScreenToWorld(const Point_2& point2D,
				Point_3& point3D);
			//�������굽��Ļ����
			bool WorldToScreen(const Point_3& point,
				Point_2& point2D);

			//��ʼ��OpenGL����
			void InitializeGL();
			//��С�仯
			void ResizeGL(int iWidth, int iHeight);
			//����OpenGL
			void Render();

			//! ���±���
			void UpdateTraversal();

			//�����
			void PressedMouse(QMouseEvent* event);
			//����ɿ�
			void ReleasedMouse(QMouseEvent* event);
			//����ƶ�
			void MovingMouse(QMouseEvent* event);
			//ת������
			void Wheeling(QWheelEvent* event);

			void KeyPress(QKeyEvent* event);
			void KeyRelease(QKeyEvent* event);
			
			//! ����gl
			void ReDraw();

			//ˢ��
			void Refresh();

			void DirtyRefresh();

			//������ͶӰ
			void EnableOrthoProjection();

			//������ת����
			void UpdatePivot(const Point_3& ptPivot);

			//�������Ĳ��ƶ����
			void UpdateCenterAndZoom(const View::BoundBox& box);

			//������ͼ
			void SetView(View::ViewMode mode);
			//������ͼ
			void ResetView();

			//����GLFilter
			void SetGLFilter(GLFilterPtr pFilter);
			//��ȡGLFilter
			GLFilterPtr GetGLFilter() const;

			//���ص�ǰ��ͼ�ķ���
			Point_3 GetCurrentViewDirection() const;

			//������Ļʵ�����ش�С
			float ComputeActualPixelSize();

			//��ȡ�Ӿ������������
			std::array<std::array<float, 4> ,6> GetFrustum();

			const ViewerParams& GetViewportParameters() const;

			//�����Ƿ�������ת����
			void SetApplyLODState(bool state);

			//! ������ά����ģʽ
			void SetInterActionMode(View::InterActionMode iaMode);

			//-----------------------------------InnerRoot[��Ҫ����������ʵ�������ѡȡ�����еĶ����]-----------------------------------------------

			//! ��ȡ�ڲ����ڵ�
			virtual DcGp::DcGpEntity* GetInnerRoot();

			//! ���ʵ�����innerRoot��
			virtual void AddtoInnerRoot(DcGp::DcGpEntity* obj2D, bool noDependency = true);

			//! �Ƴ�ָ���Ķ���
			virtual void RemoveFormInnerRoot(DcGp::DcGpEntity* obj2D);

		//----------------------------------------------------------------------------------
		
		//-----------------------------��Ļ���λ���������----------------------------
		public:
			//! ��ȡ���λ�õ�Զ�����������
			void Get3DRayUnderMouse(const Point_2& point2D, Point_3* v1, Point_3* v2);


			//! �������
			//��������Ч������
			void SetSunLightState(bool state);

			bool GetSunLightState();

			void DrawLightPos();
		
		//-----------------------------��̬֡����----------------------------
		public:
			void SetRefreshRate( int msec );
			int GetRefreshRate();

			void SetContinuousUpdate(bool continuous);


		//������Ϣ�ӿ�
		public:
			void Help();
			QString HelpString() const;
			QString MouseString() const;
			QString KeyboardString() const;

			void SetKeyDescription(int key, QString description);
		private:
			void SetDefaultShortcuts();
			void SetDefaultMouseBindings();
			void HandleKeyboardAction(KeyboardAction id);
		public slots:
			void SetShortcut(KeyboardAction action, int key);

		private:
			// M o u s e   a c t i o n s
			struct MouseActionPrivate {
				MouseHandler handler;
				MouseAction action;
				bool withConstraint;
			};

			// C l i c k   a c t i o n s
			struct ClickActionPrivate {
				Qt::KeyboardModifiers modifiers;
				Qt::MouseButtons button;
				bool doubleClick;
				Qt::MouseButtons buttonsBefore; // only defined when doubleClick is true

				// This sort order in used in mouseString() to displays sorted mouse bindings
				bool operator<(const ClickActionPrivate& cap) const
				{
					if (buttonsBefore != cap.buttonsBefore)
						return buttonsBefore < cap.buttonsBefore;
					else
						if (modifiers != cap.modifiers)
							return modifiers < cap.modifiers;
						else
							if (button != cap.button)
								return button < cap.button;
							else
								return !doubleClick && cap.doubleClick;
				}
			};

			QMap<int, MouseActionPrivate> m_mouseBinding;
			QMap<Qt::KeyboardModifiers, MouseActionPrivate> m_wheelBinding;
			QMap<ClickActionPrivate, ClickAction> m_clickBinding;
			QMap<ClickActionPrivate, QString> m_mouseDescription;

		private:
			static QString MouseActionString(MouseAction ma);
			static QString ClickActionString(ClickAction ca);
			static QString FormatClickActionPrivate(ClickActionPrivate cap);
		public slots:
			void SetMouseBinding(int state, MouseHandler handler, MouseAction action, bool withConstraint=true);
			void SetMouseBinding(int state, ClickAction action, bool doubleClick=false, Qt::MouseButtons buttonsBefore=Qt::NoButton);
			void SetWheelBinding(Qt::KeyboardModifiers modifiers, MouseHandler handler, MouseAction action, bool withConstraint=true);
			void SetMouseBindingDescription(int state, QString description, bool doubleClick=false, Qt::MouseButtons buttonsBefore=Qt::NoButton);

		public:
			void DisplayMessage(const QString& message, int delay=2000);
			void DrawText(int x, int y, const QString& text, const QFont& fnt=QFont());
		private slots:
			void HideMessage();
		private:
			//��Ļ��ʾ��Ϣ
			QString m_message;
			bool m_isDisplayMessage;
			QTimer m_messageTimer;

			//������ʾ
		public:
			void DrawGrid(float size=1.0f, int nbSubdivisions=10);
			bool IsGridDrawn() const { return m_isGridDrawn; };
			void SetGridDrawnState(bool state);
		private:
			bool m_isGridDrawn;

			//��Ļ����
		public:
			void SnapshotToClipboard();
		//����
		private:
			QImage FrameBufferSnapshot();

			//��������
		public slots:
			void FitScreenRegion(const QRect& rectangle);
		private:
			void DrawRectangleZoom();
		private:
			QPoint m_prevPos; 
			QPoint m_pressPos;

			MouseAction m_action;

		//pick
		public Q_SLOTS:
			virtual void Pick(const QMouseEvent* event);
			virtual void Pick(const QPoint& point);

		//hud�ӿ�
		public:
			void SetHudtext(QString text);

		private:
			QString m_hudText;
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
			
		public:
			//ʹͶӰ������Ч
			void DisableProjection();
			//ʹģ����ͼ��Ч
			void DisableModelView();

			//������귽��
			Vector_3 MouseOrientation(unsigned iX, unsigned iY);

			//�ƶ����
			void MoveCamera(const Vector_3& v);

			//! ���һ��������
			void AddManipulatorHandle(DCCore::GUIEventHandle* manipulator);

			//��ת��ͼ
			void RotateView(const Matrix_4_4& matrix);

			//������ת����
			void EnablePivot(bool bState);

			//ת������
			void RotatingWheel(float fDegree);

			//! ��ȡ�Ӵ�������camera
			Camera* GetCamera() const;

			MatrixPiple GetMatrixPiple() const;

			void SetMatrixPiple(MatrixPiple mpType);

			//�������λ��
			void UpdateCameraPos(const Point_3& pos);

			void UpdateRotateView(const Matrix_4_4& rm);

			//! ��ǰ���°�������
			const std::set<Qt::Key>& Keyboard() const { return m_Keyboard; }

			//! ָ�������Ƿ���
			bool IsKeyPressed(Qt::Key key) const { return m_Keyboard.find(key) != m_Keyboard.end(); }

			void UpdateFps(float deltaTime);

			//����
			void Scale(float fScale);


			
		private:
			//����OpenGL��չ
			void LoadGLExtensions();

			//����GLFilter
			bool UpdateGLFilter(unsigned uWidth, unsigned uHeight);

			//����FBO
			bool UpdateFBO(unsigned uWidth, unsigned uHeight);
			//��ȾFBO
			void RenderFBO();

			//����Render
			void UpdateRender();
			//��Ⱦ����
			void RenderBackground();

			//���¼���ͶӰ����
			void RecalculateProjectionMatrix();
			//���¼���ģ����ͼ����
			void RecalculateModelViewMatrix();
			//����ͶӰ����
			void LoadProjectionMatrix();
			//����ģ����ͼ����
			void LoadModelViewMatrix();

			//���¼���ͶӰ����
			void RecalculateProjectionMatrixByCamera();
			//���¼���ģ����ͼ����
			void RecalculateModelViewMatrixByCamera();

			
			//������������
			void SetScale(float fScale);
			
			//����͸��ͶӰ���ű���
			float ComputePerspectiveZoom() const;

			//����ػ�
			void MarkRefreshing();

			

			//����������
			void RenderAxises();
			//��������
			void DrawAxis();
			//������ת����
			void RenderPivot();

			//����̫����
			void EnableSunLight();
			//�ر�̫����
			void DisableSunLight();
			//�����Զ����
			void EnableCustomLight();
			//�ر��Զ����
			void DisableCustomLight();

			//���������Ӿ������
			void CalculateFrustum();

			void NormalizePlane(std::array<std::array<float, 4> ,6> frustum, int side);
		private:
			//�ӿ�
			View* m_pInterface;

			//OpenGL������С
			GLuint m_glWidth;
			GLuint m_glHeight;

			//View����
			ViewerParams m_params;

			//GLFilter
			GLFilterPtr m_pGLFilter;

			//�Ƿ�����̫����
			bool m_bEnableSunLight;
			//̫����λ��
			float m_sunLightPos[4];
			//�Ƿ������Զ����
			bool m_bEnableCustomLight;
			//�Զ����λ��
			float m_customLightPos[4];

			//�Ƿ�ʹ��FBO
			bool m_bUsingFBO;
			//FBO
			FrameBufferPtr m_pFBO;
			//����FBO
			bool m_bUpdatedFBO;

			//�Ƿ���ʾ��ת����
			bool m_bPivotShown;

			//! ����״̬
			View::InterActionMode m_interActionMode;

			//�Ƿ����lod����ά���
			bool m_isApplyLodForTransfrom;

			//ͶӰ�����Ƿ���Ч
			bool m_bValidProjection;
			//ģ����ͼ�����Ƿ���Ч
			bool m_bValidModelView;

			//����Ƿ����ƶ���
			bool m_bMouseMoving;
			//�����һ�ε�λ��
			QPoint m_lastPosOfMouse;
			//�����һ�εķ���
			Vector_3 m_lastOrientationOfMouse;
			//��굱ǰ����
			Vector_3 m_currentOrientationOfMouse;

			//�Ƿ����ػ�
			bool m_bRefreshMarked;

			//���������ʾ�б�
			GLuint m_axisListID;
			//��ת������ʾ�б�
			GLuint m_pivotListID;

			//��ʼ״̬�±߽����camera����
			float m_initdist_box_camer;

			//�Ӿ������������
			std::array<std::array<float, 4> ,6> m_Frustum;

			//help widget
			QTabWidget* m_helpWidget;

			// S h o r t c u t   k e y s
			QMap<KeyboardAction, QString> m_keyboardActionDescription;
			QMap<KeyboardAction, int> m_keyboardBinding;
			QMap<int, QString> m_keyDescription;

			//�ڲ�����������Ľڵ�
			DcGp::DcGpEntity* m_innerRoot;

			std::vector<Point_3 > m_rayPoints;

			//! ������
			std::vector<DCCore::GUIEventHandle* > m_manipulatorHandles;

			//! camera--����camera�࣬�����������gl����
			MatrixPiple m_matrixPiple;

			DCCore::Transform* m_transForm;
			Camera* m_camera;

			std::set<Qt::Key> m_Keyboard;

			//! ���ڶ�̬֡���ƿ���
			int    m_refreshRate;
			QTimer m_updateTimer;

			bool m_continuousUpdate;

			//!֡����
			float m_cfps;
		};
	}

#endif