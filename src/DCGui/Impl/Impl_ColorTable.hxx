#ifndef DC_GUI_IMPL_COLORTABLE_H
#define DC_GUI_IMPL_COLORTABLE_H

//Qt
#include "QtGui/QColor"
#include "QtCore/QString"

namespace DcGui
{
	class ColorTableImpl
	{
	public:
		static QColor GetColor(const QString& strName);
		//��ȡ��ɫ����
		static QString GetColorDescription(const QColor& color);

	private:
		struct ColorItem
		{
			QString name;
			QString desc;
			QString hex;
			int red;
			int green;
			int blue;
		};
		//������ɫ
		bool FindColor(const QString& strName, ColorItem& item);

	private:
		static ColorTableImpl s_instance;
	};
}

#endif