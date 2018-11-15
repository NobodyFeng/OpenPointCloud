#ifndef DCGP_COLOR_SCALE_H
#define DCGP_COLOR_SCALE_H

#include "DCGp/DcGpDLL.h"

//C++��׼��
#include <assert.h>
#include <array>
#include <map>

//Qt��׼��
#include "QList"
class QString;
class QColor;

#include "DCCore/DCDefs.h"

//Local
#include "DCGp/GpBasicTypes.h"

namespace DcGp
{

//��ɫ�̶ȳߵ�Ԫ�أ�λ�ú���ɫ��
	DECLARE_SMARTPTR(DcGpColorScaleElement)
	DECLARE_SMARTPTR(DcGpColorScaleElementImpl)

class DCGP_API DcGpColorScaleElement
{
	DECLARE_PHONY_CONSTRUCTOR(DcGpColorScaleElement)
	DECLARE_IMPLEMENT(DcGpColorScaleElement)

public:
	//����λ��������ɫ
	DcGpColorScaleElement(double position, QColor color);

	//����λ��
	inline double Position() const;
	inline double& Position();

	//��ȡ��ɫ
	inline QColor Color() const;
	inline QColor& Color();

	//�Ƚ�������ɫԪ��
	inline static bool IsSmaller(const DcGpColorScaleElement& e1, const DcGpColorScaleElement& e2);

protected:
	//Ĭ�Ϲ��캯��
	DcGpColorScaleElement();
};

//��ɫ�̶ȳ�
//����ָ���ļ�����ɫ�Ͳ���������ɫ�̶ȳ�(��ɫ����)
//������С�ڸ�������ɫ��Ŀ
DECLARE_SMARTPTR(DcGpColorScale)
DECLARE_SMARTPTR(DcGpColorScaleImpl)

class DCGP_API DcGpColorScale
{
	DECLARE_PHONY_CONSTRUCTOR(DcGpColorScale)

	DECLARE_IMPLEMENT(DcGpColorScale)

public:
	//Ĭ�Ϲ��캯��
	//����һ���̶ȳߵ�����
	//��������Unique ID
	DcGpColorScale(QString name);

	//ʹ����ɫԪ�ؽ��й���
	DcGpColorScale(QString name, QList<DcGpColorScaleElement> elements);

	//��������
	virtual ~DcGpColorScale();

	//��ȡĬ�ϵ�ԭɫ��
	static std::map<QString, DcGpColorScalePtr> DefualtColorScales();

	//��ȡ����
	QString GetName() const;

	//��������
	void SetName(const QString& name);

	//����Ψһ��ʶ
	QString GetUniqueID() const;

	//����Ψһ���
	void SetUniqueID(unsigned uniqueID);
	inline bool IsRelative() const;

	inline void SetRelative();

	void SetAbsolute(double minVal, double maxVal);

	void GetAbsoluteBoundaries(double& minVal, double& maxVal) const;

	//�̶ȳ��Ƿ������������򲻿ɸ���
	inline bool IsLocked() const;

	//���ÿ̶ȳ��Ƿ�����
	inline void SetLocked(bool state);

	//���ص�ǰ�Ĳ���
	inline int Count() const;

	//��ȡָ��λ�õ�Ԫ��
	inline DcGpColorScaleElement& Element(int index);

	//��ȡָ��λ�õ�Ԫ��(const)
	inline const DcGpColorScaleElement& Element(int index) const;

	//���Ԫ�أ��̶ȳ߿ɲ���״̬��
	void Insert(const DcGpColorScaleElement& step, bool autoUpdate = true);

	//ɾ��ָ����Ԫ��
	void Remove(int index, bool autoUpdate = true);

	//�������Ԫ��
	void Clear();

	//����
	void Update();

	inline double GetRelativePosition(double value) const;

	inline const RGBAColor GetColorByValue(double value, const RGBAColor outOfRangeColor) const;

	inline const RGBAColor GetColorByRelativePos(double relativePos) const;

	inline const RGBAColor GetColorByRelativePos(double relativePos, unsigned steps, const RGBAColor outOfRangeColor) const;

	//ͨ��������ȡ��ɫ
	inline const RGBAColor GetColorByIndex(unsigned index) const;

protected:
	DcGpColorScale();
	//��Ԫ�ؽ�������
	void Sort();
};

}
#endif //CC_COLOR_SCALE_HEADER
