#ifndef DC_PYRA_QUADTREE_H
#define DC_PYRA_QUADTREE_H

#include "pyramidDLL.h"
#include "QString"
#include <array>

namespace DC
{
	namespace Pyra
	{
		class Quadnode;
		class BoundBox;
		class NodeDebug;

		class PYRAMID_API Quadtree
		{
		public:
			Quadtree(int depth, BoundBox* box, QString fileName);

			//���ݴ������ļ�����������basename��Ȼ��Ӷ�Ӧ��box�����ļ��У���ʼ��box
			Quadtree(QString fileName);
			~Quadtree();

			//���ݴ�������ֵ�ͱ߽�У������Ĳ����ṹ�ָ�����νṹ
			void QuadCreateBranch(Quadnode* parent);

			Quadnode* GetRoot() const;

			QString GetBaseFileName() const;

			//�ػ�
			void Draw(const float& actualPsize, std::array<std::array<float, 4> ,6> frustum, const float& inter);

			BoundBox* GetBoundBox() const;

			uint CaluLoadedLevel(float pixelSize, uint depth, const float& inter);
		private:
			//���ݴ�����ļ�������ʼ��������basename
			void InitGenBaseNameFromOther(QString name);

			//����basename����ʼ��box
			void InitBoxAndDepthFromFile();
		private: 
			//��ʼ����
			float m_initDis;
			//�Ƿ��ʼ��
			bool m_isDisInited;
			
			//���
			unsigned m_depth;
			
			//���ڵ�
			Quadnode* m_root;

			//basename
			QString m_baseFileName;

			QString m_suffix;

			//dirPath
			QString m_dir;

			BoundBox* m_boundBox;

			NodeDebug* m_nodeDebug;
		};
	}
}

#endif // DC_PYRA_QUADTREE_H
