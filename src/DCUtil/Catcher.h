//----------------------------------------------------------------------
//author:wly
//time:
//dsc:ͨ�����뵱ǰʵ����ڵ������ߵ㣬ͨ�������󽻻�ȡ��׽����Ϣ
//
//----------------------------------------------------------------------

#ifndef MPUTIL_POINTPICKING_H
#define MPUTIL_POINTPICKING_H

#include "DCCore/BasicTypes.h"

#include "mputilDLL.h"

#include "Common.h"

#include "Hit.h"

namespace DcGp
{
	class DcGpEntity;
}

namespace MPUtil
{
	class MPUTIL_EXPORT Catcher
	{
	public:
		Catcher(PickMask mask = eVisible | ePointCloud);
		~Catcher();

		//! ������뾶--��Ӧ���ǵ�ǰ�Ӵ�4�����ش�С
		void SetPixelSize(float psize);

		void ComputeIntersections(DcGp::DcGpEntity* sceneRoot, const DCCore::Point_3 v1, const DCCore::Point_3 v2, HitList& hitList);
	
		//! ���ò�׽���Ĳ�׽mask
		void SetMask(PickMask mask);

		//! ����ģ����ͼ����
		 void SetModelViewMatrix(const double* modelViewMat);
	private:
		PickMask m_pickMask;

		//! ���ش�С
		float m_pixelSize;

		//ģ����ͼ����
		double* m_modelViewMatrix;
	
	};
}

#endif // MPUTIL_POINTPICKING_H
