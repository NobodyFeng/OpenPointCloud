#ifndef DC_CONSOLE_IMPL_CONSOLE_H
#define DC_CONSOLE_IMPL_CONSOLE_H

//Boost
#include "boost/shared_ptr.hpp"

//Qt
#include "QtCore/QObject"
#include "QtCore/QDir"
#include "QtCore/QList"

//DCCore
#include "DCCore/Logger.h"

//Self
#include "DCConsole/Console.h"

namespace DCCore
{
	//��������
	class InteractiveCommand;
	typedef boost::shared_ptr<InteractiveCommand> InteractiveCommandPtr;
}

namespace DCConsole
{
	//����̨UI
	class UI_Console;

	

	//���������
	class CommandManager;

	//��־������
	class LogManager : public QObject,
		public DCCore::Logger
	{
		Q_OBJECT

	public:
		LogManager(QObject* pParent);
		~LogManager(){}

signals:
		//��¼��־
		void Logged(const QString& strLog);

	private:
		//��ʾ��Ϣ
		virtual void ShowMessage(const QString& msg, LogLevel level = eMessageLog);
	};

	//����̨ʵ����
	class Console::ConsoleImpl : public QObject
	{
		Q_OBJECT
	public:
		ConsoleImpl(Console* pInterface);
		~ConsoleImpl();

		//��ȡ����ִ�е�����
		DCCore::CommandPtr GetRunningCommand() const;

		public slots:
			//��ʾ��Ϣ
			void Message(const QString& strMsg);

			//ȡ������
			void CancelCommand();

	private:
		//��ʼ��UI
		void InitUI();

		//�����ź���۵�����
		void CreateConnections();

		//��ȡ��ʾ��Ϣ
		QString Prompt();
		//���������Ϣ
		void OutputCommandLog();

		//�����ǰ����
		void ClearCurrentCommand();

		public slots:
			//ִ������
			void ExecuteCommand(const QString& strCmd);

		private slots:
			//�������
			void FinishCommand();

			//�ı���ʾ��Ϣ
			void ChangedPrompt(const QString& strPrompt);

			//�Ƴ������н���
			void RemoveCommandLineFocus();
			//���������н���
			void EnterCommandLineFocus();

			//���ϲ����������
			void SearchUp();
			//���²����������
			void SearchDown();

			//��ʾ�������˵�
			void ShowRecentCommandMenu();
			//ִ���������
			void ExecuteRecentCommand();

	protected:
		//�¼�������
		virtual bool eventFilter(QObject* pObject, QEvent* event);

	private:
		//�ӿ�
		Console* m_pInterface;
		//UI
		UI_Console* m_pUi;
		//��־������
		LogManager* m_pLogManager;

		//��ǰ����ִ�е�����
		DCCore::InteractiveCommandPtr m_pCurrentCommand;
		//���������
		CommandManager* m_pCommandManager;
		//Ĭ����ʾ��Ϣ
		QString m_defaultPrompt;
		//��ǰ������ʾ
		QString m_currentPrompt;
		//��ǰ����
		QString m_currentParam;
	};


	//������ʵ��
	class Parameters::ParametersImpl
	{
	public:
		ParametersImpl(Parameters* pInterface);

		//���������ļ�����·��
		void AppendCommandDir(const QDir& dirCmd);
		//�Ƴ������ļ�����·��
		void RemoveCommandDir(const QDir& dirCmd);
		//��������·��
		QList<QDir> CommandDirs();

	private:
		//�ӿ�
		Parameters* m_pInterface;
		//��������·��
		QList<QDir> m_dirCommands;
	};
}

#endif