#include "Impl_Calculator.hxx"
using namespace DcGui;

//Self
#include "ui_Calculator.h"

Calculator::CalculatorImpl
	::CalculatorImpl(Calculator* pInterface)
	: m_pInterface(pInterface)
	, m_pUi(nullptr)
{
	//��ʼ��UI
	InitUI();

	//�����ź���۵�����
	CreateConnections();
}

Calculator::CalculatorImpl
	::~CalculatorImpl()
{
	delete m_pUi;
}

//��ʼ��UI
void Calculator::CalculatorImpl
	::InitUI()
{
	//����UI�ļ�
	m_pUi = new Ui::Calculator;
	m_pUi->setupUi(m_pInterface);

	//����Ĭ��Ϊ��׼ģʽ
	m_pUi->rbtnStandard->setChecked(true);
}

//�����ź���۵�����
void Calculator::CalculatorImpl
	::CreateConnections()
{
	//�л�����������
	connect(m_pUi->rbtnStandard, SIGNAL(toggled(bool)),
		this, SLOT(SwitchMode(bool)));
	connect(m_pUi->rbtnScientific, SIGNAL(toggled(bool)),
		this, SLOT(SwitchMode(bool)));
}

//�л�ģʽ
void Calculator::CalculatorImpl
	::SwitchMode(bool bState)
{
	//�źŷ�����
	QObject* pSender = sender();

	//��׼ģʽ
	if (pSender == m_pUi->rbtnStandard)
	{
		if (bState)
		{
			m_pUi->wgtScientific->setVisible(false);
			m_pInterface->setFixedWidth(m_pUi->wgtStandard->width() + 20);
		}
	}
	//��ѧģʽ
	else if (pSender == m_pUi->rbtnScientific)
	{
		if (bState)
		{
			m_pUi->wgtScientific->setVisible(true);
			m_pInterface->setFixedWidth(m_pUi->wgtStandard->width()
				+ m_pUi->wgtScientific->width() + 20);
		}
	}
}