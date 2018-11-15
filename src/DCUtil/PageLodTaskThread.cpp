#include "PageLodTaskThread.h"

#include "QThread"

//DCCore
#include "DCCore/Logger.h"
#include "DCCore/GUIEventHandle.h"
#include "DCCore/IoPlugin.h"
#include "DCCore/IoInterface.h"

PageLodTaskThread::PageLodTaskThread(EntityQueueManager* queueManager, DcGp::PageLodEntity* pageLodEntity, float viewDistance)
	: QRunnable()
{
	m_queueManager = queueManager;
	m_pageLodEntity = pageLodEntity;
	m_viewDistance = viewDistance;

	m_shift = pageLodEntity->GetShift();
}

PageLodTaskThread::~PageLodTaskThread()
{
}

//��д�߳����У� ��ȡ�ļ��ɹ�������ӵ�������
//��������� ����̶߳����ܷ��ʣ��Ǹ��������ݣ�������Ҫ�Զ��еĲ���������
//
void PageLodTaskThread::run()
{
	
	QString fileName = (m_pageLodEntity->GetDatabasePath() + m_pageLodEntity->GetPerRangeDataList()[0]._filename);
	QString extension = QFileInfo(fileName).suffix();
	//������չ��������IO�ļ����еĲ����ѡ����ʵ�io�����������ļ�
	QDir dir(DCCore::GetIOPluginPath());

	DCCore::IoPlugin* currentIo = 0;

	bool hasPlugin = DCCore::GetIOPlugin(dir, extension, currentIo);
	if (!hasPlugin || !currentIo)
	{

		return;
	}

	double coordinatesShift[3] = { m_shift.x, m_shift.y, m_shift.z };
	bool shiftable = true;
	DcGp::DcGpEntity* entity = currentIo->LoadFile(fileName, 0, false, &shiftable, coordinatesShift, nullptr);

	if (entity)
	{
		m_queueManager->Add(entity);
	}
	QThread::msleep(1000);
	
	
}
