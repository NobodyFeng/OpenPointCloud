#ifndef DCGP_OBJECT_H
#define DCGP_OBJECT_H

#include "DCGp/DcGpDLL.h"

//Qt
#include "QString"
class QTableWidget;

#include "DCCore/DCDefs.h"

namespace DcGp
{
	DECLARE_SMARTPTR(DcGpObject)
	DECLARE_SMARTPTR(DcGpObjectImpl)

	class DCGP_API DcGpObject
	{
		DECLARE_PHONY_CONSTRUCTOR(DcGpObject)

		DECLARE_IMPLEMENT(DcGpObject)

		GET_CLASSNAME(DcGpObject)
		
	public:
		DcGpObject(QString name);
		virtual ~DcGpObject();

		//�Ƿ���ָ������
		inline bool IsA(QString strName);
		//�Ƿ�����ָ������
		inline bool IsKindOf(QString strName);

		//��ȡΨһID��ʶ
		unsigned long GetUniqueID() const;

		//��ʾ����
		virtual void ShowProperties(QTableWidget* pWidget);

		//�Ƿ���ã����������ѡ��״̬
		bool IsEnabled() const;

		//���ÿ�����
		void SetEnabled(bool status);

		//��ȡ����
		virtual QString GetName() const;
		//��������
		virtual void SetName(QString name);

		//��ȡ����
		virtual QString GetType() const;
		
		//��������
		virtual void SetType(const QString& typeName/*!<_[in]_�������ͱ�ʶ_*/ );

	protected:
		DcGpObject();
	};

}
#endif // DCOBJECT_H
