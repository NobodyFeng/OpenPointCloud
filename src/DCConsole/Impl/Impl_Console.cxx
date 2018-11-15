#include "Impl_Console.hxx"
using namespace DCConsole;

//Qt
#include "QtWidgets/QApplication"
#include "QtGui/QTextDocument"
#include "QtWidgets/QMenu"
#include "QtWidgets/QAction"

#include "DCCore/Command.h"
//Self
#include "UI_Console.hxx"
#include "DCConsole/CommandManager.h"


/***************************************��־������*************************************************/
LogManager::LogManager(QObject* pParent)
	: QObject(pParent)
{
	DCCore::Logger::Register(this);
}

//��ʾ��Ϣ
void LogManager::ShowMessage(const QString& msg, LogLevel level /* = eLogMessage */)
{
	//�����Ϣ����Ϊ�գ�������κ���Ϣ
	if (msg.isEmpty())
	{
		return;
	}

	//��־��Ϣ��ʽ
	QString strFormat = tr("<html><head/><body><span style=' "
					"color: rgb(%1,%2,%3); font-size: 20px;'>%4</span></body></html>");
	//��־��Ϣ
	QString strLog;
	//������Ϣ���ͣ������ı��ĸ�ʽ
	switch (level)
	{
	case eMessageLog:
		strLog = strFormat.arg(255).arg(255).arg(255).arg(msg);
		break;
	case eWarningLog:
		strLog = strFormat.arg(255).arg(165).arg(0).arg(msg);
		break;
	case eErrorLog:
		strLog = strFormat.arg(255).arg(0).arg(0).arg(msg);
		break;
	default:
		break;
	}

	//��¼��־
	emit Logged(strLog);
}
/**************************************************����̨ʵ����*********************************/
Console::ConsoleImpl
	::ConsoleImpl(Console* pInterface)
	: m_pInterface(pInterface)
	, m_pUi(nullptr)
	, m_pCurrentCommand(nullptr)
	, m_pCommandManager(nullptr)
{
	//��ʼ��UI
	InitUI();

	//��ʼ����־������
	m_pLogManager = new LogManager(this);

	//��ʼ�����������
	m_pCommandManager = CommandManager::Initialize();

	//�����ź����
	CreateConnections();
}

Console::ConsoleImpl
	::~ConsoleImpl()
{
	//ж�����������
	CommandManager::Uninitalize();

	delete m_pLogManager;
	m_pLogManager = nullptr;
	//ɾ��UI
	delete m_pUi;
}

//��ʾ��Ϣ
void Console::ConsoleImpl
	::Message(const QString& strMsg)
{
	m_pUi->msgBrower->append(strMsg);
}

DCCore::CommandPtr Console::ConsoleImpl
	::GetRunningCommand() const
{
	return m_pCurrentCommand;
}

//��ʼ��UI
void Console::ConsoleImpl
	::InitUI()
{
	//�������Ĵ���
	QWidget* pCenterWidget = new QWidget;

	//����UI
	m_pUi = new UI_Console;
	m_pUi->SetupUi(pCenterWidget);

	//�������Ĵ���
	m_pInterface->setWidget(pCenterWidget);

	//���ñ���λ��Ϊ��ֱ����
	//setFeatures(QDockWidget::DockWidgetVerticalTitleBar);
	//����Ĭ��������ʾ
	m_defaultPrompt = QString("<html><head/><body><p>"
		"<span style='color:#AAAAAA;'>%1</span></p></body></html>")
		.arg(tr("Input Command"));
	m_pUi->cmdPrompt->setText(m_defaultPrompt);

	//��װ�¼�������
	{
		//������ʾ�¼�������
		m_pUi->cmdPrompt->installEventFilter(this);
	}
}

//�����ź���۵�����
void Console::ConsoleImpl
	::CreateConnections()
{
	//��¼��־
	connect(m_pLogManager, SIGNAL(Logged(const QString&)),
		this, SLOT(Message(const QString&)));

	//������ִ������
	connect(m_pUi->cmdLine, SIGNAL(Execute(const QString&)),
		this, SLOT(ExecuteCommand(const QString&)));
	//ȡ������
	connect(m_pUi->cmdLine, SIGNAL(Cancel()),
		this, SLOT(CancelCommand()));

	//���ϲ���
	connect(m_pUi->cmdLine, SIGNAL(SearchUp()),
		this, SLOT(SearchUp()));
	//���²���
	connect(m_pUi->cmdLine, SIGNAL(SearchDown()),
		this, SLOT(SearchDown()));
	

	//�����н��뽹��
	connect(m_pUi->cmdLine, SIGNAL(FocusEntered()),
		this, SLOT(EnterCommandLineFocus()));
	//�������Ƴ�����
	connect(m_pUi->cmdLine, SIGNAL(FocusRemoved()),
		this, SLOT(RemoveCommandLineFocus()));

	//��ʾ�������˵�
	connect(m_pUi->tbtnIcon, SIGNAL(clicked()),
		this, SLOT(ShowRecentCommandMenu()));

	//! ɾ������
	connect(m_pUi->tbtnClear, SIGNAL(clicked()),
		m_pUi->msgBrower, SLOT(ClearMessage()));
}

//ִ������
void Console::ConsoleImpl
	::ExecuteCommand(const QString& strCmd)
{
	if (m_pCurrentCommand)
	{
		m_pCurrentCommand->ChangedCommandParameter(strCmd);
	}
	else
	{
		if (strCmd.isEmpty())
		{
			//��ȡ���������
			CommandManager* pCmdManager = CommandManager::Instance();
			//��ȡ�������
			QString strRecentCmd = pCmdManager->SearchUpRecentCommand();
			if (!strRecentCmd.isEmpty())
			{
				ExecuteCommand(strRecentCmd);
				return;
			}
		}
		//�������
		Message(tr("Command:").append(strCmd));
		//������������ݲ�������
		DCCore::CommandPtr pCmd = m_pCommandManager->GenerateCommand(strCmd);

		//���������ڣ��������ʾ��Ϣ
		if (!pCmd)
		{
			Message(tr("Unknown Command \"%1\". Press F1 to view helper.").arg(strCmd));
		}
		else
		{
			//��������Ƿ�Ϊ����������ǽ�����������õ�ǰ����
			//������������ʾ��Ϣ������ź�
			DCCore::InteractiveCommandPtr pInterCmd = boost::dynamic_pointer_cast<DCCore::InteractiveCommand>(pCmd);
			if (!m_pCurrentCommand && pInterCmd)
			{
				m_pCurrentCommand = pInterCmd;
				//������ʾ��Ϣ�仯���ź����
				connect(m_pCurrentCommand.get(), SIGNAL(PromptChanged(const QString&)),
					this, SLOT(ChangedPrompt(const QString&)));
				//���Ӳ����仯���ź����
				connect(m_pUi->cmdPrompt, SIGNAL(linkActivated(const QString&)),
					this, SLOT(ExecuteCommand(const QString&)));
				//������Ϣ���ݵ��ź����
				connect(m_pCurrentCommand.get(), SIGNAL(Message(const QString&)),
					this, SLOT(Message(const QString&)));
				//����������ɵ��ź�
				connect(m_pCurrentCommand.get(), SIGNAL(Done()),
					this, SLOT(FinishCommand()));

			}

			//ִ������
			pCmd->Execute();
		}
	}
}

//�ı���ʾ��Ϣ
void Console::ConsoleImpl
	::ChangedPrompt(const QString& strPrompt)
{
	m_pUi->cmdPrompt->setText(strPrompt);
	m_currentPrompt = strPrompt;
	//���������н���
	m_pUi->cmdLine->setFocus();
}

//���������Ϣ
void Console::ConsoleImpl
	::OutputCommandLog()
{
	if (m_currentPrompt.isEmpty())
	{
		return;
	}

	//��ȡ��ʾ��Ϣ
	QString strMsg = m_currentPrompt;

	//�����ʾ��Ϣ���ı���ʽ
	Qt::TextFormat format = m_pUi->cmdPrompt->textFormat();
	
	if (format == Qt::RichText)
	{
		//ͨ��QTextDocument��htmlתΪ���ı�
		QTextDocument doc;
		doc.setHtml(strMsg);
		strMsg = doc.toPlainText();
	}

	//�Ƴ���ʾ��Ϣ�е���������
	strMsg.remove(m_pCurrentCommand->GetGlobalName());

	//ȥ������Ŀո�
	strMsg = strMsg.simplified();

	//�������Ĳ�����������Ϣ
	Message(strMsg.append(m_currentParam));
}

//�Ƴ������н���
void Console::ConsoleImpl
	::RemoveCommandLineFocus()
{
	//���������ʾΪ�գ�������������ʾ
	if (m_pUi->cmdPrompt->text().isEmpty()
		&& m_pUi->cmdLine->text().isEmpty())
	{
		m_pUi->cmdPrompt->setText(m_defaultPrompt);
	}
}

//���������н���
void Console::ConsoleImpl
	::EnterCommandLineFocus()
{
	//�����ǰִ��������ڣ����������ʾ
	if (!m_pCurrentCommand)
	{
		m_pUi->cmdPrompt->clear();

		//���������н���
		m_pUi->cmdLine->setFocus();
	}
}

//ȡ������
void Console::ConsoleImpl
	::CancelCommand()
{
	//������ڵ�ǰ�����ȡ��
	if (m_pCurrentCommand)
	{
		//��ʾ������Ϣ
		Message(Prompt().append(tr("*Cancel*")));

		//ȡ������
		m_pCurrentCommand->Cancel();
		//�����ǰ����
		ClearCurrentCommand();
	}
}

//��ȡ��ʾ��Ϣ
QString Console::ConsoleImpl
	::Prompt()
{
	//��ȡ��ʾ��Ϣ
	QString strMsg = m_pUi->cmdPrompt->text();

	//ͨ��QTextDocument��htmlתΪ���ı�
	QTextDocument doc;
	doc.setHtml(strMsg);
	strMsg = doc.toPlainText();

	//�Ƴ���ʾ��Ϣ�е���������
	strMsg.remove(m_pCurrentCommand->GetGlobalName());

	//ȥ������Ŀո�
	strMsg = strMsg.simplified();

	return strMsg;
}

//�����ǰ����
void Console::ConsoleImpl
	::ClearCurrentCommand()
{
	//�����ǰ������ڣ���ȡ����ص��źŲ�
	if (m_pCurrentCommand)
	{
		//ȡ����ʾ��Ϣ�仯���ź����
		disconnect(m_pCurrentCommand.get(), SIGNAL(PromptChanged(const QString&)),
			this, SLOT(ChangedPrompt(const QString&)));
		//ȡ�������仯���ź����
		disconnect(m_pUi->cmdPrompt, SIGNAL(linkActivated(const QString&)),
			this, SLOT(ExecuteCommand(const QString&)));
		//ȡ����Ϣ���ݵ��ź����
		disconnect(m_pCurrentCommand.get(), SIGNAL(Message(const QString&)),
			this, SLOT(Message(const QString&)));

		m_pCurrentCommand = nullptr;
	}
}

//�¼�������
bool Console::ConsoleImpl
	::eventFilter(QObject* pObject, QEvent* event)
{
	//�ж�������ʾ��
	if (pObject == m_pUi->cmdPrompt)
	{
		if (event->type() == QEvent::MouseButtonPress)
		{
			//���������н���
			EnterCommandLineFocus();
		}
	}

	return QObject::eventFilter(pObject, event);
}

//���ϲ����������
void Console::ConsoleImpl
	::SearchUp()
{
	//�����������ִ�еĳ�����ִ�к�������
	if (m_pCurrentCommand)
	{
		return;
	}

	//��ȡ���������
	CommandManager* pCmdManager = CommandManager::Instance();

	//����ѡ�������
	m_pUi->cmdLine->SetCommand(pCmdManager->SearchUpRecentCommand());
}

//���²����������
void Console::ConsoleImpl
	::SearchDown()
{
	//�����������ִ�еĳ�����ִ�к�������
	if (m_pCurrentCommand)
	{
		return;
	}

	//��ȡ���������
	CommandManager* pCmdManager = CommandManager::Instance();

	//����ѡ�������
	m_pUi->cmdLine->SetCommand(pCmdManager->SearchDownRecentCommand());
}

//��ʾ�������˵�
void Console::ConsoleImpl
	::ShowRecentCommandMenu()
{
	//��ȡ���������
	CommandManager* pCmdManager = CommandManager::Instance();
	//��ȡ��������б�
	QStringList listCmd = pCmdManager->GetRecentCommands();

	//�����˵�
	QMenu menuRecentCommands;
	//�˵��߶�
	int height = 0;
	for (auto iter = listCmd.constBegin();
		iter != listCmd.constEnd(); ++iter)
	{
		QAction* pAction = menuRecentCommands.addAction(*iter);
		height += 20;
		connect(pAction, SIGNAL(triggered()),
			this, SLOT(ExecuteRecentCommand()));
	}

	//�˵�λ��
	QPoint position = m_pUi->tbtnIcon->pos();
	position = m_pInterface->mapToGlobal(position);
	//��ȡ�˵��ĸ߶�
	//int height = menuRecentCommands.height();
	position.setY(position.y() - height);
	//ִ�в˵�
	menuRecentCommands.exec(position);
}

//ִ���������
void Console::ConsoleImpl
	::ExecuteRecentCommand()
{
	//���źŷ�����תΪQAction
	QAction* pActionSender = dynamic_cast<QAction*>(sender());

	//��ȡ����
	QString strCmd = pActionSender->text();

	//ִ������
	ExecuteCommand(strCmd);
}
//�������
void Console::ConsoleImpl
	::FinishCommand()
{
	ClearCurrentCommand();
}






/******************************************Parameters********************************/
Parameters::ParametersImpl
	::ParametersImpl(Parameters* pInterface)
	: m_pInterface(pInterface)
{
	//��ʼ����������·��
	{
		
	}
}

//���������ļ�����·��
void Parameters::ParametersImpl
	::AppendCommandDir(const QDir& dirCmd)
{
	//���·�������ڣ���׷��·��������ִ���κβ���
	if (!m_dirCommands.contains(dirCmd))
	{
		m_dirCommands.append(dirCmd);
	}
	else
	{
		return;
	}
}

//�Ƴ������ļ�����·��
void Parameters::ParametersImpl
	::RemoveCommandDir(const QDir& dirCmd)
{
	m_dirCommands.removeOne(dirCmd);
}

//��������·��
QList<QDir> Parameters::ParametersImpl
	::CommandDirs()
{
	return m_dirCommands;
}