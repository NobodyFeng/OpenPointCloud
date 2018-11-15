#ifndef REFERENCECLOUD_H
#define REFERENCECLOUD_H

#include <vector>

#include "GpPointCloud.h"

typedef std::vector<unsigned> ReferencesContainer;

namespace DcGp
{
	class DCGP_API ReferenceCloud : public DcGpPointCloud
	{
	public:
		ReferenceCloud(DcGpPointCloud* associatedCloud);
		~ReferenceCloud();

		bool AddPointIndex(unsigned globalIndex);

		unsigned CurrentSize();

		//!\brief ��յ�����
		void Clear();

		//��ȡm_theIndexesָ��λ�ñ����������ֵ
		unsigned GetValue(unsigned index);

		//!\brief ���Ĵ�С
		void Resize(unsigned num);

		unsigned GetPointGlobalIndex(unsigned localIndex) const;

		void SetPointIndex(unsigned localIndex, unsigned globalIndex);

		//!\brief��ȡָ������λ�õ�����ֵ
		DCVector3D GetPoint(unsigned index);

		//!\brief ����m_theAssociatedCloud�����ı�������ֵ
		void SetPointScalarValue(unsigned pointIndex, ScalarType value);

		DcGpPointCloud* GetAssociatedCloud() {return m_theAssociatedCloud;}

		void RemovePointGlobalIndex(unsigned localIndex);
	private:
		//! ��ص�����
		ReferencesContainer* m_theIndexes;

		//!\brief �����ĵ��ƶ���
		DcGpPointCloud* m_theAssociatedCloud;
	};
}

#endif // REFERENCECLOUD_H
