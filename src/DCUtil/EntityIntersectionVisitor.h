#ifndef MPUTIL_ENTITYINTERSECTIONVISITOR_H
#define MPUTIL_ENTITYINTERSECTIONVISITOR_H

#include "EntityVisitor.h"
#include "Common.h"
#include "Hit.h"

namespace MPUtil
{
	class EntityIntersectionVisitor : public EntityVisitor
	{
	public:
		EntityIntersectionVisitor(DCCore::Point_3 v1, DCCore::Point_3 v2);
		~EntityIntersectionVisitor();

		//! ʵ�弰����
		virtual void Apply(DcGp::DcGpEntity& node);
		virtual void Apply(DcGp::DcGpPointCloud& node);

		virtual void Apply(MPGp::DistanceLine& node);

		virtual void Apply(MPGp::IndividualPlane& node);

		virtual void Apply(MPGp::Outlines& node);

		virtual void Apply(MPGp::IntersectionLine& node);

		virtual void Apply(MPGp::OperationBox& node);

		//! ������뾶--��Ӧ���ǵ�ǰ�Ӵ�4�����ش�С
		void SetPixelSize(float psize);

		void SetMask(PickMask mask);

		//���ؽ��
		HitList GetHits();

		//! ����ģ����ͼ����
		void SetModelViewMatrix(const double* modelViewMat);

		//double* GetModelViewMatrix() const;
	private:
		//����ģʽ
		PickMask m_mask;

		//! ������ʱ����뾶
		float m_pixelSize;

		//! ��������
		DCCore::Point_3 m_nearPoint;

		DCCore::Point_3 m_farPoint;

		//! nearPoint��߽�н���ľ��룬����ִ����ȼ��Ĺ���
		float m_intersectionDist;

		//! ����ֵhits
		HitList m_hits;

		//! ģ����ͼ����
		double* m_modelViewMatrix;
	};

}

#endif // MPUTIL_ENTITYINTERSECTIONVISITOR_H
