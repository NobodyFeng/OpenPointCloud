#ifndef DC_VIEW_VIEWERPARAMS_H
#define DC_VIEW_VIEWERPARAMS_H

//Eigen
#include "Common.h"

namespace DCView
	{
		//�Ӵ��Ĳ���
		class ViewerParams
		{
		public:
			//Viewer�Ĳ���
			ViewerParams()
				: projectionMode(eProjPerspective)
				, objectCentric(true)
				, scale(1.0f)
				, pixeSize(1.0f)
				, fov(30.0f)
				, aspectWH(1.0f)
				, pivotPos(Point_3(0.0f, 0.0f, 0.0f))
				, cameraPos(Point_3(0.0f, 0.0f, 0.0f))
				, zNear(0)
				, zFar(0)
				, MAX_SCALE(10e6)
				, MIN_SCALE(10e-6)
				, AXIS_LENGTH(25.0f)
				, AXIS_RADIUS(1.0f)
				, PIVOT_RADIUS_PERCENT(0.8f)
				, matrixRotated(Matrix_4_4::Zero(4,4))
			{
				//��λ����ת����
				matrixRotated.setIdentity();
			}


			//ͶӰģʽ
			ProjectionMode projectionMode;
			//�Ƿ��Զ���Ϊ����
			bool objectCentric;
			//���ű���
			float scale;
			//���ش�С
			float pixeSize;
			//��ת����
			Point_3 pivotPos;
			//���λ��
			Point_3 cameraPos;
			//��Ұ
			float fov;
			//��߱���
			float aspectWH;
			//
			double zNear;
			double zFar;

			//��ת����
			Matrix_4_4 matrixRotated;

			//�����С
			static const int MATRIX_SIZE = 16;
			//ͶӰ����
			double matrixProjection[MATRIX_SIZE];
			//ģ����ͼ����
			double matrixModelView[MATRIX_SIZE];
			//�ӿھ���
			int matrixViewport[4];

			//���ű�������ֵ��Χ
			const float MAX_SCALE;
			const float MIN_SCALE;
			//�����᳤��
			const float AXIS_LENGTH;
			//������뾶
			const float AXIS_RADIUS;
			//��ת���İ뾶
			const float PIVOT_RADIUS_PERCENT;

		public:
			
		};
	}

#endif