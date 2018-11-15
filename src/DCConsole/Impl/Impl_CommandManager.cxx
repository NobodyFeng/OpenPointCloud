#include "Impl_CommandManager.hxx"
using namespace DCConsole;

//Qt
#include "QtCore/QDir"
#include "QtCore/QList"
#include "QtCore/QPluginLoader"

#include "QtWidgets/QApplication"
#include "QtWidgets/QMessageBox"
#include "QtWidgets/QFileDialog"


//Self
#include "DCConsole/Console.h"

CommandManager::CommandManagerImpl
	::CommandManagerImpl(CommandManager* pInterface)
	: m_pInterface(pInterface)
{
	OutputRegisteredCommand::s_commands = &m_commandGenerators;
	//ע������
	RegisterCommand(OutputRegisteredCommand::GlobalName(), 
		OutputRegisteredCommand::Generate);

	//��������
	//LoadCommands();
}

//��������
CommandManager::CommandManagerImpl
	::~CommandManagerImpl()
{
	OutputRegisteredCommand::s_commands = nullptr;
}

//���������Ƿ���ע��
bool CommandManager::CommandManagerImpl
	::IsRegistered(const QString& strCmdName) const
{
	return m_commandGenerators.contains(strCmdName);
}

//ע������
void CommandManager::CommandManagerImpl
	::RegisterCommand(const QString& strCmdName,
	const DCCore::CommandGenerator& generator)
{
	//�����Ƿ���ע��
	//����ע�ᣬ�򷵻�false
	//����ע��
	if (IsRegistered(strCmdName))
	{
		QMessageBox::warning(QApplication::activeWindow(),
			tr("Warning"), tr("Command \"%1\" already exists!").arg(strCmdName));
	}
	else
	{
		m_commandGenerators.insert(strCmdName, generator);
	}
}
void CommandManager::CommandManagerImpl
	::RegisterCommands(QMap<QString, DCCore::CommandGenerator> generators)
{
	if (generators.isEmpty())
	{
		return;
	}
	for (auto iter = generators.constBegin();
		iter != generators.constEnd(); ++iter)
	{
		RegisterCommand(iter.key(), iter.value());
	}
}

//��������
DCCore::CommandPtr CommandManager::CommandManagerImpl
	::GenerateCommand(const QString& strCmd)
{
	DCCore::CommandPtr pCmd;

	//ȷ��������ִ�Сд��������ͳһתΪ��д
	QString strUpperCmd = strCmd.toUpper();

	//���Ҵ�ע������������Ƿ����
	if (IsRegistered(strUpperCmd))
	{
		//ͨ��������������������
		pCmd = m_commandGenerators.value(strUpperCmd)();

		//��¼����
		RecordCommand(pCmd);
	}

	return pCmd;
}

//��¼����
void CommandManager::CommandManagerImpl
	::RecordCommand(DCCore::CommandPtr pCmd)
{
	//ֻ��¼��������,�����¼������5������Ӷ����Ƴ�
	if (m_cmdStack.count() >= c_cmdStackSize)
	{
		m_cmdStack.pop_front();
	}

	//��¼����
	m_cmdStack.append(pCmd);

	//��������
	QString strCmdName = pCmd->GetGlobalName();

	//��¼��������
	if (m_recentCommands.count() >= c_recentCmdCount)
	{
		m_recentCommands.pop_front();
	}

	if (!m_recentCommands.contains(strCmdName))
	{
		m_recentCommands.append(strCmdName);
		m_currentRecentCmdIndex = m_recentCommands.count();
	}
}

//��ȡ���ʹ�õ�����
QStringList CommandManager::CommandManagerImpl
	::GetRecentCommands() const
{
	return m_recentCommands;
}

//���ϲ����������
QString CommandManager::CommandManagerImpl
	::SearchUpRecentCommand()
{
	if (--m_currentRecentCmdIndex < 0)
	{
		m_currentRecentCmdIndex = 0;
	}

	return m_recentCommands.value(m_currentRecentCmdIndex);
}

//���²����������
QString CommandManager::CommandManagerImpl
	::SearchDownRecentCommand()
{
	//�������������������������ʹ�����������ʱ
	//����һ����������֤����ʼ�ղ�����
	if (++m_currentRecentCmdIndex >= m_recentCommands.count())
	{
		--m_currentRecentCmdIndex;
		return "";
	}

	return m_recentCommands.value(m_currentRecentCmdIndex);
}

//��������
void CommandManager::CommandManagerImpl
	::LoadCommands()
{
	//��ȡ��������·��
// 	QList<QDir> cmdDirs = Parameters::CommandDirs();
// 	QDir dir = QApplication::applicationDirPath() + "/Plugins";
// 	cmdDirs.append(dir);
// 
// 	for (auto iter = cmdDirs.constBegin();
// 		iter != cmdDirs.constEnd(); ++iter)
// 	{
// 		//ģ��·��
// 		QDir cmdDir = *iter;
// 
// 		//ģ���б�
// 		QStringList commands = cmdDir.entryList(QStringList() << "*.mdl",
// 			QDir::NoFilter, QDir::Time | QDir::Reversed);
// 
// 		for (auto iter_0 = commands.constBegin();
// 			iter_0 != commands.constEnd(); ++iter_0)
// 		{
// 			//ģ�������
// 			QPluginLoader loader(cmdDir.absoluteFilePath(*iter_0));
// 
// 			//���ģ���ʶ�������
// 			if (CommandRegistrant* pCmdRegistrant = qobject_cast<CommandRegistrant*>(loader.instance()))
// 			{
// 				//��ȡ����������
// 				QMap<QString, CommandGenerator> generators = pCmdRegistrant->Generators();
// 
// 				//ע������
// 				for (auto iter_1 = generators.constBegin();
// 					iter_1 != generators.constEnd(); ++iter_1)
// 				{
// 					RegisterCommand(iter_1.key(), iter_1.value());
// 				}
// 			}
// 		}
// 	}
}





/****************************************���ע�������*****************************************/
QMap<QString, DCCore::CommandGenerator>* OutputRegisteredCommand::s_commands = nullptr;

//����������
DCCore::CommandPtr OutputRegisteredCommand::Generate()
{
	return OutputRegisteredCommandPtr(new OutputRegisteredCommand);
}

//ȫ������
QString OutputRegisteredCommand::GlobalName()
{
	return "$$OUTPUTCOMMANDS$$";
}
QString OutputRegisteredCommand::GetGlobalName() const
{
	return GlobalName();
}

//�������Ϣ
QString OutputRegisteredCommand::GetDesigner() const
{
	return tr("DCLW");
}

//������Ϣ
QString OutputRegisteredCommand::GetDescription() const
{
	return tr("Output registered commands.");
}

//ִ������
void OutputRegisteredCommand::Execute()
{
	//ѡ��Ҫ������ļ�·��
	QString strFileName = QFileDialog::getSaveFileName(QApplication::activeWindow(),
		tr("Save Registered Commands"), "", "Excel File(*.xls)");

	//��������
	if (s_commands && !strFileName.isEmpty())
	{
		SaveCommands(strFileName, *s_commands);
	}
}

//����������浽ָ���ļ���
void OutputRegisteredCommand::SaveCommands(const QString& strFileName,
	QMap<QString, DCCore::CommandGenerator> commands)
{
	
}