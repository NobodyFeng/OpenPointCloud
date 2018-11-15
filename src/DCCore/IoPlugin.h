#ifndef DCCOREIO_IOPLUGIN_H
#define DCCOREIO_IOPLUGIN_H

#include "QObject"
#include "QtWidgets/QTreeWidgetItem"

#include "DCCoreDLL.h"

//DcGp
namespace DcGp
{
	class DcGpEntity;
}

namespace DCCore
{
	typedef bool CallBackPos(const int pos, const char * str );
	class DCCORE_EXPORT IoPlugin : public QObject
	{
	public:
		virtual QString GetName() = 0;

		virtual ~IoPlugin() {}

		virtual void UnloadInstance() = 0;

		//���߳�ʱ����Ҫ����item����
		virtual DcGp::DcGpEntity* LoadFile(const QString& filename,
			                                CallBackPos* cb = 0, 
											bool alwaysDisplayLoadDialog = true,
											bool* coordinatesShiftEnabled = 0,
											double* coordinatesShift = 0,
											QString kind = ""
											) = 0;

		//Ϊ�˱���ԭ��io����ļ����ԣ�
		/*virtual DcGp::DcGpEntity* LoadFile(const QString& filename,
		bool alwaysDisplayLoadDialog = true,
		bool* coordinatesShiftEnabled = 0,
		double* coordinatesShift = 0
		)
		{
		return nullptr;
		}*/

		virtual bool SaveToFile(DcGp::DcGpEntity* entity, QString filename) = 0;

	signals:
		//void Notify(unsigned);
	};
}

#endif