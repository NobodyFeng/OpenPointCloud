//##########################################################################
//#                                                                        #
//#                            DCLW                                        #
//#                                                                        #
//#  This library is free software; you can redistribute it and/or modify  #
//#  it under the terms of the GNU Lesser General Public License(LGPL) as  #
//#  published by the Free Software Foundation; version 2 of the License.  #
//#                                                                        #
//#  This library is distributed in the hope that it will be useful,       #
//#  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
//#  GNU Lesser General Public License for more details.                   #
//#                                                                        #
//#          COPYRIGHT: DCLW             																	 #
//#                                                                        #
//##########################################################################

/*==========================================================================
*�����ܸ�����
*
*���ߣ�dclw         ʱ�䣺$time$
*�ļ�����$safeitemname$
*�汾��V2.1.0
*
*�޸��ߣ�          ʱ�䣺
*�޸�˵����
*===========================================================================
*/

#include "DCGui/AuxMainWindow.h"

//QT
#include "QApplication"
#include "QDir"
#include "QList"
#include "QSettings"
#include "QPluginLoader"
#include "QDebug"
#include "QMenuBar"
#include "QDesktopWidget"
#include "QMessageBox"
#include "QDesktopServices"
#include "QUrl"
#include "QFile"
#include "QFileInfo"

//DCCore
#include "DCCore/PluginInterface.h"
#include "DCCore/Command.h"

using namespace DCGui;

AuxMainWindow::AuxMainWindow(QWidget *parent, Qt::WindowFlags flags)
	: IWindow(parent, flags)
{
	
}

AuxMainWindow::~AuxMainWindow()
{
	//�Ƴ����е��Ӷ��󣬲�delete
	while(!m_Module.empty())
	{
		PluginInterface* child = m_Module.back();
		m_Module.pop_back();

		delete child;
		child = nullptr;
	}
}

//����ģ��
void AuxMainWindow::LoadModules()
{
	//ģ��·��
	QDir moduleDir = QApplication::applicationDirPath() + "/Plugins";

	//ģ���б�
	QStringList modules = moduleDir.entryList(QStringList() << "*.mdl",
		QDir::NoFilter, QDir::Time | QDir::Reversed);

	for (auto iter = modules.constBegin();
		iter != modules.constEnd(); ++iter)
	{
		//ģ�������
		QPluginLoader loader(moduleDir.absoluteFilePath(*iter));

		//���ģ���ʶ�������
		PluginInterface* pModule = qobject_cast<PluginInterface*>(loader.instance());

		qDebug() << loader.errorString() << endl;

		if (pModule)
		{
			//����ģ����ź�
			ConnectModule(pModule);

			//����ģ��˵��͹�����
			LoadMenusAndToolBars(pModule->Menus(), pModule->ToolBars());
			//����ͣ������
			LoadDockWidgets(pModule->DockWidgets());

			m_Module.push_back(pModule);
		}
	}
}

//����ģ����ź�
void AuxMainWindow::ConnectModule(PluginInterface* pModule)
{
	//��ȡԪ����
	const QMetaObject* pMeta = pModule->metaObject();

	//�ж�ִ��������ź��Ƿ���ڣ������ڣ�������
	if (pMeta->indexOfSignal("ExecuteCommand(QStringList)") != -1)
	{
		connect(pModule, SIGNAL(ExecuteCommand(const QStringList&)),
			this, SLOT(ExecuteCommand(const QStringList&)));
	}

	//�ж������¶�����ź��Ƿ���ڣ������ڣ�������
	if (pMeta->indexOfSignal("EntitiesGenerated(QVector<DcGp::DcGpEntity*>,bool)") != -1)
	{
		connect(pModule, SIGNAL(EntitiesGenerated(const QVector<DcGp::DcGpEntity*>&, bool)),
			this, SLOT(GeneratedEntities(const QVector<DcGp::DcGpEntity*>&, bool)));
	}

	//�ж������¶�����ź��Ƿ���ڣ������ڣ�������
	if (pMeta->indexOfSignal("EntitiesGenerated(QVector<DcGp::DcGpEntity*>)") != -1)
	{
		connect(pModule, SIGNAL(EntitiesGenerated(const QVector<DcGp::DcGpEntity*>&)),
			this, SLOT(GeneratedEntities(const QVector<DcGp::DcGpEntity*>&)));
	}

	//�жϵ�ǰ����仯�Ĳ��Ƿ���ڣ������ڣ�������
	if (pMeta->indexOfSlot("ChangedHandlingEntities(QVector<DcGp::DcGpEntity*>)") != -1)
	{
		connect(this, SIGNAL(HandlingEntitiesChanged(const QVector<DcGp::DcGpEntity*>&)),
			pModule, SLOT(ChangedHandlingEntities(const QVector<DcGp::DcGpEntity*>&)));
	}

	//�жϵ�ǰview�仯�������ڣ�������
	if (pMeta->indexOfSlot("ChangedCurrentViewer(DCView::View**)") != -1)
	{
		connect(this, SIGNAL(CurrentViewerChanged(DCView::View*)),
			pModule, SLOT(ChangedCurrentViewer(DCView::View*)));
	}
}

//���ز˵����͹�����
void AuxMainWindow::LoadMenusAndToolBars(QList<QMenu*> menus,
	QList<QToolBar*> toolBars)
{
	//��ȡroot�˵�����
	int rootNum = GetRootMenuNum();
	if (rootNum == 0)
	{
		return;
	}
	//��ȡ�����ڶ����˵�
	QAction* actionBefore = GetAction(rootNum/2);

	//��ȡ�˵���
	QMenuBar* pMenuBar = menuBar();


	//�˵�
	for (auto iterMenu = menus.constBegin();
		iterMenu != menus.constEnd(); ++iterMenu)
	{

		pMenuBar->insertMenu(actionBefore, *iterMenu);
	}

	//������
	for (auto iterToolBar = toolBars.constBegin();
		iterToolBar != toolBars.constEnd(); ++iterToolBar)
	{
		addToolBar(*iterToolBar);
	}
}

//����ͣ������
void AuxMainWindow::LoadDockWidgets(QMap<QDockWidget*, Qt::DockWidgetArea> docks)
{
	if (docks.isEmpty())
	{
		return;
	}

	for (auto iter = docks.constBegin();
		iter != docks.constEnd(); ++iter)
	{
		//���ͣ������
		addDockWidget(iter.value(), iter.key());
	}
}

//ע������
//void AuxMainWindow::RegisterCommands(QMap<QString, DCCore::CommandGenerator> generators)
//{
//	//���������
//	CommandManager* pManager = CommandManager::Instance();
//	pManager->RegisterCommands(generators);
//}

void AuxMainWindow::About()
{
	QString strAbout = QString("��Ȩ����!\n"
		"�汾:%1\n"
		"��˾:����������ά�Ƽ����޹�˾\n").arg(GetTitle());
	QMessageBox::about(this, QString("����"), strAbout);
	QString aa = tr("aa");
}

void AuxMainWindow::Help()
{
	//Ӧ�ó�������
	QString eName = QApplication::applicationName();

	//֧�ֵĺ�׺��
	QStringList fixName;
	fixName << ".pdf" << ".doc" << ".docx" << ".chm";

	//Ҫ�򿪵İ����ļ�ȫ��
	QString strHelpFile;
	//! �������Ҵ�
	for (int i = 0; i != fixName.size(); ++i)
	{
		strHelpFile = QApplication::applicationDirPath() + "/" + eName + fixName[i];
		QFileInfo file(strHelpFile);
		if (file.exists())
		{
			//�򿪰����ļ�
			QDesktopServices desktop;
			desktop.openUrl(QUrl(strHelpFile));
			break;
		}
	}
}