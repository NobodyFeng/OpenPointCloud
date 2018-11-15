#ifndef DC_CONSOLE_COMMANDLINE_H
#define DC_CONSOLE_COMMANDLINE_H

//Qt
#include "QString"
#include "QtWidgets/QLineEdit"

namespace DCConsole
{
	//������
	class CommandLine : public QLineEdit
	{
		Q_OBJECT

	public:
		explicit CommandLine(QWidget* pParent = nullptr);

		//��������
		void SetCommand(const QString& strCmd);

signals:
		//ִ������
		void Execute(const QString& strCmd);

		//�������
		void FocusEntered();
		//�����Ƴ�
		void FocusRemoved();

		//���ϲ�������
		void SearchUp();
		//���²���
		void SearchDown();

		//ȡ��
		void Cancel();

		private slots:
			//��������
			void ParseCommand();
			void ParseCommand(const QString& strCmd);

	protected:
		//��갴���¼�
		virtual void keyPressEvent(QKeyEvent* event);
		//��������¼�
		virtual void focusInEvent(QFocusEvent* event);
		//�����Ƴ��¼�
		virtual void focusOutEvent(QFocusEvent* event);
		//����Ƴ��¼�
		virtual void leaveEvent(QEvent* event);
	};
}

#endif