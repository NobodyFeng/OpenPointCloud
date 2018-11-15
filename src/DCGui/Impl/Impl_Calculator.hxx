#ifndef DC_GUI_IMPL_CALCULATOR_H
#define DC_GUI_IMPL_CALCULATOR_H

//Qt
#include "QtCore/QObject"

#include "../Calculator.h"

namespace Ui
{
	class Calculator;
}

namespace DcGui
{
	//������ʵ����
	class Calculator::CalculatorImpl : public QObject
	{
		Q_OBJECT
	public:
		explicit CalculatorImpl(Calculator* pInterface);
		~CalculatorImpl();

	private:
		//��ʼ��UI
		void InitUI();

		//�����ź���۵�����
		void CreateConnections();

	private slots:
		//�л�ģʽ
		void SwitchMode(bool bState);

	private:
		//�ӿ�
		Calculator* m_pInterface;

		//ui
		Ui::Calculator* m_pUi;
	};
}

#endif