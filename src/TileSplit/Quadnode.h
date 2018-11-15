#ifndef DC_PYRA_QUADNODE_H
#define DC_PYRA_QUADNODE_H

//Qt
#include "QString"
#include "QVector"

#include "Common.h"
#include "DCGp/GpBasicTypes.h"

namespace DcGp
{
	class DcGpEntity;
}

namespace DC
{
	namespace Pyra
	{
		class BoundBox;
		class Geometry;

		class Quadnode
		{
		public:
			Quadnode();
			~Quadnode();

			void SetBoundBox(BoundBox* box);
			BoundBox* GetBoundBox() const;

			void SetLevel(const LevelType& level);
			int GetLevel() const;

			void SetFullName(const QString& fullName);

			//���һ���ӽڵ�
			void AddChild(Quadnode* child, bool dependant = true);

			void SetParent(Quadnode* parent);
			Quadnode* GetParent() const;

			void Draw(const float& actualPsize, std::array<std::array<float, 4> ,6> frustum, unsigned depth);

			//��װ����ļ���ͼԪ��
			void SetGeometry(Geometry* geometry);

		//�Ӿ�����߽�вü��ӿ�
		bool PointInFrustum(float x, float y, float z, std::array<std::array<float, 4> ,6> frustum);
		bool CubeInFrustum( float x, float y, float z, float size, std::array<std::array<float, 4> ,6> frustum);
		bool BoxInFrustum(const Point3D& minCorner, const Point3D& maxCorner, std::array<std::array<float, 4> ,6> frustum);
		//���ݻ�ȡ�ӿ�
		public:

			//�����Ƿ�������
			std::vector<DCVector3D>& GetPoints();
			unsigned GetPointsSize();

			std::vector<PointColor>& GetRGBColors();
		private:
			void SetNodeLoadState(bool state);
		private:
			//�߽��
			BoundBox* m_box;

			//���ڵĲ㼶��
			LevelType m_level;

			//�������ļ���
			QString m_fileFullname;

			//�ӽڵ�����
			QVector<Quadnode* > m_children;

			//ͼԪ����
			Geometry* m_geometry;

			std::vector<DCVector3D> m_points;
			std::vector<PointColor> m_rgbColors;

			//�ڵ��Ƿ��Ѽ���
			bool m_isLoaded;

			unsigned m_lastLevel;

			//���ڵ�
			Quadnode* m_parent;
		};
	}
}



#endif // DC_PYRA_QUADNODE_H
