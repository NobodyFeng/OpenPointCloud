#include "UnitManager.h"
#include <QSettings>
#include <QMessageBox>


UnitManager::UnitManager(QObject *parent)
	: QObject(parent)
	, m_PreviousUnit("")
{
	//��ת��ϵ����ʼ��Ϊ1
}

UnitManager::~UnitManager()
{

}
void UnitManager::SetUnit(QString unit)
{
	if (m_PreviousUnit == "")
	{
		InitUnit(unit);
	}

	m_pUnit = unit;

	UnitConversion();
}

void UnitManager::InitUnit(QString unit)
{
	m_PreviousUnit = unit;
	QSettings settings("DCLW", "MP");
	settings.beginGroup("MYUnit");
	settings.setValue("PreUnit",m_PreviousUnit);
	settings.endGroup();
	m_pScale = 1;
}

QString UnitManager::GetUnit()
{
	return m_pUnit;
}
//��λת��
void UnitManager::UnitConversion()
{
	QSettings settings("DCLW", "MP");
	settings.beginGroup("MYUnit");
	QString pre = settings.value("PreUnit").toString();
	settings.endGroup();
	QString cur = m_pUnit;
	if (cur != pre)
	{
		m_pScale = 1;
		/*********��ԭ��λת������*************/
		if (pre == "����")
		{
			m_pScale = m_pScale * 0.001;
			
		}
		else if (pre == "����")
		{
			m_pScale = m_pScale * 0.01;
			
		}
		else if (pre == "Ӣ��")
		{
			m_pScale = m_pScale * 0.30479999;
			
		}
		else if (pre == "��")
		{
			m_pScale = m_pScale *0.33333333;
			
		}
		else if (pre == "��")
		{
			m_pScale = m_pScale * 1;
			m_EnUnit = "m";
		}
		/******************����ת�������ڵĵ�λ***********************/
		if (cur == "����")
		{
			m_pScale = m_pScale * 100;
			m_EnUnit = "cm";

		}
		else if (cur == "����")
		{
			m_pScale = m_pScale * 1000;
			
			m_EnUnit = "mm";
		}
		else if (cur == "Ӣ��")
		{
			m_pScale = m_pScale * 3.2808399;
			m_EnUnit = "ft";
		}
		else if (cur == "��")
		{
			m_pScale = m_pScale * 3;
			m_EnUnit = "chi";
		}
	}
	else
	{
		m_pScale = 1;
	}

}

double UnitManager::GetScale()
{
	return m_pScale;
}
void UnitManager::SetScale(double scale)
{
	m_pScale = scale;
}
QString UnitManager::GetENUnit()
{
	return m_EnUnit;
}