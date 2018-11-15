#ifndef DCGP_POLYLINE_H
#define DCGP_POLYLINE_H

#include "DCGp/DcGpDLL.h"

#include <vector>

#include "DCCore/DCDefs.h"
#include "DCGp/GpBasicTypes.h"

namespace DcGp
{
	DECLARE_SMARTPTR(DcGpPolyline)
	DECLARE_SMARTPTR(DcGpPolylineImpl)
	//�򵥵Ķ������

	class DCGP_API DcGpPolyline
	{
		DECLARE_IMPLEMENT(DcGpPolyline)
	public:

		//���캯��
		DcGpPolyline();
		//��������
		virtual ~DcGpPolyline()
		{}

		//��ӵ�
		inline void AddPoint(PointCoord point);

		//��ȡ��
		inline PointCoord GetPoint(unsigned index) const;

		//��ȡ�㣨�����棩
		inline void GetPoint(unsigned index, DCVector3D& point) const;

		//��ȡ���ж���
		inline std::vector<DCVector3D> GetPoints() const;
		void SetPoints(std::vector<DCVector3D> pts);

		//����ȫ����ɫ
		inline void SetGlobalColor(RGBColor color);

		//��ȡȫ����ɫ
		inline RGBColor GetGlobalColor() const;

		//��ȡ�պ�״̬
		inline bool GetClosingState() const;

		//���ñպ�״̬
		inline void SetClosingState(bool state);
		
		void ChangePointByIndex(unsigned index, int x, int y);

		void ChangePointByIndex(unsigned index, DCVector3D point);

		void DeleteLastPoint();

		//������ж���
		inline void Clear();

		//������Ŀ
		inline unsigned GetVertexNumber() const;
	};

}

#endif //DC_POLYLINE_HEADER
