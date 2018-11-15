#ifndef UNITMANAGER_H
#define UNITMANAGER_H

#include <QObject>

struct UnitInfo
{
	QString PreUnit;
};

class UnitManager : public QObject
{
	Q_OBJECT

public:
	UnitManager(QObject *parent);
	~UnitManager();

	QString GetUnit();
	void SetUnit(QString unit);

	QString GetENUnit();

	void SetScale(double scale);
	double GetScale();
private:
	//��ʼ��
	void InitUnit(QString unit);
	//��λת��
	void UnitConversion();
	//��¼��ǰ�ĵ�λ
	void RecordPreUnit();

private:
	QString m_PreviousUnit;
	QString m_pUnit;
	double m_pScale;
	QString m_EnUnit;
	UnitInfo uninfo;
};



#endif // UNITMANAGER_H
