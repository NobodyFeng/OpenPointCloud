#include "DCConsole/CommandManager.h"

//C++
#include <algorithm>

//Qt
#include "QString"
#include "QtAlgorithms"

//DCCore
#include "DCCore/Command.h"


#include "Impl/Impl_CommandManager.hxx"

using namespace DCConsole;

CommandManager* CommandManager::s_pInstance = nullptr;

//��ʼ�����������
CommandManager* CommandManager::Initialize()
{
	if (!s_pInstance)
	{
		s_pInstance = new CommandManager;
	}
	return s_pInstance;
}

//ж�����������
void CommandManager::Uninitalize()
{
	if (s_pInstance)
	{
		delete s_pInstance;
		s_pInstance = nullptr;
	}
}

//��ȡ���������ʵ��
CommandManager* CommandManager::Instance()
{
	return s_pInstance;
}

CommandManager::CommandManager()
{
	m_pImpl = new CommandManagerImpl(this);
}

CommandManager::~CommandManager()
{
	delete m_pImpl;;
}

//ע������
void CommandManager::RegisterCommand(const QString& strCmdName,
	const DCCore::CommandGenerator& generator)
{
	m_pImpl->RegisterCommand(strCmdName, generator);
}

void CommandManager::RegisterCommands(QMap<QString, DCCore::CommandGenerator> generators)
{
	m_pImpl->RegisterCommands(generators);
}

//��������
DCCore::CommandPtr CommandManager::GenerateCommand(const QString& strCmd)
{
	return m_pImpl->GenerateCommand(strCmd);
}

//��ȡ���ʹ�õ�����
QStringList CommandManager::GetRecentCommands() const
{
	return m_pImpl->GetRecentCommands();
}

//���ϲ����������
QString CommandManager::SearchUpRecentCommand()
{
	return m_pImpl->SearchUpRecentCommand();
}

//���²����������
QString CommandManager::SearchDownRecentCommand()
{
	return m_pImpl->SearchDownRecentCommand();
}