#ifndef DC_VIEW_PARAMETERS_H
#define DC_VIEW_PARAMETERS_H

#include "ViewDLL.h"

//Qt
#include "QtCore/QString"
class QColor;
class QLinearGradient;
class QBrush;

namespace DCView
	{
		//
		class DC_VIEW_API GlobalParameters
		{
		public:
			//��������
			enum OperationFlag
			{
				eTranslateOperation				=	0x00000001	,	//ƽ��
				eRotateOperation				=	0x00000002	,	//��ת
				eScaleOperation					=	0x00000004  ,	//����
				eZoomOnRegion					=	0x00000008  ,		//��������
				ePicking						=	0x00000012
			};
			//��ϼ�
			struct CompositeKey
			{
				Qt::MouseButtons buttons;
				Qt::KeyboardModifiers keys;

				CompositeKey()
					: buttons(Qt::NoButton)
					, keys(Qt::NoModifier)
				{}
			};

			//��������
			enum BackgroundRole
			{
				eBckFill			=	0x00001000	,	//��䱳��
				eBckGradient		=	0x00002000		//���䱳��
			};

		public:
			GlobalParameters();
			~GlobalParameters();

			//���ñ���ɫ
			static void SetBackground(const QColor& rBckFill);
			static void SetBackground(const QLinearGradient& rBckGradient);

			//���û�����
			static void SetAmbientLight(const QColor& ambient);

			//��ȡ��������
			static BackgroundRole GetBackgroundRole();
			//��ȡ����
			static QBrush GetBackground();

			//���ò�������ϼ�
			//Ĭ����ϣ�����ת-������������-ת�����᣻ƽ��-����м���
			static void SetCompositeKey(OperationFlag flag, 
				Qt::MouseButtons buttons, 
				Qt::KeyboardModifiers keys);

			//��ȡ��ϼ�
			static CompositeKey GetCompositeKey(OperationFlag flag);

			//Shaders·��
			static QString ShadersPath();

			static float* DiffuseLight();
			static float* AmbientLight();
			static float* SpecularLight();
			static unsigned char* BoundBoxColor();
		private:
			static GlobalParameters s_globalParams;

			class GlobalParametersImpl;
			GlobalParametersImpl* m_pImpl;
		};
	}



 #endif
