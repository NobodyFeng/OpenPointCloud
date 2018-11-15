#ifndef DCGP_SCALAR_FIELD_H
#define DCGP_SCALAR_FIELD_H

#include "DCGp/DcGpDLL.h"

//C++
#include <vector>
#include <map>
#include <set>

//Qt
class QString;

#include "DCCore/DCDefs.h"

//
#include "DCGp/GpBasicTypes.h"
#include "GpColorScale.h"

namespace DcGp
{
	DECLARE_SMARTPTR(DcGpScalarField)
	DECLARE_SMARTPTR(DcGpScalarFieldImpl)
	//������
	class DCGP_API DcGpScalarField
	{
		DECLARE_PHONY_CONSTRUCTOR(DcGpScalarField)
		DECLARE_IMPLEMENT(DcGpScalarField)

	public:

		class ScalarRange
		{
		public:
			//���캯��
			ScalarRange()
				: m_min(0)
				, m_max(0)
			{}

			inline ScalarType Range() const {return m_max - m_min;}

		private:
			//���ֵ
			ScalarType m_max;
			//��Сֵ
			ScalarType m_min;
		};

		DcGpScalarField(QString name);


		//ͨ��numֵ��ʼ��m_data�Ĵ�С
		DcGpScalarField(QString name, unsigned num);
		virtual ~DcGpScalarField();

		//�������
		void AddData(ScalarType data);

		//������ݵ�ָ��������λ��
		void AddData(unsigned index, ScalarType data);
		//��������
		/*void SetScalarData(SharedData data);*/

		//������ɫ��
		inline void SetColorScale(DcGpColorScalePtr colorScale);

		//��ȡ��ɫ��
		DcGpColorScalePtr GetColorScale();

		//��ȡ��ɫ��(const)
		DcGpColorScalePtr GetColorScale() const;

		//��ȡ��ɫ
		std::vector<RGBColor> GetColors() const;

		//�����ݽ���Ԥ�����������ֵ��Χ
		void Prepare();

		//�ı���ɫ��
		void ChangeColorScale(QString name);
		//����m_data�Ĵ�С
		void SetScaleFieldSize(long num);

		//!\brief ��ȡ��ʾ��Χ
		//!\retval ��ʾ��Χ
	    std::set<double> GetScaleRange() const;

		//!\brief ��ȡָ��λ�õ���ɫֵ
		const RGBAColor GetColor(ScalarType value);

		//!\brief ��ȡָ�������ŵ���ɫֵ
		RGBColor GetColor(unsigned index);

		//!\brief ��ȡ��ǰ����������
		//!\retval ����
		QString GetName() const;

		void ComputeMinAndMax();

		//!\brief �����ֵ�ͷ���
		void ComputeMeanAndVariance(ScalarType &mean, ScalarType* variance=0) const;

		ScalarType GetMin();

		ScalarType GetMax();

		double GetPointScalarValue(unsigned index);

		unsigned Size();

		//***********************Ϊlas���ݴ�����ӵķ���**************************************************
		void SetDatas(const unsigned& size, const int& value);
		void SetDatas(std::vector<ScalarType> values);
		std::vector<ScalarType>& GetDatas();

		//���ø�����ɫ��
		void SetColorSteps(unsigned steps);
	protected:
		DcGpScalarField();

		//!\brief ��ʾ��Χ
		ScalarRange m_displayRange;
	};
}
#endif