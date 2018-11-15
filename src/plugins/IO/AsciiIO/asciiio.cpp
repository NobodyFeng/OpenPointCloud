#include "asciiio.h"

//C++
#include <array>

//Qt��׼��
#include "QFile"
#include "QTime"
#include "QFileInfo"
#include "QTextStream"
#include "QtWidgets/QApplication"
#include "QDebug"
#include "QtWidgets/QApplication"

//DCCore
#include "DCCore/Logger.h"
#include "DCCore/CoordinatesShiftManager.h"

//DcGp
#include "DCGp/Custom/GpScalarField.h"
#include "DCGp/Custom/GpPointCloud.h"

#include "DlgSamplingRate.h"

using namespace DcIo;





DCCore::IoPlugin* AsciiIO::s_instance = 0;
QStringList AsciiIO::SUPPORT_EXTENSION = QStringList() << "txt" << "pts" << "asc" << "xyz";

DCCore::IoPlugin* LoadPlugin(const char* extension)
{
	return AsciiIO::Instance(extension);
}

void UnloadPlugin()
{
	//AsciiIO::UnloadInstance();
}

DCCore::IoPlugin* AsciiIO::Instance(const char* extension)
{
	bool status = false;
	for (auto i = 0; i < SUPPORT_EXTENSION.size(); ++i)
	{
		if (QString(extension).toUpper() == SUPPORT_EXTENSION.at(i).toUpper())
		{
			status = true;
		}
	}

	if (status)
	{
		if (!s_instance)
		{
			s_instance = new AsciiIO();
		}
		return s_instance;
	}
	else
	{
		return nullptr;
	}
}

void AsciiIO::UnloadInstance()
{
	if (s_instance)
	{
		delete s_instance;
		s_instance = 0;
	}
}

AsciiIO::AsciiIO()
	: DCCore::IoPlugin()
{
	
}

AsciiIO::~AsciiIO()
{
	
}

QString AsciiIO::GetName()
{
	return QString("ASCIIIO");
}

//�����ļ�
DcGp::DcGpEntity* AsciiIO::LoadFile(const QString& filename, 
	                                            DCCore::CallBackPos* cb, 
												bool alwaysDisplayLoadDialog /* = true */, 
												bool* coordinatesShiftEnabled /* = 0 */, 
												double* coordinatesShift /* = 0 */ ,
												QString kind /*= ""*/)
{
	QFile file(filename);

	//�ļ�������
	if (!file.exists())
	{
		DCCore::Logger::Error(QObject::tr("File [%1] doesn\'t exist.").arg(filename));
		return nullptr;
	}

	unsigned long fileSize = file.size();
	//�ļ�Ϊ��
	if (fileSize == 0)
	{
		DCCore::Logger::Message(QObject::tr("File [%1] is empty.").arg(filename));
	}


	//! ������ʼ��
	DataColumnType columnTypes;
	QChar separator;
	unsigned numberOfLines;
	unsigned maxCloudSize;
	int sampleRate;
	unsigned skippedLines;

	//! �������߳��в�֧��qeidget�࣬�������߳���ʹ�����io��Ļ�����Ҫ���ε�qwidget����
	if (alwaysDisplayLoadDialog)
	{
		//column attribution dialog
		DlgOpenAsciiFile dlg(filename, QApplication::activeWindow());

		//���������������3�򷵻ش�����ļ�����
		if (dlg.GetDataColumnNumber() < 3)
		{
			DCCore::Logger::Error(QObject::tr("File [%1] is illegal.").arg(filename));
		}

		if (alwaysDisplayLoadDialog) // && dlg.GetDataColumnNumber() >  3|| !AsciiOpenDlg::CheckOpenSequence(dlg.getOpenSequence(),dummyStr))
		{
			if (!dlg.exec())
			{
				DCCore::Logger::Message(QObject::tr("Canceled By User"));
				return nullptr;
			}
		}


		columnTypes = dlg.GetDataColumnType();
		separator = dlg.GetSeparator();
		numberOfLines = dlg.GetDataLines();
		maxCloudSize = dlg.GetMaxCloudSize();
		sampleRate = dlg.GetSampleRate();
		skippedLines = dlg.GetSkippedLines();

		connect(this, SIGNAL(Notify(unsigned)), QApplication::activeWindow(), SLOT(OnNotifyProgressBar(unsigned)));
	}
	else   //! Ĭ�ϵ����߳��м��ص�ʱ��ֻ֧��Ĭ�ϲ������
	{
		columnTypes.xCoordIndex = 0;
		columnTypes.yCoordIndex = 1;
		columnTypes.zCoordIndex = 2;

		separator = QChar(',');
		numberOfLines = 10;
		maxCloudSize = 0;
		sampleRate = 1;
		skippedLines = 0;
	}
	
	return LoadCloudFromFormatedAsciiFile(  filename,
											columnTypes,
											separator,
											numberOfLines,
											fileSize,
											maxCloudSize,
											cb,
											sampleRate,
											skippedLines,
											alwaysDisplayLoadDialog,
											coordinatesShiftEnabled,
											coordinatesShift);
}

//���ļ��м�������
DcGp::DcGpEntity* AsciiIO::LoadCloudFromFormatedAsciiFile(const QString& fileName,
																		const DataColumnType columnType,
																		QChar separator,
																		unsigned numberOfLines,
																		qint64 fileSize,
																		unsigned maxCloudSize,
																		DCCore::CallBackPos* cb, 
																		int sampleRate,
																		unsigned skippedLines,
																		bool alwaysDisplayLoadDialog,
																		bool* coordinatesShiftEnabled,
																		double* coordinatesShift)
{
	QFile file(fileName);

	//�ļ�������
	if (!file.exists())
	{
		DCCore::Logger::Error(QObject::tr("File [%1] doesn\'t exist.").arg(fileName));
		return false;
	}

	fileSize = file.size();
	//�ļ�Ϊ��
	if (fileSize == 0)
	{
		DCCore::Logger::Message(QObject::tr("File [%1] is empty.").arg(fileName));
	}

	//���ļ�;
	file.open(QIODevice::ReadOnly | QIODevice::Text);
	//ͨ���������Թ����Ե�����;
	long count = 0;
	char currentLine[500];
	//����������ݵ������Ŀ
	long maxSize = (numberOfLines - skippedLines);

	//��ȡ������
	int samplingRate = 1;
	if (alwaysDisplayLoadDialog)
	{
		samplingRate = sampleRate; //DlgSamplingRate::Rate();
		if (samplingRate < 0)
		{
			return false;
		}
	}

	//����һ�����ƶ���,�������������Ŀ
	DcGp::DcGpPointCloud* pointCloud = new DcGp::DcGpPointCloud(QFileInfo(fileName).baseName());
	//�ж��Ƿ���Ҫ��ǰ������ɫ�Ȳ����Ĵ�С
	if (columnType.HasRGBColors())
	{
		pointCloud->SetColorSize(maxSize);
	}

	//�ж��Ƿ���Ҫ��ǰ���䷨���������Ĵ�С
	if (columnType.HasNormals())
	{
		pointCloud->SetNormalSize(maxSize);
	}

	PerLineData record;
	Point_3D point(0,0,0);
	Point_3D shift(0,0,0);
	//���Ĭ����ɫ
	PointColor color = {255, 255, 255};

	//��ķ�����
	DCVector3D normal(0,0,0);

	//������
	for (unsigned i = 0; i < columnType.scalarIndex.size(); ++i)
	{
		DcGp::DcGpScalarFieldPtr scalarField(new DcGp::DcGpScalarField(QObject::tr("Scalar Field #%1").arg(i)));
		pointCloud->AddScalarField(scalarField);
	}
	//�ж��Ƿ�Ϊ����������ռ�
	/*if (columnType.scalarIndex.size() > 0)
	{
		for(int i = 0; i < columnType.scalarIndex.size(); ++i)
		{
			pointCloud->GetScalarFieldByIndex(i)->SetScaleFieldSize(maxSize);
		}
	}*/  
	//�Ƿ�ʶ���ѧ������
	bool scientificNotation = true;

	const char* loadingStr = "Loading";

	while(file.readLine(currentLine, 500) > 0)
	{
		if (++count <= skippedLines)
		{
			continue;
		}
		//
// 		if (count >= maxSize + skippedLines + 1)
// 		{
// 			break;
// 		}

		//������ڿ��������
		if ('\n' == currentLine[0])
		{
			continue;
		}

		//��ȡ�ļ���ÿ������;
		QStringList line = QString(currentLine).split(separator, QString::SkipEmptyParts);

		//��������е����ݲ�������ѡ�����ͣ������
		if (line.size() < columnType.ValidTypeNumber())
		{
			continue;
		}

		//��ȡXYZ����
		if (columnType.xCoordIndex >= 0)
		{
			point[0] = line[columnType.xCoordIndex].toDouble(&scientificNotation);
		} 
		if (columnType.yCoordIndex >= 0)
		{
			point[1] = line[columnType.yCoordIndex].toDouble(&scientificNotation);
		} 
		if (columnType.zCoordIndex >= 0)
		{
			point[2] = line[columnType.zCoordIndex].toDouble(&scientificNotation);
		} 
		//����Ƕ����һ�����ݣ��ж��Ƿ���Ҫ
		if (pointCloud->Size() == 0 && !coordinatesShiftEnabled)
		{
			if (DCCore::CoordinatesShiftManager::Handle(point, shift))
			{
				pointCloud->SetShift(shift);
			}
		}
		else if (pointCloud->Size() == 0 && coordinatesShiftEnabled)
		{
			shift[0] = coordinatesShift[0];
			shift[1] = coordinatesShift[1];
			shift[2] = coordinatesShift[2];
			pointCloud->SetShift(shift);
		}
		//;
		//�����ݶ�ȡ����;
		pointCloud->AddPoint(DCVector3D(point[0] + shift[0], point[1] + shift[1], point[2] + shift[2]));

		if (columnType.HasRGBColors())
		{
			if (columnType.rgbRedIndex >= 0)
			{
				color[0] = line[columnType.rgbRedIndex].toFloat();
			}  
			if (columnType.rgbGreenIndex >= 0)
			{
				color[1] = line[columnType.rgbGreenIndex].toFloat();
			}  
			if (columnType.rgbBlueIndex >= 0)
			{
				color[2] = line[columnType.rgbBlueIndex].toFloat();
			}  
			if (columnType.grayIndex >= 0)
			{
				record.gray = line[columnType.grayIndex].toFloat();
			}  
			pointCloud->AddColor(color);
		}
		
		if (columnType.HasNormals())
		{
			if (columnType.xNormalIndex >= 0)
			{
				normal[0] = line[columnType.xNormalIndex].toFloat();
			}  
			if (columnType.yNormalIndex >= 0)
			{
				normal[1] = line[columnType.yNormalIndex].toFloat();
			}  
			if (columnType.zNormalIndex >= 0)
			{
				normal[2] = line[columnType.zNormalIndex].toFloat();
			}  
			pointCloud->AddNormal(normal);
		}

		if (columnType.scalarIndex.size() > 0)
		{
			for(int i = 0; i < columnType.scalarIndex.size(); ++i)
			{
				pointCloud->GetScalarFieldByIndex(i)->AddData(line[columnType.scalarIndex[i]].toDouble());
			}
		}  

		/*if (!nprogress.OneStep())
		{
			DCCore::Logger::Message(QObject::tr("Canceled By User"));
			break;
		}*/

		
		//Ӧ�ò�����
		for (unsigned i = 0; i < samplingRate - 1; ++i)
		{
			file.readLine(currentLine, 500);
			count++;
			int percent = count / (maxSize / 100.0);
			if (cb)
			{
				(*cb)(percent, loadingStr);
			}
			
		}

		int percent = count / (maxSize / 100.0);
		if (cb)
		{
			(*cb)(percent, loadingStr);
		}
		
	}

	file.close();
	if (cb)
	{
		(*cb)(99, "Finish");
	}
	

	return pointCloud;
}

bool AsciiIO::SaveToFile(DcGp::DcGpEntity* entity, QString filename)
{
	//д������
	QFile file(filename);
	if (!file.open(QFile::WriteOnly | QFile::Truncate))
	{
		return false;
	}
	QTextStream stream(&file);
	//���ø�ʽ����
	stream.setRealNumberNotation(QTextStream::FixedNotation);
	stream.setRealNumberPrecision(3);

	DcGp::DcGpPointCloud* pointCloud = 0;
	if (entity && entity->IsA("DcGpPointCloud"))
	{
		pointCloud = static_cast<DcGp::DcGpPointCloud*>(entity);
	}
	else
	{
		return false;
	}


	//��ȡ��������ɫ����������Ϣ
	unsigned long number = pointCloud->Size();
	bool writeColors = pointCloud->HasColors();
	bool writeNormals = pointCloud->HasNormals();
	bool writeField = false;
	if (pointCloud->GetCurrentScalarField())
	{
		writeField = true;
	}
	

	//����ƫ����
	Point_3D shift = pointCloud->GetShift();

	//д������
	for (unsigned long i = 0; i != number; ++i)
	{
		//����һ����ǰ�б���
		QString line;

		//д�뵱ǰ����
		Point_3D point = Point_3D::FromArray(pointCloud->GetPoint(i).u);
		line.append(QString("%1").arg(point.x - shift.x, 0, 'f', 3));
		line.append(" "); //��ӷָ���
		line.append(QString("%1").arg(point.y - shift.y, 0, 'f', 3));
		line.append(" "); //��ӷָ���
		line.append(QString("%1").arg(point.z - shift.z, 0, 'f', 3));
		line.append(" "); //��ӷָ���

		//����һ����ǰ�б����洢��ɫֵ
		QString color;
		if (writeColors)
		{
			//���rgb��ɫ
			PointColor rgbColor = pointCloud->GetPointColor(i);
			color.append(" ");
			color.append(QString::number(rgbColor[0]));
			color.append(" ");
			color.append(QString::number(rgbColor[1]));
			color.append(" ");
			color.append(QString::number(rgbColor[2]));

			line.append(color);
		}

		//����һ����ǰ�б����洢����ֵ
		QString normal;
		if (writeNormals)
		{
			//���normal
			DCVector3D pNormal = pointCloud->GetPointNormal(i);
			normal.append(" ");
			normal.append(QString::number(pNormal[0]));
			normal.append(" ");
			normal.append(QString::number(pNormal[1]));
			normal.append(" ");
			normal.append(QString::number(pNormal[2]));

			line.append(normal);
		}

		//����һ����ǰ�д洢field
		QString field;
		if (writeField)
		{
			//��ӵ�ǰfieldֵ
			//��ȡ��ǰ���Ӧ�ı�����ֵ
			double value = pointCloud->GetCurrentScalarField()->GetPointScalarValue(i);
			field.append(" ");
			field.append(QString::number(value));

			line.append(field);
		}

		stream << line << "\n";
	}

	file.close();
	return true;
}