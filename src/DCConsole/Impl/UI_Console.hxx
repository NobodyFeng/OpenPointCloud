#ifndef DC_CONSOLE_UI_CONSOLE_H
#define DC_CONSOLE_UI_CONSOLE_H

//Qt
#include "QtWidgets/QLabel"
#include "QtWidgets/QLineEdit"
#include "QtWidgets/QPlainTextEdit"
#include "QtWidgets/QToolButton"
#include "QtWidgets/QGridLayout"
#include "QtWidgets/QHBoxLayout"
#include "QtWidgets/QSpacerItem"

//Self
#include "CommandLine.hxx"

namespace DCConsole
{
	class MessageBrower : public QTextEdit
	{
		Q_OBJECT
	public:
		explicit MessageBrower(QWidget* pParent = nullptr);

		private slots:
			//���¸߶�
			void UpdateHeight();

			void ClearMessage();
	};

	class UI_Console
	{
	public:
		//����������
		CommandLine* cmdLine;
		//������ʾ��
		QLabel* cmdPrompt;
		//����ͼ��
		QToolButton* tbtnIcon;

		//���ͼ��
		QToolButton* tbtnClear;

		//������Ϣ��
		MessageBrower* msgBrower;

		void SetupUi(QWidget* pWidget)
		{
			//����դ�񲼾ֹ�����
			QGridLayout* pGrid = new QGridLayout(pWidget);
			pGrid->setContentsMargins(0, 0, 0, 0);
			pGrid->setSpacing(0);

			//����ˮƽ���ֹ�����
			{
				QHBoxLayout* pHlyt = new QHBoxLayout;
				
				//�������toolbutton
				{
					tbtnClear = new QToolButton;
					tbtnClear->setIcon(QIcon(":/image/Resources/clear.png"));

					//���ù̶���С
					tbtnClear->setFixedSize(30, 20);
					//���ñ���ɫ
					tbtnClear->setStyleSheet("background-color: rgb(188, 199, 216);");
					//���ô�С��ʽ
					QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
					sizePolicy.setHorizontalStretch(0);
					sizePolicy.setVerticalStretch(0);
					tbtnClear->setSizePolicy(sizePolicy);

					//����Ϊƽ̹
					tbtnClear->setAutoRaise(true);

					//��ӵ�ˮƽ���ֹ�����
					pHlyt->addWidget(tbtnClear);

					QSpacerItem* horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
				
					pHlyt->addItem(horizontalSpacer);
				}

				//��ˮƽ���ֹ�������ӵ�դ�񲼾ֹ�����
				pGrid->addLayout(pHlyt, 0, 0);
			}
			

			//����������Ϣ��
			{
				msgBrower = new MessageBrower;
				//���ɱ༭
				msgBrower->setReadOnly(true);
				//���ô�С��ʽ
				msgBrower->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred));

				pGrid->addWidget(msgBrower, 1, 0);
			}


			//����ˮƽ���ֹ�����
			{
				QHBoxLayout* pHlyt = new QHBoxLayout;

				//��������ͼ��
				{
					tbtnIcon = new QToolButton;
					//����Ϊ�����˵�ģʽ
					tbtnIcon->setPopupMode(QToolButton::MenuButtonPopup);
					//���ù̶���С
					tbtnIcon->setFixedSize(30, 20);
					//����Ϊƽ̹
					tbtnIcon->setAutoRaise(true);

					//��ӵ�ˮƽ���ֹ�����
					pHlyt->addWidget(tbtnIcon);
				}

				//����������ʾ��
				{
					cmdPrompt = new QLabel;
					//���ù̶��߶�
					cmdPrompt->setFixedHeight(20);
					//�����ı���ʽ
					cmdPrompt->setTextFormat(Qt::RichText);
					//���ý���ģʽ
					cmdPrompt->setTextInteractionFlags(Qt::LinksAccessibleByMouse);
					//����Ĭ��Ϊ����
					//cmdPrompt->setVisible(false);
					//���ñ���ɫΪ��ɫ
					cmdPrompt->setStyleSheet("background-color: rgb(255, 255, 255);");
					//���ô�С��ʽ
					cmdPrompt->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed));

					//��ӵ�ˮƽ���ֹ�����
					pHlyt->addWidget(cmdPrompt);
				}

				//��������������
				{
					cmdLine = new CommandLine;
					//���ù̶��߶�
					cmdLine->setFixedHeight(20);
					//�����޿��
					cmdLine->setFrame(false);
					//���ô�С��ʽ
					cmdLine->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));

					//��ӵ�ˮƽ���ֹ�����
					pHlyt->addWidget(cmdLine);
				}

				//��ˮƽ���ֹ�������ӵ�դ�񲼾ֹ�����
				pGrid->addLayout(pHlyt, 2, 0);
			}
		}
	};
}

#endif