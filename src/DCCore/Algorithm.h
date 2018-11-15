#ifndef DC_COMMON_ALGORITHM_H
#define DC_COMMON_ALGORITHM_H

#include "DCCoreDLL.h"

//Self
#include "BasicTypes.h"

namespace DCCore
{
	//����ת�Ƕ�
	DCCORE_EXPORT float RadianToAngle(float radian);

	//�Ƕ�ת����
	DCCORE_EXPORT float AngleToRadian(float angle);

	//�����ľ��루��ά��
	DCCORE_EXPORT float Distance(const Point_2& pt1, const Point_2& pt2);

	//�����ľ��루��ά��
	DCCORE_EXPORT float Distance(const Point_3& pt1, const Point_3& pt2);

	//������ļн�(��ά������)
	DCCORE_EXPORT float VectorAngle(const Vector_2& v1, const Vector_2& v2);

	//������ļн�(��ά������)
	DCCORE_EXPORT float VectorAngle(const Vector_3& v1, const Vector_3& v2);

	//�жϵ����������ڲ�����ά��
	DCCORE_EXPORT bool InsideTriangle(const Point_2& pt, const Triangulation_2& triangle);

	//�жϵ����������ڲ�����ά��
	//DCCORE_EXPORT bool IsInTriangulation(const Point_3& pt, const Triangulation_3& triangle);

	//�����εķ���������ά��
	DCCORE_EXPORT Vector_2 NormalOfTriangle(const Triangulation_2& triangle);

	//�����εķ���������ά��
	DCCORE_EXPORT Vector_3 NormalOfTriangle(const Triangulation_3& triangle);
}

#endif