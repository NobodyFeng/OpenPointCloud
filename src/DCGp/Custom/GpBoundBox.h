#ifndef DCGP_BOUND_BOX_H
#define DCGP_BOUND_BOX_H

#include "DCGp/DcGpDLL.h"

#include "DCCore/DCDefs.h"

#include "QString"

//DCCore
#include "DCCore/BasicTypes.h"
#include "DCCore/Matrix.h"

#include "DCGp/GpMatrix.h"
#include "DCGp/GpBasicTypes.h"

namespace DcGp
{
	DECLARE_SMARTPTR(DcGpBoundBoxImpl)

class DCGP_API DcGpBoundBox
{
	DECLARE_IMPLEMENT(DcGpBoundBox)

	GET_CLASSNAME(DcGpBoundBox)

public:

	DcGpBoundBox();

	DcGpBoundBox(const DcGpBoundBox& aBBox);
	DcGpBoundBox(const DCVector3D &bbMinCorner, const DCVector3D &bbMaxCorner);

	//! ���
	DcGpBoundBox operator + (const DcGpBoundBox& aBBox) const;

	const DcGpBoundBox& operator += (const DcGpBoundBox& aBBox);
	//! ƫ��
	const DcGpBoundBox& operator += (const DCVector3D& aVector);
	//!ƫ��
	const DcGpBoundBox& operator -= (const DCVector3D& aVector);
	//!����
	const DcGpBoundBox& operator *= (const PointCoordinateType& scaleFactor);
	//! ��ת
	const DcGpBoundBox& operator *= (const DCCore::SquareMatrix& aMatrix);
	//!���ؾ���
	const DcGpBoundBox& operator *= (const DcGpMatrix& mat);

	void Clear();

	//����operator += (const DCVector3D& aVector);
	void Add(const DCVector3D& aVector);

	//! ���½�����
	const DCVector3D& MinCorner() const;
	//! ���Ͻ�����
	const DCVector3D& MaxCorner() const;

	//! ���½�����
	DCVector3D& MinCorner();
	//! ���Ͻ�����
	DCVector3D& MaxCorner();

	//! ����
	DCVector3D GetCenter() const;
	//! �Խ�������
	DCVector3D GetDiagVector() const;
	//! �Խ��߳���
	PointCoordinateType GetDiagNormal() const;

	//���Ʊ߽��
	void Draw(const ColorType col[]) const;

	//! ���ñ߽�пɼ�״̬
	void SetValidity(bool state);

	//! �����Ƿ�ɼ�
	bool IsValid() const;

	//! ���������߽�еľ��Ծ���
	// ����-1����һ���߽����Ч
	PointCoordinateType MinDistTo(const DcGpBoundBox& box) const;

	//���ص��Ƿ��ڱ߽����
	inline bool Contains(const DCVector3D& P) const;
};
}
#endif
