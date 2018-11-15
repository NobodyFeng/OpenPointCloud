#ifndef DLGSAMPLINGRATE_H
#define DLGSAMPLINGRATE_H

#include <QtWidgets/QDialog>
#include "ui_DlgSamplingRate.h"

class DlgSamplingRate : public QDialog
{
	Q_OBJECT

public:
	DlgSamplingRate(QWidget *parent = 0);
	~DlgSamplingRate();

	static int Rate();

	int GetRate() const
	{
		return m_rate;
	}

private:
	Ui::DlgSamplingRate ui;

	//������
	int m_rate;

	//�����ź���۵�����
	void CreateConnentions();

private slots:
	//���²�����
	void SetRate(int rate);
};

#endif // DLGSAMPLINGRATE_H
