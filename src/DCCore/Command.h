#ifndef DCCORE_COMMAND_H
#define DCCORE_COMMAND_H

//Boost
#include "boost/shared_ptr.hpp"

//Qt
#include "QObject"
#include "QString"

namespace DCCore
{
	//����
	class Command : public QObject
	{
	public:
		//ִ������
		virtual void Execute() = 0;
		//��������
		virtual void Undo() = 0;
		//��������
		virtual void Redo() = 0;

		//��ȡ����ȫ������(��֤Ψһ��)
		virtual QString GetGlobalName() const = 0;

		//��ȡ�������Ϣ
		virtual QString GetDesigner() const = 0;

		//��ȡ������Ϣ
		virtual QString GetDescription() const = 0;

	protected:
		Command(){}
	};
	typedef boost::shared_ptr<Command> CommandPtr;

	//����������
	typedef CommandPtr (*CommandGenerator)();

	//��������
	class InteractiveCommand : public Command
	{
	public:
		//������������仯
		virtual void ChangedCommandParameter(const QString& strState) = 0;

		//ȡ������
		virtual void Cancel() = 0;

	protected:
		explicit InteractiveCommand(){}
	};
	typedef boost::shared_ptr<InteractiveCommand> InteractiveCommandPtr;
}

#endif