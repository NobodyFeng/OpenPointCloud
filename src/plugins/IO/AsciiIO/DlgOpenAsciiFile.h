#ifndef DLGOPENASCIIFILE_H
#define DLGOPENASCIIFILE_H

//C++��׼��
#include <vector>

//Qt��׼��;
#include <QString>
#include <QStringList>
#include <QList>
#include <QtWidgets/QComboBox>
#include "QtWidgets/QDialog"

#include "ui_DlgOpenAsciiFile.h"

using std::vector;

//����ÿ������
enum DataTypePerColumn
{
	IGNORED = 0,
	COORD_X,
	COORD_Y,
	COORD_Z,
	RGB_RED,
	RGB_GREEN,
	RGB_BLUE,
	GRAY,
	SCALAR, 
	NORMAL_X,
	NORMAL_Y,
	NORMAL_Z
};


struct DataColumnType
{
	int xCoordIndex;
	int yCoordIndex;
	int zCoordIndex;
	int rgbRedIndex;
	int rgbGreenIndex;
	int rgbBlueIndex;
	int grayIndex;
	QList<unsigned> scalarIndex;
	int xNormalIndex;
	int yNormalIndex;
	int zNormalIndex;

	//�Ƿ��������ֵ
	bool m_hasRGBColor;
	//�Ƿ��������ֵ
	bool m_hasNormals;
	//��Ч��������
	unsigned m_validTypeNumbers;


	//��ʼ��
	DataColumnType()
		: xCoordIndex(-1)
		, yCoordIndex(-1)
		, zCoordIndex(-1)
		, rgbRedIndex(-1)
		, rgbGreenIndex(-1)
		, rgbBlueIndex(-1)
		, grayIndex(-1)
		, m_validTypeNumbers(0)
		, m_hasNormals(false)
		, m_hasRGBColor(false)
		, xNormalIndex(-1)
		, yNormalIndex(-1)
		, zNormalIndex(-1)
	{};

	//�Ƿ�ӵ��RGB��ɫ
	bool HasRGBColors() const 
	{
		return m_hasRGBColor;
	}
	//�Ƿ�ӵ������
	bool HasNormals() const 
	{
		return m_hasNormals;
	}
	//��ȡ��Ч����������
	unsigned ValidTypeNumber() const
	{
		return m_validTypeNumbers;
	}
};

//Ascii�ļ�Ԥ���������
const int MAX_LINES_OF_ASCII_FILE = 500;

class DlgOpenAsciiFile : public QDialog
{
	Q_OBJECT

public:
	//Ĭ�Ϲ��캯��;
	DlgOpenAsciiFile(QString fileName, QWidget *parent = 0);

	//��������;
	~DlgOpenAsciiFile();

	//��Ա����;

	//��ȡ����
	unsigned GetDataColumnNumber() {return m_columnNumber;}
	//��ȡ��������
	unsigned GetDataLines() {return m_readLines;}
	//��ȡ����������
	DataColumnType GetDataColumnType();
	//��ȡ������
	double GetMaxCloudSize() {return m_maxCloudSize * 1000000;}
	//��ȡ�ָ���;
	QChar GetSeparator() {return m_separator;}
	//��ȡ����������;
	unsigned GetSkippedLines() {return m_skippedLines;}

	//��ȡ������
	int GetSampleRate() const;

private:
	Ui::DlgOpenAsciiFile ui;
	//��Ա����;

	//�ļ���;
	QString m_fileName;
	//�ָ���;
	QChar m_separator;
	//���ݵ�����
	unsigned m_columnNumber;
	//����������
	unsigned m_skippedLines;
	//�ļ���Ҫ���������
	unsigned m_readLines;
	//ÿ��������������
	double m_maxCloudSize;
	//�Ƿ��ֶ���������������
	bool m_isSettedColumnType;
	//�������������б�
	vector<QComboBox*> m_dataType;
	vector<DataTypePerColumn> m_header;
	//ͷ��Ϣ
	static DataTypePerColumn dataHeaders[12];
	//��Ա����;

	//�����ı�Ԥ����������;
	void AddDataToTable(QStringList lines, QChar separator);
	//Ϊ�ı�Ԥ������������������ѡ��������б�;
	void AddDataTypeComboBox(int columnNumber);
	//ʹ�÷ָ�����Ԥ���ı����зֿ飬�������ݿ�ĸ���;
	unsigned GetDataColumnNumber(const QStringList&, const QChar&);
	//��ȡ�ָ���;
	QChar GetSeparator(const QString&);
	////��ȡ����;
	QStringList LoadDataFromFile();
	//����Ĭ�ϵ�����������
	void SetDefaultColumnType();
	//����ı�Ԥ������壬��������Ӧ������;
	void SetTableWidget(int rowNumber, int columnNumber);
	//���±�����
	void UpdateTable();

//�����;
private slots:
	//�ı��е���������
	void ChangeColumnType(int);
	//����ո�tab�����š��ֺŷָ���;
	void ClickedSeparator();
	//�������ݽṹ;
	void SetMaxCloudSize();
	//���÷ָ���
	void SetSeparator();
	//������������;
	void SetSkippedLines();
	////��ȡ���ݵ�����;
		//void getDataType(QComboBox *dataType);

protected:


	//��Ա����;

	//��д�ػ淽��;
	//void paintEvent(QPaintEvent *event);
};

#endif // DLGOPENASCIIFILE_H
