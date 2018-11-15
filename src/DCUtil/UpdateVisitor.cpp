#include "UpdateVisitor.h"

#include "QThreadPool"

//dcgp
#include "DCGp/Custom/GpPointCloud.h"
#include "DCGp/Custom/PageLodEntity.h"

#include "DCUtil/PageLodTaskThread.h"

using namespace DCUtil;

UpdateVisitor::UpdateVisitor(TraversalMode tm)
	: DCUtil::AbstractEntityVisitor(tm)
	, m_lodScale(1.0)
{
	m_pEntityQueueManager = new EntityQueueManager();

	m_databaseRequestHandler = new DCUtil::DatabasePage();
}

UpdateVisitor::~UpdateVisitor()
{

}


void UpdateVisitor::Apply(DcGp::DcGpEntity& node)
{
	Traverse(node);
}

void UpdateVisitor::Apply(DcGp::DcGpPointCloud& node)
{
	Apply(static_cast<DcGp::DcGpEntity& >(node));
}

void UpdateVisitor::Apply(DcGp::PageLodEntity& node)
{
	//! ���ڷ�ҳ�ڵ㣬����ݵ�ǰ�ӵ��뵱ǰ���ڵ�߽�еľ��룬��ѡ��ȡ����
	//! ��ȡ��ҳ�ڵ���Ĳ����ļ�����������й�������ʹ���̼߳�������
	//! ���߳��ж�ȡ���������ӵ����ڵ���
	//PageLodTaskThread* ioThread = new PageLodTaskThread(m_pEntityQueueManager, &node, 1000);
	//QThreadPool::globalInstance()->start(ioThread);

	//! ��ȡ�����е�ʵ�壬ɾ��֮ǰ���ӽڵ㣬������ڶ��е�page�ӽڵ���
	//while (!node.GetChildren().isEmpty())
	//{
		//node.RemoveChild(0);
	//}

	//std::list<DcGp::DcGpEntity* > en;
	//m_pEntityQueueManager->GetEntityQueue(en);
	//for (std::list<DcGp::DcGpEntity* >::iterator it = en.begin(); it != en.end(); ++it)
	//{
	//	node.AddChild(*it);
	//}
	Traverse(node);

	//Apply(static_cast<DcGp::DcGpEntity& >(node));
}

float UpdateVisitor::GetDistanceToViewPoint(const DCVector3D& pos, bool withLODScale) const
{
	if (withLODScale) return (pos - m_viewPointLocal).Length() * m_lodScale;
	else return (pos - m_viewPointLocal).Length();
}

void UpdateVisitor::SetViewPointLocal(DCVector3D vPoint)
{
	m_viewPointLocal = vPoint;
}