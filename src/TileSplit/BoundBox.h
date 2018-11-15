#ifndef DC_PYRA_BOUNDBOX_H
#define DC_PYRA_BOUNDBOX_H

#include "pyramidDLL.h"
//common
#include "DCCore/BasicTypes.h"
#include "DCCore/Const.h"

#include "Common.h"

namespace DC
{
	namespace Pyra
	{
		class PYRAMID_API BoundBox
		{
		public:
			Point3D m_minCorner;
			Point3D m_maxCorner;
			double m_length;  //��
			double m_width;   //��
			double m_zBuffer; //zֵ

			double m_sphereRadius;

			BoundBox()
				: m_minCorner(DCCore::MAX_POINTCOORD, DCCore::MAX_POINTCOORD, DCCore::MAX_POINTCOORD)
				, m_maxCorner(DCCore::MIN_POINTCOORD, DCCore::MIN_POINTCOORD, DCCore::MIN_POINTCOORD)
			{
				m_length = m_width = m_zBuffer = 0.0;
			}

			BoundBox(const Point3D& minCorner, const Point3D& maxCorner)
				: m_minCorner(minCorner)
				, m_maxCorner(maxCorner)
			{
				m_length = m_maxCorner.x - m_minCorner.x;
				m_width = m_maxCorner.y - m_minCorner.y;
				m_zBuffer = m_maxCorner.z - m_minCorner.z;

				m_sphereRadius = sqrt(0.25 *  ((maxCorner - minCorner).Normal2()) );
			}

			Point3D GetCenter() const
			{
				//����zֵ�Ĵ���
				Point3D pp = (m_maxCorner + m_minCorner);
				pp.x *= 0.5;
				pp.y *= 0.5;
				return pp;
			}

			void GetPointIndexInLevel(const Point3D& point, const int& level, unsigned& column, unsigned& row)
			{
				int colCellNum =  std::pow(double(2),(level-1));
				double xInter = m_length / colCellNum;
				double yInter = m_width / colCellNum;
				
				Point3D tp = point - m_minCorner;
				column =  std::floor(tp.x / xInter) == colCellNum ? std::floor(tp.x / xInter) - 1 : std::floor(tp.x / xInter);

				row = std::floor(tp.y / yInter)  == colCellNum ? std::floor(tp.y / yInter) - 1 : std::floor(tp.y / yInter);

			}
		};
	}
}

#endif