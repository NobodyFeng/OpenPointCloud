//Qt
#include "QtWidgets/QApplication"
#include "DlgSamplingRate.h"

static DlgSamplingRate* s_dlgSamplingRate;

DlgSamplingRate::DlgSamplingRate(QWidget *parent)
	: QDialog(parent)
	, m_rate(1)
{
	ui.setupUi(this);

	//����Ĭ�ϲ�����Ϊ1/2
	ui.cmbxSamplingRate->setCurrentIndex(1);
	m_rate = 2;

	CreateConnentions();
}

DlgSamplingRate::~DlgSamplingRate()
{

}

//�����ź���۵�����
void DlgSamplingRate::CreateConnentions()
{
	//
	connect(ui.cmbxSamplingRate, SIGNAL(currentIndexChanged(int)), this, SLOT(SetRate(int)));

	//���ȷ��
	connect(ui.btnxAffirm, SIGNAL(accepted()), this, SLOT(accept()));

	//���ȡ���رնԻ���
	connect(ui.btnxAffirm, SIGNAL(rejected()), this, SLOT(reject()));
}

//���ò�����
void DlgSamplingRate::SetRate(int rate)
{
	m_rate = rate + 1;
}

//��ȡ������
int DlgSamplingRate::Rate()
{
	s_dlgSamplingRate = new DlgSamplingRate(QApplication::activeWindow());

	if (s_dlgSamplingRate->exec())
	{
		return s_dlgSamplingRate->GetRate();
	}
	else
	{
		return -1;
	}
}