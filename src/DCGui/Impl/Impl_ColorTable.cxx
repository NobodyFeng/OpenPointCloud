#include "Impl_ColorTable.hxx"
using namespace DcGui;

//Qt
#include "QtCore/QFile"
#include "QtCore/QXmlStreamReader"
#include "QtCore/QStringList"

//ʵ��
ColorTableImpl ColorTableImpl::s_instance;

//��ȡ��ɫ
QColor ColorTableImpl::GetColor(const QString& strName)
{
	ColorItem item;

	if (s_instance.FindColor(strName, item))
	{
		return QColor(item.hex);
	}

	return QColor();
}

//��ȡ��ɫ����
QString ColorTableImpl::GetColorDescription(const QColor& color)
{
	QString strColorName = color.name();

	ColorItem item;

	if (s_instance.FindColor(strColorName, item))
	{
		return item.desc;
	}
	else
	{
		QString strDec = QString("(%1,%2,%3)")
			.arg(color.red()).arg(color.green()).arg(color.blue());
		return strDec;
	}
}

//������ɫ
bool ColorTableImpl::FindColor(const QString& strName,
	ColorItem& item)
{
	//��ɫ���ļ����
	QFile fColorTable(":/Files/Resources/colortable.xml");

	//���ļ�������ʧ�ܷ���false
	if (!fColorTable.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		return false;
	}

	//����XML��ȡ��
	QXmlStreamReader iReader(&fColorTable);

	//��ȡXML�ļ�
	while (!iReader.atEnd())
	{
		//��ȡ��¼
		QXmlStreamReader::TokenType record = iReader.readNext();

		//
		if (record == QXmlStreamReader::StartElement)
		{
			//��ȡ��ɫ��¼
			if ("Color" == iReader.name())
			{
				QString strColorName = iReader.attributes().value("Name").toString();
				QString strHex = iReader.attributes().value("Hex").toString();
				QString strDesc = iReader.attributes().value("Description").toString();

				//�����ѯ����Ҫ�ҵ���ɫ
				if (strName.compare(strColorName, Qt::CaseInsensitive) == 0
					|| strName.compare(strHex, Qt::CaseInsensitive) == 0
					|| strName.compare(strDesc, Qt::CaseInsensitive) == 0)
				{
					item.name = strColorName;
					item.desc = iReader.attributes().value("Description").toString();
					item.hex = strHex;

					QString strRGB = iReader.attributes().value("RGB").toString();
					QStringList list = strRGB.split(",", QString::SkipEmptyParts);
					item.red = list[0].toInt();
					item.green = list[1].toInt();
					item.blue = list[2].toInt();

					fColorTable.close();

					return true;
				}
			}
		}
	}

	return false;
}