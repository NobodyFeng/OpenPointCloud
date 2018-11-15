#ifndef DC_VIEW_IMPL_PARAMETERS_H
#define DC_VIEW_IMPL_PARAMETERS_H

#include "DCView/Parameters.h"

//Qt
#include "QtCore/QMap"

#include "QtGui/QColor"
#include "QtGui/QLinearGradient"
#include "QtGui/QBrush"

namespace DCView
	{
		//��������
		typedef GlobalParameters::OperationFlag OperationFlag;
		//��ϼ�
		typedef GlobalParameters::CompositeKey CompositeKey;
		//��������
		typedef GlobalParameters::BackgroundRole BackgroundRole;

		//ȫ�ֲ���ʵ����
		class GlobalParameters::GlobalParametersImpl
		{
		public:
			GlobalParametersImpl(GlobalParameters* pInterface);

			//������ϼ�
			void SetCompositeKey(OperationFlag flag, 
				CompositeKey keys);

			//��ȡ��ϼ�
			CompositeKey GetCompositeKey(OperationFlag flag) const;

			//���ñ���ɫ
			void SetBackground(const QColor& rBckFill);
			void SetBackground(const QLinearGradient& rBckGradient);

			//��ȡ��������
			BackgroundRole GetBackgroundRole() const;
			//��ȡ����
			QBrush GetBackground() const;

		private:
			//�ӿ�
			GlobalParameters* m_pInterface;
			//��ϼ�ע���
			QMap<OperationFlag, CompositeKey> m_compositeKeys;

			//����
			struct
			{
				//��������
				BackgroundRole role;
				//��ɫ
				QColor color;
				//����
				QLinearGradient gradient;
			}m_background;
		};
	}

#endif