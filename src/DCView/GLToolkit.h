#ifndef DC_VIEW_GLTOOLKIT_H
#define DC_VIEW_GLTOOLKIT_H

#include "ViewDLL.h"

//Qt
#include "QtCore/QObject"
#include "QtCore/QString"

//Eigen
#include "Eigen/Dense"

//DcCommon
#include "DCCore/BasicTypes.h"

namespace DCView
	{
		//4*4����
		typedef Eigen::Matrix4d Matrix_4_4;
		//����
		typedef DCCore::Vector_3 Vector_3;

		//OpenGL���߰�
		class DC_VIEW_API GLToolkit: public QObject
		{
		public:
			//��ͼģʽ
			enum ViewMode
			{
				eTopViewMode				=	0x00000010	,	//����ͼģʽ
				eBottomViewMode				=	0x00000020	,	//����ͼģʽ
				eFrontViewMode				=	0x00000040	,	//ǰ��ͼģʽ
				eBackViewMode				=	0x00000080	,	//����ͼģʽ
				eLeftViewMode				=	0x00000100	,	//����ͼģʽ
				eRightViewMode				=	0x00000200	,	//����ͼģʽ
			};

		public:
			static void RenderTexture2D(unsigned uTextureID, int iWidth, int iHeight);

			static void RenderTexture2DCorner(unsigned uTextureID, int iWidth, int iHeight);

			//��ʼ��glew
			static bool InitGlew();

			//�����չ�Ƿ���Ч
			static bool IsValidExtension(const char* pExtName);

			//���shader����Ч��
			static bool CheckShadersAvailability();

			//���FBO�Ƿ���Ч
			static bool CheckFBOAvailability();

			//���������OpenGL����
			static QString CatchError();

			//������ת����
			static Matrix_4_4 CalculateRotationMatrix(const Vector_3& vSource, 
				const Vector_3& vDest);
			static Matrix_4_4 CalculateRotationMatrix(const Vector_3& vAxis, 
				float fAngle);

			//������ͼ����
			static Matrix_4_4 GenerateViewMatrix(ViewMode mode);
		};
	}

#endif
