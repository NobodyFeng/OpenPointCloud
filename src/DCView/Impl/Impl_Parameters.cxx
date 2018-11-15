#include "Impl_Parameters.hxx"
using namespace DCView;

//���캯��
GlobalParameters::GlobalParametersImpl
	::GlobalParametersImpl(GlobalParameters* pInterface)
	: m_pInterface(pInterface)
{
	//����Ĭ�ϲ�����ϼ�
	CompositeKey key;
	{
		//ƽ��
		key.buttons = Qt::MidButton;
		m_compositeKeys.insert(eTranslateOperation, key);

		//��ת
		key.buttons = Qt::LeftButton;
		m_compositeKeys.insert(eRotateOperation, key);

		//��������
		key.buttons = Qt::MidButton;
		key.keys = Qt::ShiftModifier;
		m_compositeKeys.insert(eZoomOnRegion, key);
	}

	//����Ĭ�ϱ���
	m_background.role = eBckFill;
	m_background.color = Qt::black;
}

//������ϼ�
void GlobalParameters::GlobalParametersImpl
	::SetCompositeKey(OperationFlag flag, CompositeKey keys)
{
	m_compositeKeys[flag] = keys;
}

//��ȡ��ϼ�
CompositeKey GlobalParameters::GlobalParametersImpl
	::GetCompositeKey(OperationFlag flag) const
{
	return m_compositeKeys[flag];
}

//���ñ���ɫ
void GlobalParameters::GlobalParametersImpl
	::SetBackground(const QColor& rBckFill)
{
	m_background.role = eBckFill;
	m_background.color = rBckFill;
}
void GlobalParameters::GlobalParametersImpl
	::SetBackground(const QLinearGradient& rBckGradient)
{
	m_background.role = eBckGradient;
	m_background.gradient = rBckGradient;
}

//��ȡ��������
BackgroundRole GlobalParameters::GlobalParametersImpl
	::GetBackgroundRole() const
{
	return m_background.role;
}
//��ȡ����
QBrush GlobalParameters::GlobalParametersImpl
	::GetBackground() const
{
	if (m_background.role == eBckFill)
	{
		return QBrush(m_background.color);
	}
	else 
	{
		return QBrush(m_background.gradient);
	}
}