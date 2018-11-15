
//Qt
#include "QDir"
#include <QtWidgets/QApplication>
#include "QObject"

//DCCore
#include "DCCore/Logger.h"
#include "DCCore/IoInterface.h"

#include <Windows.h>
#include <WinBase.h>

QString DCCore::GetIOPluginPath()
{
	QDir ioDir(QApplication::applicationDirPath());
	if (!ioDir.exists("IO"))
	{
		Logger::Message(QObject::tr("Unable to find the IO plugins directory!"));
	}
	ioDir.cd("IO");

	return ioDir.absolutePath();
}

bool DCCore::GetIOPlugin(QDir dir, QString extension, IoPlugin*& io)
{
	//���������ļ�
	dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
	QStringList filters;
	filters << "*.dll";
	dir.setNameFilters(filters);
	QStringList list = dir.entryList();

	HINSTANCE hInst;
	for (unsigned i = 0; i < list.size(); ++i)
	{
		//��ǰdll����
		QString ioName = list.at(i);

		QString dllFullname = dir.absoluteFilePath(ioName);

		//��ʾ���ظ�dll�������ù̶��ӿں���
		hInst = LoadLibrary(dllFullname.toStdString().c_str());

		//���庯��ָ��
		typedef IoPlugin* (*LoadPlugin)(const char* );
		//��ȡdll�е����ĺ���
		LoadPlugin plugin = (LoadPlugin)GetProcAddress(hInst, "LoadPlugin");

		if (!plugin)
		{
			FreeLibrary(hInst);
			continue;
		}

		io = plugin(extension.toStdString().c_str());
		if (io) //ֻҪ�ҵ�һ��ƥ��Ľ�������������ѭ��
		{
			break;
		}
		else
		{
			FreeLibrary(hInst);
		}
	}
	if (!io)
	{
		Logger::Message(QObject::tr("Does not find IO plugin(Maybe missed IO or depend DLL)!"));
		return nullptr;
	}
	return io;
}