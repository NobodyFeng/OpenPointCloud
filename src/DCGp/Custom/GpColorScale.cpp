#include <algorithm>

//Qt
#include "QString"
#include "QtGui/QColor"
#include "QFile"

//common
#include "DCCore/Logger.h"

#include "GpColorScale.h"

//Ĭ�ϵ���ɫ��
static std::map<QString, DcGp::DcGpColorScalePtr> DefaultColorScales;

namespace DcGp
{
	DECLARE_SMARTPTR(DcGpColorScaleElementImpl)

	class DcGpColorScaleElementImpl
	{
		DECLARE_PHONY_CONSTRUCTOR(DcGpColorScaleElementImpl)

	protected:
		DcGpColorScaleElementImpl()
			: m_position(0.0)
			, m_color(Qt::black)
		{}

	private:
		friend class DcGpColorScaleElement;

		//�̶ȳ��е�λ��
		//������0.0~1.0֮��
		double m_position;
		//��ɫ
		QColor m_color;
	};

	DECLARE_SMARTPTR(DcGpColorScaleImpl)

	class DcGpColorScaleImpl
	{
		DECLARE_PHONY_CONSTRUCTOR(DcGpColorScaleImpl)

	protected:
		DcGpColorScaleImpl()
		{}

	private:
		friend class DcGpColorScale;

		//����
		static unsigned s_number;
		//��С����
		static const unsigned MIN_STEPS = 2;
		//Ĭ�ϲ���
		static const unsigned DEFAULT_STEPS = 256;
		//��󲽳�
		static const unsigned MAX_STEPS = 1024;
		//����
		QString m_name;
		//Ψһ��ʶ
		unsigned m_uniqueID;
		//Ԫ��
		QList<DcGpColorScaleElement> m_elements;
		//
		std::array<RGBAColor, MAX_STEPS> m_rgbaScale;
		//
		bool m_updated;
		//
		bool m_relative;
		//�Ƿ�����
		bool m_locked;
		double m_absoluteMinValue;
		double m_absoluteRange;
	};
}

unsigned DcGp::DcGpColorScaleImpl::s_number = 0;

/***************************************DcGpColorScaleElement**************************/
DcGp::DcGpColorScaleElement::DcGpColorScaleElement()
{
	m_pDcGpColorScaleElementImpl = DcGpColorScaleElementImpl::CreateObject();
}

DcGp::DcGpColorScaleElement::DcGpColorScaleElement(double position, QColor color)
{
	m_pDcGpColorScaleElementImpl = DcGpColorScaleElementImpl::CreateObject();

	m_pDcGpColorScaleElementImpl->m_position = position;
	m_pDcGpColorScaleElementImpl->m_color = color;
}

double DcGp::DcGpColorScaleElement::Position() const
{ 
	return m_pDcGpColorScaleElementImpl->m_position; 
}

double& DcGp::DcGpColorScaleElement::Position()
{ 
	return m_pDcGpColorScaleElementImpl->m_position; 
}

QColor DcGp::DcGpColorScaleElement::Color() const
{
	return m_pDcGpColorScaleElementImpl->m_color;
}

QColor& DcGp::DcGpColorScaleElement::Color()
{
	return m_pDcGpColorScaleElementImpl->m_color;
}

bool DcGp::DcGpColorScaleElement::IsSmaller(const DcGpColorScaleElement& e1, const DcGpColorScaleElement& e2)
{
	return e1.Position() < e2.Position();
}

/**************************************DcGpColorScale**********************************************/

DcGp::DcGpColorScale::DcGpColorScale()
{
	m_pDcGpColorScaleImpl = DcGpColorScaleImpl::CreateObject();

	m_pDcGpColorScaleImpl->m_name = "";

	m_pDcGpColorScaleImpl->m_uniqueID = m_pDcGpColorScaleImpl->s_number++;
	m_pDcGpColorScaleImpl->m_updated = false;
	m_pDcGpColorScaleImpl->m_relative = true;
	m_pDcGpColorScaleImpl->m_locked = false;
	m_pDcGpColorScaleImpl->m_absoluteMinValue = 0.0;
	m_pDcGpColorScaleImpl->m_absoluteRange = 1.0;
}

DcGp::DcGpColorScale::DcGpColorScale(QString name)
{
	m_pDcGpColorScaleImpl = DcGpColorScaleImpl::CreateObject();

	m_pDcGpColorScaleImpl->m_name = name;

	m_pDcGpColorScaleImpl->m_uniqueID = m_pDcGpColorScaleImpl->s_number++;
	m_pDcGpColorScaleImpl->m_updated = false;
	m_pDcGpColorScaleImpl->m_relative = true;
	m_pDcGpColorScaleImpl->m_locked = false;
	m_pDcGpColorScaleImpl->m_absoluteMinValue = 0.0;
	m_pDcGpColorScaleImpl->m_absoluteRange = 1.0;
}

DcGp::DcGpColorScale::DcGpColorScale(QString name, QList<DcGpColorScaleElement> elements)
{
	m_pDcGpColorScaleImpl = DcGpColorScaleImpl::CreateObject();

	m_pDcGpColorScaleImpl->m_name = name;

	m_pDcGpColorScaleImpl->m_uniqueID = m_pDcGpColorScaleImpl->s_number++;
	m_pDcGpColorScaleImpl->m_updated = false;
	m_pDcGpColorScaleImpl->m_relative = true;
	m_pDcGpColorScaleImpl->m_locked = false;
	m_pDcGpColorScaleImpl->m_absoluteMinValue = 0.0;
	m_pDcGpColorScaleImpl->m_absoluteRange = 1.0;
	m_pDcGpColorScaleImpl->m_elements = elements;

	Update();
}

DcGp::DcGpColorScale::~DcGpColorScale()
{
}

std::map<QString, DcGp::DcGpColorScalePtr> DcGp::DcGpColorScale::DefualtColorScales()
{	
	if (!DefaultColorScales.empty())
	{
		return DefaultColorScales;
	}

	//Ĭ�ϵļ���
	std::map<QString, DcGpColorScalePtr> result;
	//��->��
	DcGpColorScalePtr whiteToBlack(new DcGpColorScale("White->Black"));
	whiteToBlack->Insert(DcGpColorScaleElement(0.0, Qt::white));
	whiteToBlack->Insert(DcGpColorScaleElement(1.0, Qt::black));
	result["White->Black"] = whiteToBlack;

	//��->��
	DcGpColorScalePtr blackToWhite(new DcGpColorScale("Black->White"));
	blackToWhite->Insert(DcGpColorScaleElement(0.0, Qt::black));
	blackToWhite->Insert(DcGpColorScaleElement(1.0, Qt::white));
	result["Black->White"] = blackToWhite;

	//��->��->��
	DcGpColorScalePtr whiteBlackRed(new DcGpColorScale("White->Black->Red"));
	whiteBlackRed->Insert(DcGpColorScaleElement(0.0, Qt::white));
	whiteBlackRed->Insert(DcGpColorScaleElement(1.0, Qt::red));
	whiteBlackRed->Insert(DcGpColorScaleElement(0.5, Qt::black));
	result["White->Black->Red"] = whiteBlackRed;

	//��->��->��
	DcGpColorScalePtr redBlackWhite(new DcGpColorScale("Red->Black->White"));
	redBlackWhite->Insert(DcGpColorScaleElement(0.0, Qt::red));
	redBlackWhite->Insert(DcGpColorScaleElement(1.0, Qt::white));
	redBlackWhite->Insert(DcGpColorScaleElement(0.5, Qt::black));
	result["Red->Black->White"] = redBlackWhite;

	//�ʺ磺��->��
	DcGpColorScalePtr rainbowRedToPurple(new DcGpColorScale("Rainbow:Red->Purple"));
	rainbowRedToPurple->Insert(DcGpColorScaleElement(0.0, Qt::red));
	rainbowRedToPurple->Insert(DcGpColorScaleElement(1.0, QColor(160, 32, 240)));
	rainbowRedToPurple->Insert(DcGpColorScaleElement(0.166, QColor(255, 165, 0)));
	rainbowRedToPurple->Insert(DcGpColorScaleElement(0.333, Qt::yellow));
	rainbowRedToPurple->Insert(DcGpColorScaleElement(0.5, Qt::green));
	rainbowRedToPurple->Insert(DcGpColorScaleElement(0.666, Qt::cyan));
	rainbowRedToPurple->Insert(DcGpColorScaleElement(0.833, Qt::blue));
	result["Rainbow:Red->Purple"] = rainbowRedToPurple;

	//�ʺ磺��->��
	DcGpColorScalePtr rainbowPurpleToRed(new DcGpColorScale("Rainbow:Purple->Red"));
	rainbowPurpleToRed->Insert(DcGpColorScaleElement(0.0, QColor(160, 32, 240)));
	rainbowPurpleToRed->Insert(DcGpColorScaleElement(1.0, Qt::red));
	rainbowPurpleToRed->Insert(DcGpColorScaleElement(0.166, Qt::blue));
	rainbowPurpleToRed->Insert(DcGpColorScaleElement(0.333, Qt::cyan));
	rainbowPurpleToRed->Insert(DcGpColorScaleElement(0.5, Qt::green));
	rainbowPurpleToRed->Insert(DcGpColorScaleElement(0.666, Qt::yellow));
	rainbowPurpleToRed->Insert(DcGpColorScaleElement(0.833, QColor(255, 165, 0)));
	result["Rainbow:Purple->Red"] = rainbowPurpleToRed;

	//BGYR
	DcGpColorScalePtr bgyr(new DcGpColorScale("Blue>Green>Yellow>Red"));
	bgyr->Insert(DcGpColorScaleElement(0.0,Qt::blue));
	bgyr->Insert(DcGpColorScaleElement(1.0/3.0,Qt::green));
	bgyr->Insert(DcGpColorScaleElement(2.0/3.0,Qt::yellow));
	bgyr->Insert(DcGpColorScaleElement(1.0,Qt::red));
	result["Blue>Green>Yellow>Red"] = bgyr;

	//RYGB
	DcGpColorScalePtr rygb(new DcGpColorScale("Red>Yellow>Green>Blue"));
	rygb->Insert(DcGpColorScaleElement(0.0,Qt::red));
	rygb->Insert(DcGpColorScaleElement(1.0/3.0,Qt::yellow));
	rygb->Insert(DcGpColorScaleElement(2.0/3.0,Qt::green));
	rygb->Insert(DcGpColorScaleElement(1.0,Qt::blue));
	result["Red>Yellow>Green>Blue"] = rygb;

	DefaultColorScales = result;

	return result;
}

QString DcGp::DcGpColorScale::GetName() const 
{ 
	return m_pDcGpColorScaleImpl->m_name;
}

void DcGp::DcGpColorScale::SetName(const QString& name) 
{
	m_pDcGpColorScaleImpl->m_name = name;
}

QString DcGp::DcGpColorScale::GetUniqueID() const 
{ 
	return m_pDcGpColorScaleImpl->m_uniqueID;
}


void DcGp::DcGpColorScale::SetUniqueID(unsigned uniqueID) 
{
	m_pDcGpColorScaleImpl->m_uniqueID = uniqueID;
}

bool DcGp::DcGpColorScale::IsRelative() const 
{ 
	return m_pDcGpColorScaleImpl->m_relative; 
}

void DcGp::DcGpColorScale::SetRelative() 
{
	m_pDcGpColorScaleImpl->m_relative = true;
}

bool DcGp::DcGpColorScale::IsLocked() const 
{
	return m_pDcGpColorScaleImpl->m_locked;
}

void DcGp::DcGpColorScale::SetLocked(bool state)
{
	m_pDcGpColorScaleImpl->m_locked = state;
}

int DcGp::DcGpColorScale::Count() const 
{
	return m_pDcGpColorScaleImpl->m_elements.size(); 
}

DcGp::DcGpColorScaleElement& DcGp::DcGpColorScale::Element(int index)
{
	return m_pDcGpColorScaleImpl->m_elements[index]; 
}

const DcGp::DcGpColorScaleElement& DcGp::DcGpColorScale::Element(int index) const
{
	return m_pDcGpColorScaleImpl->m_elements[index];
}

double DcGp::DcGpColorScale::GetRelativePosition(double value) const
{
	//assert(m_pDcGpColorScaleImpl->m_updated && !m_pDcGpColorScaleImpl->m_relative);
	return (value - m_pDcGpColorScaleImpl->m_absoluteMinValue)/m_pDcGpColorScaleImpl->m_absoluteRange;
}

const RGBAColor DcGp::DcGpColorScale::GetColorByValue(double value, const RGBAColor outOfRangeColor) const
{
//	assert(m_pDcGpColorScaleImpl->m_updated && !m_pDcGpColorScaleImpl->m_relative);
	double relativePos = GetRelativePosition(value);
	return (relativePos >= 0.0 && relativePos <= 1.0 ? GetColorByRelativePos(relativePos) : outOfRangeColor);
}

const RGBAColor DcGp::DcGpColorScale::GetColorByRelativePos(double relativePos) const
{
	assert(m_pDcGpColorScaleImpl->m_updated);
	if (relativePos >= 0.0 && relativePos <= 1.0)
	{
		auto aaa = relativePos * (double)(m_pDcGpColorScaleImpl->MAX_STEPS-1);
		return GetColorByIndex((unsigned)(relativePos * (double)(m_pDcGpColorScaleImpl->MAX_STEPS-1)));
	}
	else
	{
		RGBAColor outOfRangeColor = {0, 0, 0, 0};
		return outOfRangeColor;
	}
}

const RGBAColor DcGp::DcGpColorScale::GetColorByRelativePos(double relativePos, unsigned steps, const RGBAColor outOfRangeColor) const
{
	assert(m_pDcGpColorScaleImpl->m_updated);
	if (relativePos >= 0.0 && relativePos <= 1.0)
	{
		//quantized (16 bits) version --> much faster than floor!
		unsigned index = ((unsigned)((relativePos*(double)steps)*65535.0))>>16;
		return GetColorByIndex((index*(m_pDcGpColorScaleImpl->MAX_STEPS-1)) / (steps-1));
	}
	else
	{
		return outOfRangeColor;
	}
}

const RGBAColor DcGp::DcGpColorScale::GetColorByIndex(unsigned index) const
{
	assert(m_pDcGpColorScaleImpl->m_updated && index < m_pDcGpColorScaleImpl->MAX_STEPS);
	return m_pDcGpColorScaleImpl->m_rgbaScale[index];
}

//����Ԫ��
void DcGp::DcGpColorScale::Insert(const DcGpColorScaleElement& element, bool autoUpdate/*=true*/)
{
	//����̶ȳ��Ƿ�����
	if (m_pDcGpColorScaleImpl->m_locked)
	{
		DCCore::Logger::Warning(QObject::tr("[DcGpColorScale::insert] Scale '%1' is locked!").arg(m_pDcGpColorScaleImpl->m_name));
		return;
	}

	//���Ԫ��
	m_pDcGpColorScaleImpl->m_elements.push_back(element);

	//
	m_pDcGpColorScaleImpl->m_updated = false;

	//���и��¿̶ȳ�
	if (autoUpdate && m_pDcGpColorScaleImpl->m_elements.size() 
		>= (int)m_pDcGpColorScaleImpl->MIN_STEPS)
		Update();
}

//��տ̶ȳ�
void DcGp::DcGpColorScale::Clear()
{
	//����̶ȳ��Ƿ�����
	if (m_pDcGpColorScaleImpl->m_locked)
	{
		DCCore::Logger::Warning(QObject::tr("[DcGpColorScale::clear] Scale '%1' is locked!").arg(m_pDcGpColorScaleImpl->m_name));
		return;
	}

	//���Ԫ��
	m_pDcGpColorScaleImpl->m_elements.clear();

	m_pDcGpColorScaleImpl->m_updated = false;
}

//�Ƴ�Ԫ��
void DcGp::DcGpColorScale::Remove(int index, bool autoUpdate/*=true*/)
{
	//����̶ȳ��Ƿ�����
	if (m_pDcGpColorScaleImpl->m_locked)
	{
		DCCore::Logger::Warning(QObject::tr("[DcGpColorScale::remove] Scale '%1' is locked!").arg(m_pDcGpColorScaleImpl->m_name));
		return;
	}

	m_pDcGpColorScaleImpl->m_elements.removeAt(index);
	m_pDcGpColorScaleImpl->m_updated = false;

	if (autoUpdate)
		Update();
}

//��Ԫ�ؽ�������
void DcGp::DcGpColorScale::Sort()
{
	qSort(m_pDcGpColorScaleImpl->m_elements.begin(), 
			m_pDcGpColorScaleImpl->m_elements.end(), 
			DcGpColorScaleElement::IsSmaller);
}

//���¿̶ȳ�
void DcGp::DcGpColorScale::Update()
{
	if (m_pDcGpColorScaleImpl->m_elements.size() 
		>= (int)m_pDcGpColorScaleImpl->MIN_STEPS)
	{
		//�Ƚ�������
		Sort();

		unsigned stepCount = static_cast<unsigned>(m_pDcGpColorScaleImpl->m_elements.size());

		//�ж�ֵ�Ƿ���Ч
		if (m_pDcGpColorScaleImpl->m_elements.front().Position() != 0.0 
			|| m_pDcGpColorScaleImpl->m_elements.back().Position() != 1.0)
		{
			//invalid scale: paint it black ;)
			RGBAColor color = {0, 0, 0};
			std::fill(m_pDcGpColorScaleImpl->m_rgbaScale.begin(), 
						m_pDcGpColorScaleImpl->m_rgbaScale.end(), color);

			//DcLib::DcLog::Warning(QObject::tr("[DcGpColorScale] Scale '%1' is invalid! (boundaries are not [0.0-1.0]").arg(GetName()));
			return;
		}

		unsigned j = 0; //current intervale
		for (unsigned i = 0; i < m_pDcGpColorScaleImpl->MAX_STEPS; ++i)
		{
			double relativePos = static_cast<double>(i) / static_cast<double>(m_pDcGpColorScaleImpl->MAX_STEPS - 1);

			//forward to the right intervale
			while (j + 2 < stepCount && m_pDcGpColorScaleImpl->m_elements[j + 1].Position() < relativePos)
			{
				++j;
			}

			// ���Բ�ֵ��ɫ
			DCVector3D colBefore(m_pDcGpColorScaleImpl->m_elements[j].Color().redF(),
								m_pDcGpColorScaleImpl->m_elements[j].Color().greenF(),
								m_pDcGpColorScaleImpl->m_elements[j].Color().blueF());

			DCVector3D colNext(m_pDcGpColorScaleImpl->m_elements[j+1].Color().redF(),
								m_pDcGpColorScaleImpl->m_elements[j+1].Color().greenF(),
								m_pDcGpColorScaleImpl->m_elements[j+1].Color().blueF());

			//͸����
			double alpha = (relativePos - m_pDcGpColorScaleImpl->m_elements[j].Position())
							/(m_pDcGpColorScaleImpl->m_elements[j+1].Position()
							- m_pDcGpColorScaleImpl->m_elements[j].Position());

			DCVector3D interpCol = colBefore + (colNext-colBefore) * alpha;

			m_pDcGpColorScaleImpl->m_rgbaScale[i][0] = static_cast<ColorType>(interpCol.x * (double)MAX_COLOR_COMP);
			m_pDcGpColorScaleImpl->m_rgbaScale[i][1] = static_cast<ColorType>(interpCol.y * (double)MAX_COLOR_COMP);
			m_pDcGpColorScaleImpl->m_rgbaScale[i][2] = static_cast<ColorType>(interpCol.z * (double)MAX_COLOR_COMP);
			m_pDcGpColorScaleImpl->m_rgbaScale[i][3] = MAX_COLOR_COMP; //do not dream: no transparency ;)
		}

		m_pDcGpColorScaleImpl->m_updated = true;
	}
	else //invalid scale: paint it black ;)
	{
		DCCore::Logger::Warning(QObject::tr("[DcGpColorScale] Scale '%1' is invalid! (not enough elements)").arg(GetName()));
		RGBAColor color = {0, 0, 0};
		std::fill(m_pDcGpColorScaleImpl->m_rgbaScale.begin(), m_pDcGpColorScaleImpl->m_rgbaScale.end(), color);
	}
}

void DcGp::DcGpColorScale::SetAbsolute(double minVal, double maxVal)
{
	assert(maxVal >= minVal);

	m_pDcGpColorScaleImpl->m_relative = false;

	m_pDcGpColorScaleImpl->m_absoluteMinValue = minVal;
	m_pDcGpColorScaleImpl->m_absoluteRange = maxVal - minVal;

	//as 'm_absoluteRange' is used for division, we make sure it is not left to 0!
	m_pDcGpColorScaleImpl->m_absoluteRange = std::max(m_pDcGpColorScaleImpl->m_absoluteRange, 1e-12);
}

void DcGp::DcGpColorScale::GetAbsoluteBoundaries(double& minVal, double& maxVal) const
{
	minVal = m_pDcGpColorScaleImpl->m_absoluteMinValue;
	maxVal = m_pDcGpColorScaleImpl->m_absoluteMinValue + m_pDcGpColorScaleImpl->m_absoluteRange;
}
