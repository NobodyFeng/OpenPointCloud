#pragma once

#include <QObject>
#include "QMutex"

#include "DCGp/GpEntity.h"

#include <list>

class EntityQueueManager : public QObject
{
	Q_OBJECT

public:
	EntityQueueManager(QObject *parent = nullptr);
	~EntityQueueManager();


	void Add(DcGp::DcGpEntity* entity);

	void GetEntityQueue(std::list<DcGp::DcGpEntity* >& enList);

private:
	QMutex m_mutex;

	//! ʵ����нڵ�ָ��
	std::list<DcGp::DcGpEntity* > m_queueList;
};
