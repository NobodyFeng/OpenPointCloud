#include "Catcher.h"

//#include "PointCloudIntersectionVisitor.h"
//#include "EntityIntersectionVisitor.h"
//#include "DistanceLineIntersectionVisitor.h"
//#include "OutlineIntersectionVisitor.h"
//#include "EndPointIntersectionVisitor.h"
//#include "IntersectionLineIntersectionVisitor.h"
//#include "OperationBoxIntersectionVisitor.h"

using namespace MPUtil;

Catcher::Catcher(PickMask mask)
	: m_pickMask(mask)
	, m_pixelSize(1)
{

}

Catcher::~Catcher()
{

}

void Catcher::SetPixelSize(float psize)
{
	m_pixelSize = psize;
}

void Catcher::SetMask(PickMask mask)
{
	m_pickMask = mask;
}

void Catcher::SetModelViewMatrix(const double* modelViewMat)
{
	m_modelViewMatrix =  (double*)modelViewMat;
}

void Catcher::ComputeIntersections(DcGp::DcGpEntity* sceneRoot, const DCCore::Point_3 v1, const DCCore::Point_3 v2, HitList& hitList)
{
	////! ���������������������ʷ���Ҫ��Ľڵ㣬��ȡĿ��ֵ

	////! ����mask�����ͣ�������ͬ�ķ�����
	//if (m_pickMask&ePointCloud)
	//{
	//	PointCloudIntersectionVisitor nv(v1, v2);
	//	nv.SetPixelSize(m_pixelSize);
	//	nv.SetMask(m_pickMask);
	//	sceneRoot->Accept(nv);
	//	hitList = nv.GetHits();
	//}

	//if (m_pickMask&eEntity)
	//{
	//	EntityIntersectionVisitor nv(v1, v2);
	//	nv.SetPixelSize(m_pixelSize);
	//	nv.SetMask(m_pickMask);
	//	nv.SetModelViewMatrix(m_modelViewMatrix);
	//	sceneRoot->Accept(nv);
	//	hitList = nv.GetHits();
	//}
	//
	//if (m_pickMask & eDistanceLine)
	//{
	//	DistanceLineIntersectionVisitor nv(v1, v2);
	//	nv.SetPixelSize(m_pixelSize);
	//	nv.SetMask(m_pickMask);
	//	nv.SetModelViewMatrix(m_modelViewMatrix);
	//	sceneRoot->Accept(nv);
	//	hitList = nv.GetHits();
	//}
	//if (m_pickMask & eIntersectionLine)
	//{
	//	IntersectionLineIntersectionVisitor nv(v1, v2);
	//	nv.SetPixelSize(m_pixelSize);
	//	nv.SetMask(m_pickMask);
	//	nv.SetModelViewMatrix(m_modelViewMatrix);
	//	sceneRoot->Accept(nv);
	//	hitList = nv.GetHits();
	//}
	//if (m_pickMask & eOutline)
	//{
	//	OutlineIntersectionVisitor nv(v1, v2);
	//	nv.SetPixelSize(m_pixelSize);
	//	nv.SetMask(m_pickMask);
	//	nv.SetModelViewMatrix(m_modelViewMatrix);
	//	sceneRoot->Accept(nv);
	//	hitList = nv.GetHits();
	//}

	//if (m_pickMask & eEndpoint)
	//{
	//	//���˵����
	//	EndPointIntersectionVisitor nv(v1, v2);
	//	nv.SetPixelSize(m_pixelSize);
	//	nv.SetMask(m_pickMask);
	//	sceneRoot->Accept(nv);
	//	hitList = nv.GetHits();
	//}

	//if (m_pickMask & eIndividulPlane)
	//{
	//	EntityIntersectionVisitor nv(v1, v2);
	//	nv.SetPixelSize(m_pixelSize);
	//	nv.SetMask(m_pickMask);
	//	nv.SetModelViewMatrix(m_modelViewMatrix);
	//	sceneRoot->Accept(nv);
	//	hitList = nv.GetHits();
	//}

	//if (m_pickMask & eOperationBox)
	//{
	//	OperationBoxIntersectionVisitor nv(v1, v2);
	//	nv.SetPixelSize(m_pixelSize);
	//	nv.SetMask(m_pickMask);
	//	nv.SetModelViewMatrix(m_modelViewMatrix);
	//	sceneRoot->Accept(nv);
	//	hitList = nv.GetHits();
	//}
}
