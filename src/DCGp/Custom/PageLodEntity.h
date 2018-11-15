#pragma once

#include "DCGp/DcGpDLL.h"

#include <QObject>
#include "DCGp/GpEntity.h"
#include "DCGp/Custom/GpPointCloud.h"

namespace DCUtil
{
	class DatabaseRequest;
	
}

namespace DcGp
{
	class DcGpDrawContext;
	class DcGpBoundBox;

	struct DCGP_API PerRangeData
	{
		PerRangeData();

		QString                     _filename;
		float                           _priorityOffset;
		float                           _priorityScale;
		double                          _timeStamp;
		unsigned int                    _frameNumber;

		//��Ҫ����һ����������ڵ㣬Ĭ��Ϊnullptr�����غ���ֵ���������ּ��������Ƿ�����ӣ������Ƿ��Ѷ��ꡣ
		boost::shared_ptr<DCUtil::DatabaseRequest > m_databaseRequest;
	};

	typedef std::vector<PerRangeData> PerRangeDataList;
	typedef std::pair<float, float>  MinMaxPair;
	typedef MinMaxPair Range;

	class DCGP_API PageLodEntity : public DcGp::DcGpEntity
	{
		Q_OBJECT
			GET_CLASSNAME(PageLodEntity)
	public:
		PageLodEntity(QString name);
		~PageLodEntity();

		virtual void Accept(DCUtil::AbstractEntityVisitor& nv);

		virtual void Traverse(DCUtil::AbstractEntityVisitor& nv);

		//����ͼ��
		virtual void DrawMyselfOnly(DcGp::DcGpDrawContext& context);
		virtual void FastDrawMyselfOnly(DcGpDrawContext& context);

		void SetTopLevelEntity(DcGp::DcGpPointCloud* topLevelEntity);

		//��ӵ�
		void AddPoint(DCVector3D vector);

		//������ɫ
		void AddColor(PointColor color);

		unsigned Size();

		//
		virtual DcGpBoundBox GetDisplayBoundBox();
		//��ȡ�߽��
		virtual DcGpBoundBox GetMyOwnBoundBox();

		//std::vector<QString > GetPageQuadtree();
		void SetFilename(unsigned int childNo, const QString& filename);

		/** Set the database path to prepend to children's filenames.*/
		void SetDatabasePath(const QString& path)
		{
			m_databasePath = path;
		}

		/** Get the database path used to prepend to children's filenames.*/
		inline const QString& GetDatabasePath() const { return m_databasePath; }
		PerRangeDataList GetPerRangeDataList() { return m_perRangeDataList;  }

		void SetRange(Range rg)
		{
			m_range = rg;
		}

		void SetCenter(DCVector3D center)
		{
			m_pageBoxCenter = center;
		}
	private:
		DcGp::DcGpPointCloud* m_topLevelEntity;

		//!�������Ӽ��ļ�����
		PerRangeDataList m_perRangeDataList;
		QString         m_databasePath;

		//! �����߽�����ģ���ͬ��ʵ�ʵ����ݱ߽��
		DCVector3D m_pageBoxCenter;

		//! ֧�ֵľ���ֵ
		Range m_range;

		//std::vector<QString > m_pageQuadtree;
	};
}

