#ifndef DCGP_INTERACTOR_H
#define DCGP_INTERACTOR_H

#include "DCGp/DcGpDLL.h"

//Common
#include "DCCore/BasicTypes.h"

namespace DcGp
{

//�������ӿ�(��ά�Ӵ��п����϶���ѡ���ʵ��)
	class DCGP_API DcGpInteractor
	{
	public:
		virtual bool AcceptClick(int x, int y, Qt::MouseButton button) { return false; }

		//
		virtual bool Move2D(int x, int y, int dx, int dy, int screenWidth, int screenHeight) { return false; }

		//
		virtual bool Move3D(const DCVector3D& u) { return false; }

	};

}
#endif
