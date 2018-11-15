//C++��׼��
#include <algorithm>

//Qt��׼��;
#include <QFile>
#include <QTextStream>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QScrollBar>
#include <QRegExp>
#include "QtGui/QColor"

#include "DlgOpenAsciiFile.h"

DataTypePerColumn DlgOpenAsciiFile::dataHeaders[12] = {IGNORED, COORD_X, COORD_Y, COORD_Z, 
	RGB_RED, RGB_GREEN, RGB_BLUE, GRAY, SCALAR, NORMAL_X, NORMAL_Y, NORMAL_Z};

DlgOpenAsciiFile::DlgOpenAsciiFile(QString fileName, QWidget *parent)
	: QDialog(parent)
	, m_fileName(fileName)
	, m_separator(',')
	, m_columnNumber(0)
	, m_readLines(0)
	, m_skippedLines(0)
	, m_maxCloudSize(3.0)
	, m_isSettedColumnType(false)
{
	ui.setupUi(this);

	ui.pushBtnOk->setFocus();

	//����ͼ��
	/*QIcon icon;
	icon.addFile(QString::fromUtf8(":/MainWindow/Resources/images/lpcp.ico"), QSize(), QIcon::Normal, QIcon::Off);
	this->setWindowIcon(icon);*/

	//�����ļ����ı����ֵ
	ui.txtFileName->setText(m_fileName);
	
	//����Ĭ�Ϸָ����б�
	QList<QChar> separators;
	separators << QChar(' ');
	separators << QChar('\t');
	separators << QChar(',');
	separators << QChar(';');

	//�������е�Ĭ�Ϸָ�����ƥ���ļ��еķָ���
	for (unsigned i = 0; i < separators.size(); ++i)
	{
		ui.txtSeparator->setText(separators[i]); //�˹��̻�ˢ�±��
		//ֱ��������������2Ϊֹ����Ϊ�������Ϊ��ά
		if (GetDataColumnNumber() > 2)
		{
			
			break;
		}
	}
}

DlgOpenAsciiFile::~DlgOpenAsciiFile()
{

}

//�����ı�Ԥ����������;
//��һ������ΪҪ������ݵı���壬�ڶ���������Ҫ��ӵ��ı�����;
//���������ı��ķָ���;
void DlgOpenAsciiFile::AddDataToTable(QStringList lines,
	QChar separator)
{
	bool isValid = true;
	//�ӵ�һ�п�ʼ����ı�����Ϊ��һ����ʾѡ�������б�;
	for (int i = 0; i < lines.size(); ++i)
	{
		//ʹ�÷ָ����ָ�ÿ���ı�;
		QStringList cellStings = lines[i].trimmed().split(separator, QString::SkipEmptyParts);

		//���ı���ӵ��ı�Ԥ�����ĵ�Ԫ����;
		for (int j = 0; j < cellStings.size(); ++j)
		{
			//���������Ŀ����;
			QTableWidgetItem *item = new QTableWidgetItem(cellStings[j], QTableWidgetItem::Type);

			//���ñ���ɫ�����л�ɫ��ͻ����ʾ;
			if (i % 2 == 0)
			{
				item->setBackgroundColor(QColor(180, 180, 180));
			}

			//����ı�����ڷǷ��ַ��������ñ���Ϊ��ɫ;
			//�Ƿ��ַ�Ϊ������0-9��С����֮��������ַ�;
			if (!cellStings[j].contains(QRegExp("^\-?[0-9]*\.[0-9]*(e|E)?\-?[0-9]*$")) && m_header[j] != IGNORED)
			{
				isValid = false;
				item->setBackgroundColor(QColor(255, 0, 0));
			}

			//�������Ŀ��ӵ���Ԫ����;
			ui.tblAsciiFile->setItem(i + 1, j, item);
		}
	}
	//����ȷ����ť�Ŀɲ�����
	ui.pushBtnOk->setEnabled(isValid);
}

//Ϊ�ı�Ԥ������������������ѡ��������б�;
//����Ϊ�ı����ݵ�����;
void DlgOpenAsciiFile::AddDataTypeComboBox(int columnNumber)
{
	m_dataType.clear();
	//ÿ�������б�Ķ�����
	QStringList typeList = QStringList() << tr("Ignore") << tr("XCoord") << tr("YCoord") << tr("ZCoord")
							<< tr("Red") << tr("Green") << tr("Blue") << tr("Gray") << tr("XNORMAL") << tr("YNORMAL") << tr("ZNORMAL");
	for (unsigned i = 11; i < columnNumber; ++i)
	{
		typeList.push_back(tr("Scalar"));
	}
	//ͨ��ѭ������ѡ�������б�;
	for (int i = 0; i < columnNumber; ++i)
	{
		//������������ݽṹѡ�������б�;
		QComboBox *dataType = new QComboBox;

		//�����б����������;
		dataType->addItem(tr("Ignore"), Qt::DisplayRole);
		dataType->addItem(QIcon(":/Dialog/Resources/images/ComboBox/xCoord.png"), tr("XCoord"), Qt::DisplayRole);
		dataType->addItem(QIcon(":/Dialog/Resources/images/ComboBox/yCoord.png"), tr("YCoord"), Qt::DisplayRole);
		dataType->addItem(QIcon(":/Dialog/Resources/images/ComboBox/zCoord.png"), tr("ZCoord"), Qt::DisplayRole);
		dataType->addItem(QIcon(":/Dialog/Resources/images/ComboBox/rgb.png"), tr("Red"), Qt::DisplayRole);
		dataType->addItem(QIcon(":/Dialog/Resources/images/ComboBox/rgb.png"), tr("Green"), Qt::DisplayRole);
		dataType->addItem(QIcon(":/Dialog/Resources/images/ComboBox/rgb.png"), tr("Blue"), Qt::DisplayRole);
		dataType->addItem(QIcon(":/Dialog/Resources/images/ComboBox/gray.png"), tr("Gray"), Qt::DisplayRole);
		dataType->addItem(QIcon(":/Dialog/Resources/images/ComboBox/scalar.png"), tr("Scalar"), Qt::DisplayRole);

		dataType->addItem(QIcon(":/logo/Resource/xnormal.png"), tr("XNORMAL"), Qt::DisplayRole);
		dataType->addItem(QIcon(":/logo/Resource/ynormal.png"), tr("YNORMAL"), Qt::DisplayRole);
		dataType->addItem(QIcon(":/logo/Resource/znormal.png"), tr("ZNORMAL"), Qt::DisplayRole);

		//����Ĭ��ֵ
		dataType->setCurrentIndex(m_header[i]);
		dataType->setObjectName(typeList[i]);

		//�������б���ӵ���Ԫ����;
		ui.tblAsciiFile->setCellWidget(0, i, dataType);

		//�����ź����;
		connect(dataType, SIGNAL(currentIndexChanged(int)), this, SLOT(ChangeColumnType(int)));

		m_dataType.push_back(dataType);

	}
}

//�ı�����������
void DlgOpenAsciiFile::ChangeColumnType(int index)
{
	QObject* objectSender = sender();

	if (!objectSender)
	{
		return;
	}
	//ת��ΪQComboBox
	QComboBox* cmbxSender = static_cast<QComboBox*>(objectSender);
	//�ж��Ƿ���ֳ�ͻ������ڵ������ظ�
	//����ظ�����н���
	auto repetition = std::find_if(m_dataType.cbegin(), m_dataType.cend(), [cmbxSender](QComboBox* cmbxTemp)
							{return (cmbxTemp->currentIndex() == cmbxSender->currentIndex() && cmbxTemp != cmbxSender);});
	//���ҵ�ǰ�����źŵĶ����������е�λ��
	unsigned pos = 0;
	std::find_if(m_dataType.cbegin(), m_dataType.cend(), [cmbxSender, &pos](QComboBox* cmbxTemp)->bool
		{++pos;return (cmbxTemp == cmbxSender);});
	//�����ظ������ͣ����ѡ����Ǻ����򲻽���
	if (repetition != m_dataType.cend() && (*repetition)->currentIndex() != IGNORED && (*repetition)->currentIndex() != SCALAR)
	{
		(*repetition)->setCurrentIndex(m_header[pos - 1]);
	}
	//��������������
	//���������
	m_header.clear();
	//�ٸ�������
	for (auto i = m_dataType.cbegin(); i != m_dataType.cend(); ++i)
	{
		m_header.push_back(dataHeaders[(*i)->currentIndex()]);
	}

	m_isSettedColumnType = true;

	UpdateTable();
}

//����ָ�����ť
void DlgOpenAsciiFile::ClickedSeparator()
{
	/*if (!m_columnsCount)
		return;*/

	//we get the signal sender
	QObject* obj = sender();
	if (!obj)
		return;

	//it should be a QToolButton (could we test this?)
	QToolButton* btnSeparator = static_cast<QToolButton*>(obj);

	uchar newSeparator = 0;
	if (btnSeparator == ui.toolBtnSpace)
		newSeparator = uchar(32);
	else if (btnSeparator == ui.toolBtnTab)
		newSeparator = uchar(9);
	else if (btnSeparator == ui.toolBtnComma)
		newSeparator = uchar(44);
	else if (btnSeparator == ui.toolBtnSemicolon)
		newSeparator = uchar(59);

	if (newSeparator > 0 && GetSeparator() != newSeparator)
		ui.txtSeparator->setText(QChar(newSeparator));
}

//��ȡ�ı�;
QStringList DlgOpenAsciiFile::LoadDataFromFile()
{
	//��ȡ�ļ��豸()
	QFile file(m_fileName);

	//���ļ�;
	file.open(QIODevice::ReadOnly | QIODevice::Text);

	QTextStream in(&file);
	//�ļ��е�����
	QStringList fileData;
	//��¼�������ݵ��ֽ���
	unsigned number = 500;
	//ͨ����������������������
	unsigned count = 0;
	while(!in.atEnd())
	{
		if (++count <= m_skippedLines)
		{
			QString line = in.readLine();
			continue;
		}
		//���Ԥ��500������
		if (fileData.size() == MAX_LINES_OF_ASCII_FILE)
		{
			break;
		}
		//��ȡ�ļ���ÿ������;
		QString line = in.readLine();
		number = line.size() > number ? number : line.size();

		fileData.push_back(line);
	}

	//
	m_readLines = file.size() / number;

	//�ر��ļ�;
	file.close();

	//ͨ���ļ�����������������������Ŀ
	//ȷ�����������ܱ���һ������
	ui.spinBoxSkipLines->setMaximum(count - 1);

	//��������
	return fileData;
}

//�������ݵ�����;
unsigned DlgOpenAsciiFile::GetDataColumnNumber(const QStringList& lines, const QChar& separator)
{
	unsigned columnNumber = 0;
	unsigned count = 0;
	for (auto i = lines.constBegin(); i != lines.constEnd(); ++i, ++count)
	{
		//������ǰ50��
		if (count == 50)
		{
			break;
		}
		//���ı�ʹ�÷ָ������룬��ȥ���������˵Ŀո�;
		QStringList list = (*i).trimmed().split(separator, QString::SkipEmptyParts);
		//columnNumber = MAX(columnNumber, list.count());
		columnNumber = columnNumber > list.count() ? columnNumber : list.count();
	}

	//�������ݵ�����;
	return columnNumber;
}

//��ȡ����������
DataColumnType DlgOpenAsciiFile::GetDataColumnType()
{
	DataColumnType header;

	//ȷ��������Ϊ0
	if (m_columnNumber > 0)
	{
		//ѭ��ȷ��ÿ�������б��ֵ
		for(int i = 0; i < m_columnNumber; ++i)
		{
			//��ȡ�����б�
			QComboBox* cur = static_cast<QComboBox*>(ui.tblAsciiFile->cellWidget(0, i));
			
			if (IGNORED == cur->currentIndex())
			{
			}
			else if (COORD_X == cur->currentIndex())
			{
				header.xCoordIndex = i;
				++header.m_validTypeNumbers;
			} 
			else if (COORD_Y == cur->currentIndex())
			{
				header.yCoordIndex = i;
				++header.m_validTypeNumbers;
			}
			else if (COORD_Z == cur->currentIndex())
			{
				header.zCoordIndex = i;
				++header.m_validTypeNumbers;
			}
			else if (RGB_RED == cur->currentIndex())
			{
				header.rgbRedIndex = i;
				++header.m_validTypeNumbers;
				header.m_hasRGBColor = true;
			}
			else if (RGB_GREEN == cur->currentIndex())
			{
				header.rgbGreenIndex = i;
				++header.m_validTypeNumbers;
				header.m_hasRGBColor = true;
			}
			else if (RGB_BLUE == cur->currentIndex())
			{
				header.rgbBlueIndex = i;
				++header.m_validTypeNumbers;
				header.m_hasRGBColor = true;
			}
			else if (GRAY == cur->currentIndex())
			{
				header.grayIndex = i;
				++header.m_validTypeNumbers;
			}
			else if (SCALAR == cur->currentIndex())
			{
				header.scalarIndex.push_back(i);
				++header.m_validTypeNumbers;
			}
			else if (NORMAL_X == cur->currentIndex())
			{
				header.xNormalIndex = i;
				++header.m_validTypeNumbers;
				header.m_hasNormals = true;
			}
			else if (NORMAL_Y == cur->currentIndex())
			{
				header.yNormalIndex = i;
				++header.m_validTypeNumbers;
				header.m_hasNormals = true;
			}
			else if (NORMAL_Z == cur->currentIndex())
			{
				header.zNormalIndex = i;
				++header.m_validTypeNumbers;
				header.m_hasNormals = true;
			}

		}
	}
	

	return header;
}

//��ȡ�ı��ķָ���()
QChar DlgOpenAsciiFile::GetSeparator(const QString& line)
{
	//��ȡ�ַ�����ÿ���ַ��ĵ�ַ;
	QChar *ch = const_cast<QString&>(line).data();

	//�����ʶ��������ַ�����(������С����)������ڷָ���;
	//���򲻴��ڷָ���;
	bool flag = false;

	while(!ch->isNull())
	{
		//�ж��ַ��Ƿ������ֻ�С����򸺺Ż���ַ�;
		if (ch->isNumber() || *ch == QChar(45)/*����;*/ 
			|| *ch == QChar(46)/*С����;*/ || *ch == QChar(0)/*'\0'*/)
		{
			flag = false;
		} 
		else
		{
			flag = true;
			break;
		}

		//��ָ���Ƶ������һ���ַ���ַ;
		++ch;
	}

	if (flag)
	{
		//����Ѱ�ҵ��ķָ���;
		return *ch;
	} 
	else
	{
		//Ĭ�϶���Ϊ�ָ���()
		return ',';
	}
}

//����Ĭ�ϵ�����������
void DlgOpenAsciiFile::SetDefaultColumnType()
{
	//�������������
	m_header.clear();

	//����Ĭ�ϵ�����������
	switch (GetDataColumnNumber())
	{
		case 1:
			m_header.push_back(IGNORED);
			break;
		case 2:
			m_header.push_back(COORD_X);
			m_header.push_back(COORD_Y);
			break;
		case 3:
			m_header.push_back(COORD_X);
			m_header.push_back(COORD_Y);
			m_header.push_back(COORD_Z);
			break;
		case 6:
			m_header.push_back(COORD_X);
			m_header.push_back(COORD_Y);
			m_header.push_back(COORD_Z);
			m_header.push_back(RGB_RED);
			m_header.push_back(RGB_GREEN);
			m_header.push_back(RGB_BLUE);
			break;
// 		case 4:
// 			m_header.push_back(COORD_X);
// 			m_header.push_back(COORD_Y);
// 			m_header.push_back(COORD_Z);
// 			m_header.push_back(SCALAR);
// 			break;
// 		case 5:
// 			m_header.push_back(COORD_X);
// 			m_header.push_back(COORD_Y);
// 			m_header.push_back(COORD_Z);
// 			m_header.push_back(SCALAR);
// 			m_header.push_back(SCALAR);
// 			break;
		default:
			m_header.push_back(COORD_X);
			m_header.push_back(COORD_Y);
			m_header.push_back(COORD_Z);
// 			m_header.push_back(RGB_RED);
// 			m_header.push_back(RGB_GREEN);
// 			m_header.push_back(RGB_BLUE);
			for (unsigned i = 3; i != GetDataColumnNumber(); ++i)
			{
				//m_header.push_back(SCALAR);
				m_header.push_back(IGNORED);
			}
			break;
	}
}

//����������
void DlgOpenAsciiFile::SetMaxCloudSize()
{
	m_maxCloudSize = ui.dblSpinBoxMaxPoints->value();
}

//���÷ָ���
void DlgOpenAsciiFile::SetSeparator()
{
	//���÷ָ���
	//ֻ�е��ָ����仯ʱ�Ÿ��´��壬����������������ΪĬ��
	if (m_separator != ui.txtSeparator->text()[0])
	{
		m_separator = ui.txtSeparator->text()[0];
		m_isSettedColumnType = false;
		//��ʾ�ָ�����ASCII��
		ui.lblAsciiCode->setText(tr("ASCII code %1").arg(int(m_separator.toLatin1())));
		//ˢ�±�
		UpdateTable();
	}
}

//��������������
void DlgOpenAsciiFile::SetSkippedLines()
{
	m_skippedLines = ui.spinBoxSkipLines->value();
	m_readLines -= m_skippedLines;
	//���±�����
	UpdateTable();
}

//����ı�Ԥ������壬��������Ӧ������;
void DlgOpenAsciiFile::SetTableWidget(int rowNumber, int columnNumber)
{
	//�����ı�Ԥ�����ĵ�Ԫ�񲻿ɲ���;
	ui.tblAsciiFile->setEditTriggers(QAbstractItemView::NoEditTriggers);

	//������������ʵ�����ݶ�һ�У�����������������ѡ�������б�;
	ui.tblAsciiFile->setRowCount(rowNumber);

	//��������;
	ui.tblAsciiFile->setColumnCount(columnNumber);

	//��ֱ�������Ĵ�С;
	//QSize scrSize = ui.tblAsciiFile->verticalScrollBar()->size();
	//����ÿ�б�����С���;
	//ȷ�������пɼ��ı���������Ϊ5��
// 	int minSize = 0;
// 	const int MAXCOLUMN = 5;
// 	if (columnNumber <= MAXCOLUMN)
// 	{
// 		minSize = ui.tblAsciiFile->size().width() / columnNumber;
// 	} 
// 	else
// 	{
// 		minSize = ui.tblAsciiFile->size().width() / MAXCOLUMN;
// 	}
	const unsigned MINSIZE = 100;
	//�����е�Ԫ�����С���;
	/*ui.tblAsciiFile->horizontalHeader()->setMinimumSectionSize(minSize);*/
	ui.tblAsciiFile->horizontalHeader()->setMinimumSectionSize(MINSIZE);
	//�����е�Ԫ���Ĭ�Ͽ��;
	/*ui.tblAsciiFile->horizontalHeader()->setDefaultSectionSize(minSize);*/
	ui.tblAsciiFile->horizontalHeader()->setDefaultSectionSize(MINSIZE);
}

//�������ݱ��
void DlgOpenAsciiFile::UpdateTable()
{
	//ui.tblAsciiFile->set Enabled(false);
	//�ж��ļ����Ƿ�Ϊ��
	if (m_fileName.isEmpty())
	{
		ui.tblAsciiFile->clear();
		return;
	}
	else
	{
		//����ļ��������򷵻��ϲ㺯��
		if (!QFile::exists(m_fileName))
		{
			return;
		}
	}

	//�ж��Ƿ���ڷָ���
	if (m_separator == 0)
	{
		ui.lblAsciiCode->setText(tr("Enter a valid character!"));
		ui.pushBtnOk->setEnabled(false);
		ui.tblAsciiFile->clear();
		return;
	} 
	else
	{
		//����ָ���Ƿ�Ϊ���֡�С����򸺺�
		if (m_separator.isNumber() || m_separator == 45/*����;*/ 
			|| m_separator == 46/*С����;*/ )
		{
			ui.lblAsciiCode->setText(tr("Enter a legal character!"));
			ui.pushBtnOk->setEnabled(false);
			ui.tblAsciiFile->clear();
			return;
		}
		else
		{
			ui.pushBtnOk->setEnabled(true);
		}
	}

	//��ȡ�ļ�����
	QStringList fileData = LoadDataFromFile();

	//�����ı�������;
	int rowNumber = fileData.size() + 1;

	//�����ı�������;
	m_columnNumber = GetDataColumnNumber(fileData, m_separator);

	//����ı�Ԥ������壬��������Ӧ������;
	SetTableWidget(rowNumber, m_columnNumber);

	//����Ĭ�ϵ�����������
	if (!m_isSettedColumnType)
	{
		SetDefaultColumnType();
	}

	//Ϊ�ı�Ԥ������������������ѡ��������б�;
	AddDataTypeComboBox(m_columnNumber);

	//�������ݱ��;
	AddDataToTable(fileData, m_separator);
}

int DlgOpenAsciiFile::GetSampleRate() const
{
	return ui.cmbxSamplingRate->currentIndex() + 1;
}