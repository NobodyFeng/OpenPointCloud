#include "Viewer.h"

#include <thread>

//Qt
#include "QApplication"
#include "QGridLayout"
#include "QLinearGradient"
#include "QMouseEvent"
#include "QToolTip"
#include "QDebug"
#include "QThreadPool"

//DCCore
#include "DCCore/Logger.h"
#include "DCCore/GUIEventHandle.h"
#include "DCCore/IoPlugin.h"
#include "DCCore/IoInterface.h"
#include "DCCore/DCConfig.h"

//View
#include "DCView/Parameters.h"
#include "DCView/GLToolkit.h"



//Console
//#include "DCConsole/Console.h"

//DcGp
#include "DCGp/GpEntity.h"
#include "DCGp/Custom/GpBoundBox.h"
#include "DCGp/Custom/GpPointCloud.h"
#include "DCGp/Custom/GpMesh.h"
#include "DCGp/Custom/GpDEM.h"
#include "DCGp/Custom/DC2DLabel.h"



//Self
#include "ViewRenderingTool.h"

#include "DCUtil/UpdateVisitor.h"

static int s_dcViewerNum = 0;

using namespace MPViewer;

Viewer::Viewer(QWidget* pParent /* = nullptr */, 
	const QGLFormat& format /* =QGLFormat::defaultFormat() */, 
	QGLWidget* pSharedWidget /* = nullptr */)
	: DCView::View(pParent, format, pSharedWidget)
	, m_pSceneRoot(nullptr)
	, m_bUpdatingPivot(false)
	, m_useSelectBoundbox(false)
	//, m_isDrawPickedPoint(false)
	, m_catcher(new MPUtil::Catcher)
	, m_unitMan(nullptr)
	//, m_isGetLeftPressedPoint(false)
{
	Init();
}

Viewer::Viewer(QString fileName)
	: DCView::View(0, QGLFormat::defaultFormat(), 0)
	, m_pSceneRoot(nullptr)
	, m_bUpdatingPivot(false)
	, m_useSelectBoundbox(false)
	, m_catcher(new MPUtil::Catcher)
	, m_unitMan(nullptr)
{
	Init();

	QString extension = QFileInfo(fileName).suffix();
	//������չ��������IO�ļ����еĲ����ѡ����ʵ�io�����������ļ�
	QDir dir(DCCore::GetIOPluginPath());

	DCCore::IoPlugin* currentIo = 0;

	bool hasPlugin = DCCore::GetIOPlugin(dir, extension, currentIo);
	if (!hasPlugin || !currentIo)
	{
		QApplication::restoreOverrideCursor();
		return;
	}

	double coordinatesShift[3] = { 0,0,0 };
	DcGp::DcGpEntity* entity = currentIo->LoadFile(fileName, 0, true, 0, coordinatesShift, nullptr);

	if (entity)
		SetSceneRoot(entity);
}

void Viewer::Init()
{
	m_unitMan = new UnitManager(this);

	++s_dcViewerNum;
	setWindowTitle(QString("Viewer %1").arg(s_dcViewerNum));

	setMouseTracking(true);
	setAcceptDrops(true);

	setMinimumSize(400, 300);

	//�����źŲ۵�����
	CreateConnections();

	//! ���þ��������
	SetMatrixPiple(DCView::eOpenGL);

	//���ô���ı���(���д���)
	{
		QLinearGradient gradient;
		QColor back(138, 138, 138);
		gradient.setColorAt(0, back);
		gradient.setColorAt(1, Qt::black);

		DCView::GlobalParameters::SetBackground(gradient);
	}

	//DCView::GlobalParameters::SetCompositeKey(DCView::GlobalParameters::eRotateOperation,
	//	Qt::MidButton, Qt::AltModifier);

	//pick
	DCView::GlobalParameters::SetCompositeKey(DCView::GlobalParameters::ePicking,
		Qt::LeftButton, Qt::ShiftModifier);

	//�����Ƿ�����lod
	SetApplyLODState(true);

	//���ü��̽���
	setFocusPolicy(Qt::StrongFocus);

	
}

void Viewer::closeEvent(QCloseEvent* event)
{
	View::NullCurrentViewer();
	event->accept();
}

//��������
Viewer::~Viewer()
{
	//�����������ʵ�崰��Ϊnull
	if (m_pSceneRoot)
	{
		m_pSceneRoot->LinkWindow(nullptr);
		emit RemoveRootEntity(m_pSceneRoot);
	}

	//ɾ����׽��
	if (m_catcher)
	{
		delete m_catcher;
		m_catcher = nullptr;

	}

	//��������е��¼�������
	while(!m_eventHandles.empty())
	{
		DCCore::GUIEventHandle* ehandle = m_eventHandles.back();
		m_eventHandles.pop_back();

		delete ehandle;
		ehandle = nullptr;
	}
	//m_eventHandles.clear();

	if (m_unitMan)
	{
		delete m_unitMan;
		m_unitMan = nullptr;
	}
}

//�����źŲ۵�����
void Viewer::CreateConnections()
{
	//CAD�Ӵ��ľ���任
	connect(this, SIGNAL(ProjectionMatrixChanged(const double*)),
		this, SLOT(ChangeProjectionMatrix(const double*)));
	connect(this, SIGNAL(ModelViewMatrixChanged(const double*)),
		this, SLOT(ChangeModelViewMatrix(const double*)));
	connect(this, SIGNAL(ViewportMatrixChanged(const int*)),
		this, SLOT(ChangeViewportMatrix(const int*)));

	//��ǰ���ű��������任
	connect(this, SIGNAL(CurrentScaleChanged(double)),
		this, SLOT(ChangedCurrentScale(double)));
}

//���ͼԪ
void Viewer::SetSceneRoot(DcGp::DcGpEntity* pEntity)
{
	m_pSceneRoot = pEntity;

	if (m_pSceneRoot)
	{
		//���ӵ��˴���
		m_pSceneRoot->LinkWindow(this);

		UpdateBoundingBox();
	}
}

// �����Ƿ����ѡ���������
// void Viewer::SetDrawPickedPointState(bool state)
// {
// 	if (!state)
// 	{
// 		m_cursorText = "";
// 	}
// 	m_isDrawPickedPoint = state;
// }

//��ȡͼԪ
DcGp::DcGpEntity* Viewer::GetSceneRoot() const
{
	return m_pSceneRoot;
}

//���·�Χ��
void Viewer::UpdateBoundingBox()
{
	UpdateCenterAndZoom(BoundingBox());
}

Viewer::BoundBox Viewer::BoundingBox() const
{
	DcGp::DcGpBoundBox box;

	if (m_useSelectBoundbox && m_pSceneRoot)
	{
		return m_selectBoundbox;
	}
	else if(!m_useSelectBoundbox && m_pSceneRoot)
	{

		box = m_pSceneRoot->GetBoundBox();

		BoundBox bbox;
		bbox.center = box.GetCenter();
		bbox.minCorner = box.MinCorner();
		bbox.maxCorner = box.MaxCorner();

		return bbox;
	}
	else
	{
		BoundBox bbox;
		return bbox;
	}
}

void Viewer::Resize(int iWidth, int iHeight)
{
	m_glWidth = iWidth;
	m_glHeight = iHeight;

	UpdateBoundingBox();
}

void Viewer::Paint2D()
{
	makeCurrent();

	DcGp::DcGpDrawContext context = GetContext();
	context.flags = DC_DRAW_2D | DC_DRAW_FOREGROUND;

	if (m_pSceneRoot)
	{
		m_pSceneRoot->Draw(context);
	}

	if (GetInnerRoot())
	{
		context._win = nullptr;
		GetInnerRoot()->Draw(context);
	}

	DrawSnapSymbol();

	//������ɫ��
	context._win = this;
	ViewRenderingTool::DrawColorBar(context);
}

void Viewer::DrawSnapSymbol()
{
	//! ���ƹ�괦�ı�ֵ
	QFont serifFont("Times", 14, QFont::Bold);
	glPushAttrib(GL_COLOR_BUFFER_BIT);
	glColor3f(0,1,1);
	DrawText(m_cursorPos.x(), m_cursorPos.y(), m_cursorText, serifFont);
	glPopAttrib();


	// ����m_cursorText����������ά�������꣬ת������Ļ���꣬�����ı���
	QStringList list = m_pickedWorldPoint.split(",");
	if (list.size() < 3)
	{
		return;
	}
	DCView::Point_3 ptw(list[0].toFloat(), list[1].toFloat(), list[2].toFloat());
	DCView::Point_2 pts;
	WorldToScreen(ptw, pts);

	//��׽��뾶��С
	int rectRadius = 4; // 1.0 / GetViewportParameters().pixeSize;

	GLint pcik_vertex_buffer_data[] = { 
		pts.x - rectRadius, pts.y - rectRadius,
		pts.x + rectRadius, pts.y - rectRadius,
		pts.x + rectRadius, pts.y + rectRadius,
		pts.x - rectRadius, pts.y + rectRadius
	};

	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glEnableClientState(GL_VERTEX_ARRAY);

	glVertexPointer(2, GL_INT, 0, &pcik_vertex_buffer_data[0]);

	glEnable(GL_LINE_SMOOTH);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glLineWidth(2);
	glColor3f(0, 1.0, 0);
	glDrawArrays(GL_QUADS, 0, 4);   // 1(ͼԪ����) * 4(GL_QUADS��Ӧ�Ķ�����) = 4�����������

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisable(GL_LINE_SMOOTH);
	glPopAttrib();
}

void Viewer::FastDraw3D()
{
	makeCurrent();

	DcGp::DcGpDrawContext context = GetContext();
	context.flags = DC_DRAW_3D | DC_DRAW_FOREGROUND
		| DC_LIGHT_ENABLED;

	if (m_pSceneRoot)
	{
		m_pSceneRoot->FastDraw3D(context);
	}
}

void Viewer::Paint3D()
{
	makeCurrent();

	DcGp::DcGpDrawContext context = GetContext();
	context.flags = DC_DRAW_3D | DC_DRAW_FOREGROUND
		| DC_LIGHT_ENABLED;

	if (m_pSceneRoot)
	{
		m_pSceneRoot->Draw(context);
	}

	if (GetInnerRoot())
	{
		context._win = nullptr;
		GetInnerRoot()->Draw(context);
	}

	context._win = this;
}

//
DcGp::DcGpDrawContext Viewer::GetContext()
{
	DcGp::DcGpDrawContext context;
	context._win = this;
	context.flags = 0;

	//���ڴ�С
	context.glW = m_glWidth;
	context.glH = m_glHeight;

	//����Ĭ����ɫ
	context.pointsDefaultCol[0] = 255;
	context.pointsDefaultCol[1] = 255;
	context.pointsDefaultCol[2] = 255;

	//Ĭ�ϲ��ʲ���
	context.defaultMat.SetName("default");
	context.defaultMat.SetDiffuseFront(DcGp::Color::DEFAULTMESHFRONTDIFFUSE);
	context.defaultMat.SetDiffuseBack(DcGp::Color::DEFAULTMESHBACKDIFFUSE);
	context.defaultMat.SetAmbient(DcGp::Color::BRIGHT);
	context.defaultMat.SetSpecular(DcGp::Color::MIDDLE);
	context.defaultMat.SetEmission(DcGp::Color::NIGHT);
	context.defaultMat.SetShininessFront(30);
	context.defaultMat.SetShininessBack(50);

	//boxColor
	context.bbDefaultCol[0] = DCView::GlobalParameters::BoundBoxColor()[0];
	context.bbDefaultCol[1] = DCView::GlobalParameters::BoundBoxColor()[1];
	context.bbDefaultCol[2] = DCView::GlobalParameters::BoundBoxColor()[2];

	return context;
}

//�ı�ͶӰ����
void Viewer::ChangeProjectionMatrix(const double* pMatrix)
{
	memcpy(m_projectionMatrix, pMatrix, sizeof(double) * 16);
}
//�ı�ģ����ͼ����
void Viewer::ChangeModelViewMatrix(const double* pMatrix)
{
	memcpy(m_modelViewMatrix, pMatrix, sizeof(double) * 16);
}
//�ı��ӿھ���
void Viewer::ChangeViewportMatrix(const int* pMatrix)
{
	
}

//��갴��
void Viewer::mousePressEvent(QMouseEvent* event)
{
	for (int i = 0; i != m_eventHandles.size(); ++i)
	{
		if (m_eventHandles[i]->GetValid())
		{
			m_eventHandles[i]->mousePressEvent(event, this);
		}
	}

	//�����̬������ת���ģ���ӵ�ǰ��׽���в�׽���ĵ������ת����
	if (m_bUpdatingPivot)
	{
		//! ͨ��������źţ�����m_cursorText��ֵ
		m_catcher->SetMask(MPUtil::eVisible | MPUtil::ePointCloud);
		MPUtil::HitList hits = GenHitsByEvent(event);
		if (!hits.empty())
		{
			UpdatePivot(hits[0].GetWordPoint());
			Refresh();
		}
	}

	if (event->buttons() & Qt::LeftButton)
	{
		//ȷ���Ƿ����ѡ��ʵ��(Ctrl + ���)
		if (QApplication::keyboardModifiers() == Qt::ControlModifier )
		{
			//QApplication::setOverrideCursor(QCursor(Qt::PointingHandCursor));
			//UpdateActiveItemsList(event->x(), event->y(), true);
			//! ʵ�岶׽
			m_catcher->SetMask(MPUtil::eVisible | MPUtil::eEntity);
			/*m_catcher->SetModelViewMatrix(GetViewportParameters().matrixModelView);*/

			MPUtil::HitList hits = GenHitsByEvent(event);
			if (!hits.empty())
			{
				emit EntityPicked(hits[0].GetEntity());
			}

		}
		//UpdateTraversal();
	}

	//���ø���
	DCView::View::mousePressEvent(event);
}

void Viewer::mouseReleaseEvent(QMouseEvent* event)
{
	for (int i = 0; i != m_eventHandles.size(); ++i)
	{
		if (m_eventHandles[i]->GetValid())
		{
			m_eventHandles[i]->mouseReleaseEvent(event, this);
		}
	}

	//���ø���
	DCView::View::mouseReleaseEvent(event);

	//Refresh();
}

void Viewer::mouseDoubleClickEvent(QMouseEvent* event)
{
	for (int i = 0; i != m_eventHandles.size(); ++i)
	{
		if (m_eventHandles[i]->GetValid())
		{
			m_eventHandles[i]->mouseDoubleClickEvent(event, this);
		}
	}

	//���ø���
	//DCView::View::mouseDoubleClickEvent(event);

	Refresh();
}

MPUtil::Catcher* Viewer::GetCatcher()
{
	return m_catcher;
}

void Viewer::SetPickedWorldPoint(QString pickedWorldPoint)
{
	m_pickedWorldPoint = pickedWorldPoint;
}

void Viewer::SetCursorText(QString cursorText)
{
	m_cursorText = cursorText;
}

MPUtil::HitList Viewer::GenHitsByEvent(QMouseEvent* event)
{
	DCView::Point_2 pt(event->x(), event->y());

	//! ��¼���λ��
	m_cursorPos = QPoint(event->x(), event->y());

	//! ��ȡ��������
	DCView::Point_3 pt1;
	DCView::Point_3 pt2;
	Get3DRayUnderMouse(pt, &pt1, &pt2);

	//! ����MPUtil���߼�������hits
	//! ����һ��PointPicking����ִ�в�׽
	MPUtil::HitList hits;
	m_catcher->SetPixelSize(ComputeActualPixelSize());
	m_catcher->ComputeIntersections(m_pSceneRoot, pt1, pt2, hits);


	emit RayHits(hits);

	return hits;
}

//����ƶ�
void Viewer::mouseMoveEvent(QMouseEvent* event)
{
	for (int i = 0; i != m_eventHandles.size(); ++i)
	{
		if (m_eventHandles[i]->GetValid())
		{
			m_eventHandles[i]->mouseMoveEvent(event, this);
		}
	}

	//���ø���
	DCView::View::mouseMoveEvent(event);

	//��ʵ�岶׽
	Pick(eEntityPicking, DCCore::Point_2(event->pos().x(), event->pos().y()));
	
	//ˢ��
	//Refresh();
}

//���̰���
void Viewer::keyPressEvent(QKeyEvent* event)
{
	for (int i = 0; i != m_eventHandles.size(); ++i)
	{
		if (m_eventHandles[i]->GetValid())
		{
			m_eventHandles[i]->keyPressEvent(event, this);
		}
	}

	if (event->key() == Qt::Key_Escape)
	{
	}
	//ɾ��
	else if (event->key() == Qt::Key_Delete)
	{
		//����̨
		/*DCConsole::Console* pConsole = DCConsole::Console::Instance();

		pConsole->ExecuteCommand("DELETE");*/
	}

	DCView::View::keyPressEvent(event);
}

bool Viewer::IsRCenterPicked(const DCCore::Point_2& point2D, DCCore::Point_3& point3D)
{
	//ת���Ƿ�ɹ�
	bool bState = false;
	{
		//�����
		int subSelectedID;

		//�����׽�������Ҷ���Ϊ���ƣ���ӵ����л�ȡ����
		DcGp::DcGpEntity* pEntity = nullptr;
		int selectedID = Pick(ePointPicking, point2D, &subSelectedID, 5, 5, true/* ����lodģ�� */);

		if (selectedID >= 0)
		{
			DcGp::Container vEntity = m_pSceneRoot->GetChildren();

			auto result = std::find_if(vEntity.begin(), vEntity.end(), 
				[selectedID](DcGp::DcGpEntity* pEntity)->bool 
			{return (pEntity->GetUniqueID() == selectedID);});

			if (result != vEntity.end())
			{
				pEntity = *result;
			}
		}
		if (pEntity)
		{
			if (subSelectedID >= 0)
			{
				//����
				if (pEntity->IsA("DcGpPointCloud"))
				{
					DcGp::DcGpPointCloud* pCloud = static_cast<DcGp::DcGpPointCloud*>(pEntity);
					point3D = pCloud->GetPoint(subSelectedID);

					bState = true;
				}
				//Mesh
				else if (pEntity->IsA("DcGpMesh"))
				{
					DcGp::DcGpMesh* mesh = static_cast<DcGp::DcGpMesh*>(pEntity);
					//��ȡ��ǰѡ��������
					DcGp::Link link = mesh->GetLink(subSelectedID);
					point3D = mesh->GetPoint(link[0]);

					bState = true;
				}
				if (pEntity->IsA("DcGpDEM"))
				{
					DcGp::DcGpDEM* dem = static_cast<DcGp::DcGpDEM*>(pEntity);
					//��ȡ��ǰѡ��dem
					std::array<unsigned, 4> link = dem->GetLink(subSelectedID);
					DCVector3D point = dem->GetVertex(link[0]);
					point3D = DCVector3D(point.x, point.y, point.z);

					bState = true;
				}
			}
		}
		
	}

	return bState;
}

//��Ļ����תΪ��������
bool Viewer::ScreenToWorld(const DCCore::Point_2& point2D,
	DCCore::Point_3& point3D)
{
	//ת���Ƿ�ɹ�
	bool bState = false;

	//���ж��Ƿ�׽������
	int selectedID = Pick(eElementPicking, point2D);

	//�����׽�������Ҷ���Ϊ���ƣ���ӵ����л�ȡ����
	DcGp::DcGpEntity* pEntity = nullptr;
	if (selectedID >= 0)
	{
		DcGp::Container vEntity = m_pSceneRoot->GetChildren();

		auto result = std::find_if(vEntity.begin(), vEntity.end(), 
			[selectedID](DcGp::DcGpEntity* pEntity)->bool 
		{return (pEntity->GetUniqueID() == selectedID);});

		if (result != vEntity.end())
		{
			pEntity = *result;
		}
	}

	//���ʵ�����
	if (pEntity)
	{
		//�����
		int subSelectedID;
		Pick(ePointPicking, point2D, &subSelectedID, 5, 5);
		if (subSelectedID >= 0)
		{
			//����
			if (pEntity->IsA("DcGpPointCloud"))
			{
				DcGp::DcGpPointCloud* pCloud = static_cast<DcGp::DcGpPointCloud*>(pEntity);
				point3D = pCloud->GetPoint(subSelectedID);

				bState = true;
			}
			//Mesh
			else if (pEntity->IsA("DcGpMesh"))
			{
				DcGp::DcGpMesh* mesh = static_cast<DcGp::DcGpMesh*>(pEntity);
				//��ȡ��ǰѡ��������
				DcGp::Link link = mesh->GetLink(subSelectedID);
				point3D = mesh->GetPoint(link[0]);

				bState = true;
			}
			if (pEntity->IsA("DcGpDEM"))
			{
				DcGp::DcGpDEM* dem = static_cast<DcGp::DcGpDEM*>(pEntity);
				//��ȡ��ǰѡ��dem
				std::array<unsigned, 4> link = dem->GetLink(subSelectedID);
				DCVector3D point = dem->GetVertex(link[0]);
				point3D = DCVector3D(point.x, point.y, point.z);

				bState = true;
			}
		}
	}

	if (!bState)
	{
		bState = DCView::View::ScreenToWorld(point2D, point3D);
	}

	return bState;
}

//��׽
int Viewer::Pick(PickingMode mode, const DCCore::Point_2& point, 
	int* subID /* = nullptr */, int width /* = 5 */, int height /* = 5 */, bool drawSample/* = false*/)
{
	if (subID)
		*subID = -1;

	if (!m_pSceneRoot)
		return -1;

	DcGp::DcGpDrawContext context = GetContext();
	
	unsigned short pickingFlags = DC_DRAW_FOREGROUND;

	switch(mode)
	{
	case eEntityPicking:
	case eEntityRectPicking:
		pickingFlags |= DC_DRAW_ENTITY_NAMES;
		break;
	case eFastPicking:
		pickingFlags |= DC_DRAW_ENTITY_NAMES;
		pickingFlags |= DC_DRAW_FAST_NAMES_ONLY;
		break;
	case ePointPicking:
	case eDoubleClickedPicking:
		pickingFlags |= DC_DRAW_POINT_NAMES;	//automatically push entity names as well!
		break;
	case eTrianglePicking:
		pickingFlags |= DC_DRAW_TRI_NAMES;		//automatically push entity names as well!
		break;
	case eAutoPointPicking:
		pickingFlags |= DC_DRAW_POINT_NAMES;	//shift���ʱ����ٲ�׽!
		pickingFlags |= DC_DRAW_TRI_NAMES;
		break;
	case eElementPicking:
		pickingFlags |= DC_DRAW_POINT_NAMES;	//��eAutoPointPicking���ƣ����������ָ�����ת����ʱ�����eElementPicking
		pickingFlags |= DC_DRAW_TRI_NAMES;
	case eDefaultPicking:
		pickingFlags |= DC_DRAW_POINT_NAMES;
		break;
	default:
		return -1;
	}

	makeCurrent();

	//ѡȡ����
	memset(m_pickBuffer, 0, sizeof(GLuint) * s_c_pickBufferSize);
	glSelectBuffer(s_c_pickBufferSize, m_pickBuffer);
	glRenderMode(GL_SELECT);
	glInitNames();

	//�ӿ�
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	//��άʵ��ѡȡ
	{
		context.flags = DC_DRAW_3D | pickingFlags;

		glEnable(GL_DEPTH_TEST);

		//ͶӰ����
		glMatrixMode(GL_PROJECTION);

		glLoadIdentity();
		gluPickMatrix((GLdouble)point.x,(GLdouble)(viewport[3]-point.y),(GLdouble)width,(GLdouble)height,viewport);
		glMultMatrixd(m_projectionMatrix);

		//ģ����ͼ����
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixd(m_modelViewMatrix);

		if (m_pSceneRoot)
		{
			m_pSceneRoot->DrawWithNames(context, drawSample);
		}
			
	}

	//��άѡȡ
	//if (mode == eEntityPicking || mode == eEntityRectPicking || mode == eFastPicking)
	//{
	//	context.flags = DC_DRAW_2D | pickingFlags;

	//	glDisable(GL_DEPTH_TEST);

	//	//������άƽ��ͶӰ
	//	EnableOrthoProjection();
	//	glMatrixMode(GL_PROJECTION);
	//	double orthoProjMatd[DcGp::OPENGL_MATRIX_SIZE];
	//	glGetDoublev(GL_PROJECTION_MATRIX, orthoProjMatd);

	//	glLoadIdentity();
	//	gluPickMatrix((GLdouble)point.x,(GLdouble)(viewport[3]-point.y),(GLdouble)width,(GLdouble)height,viewport);
	//	glMultMatrixd(orthoProjMatd);
	//	glMatrixMode(GL_MODELVIEW);

	//	if (m_pSceneRoot)
	//		m_pSceneRoot->Draw(context);

	//}

	glFlush();

	// ��Ⱦģʽ
	int hits = glRenderMode(GL_RENDER);
	if (hits<0)
	{
		DCCore::Logger::Warning(tr("Too many items inside picking zone! Try to zoom in..."));
		return -1;
	}

	//������
	int selectedID = -1,subSelectedID = -1;
	std::set<int> selectedIDs; //ֻ��ʵ��ѡ������
	{
		GLuint minMinDepth = (~0);
		const GLuint* _selectBuf = m_pickBuffer;
		for (int i=0;i<hits;++i)
		{
			const GLuint& n = _selectBuf[0]; //number of names on stack
			if (n) //if we draw anything outside of 'glPushName()... glPopName()' then it will appear here with as an empty set!
			{
				//n should be esqual to 1 (CC_DRAW_ENTITY_NAMES mode) or 2 (CC_DRAW_POINT_NAMES/CC_DRAW_TRIANGLES_NAMES modes)!
				assert(n==1 || n==2);
				const GLuint& minDepth = _selectBuf[1];
				//const GLuint& maxDepth = _selectBuf[2];
				const GLuint& currentID = _selectBuf[3];

				if (mode == eEntityRectPicking)
				{
					//pick them all!
					selectedIDs.insert(currentID);
				}
				else
				{
					//if there are multiple hits, we keep only the nearest
					if (selectedID < 0 || minDepth < minMinDepth)
					{
						selectedID = currentID;
						subSelectedID = (n>1 ? _selectBuf[4] : -1);
						minMinDepth = minDepth;
					}
				}
			}

			_selectBuf += (3+n);
		}
	}

	if (subID)
		*subID = subSelectedID;

	//����ʵ��ѡ��
	if (mode == eEntityPicking)
	{
		if (m_pSceneRoot)
		{
			//����ѡȡ���Ķ���
			DcGp::DcGpEntity* pEntity = m_pSceneRoot->Find(selectedID);

			//��������ڣ����Ͷ���ѡ�е��ź�
			if (pEntity && pEntity->GetClassname() == "IntersectionLine")
			{
				if (pEntity->IsEnabled() && pEntity->IsVisible())
				{
					//emit EntityPicked(pEntity);

					//��������ʾ��Ϣ������ʾ��ʾ��Ϣ
					if (!pEntity->GetToolTip().isEmpty())
					{
						QToolTip::showText(QCursor::pos(), pEntity->GetToolTip());
					}
				}
				
			}
		}
	}
	//rectangular "entity" picking
	else if (mode == eEntityRectPicking)
	{
		//emit entitiesSelectionChanged(selectedIDs);
	}
	//ʰȡ����
	else if (mode == ePointPicking)
	{
		if (selectedID >= 0 && subSelectedID >= 0)
		{
			//emit pointPicked(selectedID,(unsigned)subSelectedID,centerX,centerY);
			//��ȡʰȡ���Ķ���
			DcGp::DcGpEntity* pEntity = m_pSceneRoot->Find(selectedID);

			//��ȡ�����е�ָ��������
			if (pEntity->IsA("DcGpPointCloud"))
			{
				DCCore::Point_3 point = dynamic_cast<DcGp::DcGpPointCloud*>(pEntity)->GetPoint(subSelectedID);

				//����ʰȡ���ĵ�
				emit PointPicked(point);
			}
		}
	}
	else if (mode == eAutoPointPicking && (QApplication::keyboardModifiers() & Qt::ShiftModifier))
	{
		{
			if (m_pSceneRoot && selectedID>=0 && subSelectedID>=0)
			{
				DcGp::DcGpEntity* obj =m_pSceneRoot->Find(selectedID);
				if (obj)
				{
					//auto spawn the right label
					DcGp::DC2DLabel* label = 0;
					if (obj->IsKindOf("DcGpPointCloud"))
					{
						label = new DcGp::DC2DLabel();
						label->addPoint(static_cast<DcGp::DcGpPointCloud*>(obj),subSelectedID);
						obj->AddChild(label,true);
					}
					else if (obj->IsKindOf("DcGpMesh"))
					{
						label = new DcGp::DC2DLabel();
						DcGp::DcGpMesh *mesh = static_cast<DcGp::DcGpMesh*>(obj);
						std::vector<DCVector3D> points = mesh->GetVertexs();
						DcGp::DcGpPointCloud* cloud = new DcGp::DcGpPointCloud("name");
						cloud->SetPoints(points);
						cloud->SetShift(mesh->GetShift());

						DcGp::Link summitsIndexes = mesh->GetLink(subSelectedID);
						mesh->GetLinks();
						label->addPoint(cloud,summitsIndexes[0]);
						label->addPoint(cloud,summitsIndexes[1]);
						label->addPoint(cloud,summitsIndexes[2]);
						mesh->AddChild(label,true);
						if (!cloud->IsEnabled())
						{
							cloud->SetVisible(false);
							cloud->SetEnabled(true);
						}
					}
					else if (obj->IsKindOf("DcGpDEM"))
					{
						label = new DcGp::DC2DLabel();
						DcGp::DcGpDEM* dem = static_cast<DcGp::DcGpDEM*>(obj);
						std::vector<DCVector3D> points = dem->GetVertexs();
						DcGp::DcGpPointCloud* cloud = new DcGp::DcGpPointCloud("name");
						cloud->SetPoints(points);
						cloud->SetShift(dem->GetShift());

						std::array<unsigned, 4> summitsIndexes = dem->GetLink(subSelectedID);
						label->addPoint(cloud,summitsIndexes[0]);
						label->addPoint(cloud,summitsIndexes[1]);
						label->addPoint(cloud,summitsIndexes[2]);
						label->addPoint(cloud,summitsIndexes[3]);
						dem->AddChild(label,true);
						if (!cloud->IsEnabled())
						{
							cloud->SetVisible(false);
							cloud->SetEnabled(true);
						}
					}

					if (label)
					{
						label->SetVisible(true);
						//label->SetDisplay(obj->getDisplay());
						label->setPosition((float)(point.x + 20) / (float)QWidget::width(),
							(float)(point.y + 20) / (float)QWidget::height());
						emit EntityGenerated(label);
						QApplication::processEvents();
					}
				}
			}
		}

	}

	return selectedID;
}

void Viewer::DrawWithNames(const QPoint& point)
{
	//�����
	int subSelectedID;

	//�����׽�������Ҷ���Ϊ���ƣ���ӵ����л�ȡ����
	DcGp::DcGpEntity* pEntity = nullptr;
	int selectedID = Pick(ePointPicking, DCCore::Point_2(point.x(), point.y()), &subSelectedID, 5, 5, false/* ����lodģ�� */);
	emit IdIndexPicked(selectedID, subSelectedID);
	auto aa =5;
}

//���ö�̬������ת����
void Viewer::SetDynamicPivotState(bool state)
{
	m_bUpdatingPivot = state;
}

bool Viewer::GetDynamicPivotState()
{
	return m_bUpdatingPivot;
}

//�ı䵱ǰ���ű���
void Viewer::ChangedCurrentScale(double scale)
{
	static double symbolSize = 10.0 * (BoundingBox().maxCorner.y - BoundingBox().minCorner.y) / height();
	//���Ŵ�С
	//static const double SYMBOL_SIZE = 2.0;

	//�ı��ķ��Ŵ�С
	double size = symbolSize / scale;
}

//��ʾ��ά�ı�
void Viewer::Display3DLabel(const QString& str, 
	const DCVector3D& pos3D,
	const unsigned char* rgbColor /* = 0 */, 
	const QFont& font /* = QFont() */)
{
	const unsigned char* col = rgbColor ? rgbColor : DcGp::Color::DEFAULTCOLOR;
	glColor3f((float)col[0]/(float)MAX_COLOR_COMP,(float)col[1]/(float)MAX_COLOR_COMP,(float)col[2]/(float)MAX_COLOR_COMP);

	renderText(pos3D.x, pos3D.y, pos3D.z, str, font);
}


//������ʾȫ������
void Viewer::CenterAll()
{
	m_useSelectBoundbox = false;
	UpdateCenterAndZoom(BoundingBox());
}
//������ʾѡ������
void Viewer::CenterSelected(DcGp::DcGpEntity* pEntity)
{
	if (!pEntity)
	{
		return;
	}

	DcGp::DcGpBoundBox box = pEntity->GetBoundBox();

	BoundBox bbox;
	bbox.center = box.GetCenter();
	bbox.minCorner = box.MinCorner();
	bbox.maxCorner = box.MaxCorner();

	m_useSelectBoundbox = true;
	m_selectBoundbox = bbox;

	UpdateCenterAndZoom(bbox);
}

void Viewer::Display2DLabel(QString text, int x, int y, unsigned char align /* = eDefaultAlign */, float bkgAlpha /* = 0 */, const unsigned char* rgbColor /* = 0 */, const QFont* font /* = 0 */)
{
	makeCurrent();

	int x2 = x;
	int y2 = m_glHeight - 1 - y;

	const unsigned char* col = (rgbColor ? rgbColor : DcGp::Color::DEFAULTLABELCOLOR);

	QFont textFont = (font ? *font : QFont());

	QFontMetrics fm(textFont);
	int margin = fm.height()/4;

	if (align != (eLeftAlign | eTopAlign) || bkgAlpha != 0)
	{
		QRect rect = fm.boundingRect(text);

		//text alignment
		if (align & eHMidAlign)
			x2 -= rect.width()/2;
		else if (align & eRightAlign)
			x2 -= rect.width();
		if (align & eVMidAlign)
			y2 += rect.height()/2;
		else if (align & eBottomAlign)
			y2 += rect.height();

		//background is not totally transparent
		if (bkgAlpha != 0)
		{
			//inverted color with a bit of transparency
			glPushAttrib(GL_COLOR_BUFFER_BIT);
			glEnable(GL_BLEND);
			
			const float invertedCol[4] = {	1.0f-static_cast<float>(col[0])/255.0f,
											1.0f-static_cast<float>(col[1])/255.0f,
											1.0f-static_cast<float>(col[2])/255.0f,
											bkgAlpha };
			glColor4fv(invertedCol);

			int xB = x2 - m_glWidth/2;
			int yB = m_glHeight/2 - y2;
			yB += margin/2; //empirical compensation

			glBegin(GL_POLYGON);
			glVertex2d(xB - margin, yB - margin);
			glVertex2d(xB - margin, yB + rect.height() + margin/2);
			glVertex2d(xB + rect.width() + margin, yB + rect.height() + margin/2); 
			glVertex2d(xB + rect.width() + margin, yB - margin); 
			glEnd();
			glPopAttrib();
		}
	}

	glColor3f((float)col[0]/(float)MAX_COLOR_COMP,(float)col[1]/(float)MAX_COLOR_COMP,(float)col[2]/(float)MAX_COLOR_COMP);
	if (align & eBottomAlign)
		y2 -= margin; //empirical compensation
	else if (align & eVMidAlign)
		y2 -= margin/2; //empirical compensation
	renderText(x2, y2, text, textFont);
}

void Viewer::UpdateActiveItemsList(int x, int y, bool extendToSelectedLabels/*=false*/)
{
	m_activeItems.clear();

	if (!m_pSceneRoot)
		return;

	int subID=-1;
	DCCore::Point_2 pt(x,y);
	int itemID = Pick(eEntityPicking, pt, &subID, 5, 5);
	if (itemID < 1)
		return;

	//����
	DcGp::DcGpEntity* pickedObj = m_pSceneRoot->Find(itemID);

	if (pickedObj)
	{
		//if (pickedObj->IsA("PlanePolygon"))
		//{
		//	MPGp::PlanePolygon* planeGon = static_cast<MPGp::PlanePolygon*>(pickedObj);
		//	if (!planeGon->IsSelected() || !extendToSelectedLabels)
		//	{
		//		//select it?
		//		//emit entitySelectionChanged(label->getUniqueID());
		//		m_activeItems.push_back(planeGon);
		//		return;
		//	}
		//}
	}
}

void Viewer::AddEventHandle(DCCore::GUIEventHandle* eventHandle)
{
	m_eventHandles.push_back(eventHandle);
}

void Viewer::UpdateEventhandleValidState(QStringList handles, bool state)
{
	//���������е�handle
	for (int i = 0; i != m_eventHandles.size(); ++i)
	{
		QString eventHandleName = m_eventHandles[i]->metaObject()->className();
		if (handles.contains(eventHandleName))
		{
			m_eventHandles[i]->SetValid(state);
		}
		else
		{
			m_eventHandles[i]->SetValid(!state);
		}
	}
}

DCCore::GUIEventHandle* Viewer::GetEventHandleByClassName(QString uniqueName)
{
	for (int i = 0; i != m_eventHandles.size(); ++i)
	{
		QString eventHandleName = m_eventHandles[i]->metaObject()->className();
		if (eventHandleName == uniqueName)
		{
			return m_eventHandles[i];
		}
	}

	return nullptr;
}

std::vector<DCCore::GUIEventHandle* > Viewer::GetEventHandles()
{
	return m_eventHandles;
}
const double* Viewer::GetModelViewMatrixd()
{
	return GetViewportParameters().matrixModelView;
}

const double* Viewer::GetProjectionMatrixd()
{
	return GetViewportParameters().matrixProjection;
}

void Viewer::GetViewportArray(int viewport[])
{
	//DCView::ViewerParams paras = GetViewportParameters();
	//viewport = paras.matrixViewport;
	makeCurrent();
	glGetIntegerv(GL_VIEWPORT, viewport);
}

void Viewer::ToBeRefreshed()
{
	//! ����refresh״̬
	Refresh();

	DisableProjection();
}
void Viewer::SetUnit(QString unit)
{
	m_unitMan->SetUnit(unit);
}
QString Viewer::GetUnit()
{
	return m_unitMan->GetUnit();
}
QString Viewer::GetENUnit()
{
	return m_unitMan->GetENUnit();
}
double Viewer::GetScale()
{
	return m_unitMan->GetScale();
}
void Viewer::SetScale(double scale)
{
	return m_unitMan->SetScale(scale);
}
QImage Viewer::FrameBufferSnapshot()
{
	// View must be on top of other windows.
	makeCurrent();
	raise();
	// Hack: Qt has problems if the frame buffer is grabbed after QFileDialog is displayed.
	// We grab the frame buffer before, even if it might be not necessary (vectorial rendering).
	// The problem could not be reproduced on a simple example to submit a Qt bug.
	// However, only grabs the backgroundImage in the eponym example. May come from the driver.
	return grabFrameBuffer(true);
}

void Viewer::EventTraversal()
{

}

//void Viewer::UpdateTraversal()
//{
//	//! �Ȳ����£���������̬�������ݣ�������ڵ���ֻ��һ���Ӷ���
//	static int i = 0;
//
//	//!��������
//
//	QString fileName;
//	if (i % 2 == 0)
//	{
//		fileName = "D:\\data\\TestData\\ƽ����.txt";
//		//fileName = "D:\\data\\TestData\\HOUSE.txt";
//
//
//
//	}
//	else
//	{
//		fileName = "D:\\data\\TestData\\LOUTI-5.txt";
//	}
//
//	//! ���߳��ж�ȡ���������ӵ����ڵ���
//	PageLodTaskThread* ioThread = new PageLodTaskThread(m_pEntityQueue, fileName);
//	QThreadPool::globalInstance()->start(ioThread);
//
//
//
//	i++;
//}

void Viewer::UpdateTraversal()
{
	static_cast<DCUtil::DatabasePage* >(m_updateVisitor->GetDatabaseRequestHandler())->UpdateScene();
}

void Viewer::RenderingTraversals()
{
	//! ������·����������������У������޳����������ļ������̶߳�ȡ�ļ���
	//! ����m_updateVisitor�е�viewpoint
	m_updateVisitor->SetViewPointLocal(GetViewportParameters().cameraPos);

	if (!m_pSceneRoot)
	{
		return;
	}

	m_pSceneRoot->Accept(*m_updateVisitor);

	//! ���ø������Ⱦ��
	Render();
}

void Viewer::paintGL()
{
	Frame();
}
