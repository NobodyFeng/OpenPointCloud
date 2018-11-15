#include "ColorTable.h"
using namespace DcGui;

//Qt
#include "QtCore/QString"

//Impl
#include "Impl/Impl_ColorTable.hxx"

//��ȡ��ɫ
QColor ColorTable::GetColor(const QString& strName)
{
	return ColorTableImpl::GetColor(strName);
}

//��ȡ��ɫ����
QString ColorTable::GetColorDescription(const QColor& color)
{
	return ColorTableImpl::GetColorDescription(color);
}