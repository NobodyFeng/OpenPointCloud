#ifndef DC_CONSOLE_COMMANDMANAGER_H
#define DC_CONSOLE_COMMANDMANAGER_H

#include "DCConsoleDLL.h"

//Boost
#include "boost/shared_ptr.hpp"

//Qt
#include "QMap"
#include "QStringList"
class QString;

namespace DCCore
{
	//����
	class Command;

	typedef boost::shared_ptr<Command> CommandPtr;

	//����������
	typedef CommandPtr (*CommandGenerator)();
}

namespace DCConsole
{
	//���������
	class DC_CONSOLE_API CommandManager
	{
	public:
		//��ʼ�����������
		static CommandManager* Initialize();
		//ж�����������
		static void Uninitalize();
		//��ȡ���������ʵ��
		static CommandManager* Instance();

		//ע������
		void RegisterCommand(const QString& strCmdName,
			const DCCore::CommandGenerator& generator);
		void RegisterCommands(QMap<QString, DCCore::CommandGenerator> generators);

		//��������
		DCCore::CommandPtr GenerateCommand(const QString& strCmd);

		//��ȡ���ʹ�õ�����
		QStringList GetRecentCommands() const;

		//���ϲ����������
		QString SearchUpRecentCommand();
		//���²����������
		QString SearchDownRecentCommand();

	private:
		CommandManager();
		~CommandManager();

	private:
		static CommandManager* s_pInstance;
		class CommandManagerImpl;
		CommandManagerImpl* m_pImpl;
	};
}

#endif