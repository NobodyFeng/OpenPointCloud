#include "UI_Console.hxx"
using namespace DCConsole;

MessageBrower::MessageBrower(QWidget* pParent /* = nullptr */)
	: QTextEdit(pParent)
{
	////���ù̶��߶�
	//setMinimumHeight(document()->size().height());

	////�����ĵ����ݱ仯���ź�
	//connect(document(), SIGNAL(contentsChanged()),
	//	this, SLOT(UpdateHeight()));
}

//���¸߶�
void MessageBrower::UpdateHeight()
{
	//���߶��Ƿ�仯
	QTextDocument* pDoc = dynamic_cast<QTextDocument*>(sender());
	//�ĵ��߶�
	int heightDoc = pDoc->size().height();

	//�߶ȷ����仯�����ø߶�
	if (heightDoc != height())
	{
		setMinimumHeight(heightDoc);
	}
}

void MessageBrower::ClearMessage()
{
	clear();
}