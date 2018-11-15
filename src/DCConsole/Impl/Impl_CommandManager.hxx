#ifndef DC_CONSOLE_IMPL_COMMANDMANAGER_H
#define DC_CONSOLE_IMPL_COMMANDMANAGER_H

//Qt
#include "QtCore/QObject"
#include "QtCore/QMap"
#include "QtCore/QList"
class QString;

#include "DCCore/Command.h"
#include "DCConsole/CommandManager.h"

namespace DCConsole
{
	//���������ʵ����
	class CommandManager::CommandManagerImpl : public QObject
	{
		Q_OBJECT

	public:
		explicit CommandManagerImpl(CommandManager* pInterface);
		~CommandManagerImpl();

		//ע������
		void RegisterCommand(const QString& strCmdName,
			const DCCore::CommandGenerator& generator);
		void RegisterCommands(QMap<QString, DCCore::CommandGenerator> generators);

		//���������Ƿ���ע��
		bool IsRegistered(const QString& strCmdName) const;

		//��������
		DCCore::CommandPtr GenerateCommand(const QString& strCmd);

		//��ȡ���ʹ�õ�����
		QStringList GetRecentCommands() const;

		//���ϲ����������
		QString SearchUpRecentCommand();
		//���²����������
		QString SearchDownRecentCommand();

	private:
		//��¼����
		void RecordCommand(DCCore::CommandPtr pCmd);

		//��������ģ��
		void LoadCommands();

	private:
		//�ӿ�
		CommandManager* m_pInterface;
		//��������
		QMap<QString, DCCore::CommandGenerator> m_commandGenerators;
		//����ջ
		QList<DCCore::CommandPtr> m_cmdStack;
		//���ʹ�õ������¼�������ִ�е����
		QStringList m_recentCommands;

		//����ջ�������
		static const unsigned c_cmdStackSize = 5;
		//��ʷ��������
		static const unsigned c_recentCmdCount = 10;
		//��ǰ�����������
		int m_currentRecentCmdIndex;
	};

	//���ע�������
	class OutputRegisteredCommand : public DCCore::Command
	{
		Q_OBJECT
	public:
		//����������
		static DCCore::CommandPtr Generate();

		//��ȡ����ȫ������(��֤Ψһ��)
		static QString GlobalName();
		//ִ������
		virtual void Execute();
		//��������
		virtual void Undo(){}
		//��������
		virtual void Redo(){}

		//��ȡ����ȫ������(��֤Ψһ��)
		virtual QString GetGlobalName() const;

		//��ȡ�������Ϣ
		virtual QString GetDesigner() const;

		//��ȡ������Ϣ
		virtual QString GetDescription() const;

	private:
		//����������浽ָ���ļ���
		void SaveCommands(const QString& strFileName,
			QMap<QString, DCCore::CommandGenerator> commands);

	public:
		//ע�����������
		static QMap<QString, DCCore::CommandGenerator>* s_commands;
	};
	typedef boost::shared_ptr<OutputRegisteredCommand> OutputRegisteredCommandPtr;
}
#endif