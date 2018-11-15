#include "DCConsole/Console.h"
#include "DCCore/DCConfig.h"

using namespace DCConsole;

//self
#include "Impl/Impl_Console.hxx"

Console* Console::s_pInstance = nullptr;

//��ʼ������̨
Console* Console::Initialize()
{
	if (!s_pInstance)
	{
		s_pInstance = new Console;
	}

	return s_pInstance;
}

//ж�ؿ���̨
void Console::Uninitialize()
{
	delete s_pInstance;
	s_pInstance = nullptr;
}

//��ȡ����̨ʵ��
Console* Console::Instance()
{
	return s_pInstance;
}


Console::Console(QWidget* pParent /* = nullptr */)
	: QDockWidget(pParent)
{
	m_pImpl = new ConsoleImpl(this);
	setWindowTitle(("����̨"));
	setObjectName("Console");
}

Console::~Console()
{
	delete m_pImpl;
}

//ִ������
void Console::ExecuteCommand(const QString& strCmd)
{
	m_pImpl->ExecuteCommand(strCmd);
}

//��ȡ����ִ�е�����
DCCore::CommandPtr Console::GetRunningCommand() const
{
	return m_pImpl->GetRunningCommand();
}

//ȡ������
void Console::CancelRunningCommand()
{
	m_pImpl->CancelCommand();
}






/********************************Parameters***********************************/
Parameters Parameters::s_instance;

Parameters::Parameters()
{
	m_pImpl = new ParametersImpl(this);
}

Parameters::~Parameters()
{
	delete m_pImpl;
}

//���������ļ�����·��
void Parameters::AppendCommandDir(const QDir& dirCmd)
{
	s_instance.m_pImpl->AppendCommandDir(dirCmd);
}

//�Ƴ������ļ�����·��
void Parameters::RemoveCommandDir(const QDir& dirCmd)
{
	s_instance.m_pImpl->RemoveCommandDir(dirCmd);
}

//��������·��
QList<QDir> Parameters::CommandDirs()
{
	return s_instance.m_pImpl->CommandDirs();
}