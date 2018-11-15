#ifndef DC_CONSOLE_CONSOLE_H
#define DC_CONSOLE_CONSOLE_H

#include "DCConsoleDLL.h"

//boost
#include "boost/shared_ptr.hpp"

//Qt
#include "QtCore/QList"

#include "QtWidgets/QDockWidget"
class QDir;

namespace DCCore
{
	//����
	class Command;

	//����
	typedef boost::shared_ptr<Command> CommandPtr;
}


namespace DCConsole
{
	

	//����̨
	class DC_CONSOLE_API Console : public QDockWidget
	{
		Q_OBJECT

	public:
		//��ʼ������̨
		static Console* Initialize();
		//ж�ؿ���̨
		static void Uninitialize();
		//��ȡ����̨ʵ��
		static Console* Instance();

		//ִ������
		void ExecuteCommand(const QString& strCmd);

		//��ȡ����ִ�е�����
		DCCore::CommandPtr GetRunningCommand() const;

		//ȡ������
		void CancelRunningCommand();

	private:
		explicit Console(QWidget* pParent = nullptr);
		virtual ~Console();

	private:
		static Console* s_pInstance;

		class ConsoleImpl;
		ConsoleImpl* m_pImpl;
	};

	//������
	class DC_CONSOLE_API Parameters
	{
	public:
		//���������ļ�����·��
		static void AppendCommandDir(const QDir& dirCmd);
		//�Ƴ������ļ�����·��
		static void RemoveCommandDir(const QDir& dirCmd);
		//��������·��
		static QList<QDir> CommandDirs();

	protected:
		Parameters();
		~Parameters();

	private:
		static Parameters s_instance;

		class ParametersImpl;
		ParametersImpl* m_pImpl;
	};
}

#endif