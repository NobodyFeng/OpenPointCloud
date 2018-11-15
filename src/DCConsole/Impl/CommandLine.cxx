#include "CommandLine.hxx"
using namespace DCConsole;

//Qt
#include "QtGui/QKeyEvent"
#include "QtGui/QFocusEvent"

CommandLine::CommandLine(QWidget* pParent /* = nullptr */)
	: QLineEdit(pParent)
{

	//�����ź���۵�����
	connect(this, SIGNAL(returnPressed()),
		this, SLOT(ParseCommand()));
	connect(this, SIGNAL(textChanged(const QString&)),
		this, SLOT(ParseCommand(const QString&)));
}

//��������
void CommandLine::SetCommand(const QString& strCmd)
{
	setText(strCmd);
}

//��������
void CommandLine::ParseCommand()
{
	//��ȡ��������
	QString strCmd = text();

	//������Ϊ�գ���ִ������
	//�������
	clear();

	//��������
	emit Execute(strCmd);
}

void CommandLine::ParseCommand(const QString& strCmd)
{
	//�ж������Ƿ��Կո������������ִ������
	if (strCmd.contains(" "))
	{
		//�������
		clear();

		//��������
		QString strCmd1 = strCmd;
		strCmd1.remove(" ");
		emit Execute(strCmd1);
	}
}

//��갴���¼�
void CommandLine::keyPressEvent(QKeyEvent* event)
{
	//�ж��Ƿ���Esc��������£�����ȡ���ź�
	if (event->key() == Qt::Key_Escape)
	{
		emit Cancel();
	}
	//���ϲ���
	else if (event->key() == Qt::Key_Up)
	{
		emit SearchUp();
	}
	//���²���
	else if (event->key() == Qt::Key_Down)
	{
		emit SearchDown();
	}

	QLineEdit::keyPressEvent(event);
}

//��������¼�
void CommandLine::focusInEvent(QFocusEvent* event)
{
	//���ͽ�������ź�
	emit FocusEntered();

	QLineEdit::focusInEvent(event);
}

//�����Ƴ��¼�
void CommandLine::focusOutEvent(QFocusEvent* event)
{
	//���ͽ����Ƴ����ź�
	emit FocusRemoved();

	QLineEdit::focusOutEvent(event);
}

//����Ƴ��¼�
void CommandLine::leaveEvent(QEvent* event)
{
	//�������
	//clearFocus();

	//���ͽ����Ƴ����ź�
	//emit FocusRemoved();

//	QLineEdit::leaveEvent(event);
}