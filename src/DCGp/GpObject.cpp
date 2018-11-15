#include "GpObject.h"

//Qt
#include "QString"

namespace DcGp
{
	DECLARE_SMARTPTR(DcGpObjectImpl)

	class DcGpObjectImpl
	{
		DECLARE_PHONY_CONSTRUCTOR(DcGpObjectImpl)

	protected:
		DcGpObjectImpl()
			: m_name("")
			, m_type("Unknow")
			, m_enabled(true)
		{
			//ͳ�ƴ���������
			++s_count;

			m_uniqueID = s_count;
		}

	private:
		friend class DcGpObject;

		//������DcGpObject������
		static unsigned long s_count;

		//��ʹ����
		bool m_enabled;

		//Ψһ��ʶID������OpenGLѡ��ʹ��
		unsigned long m_uniqueID;

		//����
		QString m_name;

		//! �������ͱ�ʶ
		QString m_type;
	};
}

unsigned long DcGp::DcGpObjectImpl::s_count = 0;

DcGp::DcGpObject::DcGpObject()
{
	m_pDcGpObjectImpl = DcGpObjectImpl::CreateObject();
}

DcGp::DcGpObject::DcGpObject(QString name)
{
	m_pDcGpObjectImpl = DcGpObjectImpl::CreateObject();

	m_pDcGpObjectImpl->m_name = name;
}

DcGp::DcGpObject::~DcGpObject()
{
	//delete m_pDcGpObjectImpl.get();
}

bool DcGp::DcGpObject::IsA(QString strName)
{
	return GetClassname() == strName;
}

bool DcGp::DcGpObject::IsKindOf(QString strName)
{
	return DcGpObject::IsA(strName);
}

void DcGp::DcGpObject::ShowProperties(QTableWidget* pWidget)
{

}

unsigned long DcGp::DcGpObject::GetUniqueID() const 
{
	return m_pDcGpObjectImpl->m_uniqueID;
}

//�Ƿ����
bool DcGp::DcGpObject::IsEnabled() const 
{
	return m_pDcGpObjectImpl->m_enabled;
}

//���ÿ�����
void DcGp::DcGpObject::SetEnabled(bool status)
{
	m_pDcGpObjectImpl->m_enabled = status;
}

//��ȡ����
QString DcGp::DcGpObject::GetName() const
{
	return m_pDcGpObjectImpl->m_name;
}
//��������
void DcGp::DcGpObject::SetName(QString name)
{
	m_pDcGpObjectImpl->m_name = name;
}

QString DcGp::DcGpObject::GetType() const
{
	return m_pDcGpObjectImpl->m_type;
}

void DcGp::DcGpObject::SetType(const QString& typeName)
{
	m_pDcGpObjectImpl->m_type = typeName;
}