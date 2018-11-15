#include "GpScalarField.h"

//c++
#include <algorithm>

//
#include "QString"

#include "GpColorScale.h"

namespace DcGp
{
	DECLARE_SMARTPTR(DcGpScalarFieldImpl)
	
	class DcGpScalarFieldImpl
	{
		DECLARE_PHONY_CONSTRUCTOR(DcGpScalarFieldImpl)

	protected:
		DcGpScalarFieldImpl()
			: m_borderValid(false)
			, m_colorSteps(256)
		{}

	private:
		friend class DcGpScalarField;
		//����
		QString m_name;

		//������ɫ��
		std::map<QString, DcGpColorScalePtr> m_colorScales;

		//��ǰ��ɫ��
		DcGpColorScalePtr m_currentColorScale;

		//����ֵ
		std::vector<ScalarType> m_data;

		//ÿ��������Ӧ����ɫ
		std::vector<RGBColor> m_colors;

		//�������������Сֵ
		std::set<double> m_keyValues;

		//m_data�е������Сֵ
		ScalarType m_minVal;

		//m_data�е������Сֵ
		ScalarType m_maxVal;

		//�Ƿ��Ѿ�����������Сֵ
		bool m_borderValid;

		//��ɫ����
		unsigned m_colorSteps;
	};
}

DcGp::DcGpScalarField::DcGpScalarField()
{
	m_pDcGpScalarFieldImpl = DcGpScalarFieldImpl::CreateObject();

	m_pDcGpScalarFieldImpl->m_name = "";
	m_pDcGpScalarFieldImpl->m_colorScales = DcGpColorScale::DefualtColorScales();
	//m_pDcGpScalarFieldImpl->m_currentColorScale = m_pDcGpScalarFieldImpl->m_colorScales.begin()->second;
	m_pDcGpScalarFieldImpl->m_currentColorScale = m_pDcGpScalarFieldImpl->m_colorScales["Blue>Green>Yellow>Red"];
}

DcGp::DcGpScalarField::DcGpScalarField(QString name)
{
	m_pDcGpScalarFieldImpl = DcGpScalarFieldImpl::CreateObject();

	m_pDcGpScalarFieldImpl->m_name = name;
	m_pDcGpScalarFieldImpl->m_colorScales = DcGpColorScale::DefualtColorScales();
	m_pDcGpScalarFieldImpl->m_currentColorScale = m_pDcGpScalarFieldImpl->m_colorScales["Blue>Green>Yellow>Red"];
}

DcGp::DcGpScalarField::DcGpScalarField(QString name, unsigned num)
{
	m_pDcGpScalarFieldImpl = DcGpScalarFieldImpl::CreateObject();

	m_pDcGpScalarFieldImpl->m_name = name;
	m_pDcGpScalarFieldImpl->m_data.resize(num);
	m_pDcGpScalarFieldImpl->m_colorScales = DcGpColorScale::DefualtColorScales();
	m_pDcGpScalarFieldImpl->m_currentColorScale = m_pDcGpScalarFieldImpl->m_colorScales["Blue>Green>Yellow>Red"];
}

DcGp::DcGpScalarField::~DcGpScalarField()
{

}


void DcGp::DcGpScalarField::AddData(ScalarType data)
{
	m_pDcGpScalarFieldImpl->m_data.push_back(data);
}
void DcGp::DcGpScalarField::SetColorScale(DcGpColorScalePtr colorScale)
{
	m_pDcGpScalarFieldImpl->m_currentColorScale = colorScale;
}


DcGp::DcGpColorScalePtr DcGp::DcGpScalarField::GetColorScale()
{
	return m_pDcGpScalarFieldImpl->m_currentColorScale;
}

DcGp::DcGpColorScalePtr DcGp::DcGpScalarField::GetColorScale() const
{
	return m_pDcGpScalarFieldImpl->m_currentColorScale;
}

std::vector<RGBColor> DcGp::DcGpScalarField::GetColors() const
{
	return m_pDcGpScalarFieldImpl->m_colors;
}

void DcGp::DcGpScalarField::SetScaleFieldSize(long num)
{
	m_pDcGpScalarFieldImpl->m_data.reserve(num);
}

//׼������
void DcGp::DcGpScalarField::Prepare()
{
	if (m_pDcGpScalarFieldImpl->m_data.empty())
	{
		return;
	}
	auto interval = (*std::max_element(m_pDcGpScalarFieldImpl->m_data.cbegin(), m_pDcGpScalarFieldImpl->m_data.cend()) - 
		*std::min_element(m_pDcGpScalarFieldImpl->m_data.cbegin(), m_pDcGpScalarFieldImpl->m_data.cend()));
	auto maxValue = *std::max_element(m_pDcGpScalarFieldImpl->m_data.cbegin(), m_pDcGpScalarFieldImpl->m_data.cend());
	auto minValue = *std::min_element(m_pDcGpScalarFieldImpl->m_data.cbegin(), m_pDcGpScalarFieldImpl->m_data.cend());

	//���±����������Сֵ
	m_pDcGpScalarFieldImpl->m_keyValues.clear();
	m_pDcGpScalarFieldImpl->m_keyValues.insert(minValue);
	m_pDcGpScalarFieldImpl->m_keyValues.insert(maxValue);

	m_pDcGpScalarFieldImpl->m_colors.clear();

	for (auto i = m_pDcGpScalarFieldImpl->m_data.begin(); i != m_pDcGpScalarFieldImpl->m_data.end(); ++i)
	{
		RGBAColor outOfRangeColor = {0, 0, 0, 0};
		
		double rPos;
		if (interval == 0)
		{
			rPos = 1;
		}
		else
		{
			rPos = abs((*i - minValue) / interval);
		}
		RGBAColor rgbaColor =m_pDcGpScalarFieldImpl-> m_currentColorScale->GetColorByRelativePos(rPos, m_pDcGpScalarFieldImpl->m_colorSteps, outOfRangeColor);
		RGBColor color = {rgbaColor[0], rgbaColor[1], rgbaColor[2]};
		m_pDcGpScalarFieldImpl->m_colors.push_back(color);
	}
}

//�ı䵱ǰ��ɫ��
void DcGp::DcGpScalarField::ChangeColorScale(QString name)
{
	//�����ɫ�����ڣ��򲻽��в���
	if (m_pDcGpScalarFieldImpl->m_colorScales.find(name) == m_pDcGpScalarFieldImpl->m_colorScales.end())
	{
		return;
	}

	//�ı䵱ǰ��ɫ��
	m_pDcGpScalarFieldImpl->m_currentColorScale = m_pDcGpScalarFieldImpl->m_colorScales[name];
}

std::set<double> DcGp::DcGpScalarField::GetScaleRange() const
{
	return m_pDcGpScalarFieldImpl->m_keyValues;
}

RGBColor DcGp::DcGpScalarField::GetColor(unsigned index)
{
	if (m_pDcGpScalarFieldImpl->m_keyValues.empty())
	{
		Prepare();
	}
	
	return m_pDcGpScalarFieldImpl->m_colors[index];
}

const RGBAColor DcGp::DcGpScalarField::GetColor(ScalarType value)
{
	if (m_pDcGpScalarFieldImpl->m_keyValues.empty())
	{
		Prepare();
	}
	std::vector<double> sortedKeyValues(m_pDcGpScalarFieldImpl->m_keyValues.begin(),m_pDcGpScalarFieldImpl->m_keyValues.end());
	double interval = sortedKeyValues.back()-sortedKeyValues.front();
	RGBAColor outOfRangeColor = {0, 0, 0, 0};
	return m_pDcGpScalarFieldImpl->m_currentColorScale->GetColorByRelativePos(abs((value - sortedKeyValues.front()) / interval), m_pDcGpScalarFieldImpl->m_colorSteps, outOfRangeColor);
}

QString DcGp::DcGpScalarField::GetName() const
{
	return m_pDcGpScalarFieldImpl->m_name;
}

void DcGp::DcGpScalarField::ComputeMinAndMax()
{
	if (!m_pDcGpScalarFieldImpl->m_borderValid)
	{
		if (!m_pDcGpScalarFieldImpl->m_data.empty())
		{
			bool minMaxInitialized = false;
			for (unsigned i=0;i<m_pDcGpScalarFieldImpl->m_data.size();++i)
			{
				const ScalarType& val = m_pDcGpScalarFieldImpl->m_data[i];
				if (minMaxInitialized)
				{
					if (val < m_pDcGpScalarFieldImpl->m_minVal)
						m_pDcGpScalarFieldImpl->m_minVal=val;
					else if (val>m_pDcGpScalarFieldImpl->m_maxVal)
						m_pDcGpScalarFieldImpl->m_maxVal=val;
				}
				else
				{
					//first valid value is used to init min and max
					m_pDcGpScalarFieldImpl->m_minVal = m_pDcGpScalarFieldImpl->m_maxVal = val;
					minMaxInitialized = true;
				}
			}
		}
		else
		{
			m_pDcGpScalarFieldImpl->m_minVal = m_pDcGpScalarFieldImpl->m_maxVal = 0;
		}
		m_pDcGpScalarFieldImpl->m_borderValid = true;
	}
}

void DcGp::DcGpScalarField::ComputeMeanAndVariance(ScalarType &mean, ScalarType* variance) const
{
	double _mean=0.0, _std2=0.0;
	unsigned count=0;

	for (unsigned i=0;i<m_pDcGpScalarFieldImpl->m_data.size();++i)
	{
		const ScalarType& val = m_pDcGpScalarFieldImpl->m_data[i];
		_mean += (double)val;
		_std2 += (double)val * (double)val;
		++count;
	}

	if (count)
	{
		_mean /= (double)count;
		mean = (ScalarType)_mean;

		if (variance)
		{
			_std2 = fabs(_std2/(double)count - _mean*_mean);
			*variance = (ScalarType)_std2;
		}
	}
	else
	{
		mean = 0;
		if (variance)
			*variance = 0;
	}
}

ScalarType DcGp::DcGpScalarField::GetMin()
{
	ComputeMinAndMax();
	return m_pDcGpScalarFieldImpl->m_minVal;
}

ScalarType DcGp::DcGpScalarField::GetMax()
{
	ComputeMinAndMax();
	return m_pDcGpScalarFieldImpl->m_maxVal;
}

void DcGp::DcGpScalarField::AddData(unsigned index, ScalarType data)
{
	m_pDcGpScalarFieldImpl->m_data[index] = data;
}

double DcGp::DcGpScalarField::GetPointScalarValue(unsigned index)
{
	return m_pDcGpScalarFieldImpl->m_data.at(index);
}

unsigned DcGp::DcGpScalarField::Size()
{
	return m_pDcGpScalarFieldImpl->m_data.size();
}

void DcGp::DcGpScalarField::SetDatas(const unsigned& size, const int& value)
{
	//�������
	m_pDcGpScalarFieldImpl->m_data.resize(size);
	std::fill(m_pDcGpScalarFieldImpl->m_data.begin(), m_pDcGpScalarFieldImpl->m_data.end(), value);
}

void DcGp::DcGpScalarField::SetDatas(std::vector<ScalarType> values)
{
	if (m_pDcGpScalarFieldImpl->m_data.size() > 0)
	{
		m_pDcGpScalarFieldImpl->m_data = values;
	}
}

std::vector<ScalarType >& DcGp::DcGpScalarField::GetDatas()
{
	return m_pDcGpScalarFieldImpl->m_data;
}

void DcGp::DcGpScalarField::SetColorSteps(unsigned steps)
{
	if (steps > 1024)
		m_pDcGpScalarFieldImpl->m_colorSteps = 1024;
	else if (steps < 2)
		m_pDcGpScalarFieldImpl->m_colorSteps = 2;
	else
		m_pDcGpScalarFieldImpl->m_colorSteps = steps;
}