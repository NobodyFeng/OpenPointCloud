#include "myclass.h"

//Qt
#include "QStatusBar"
#include "QMdiArea"
#include "QGridLayout"
#include "QApplication"
#include "QDir"

//qglviewer

//DCConsole
#include "DCConsole/Console.h"

//DCCore
#include "DCCore/IoPlugin.h"
#include "DCCore/IoInterface.h"
#include "DCCore/DCConfig.h"

#include "DCView/Viewer.h"
#include "Mdiarea.h"

//! DCGa
#include "DCGa/TrackballManipulator.h"
#include "DCGa/FlyManipulator.h"


MyClass::MyClass(QWidget *parent, Qt::WindowFlags flags)
	: DCGui::AuxMainWindow(parent, flags)
{
	bool status = ConfigInit(this);

	if (status)
	{
		//! ����ģ��
		//MPMg::Manager* manager = MPMg::Manager::Instance();
		//manager->CreateUI(this);

		//�źŲ�����
		//connect(manager, SIGNAL(SelectionChanged(const QVector<DcGp::DcGpEntity*>&)), this, SLOT(RefreshAll()));
		//connect(manager, SIGNAL(SelectionChanged(const QVector<DcGp::DcGpEntity*>&)), this, SIGNAL( HandlingEntitiesChanged(const QVector<DcGp::DcGpEntity*>&)) );

		//����̨��ʼ��
		DCConsole::Console* pConsole = DCConsole::Console::Initialize();
		//���ÿ���̨λ��
		pConsole->setParent(this);
		addDockWidget(Qt::BottomDockWidgetArea, pConsole);

		//��ʼ���ര�ڹ���
		QGridLayout* gridLayout = new QGridLayout(centralWidget());
		m_pMdiArea = new MdiArea(centralWidget());
		m_pMdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
		m_pMdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
		gridLayout->addWidget(m_pMdiArea, 0, 0, 1, 1);

		//! ����ര�ڵ��ϷŲ���
		m_pMdiArea->setAcceptDrops(true);
		m_pMdiArea->setMainWindow(this);

		connect(m_pMdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(ChangedCurrentViewer()));

		//״̬��
		statusBar()->showMessage(("����"));

		LoadModules();
	}

	ConfigFinish(this);

	MPViewer::Viewer* pNewViewer = new MPViewer::Viewer(this);
	//! ��װ������
	pNewViewer->AddManipulatorHandle(new DCGa::TrackballManipulator("TrackBall"));
	pNewViewer->AddManipulatorHandle(new DCGa::FlyManipulator("Fly"));
	pNewViewer->showMaximized();
	
	//pNewViewer->setAttribute(Qt::WA_DeleteOnClose);


	//QGridLayout* gridLayout = new QGridLayout(centralWidget());
	//gridLayout->addWidget(pNewViewer, 0, 0, 1, 1);
	m_pMdiArea->addSubWindow(pNewViewer);
	pNewViewer->showMaximized();
	loadFile("D:\\data\\TestData\\LOUTI-5.txt", pNewViewer);
}

MyClass::~MyClass()
{

}

int MyClass::loadFile(const QString &fileName, MPViewer::Viewer* pNewViewer)
{
	//MPViewer::Viewer* pNewViewer = new MPViewer::Viewer(this);
	//m_pMdiArea->addSubWindow(pNewViewer);

	////�����ʾ
	//pNewViewer->showMaximized();
	//activeMdiChild();
	QString extension = QFileInfo(fileName).suffix();
	//������չ��������IO�ļ����еĲ����ѡ����ʵ�io�����������ļ�
	QDir dir(DCCore::GetIOPluginPath());

	DCCore::IoPlugin* currentIo = 0;

	bool hasPlugin = DCCore::GetIOPlugin(dir, extension, currentIo);
	if (!hasPlugin || !currentIo)
	{
		QApplication::restoreOverrideCursor();
		return 0;
	}

	double coordinatesShift[3] = {0,0,0};
	DcGp::DcGpEntity* entity = currentIo->LoadFile(fileName, 0, true, 0, coordinatesShift, nullptr);

	if (pNewViewer)
	{
		//�����������ʵ�����
		pNewViewer->SetSceneRoot(entity);
	}

	//����߽�У������������
	//pNewViewer->ShowAllScene();

	return 0;
}

QWidget* MyClass::activeMdiChild()
{
	if (QMdiSubWindow *activeSubWindow = m_pMdiArea->activeSubWindow())
		return qobject_cast<QWidget *>(activeSubWindow->widget());

	return 0;
}

int MyClass::addFile(MPViewer::Viewer*viewer, const QString &fileName)
{
	QApplication::setOverrideCursor(Qt::WaitCursor);

	
	QApplication::restoreOverrideCursor();
	return 0;
}