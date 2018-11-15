#include "DCCore/Algorithm.h"

using namespace DCCore;

//����ת�Ƕ�
float DCCore::RadianToAngle(float radian)
{
	float angle = radian * 180.0f / M_pi;

	return angle;
}

//�Ƕ�ת����
float DCCore::AngleToRadian(float angle)
{
	float radian = angle * M_pi / 180.0f;

	return radian;
}

//�����ľ��루��ά��
float DCCore::Distance(const Point_2& pt1, const Point_2& pt2)
{
	return Point_2::VDistance(pt1, pt2);
}

//�����ľ��루��ά��
float DCCore::Distance(const Point_3& pt1, const Point_3& pt2)
{
	return Point_3::VDistance(pt1, pt2);
}

//������ļн�
float DCCore::VectorAngle(const Vector_2& v1, const Vector_2& v2)
{
	float radian = acos(v1.Dot(v2) / (v1.Normal() * v2.Normal()));

	return radian;
}

//������ļн�
float DCCore::VectorAngle(const Vector_3& v1, const Vector_3& v2)
{
	float radian = acos(v1.Dot(v2) / (v1.Normal() * v2.Normal()));

	return radian;
}

//�жϵ��Ƿ����������ڲ�(��ά)
bool DCCore::InsideTriangle(const Point_2& pt, const Triangulation_2& triangle)
{
	//v0 = C �C A, v1 = B �C A, v2 = P �C A
	Vector_2 v0 = triangle.VertexC - triangle.VertexA;
	Vector_2 v1 = triangle.VertexB - triangle.VertexA;
	Vector_2 v2 = pt - triangle.VertexA;

	float u = (v1.Dot(v1) * v2.Dot(v0) - v1.Dot(v0) * v2.Dot(v1)) 
		/ (v0.Dot(v0) * v1.Dot(v1) - v0.Dot(v1) * v1.Dot(v0));

	float v = (v0.Dot(v0) * v2.Dot(v1) - v0.Dot(v1) * v2.Dot(v0))
		/ (v0.Dot(v0) * v1.Dot(v1) - v0.Dot(v1) * v1.Dot(v0));

	if (u >= 0.0f && v >= 0.0f && (u + v) < 1.0f)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//�����εķ���������ά��
Vector_2 DCCore::NormalOfTriangle(const Triangulation_2& triangle)
{
	Vector_2 vAB = triangle.VertexA - triangle.VertexB;
	Vector_2 vAC = triangle.VertexA - triangle.VertexC;

	Vector_2 vNormal = vAB.Cross(vAC);

	return vNormal;
}

//�����εķ���������ά��
Vector_3 DCCore::NormalOfTriangle(const Triangulation_3& triangle)
{
	Vector_3 vAB = triangle.VertexA - triangle.VertexB;
	Vector_3 vAC = triangle.VertexA - triangle.VertexC;

	Vector_3 vNormal = vAB.Cross(vAC);

	return vNormal;
}