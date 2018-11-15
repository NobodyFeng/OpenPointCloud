#ifndef DCCORE_PLUGININTERFACE_H
#define DCCORE_PLUGININTERFACE_H

//Qt
#include "QObject"
#include "QVector"
#include "QList"
#include "QMap"

class QMenu;
class QToolBar;
class QWidget;
class QTreeWidgetItem;
class QDockWidget;

//����
#include "Command.h"

class PluginInterface : public QObject
{
public:
	//ģ��˵�
	virtual QList<QMenu*> Menus() const = 0;
	//ģ�鹤����
	virtual QList<QToolBar*> ToolBars() const = 0;
	//ͣ������
	virtual QMap<QDockWidget*, Qt::DockWidgetArea> DockWidgets() const = 0;

	//ѡ����ڵ�
	virtual QTreeWidgetItem* OptionRoot() const = 0;
	//ѡ���
	virtual QMap<QTreeWidgetItem*, QWidget*> OptionWidgets() const = 0;

	//����������
	virtual QMap<QString, DCCore::CommandGenerator> CommandGenerators() const = 0;

	/*******************�����źźͲ۸�����Ŀ�����������*************************/
//	//slots:
//		//�ı����������
//		void ChangedHandlingEntities(const QVector<DcGp::DcGpEntity*>& entities);
//
//signals:
//	//���ʹ���������
//	void EntitiesGenerated(const QVector<DcGp::DcGpEntity*>& entities);
//	//ִ������
//	void ExecuteCommand(const QStringList& commands);
};

Q_DECLARE_INTERFACE(PluginInterface, "com.bjdclw.DC-Platform.PluginInterface/2.0")

#endif