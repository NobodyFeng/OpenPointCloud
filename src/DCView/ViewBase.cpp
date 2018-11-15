#include "DCView/ViewBase.h"

#include "DCUtil/UpdateVisitor.h"

ViewBase::ViewBase()
	: m_firstFrame(true)
	, m_updateVisitor(nullptr)
{
	m_updateVisitor = new DCUtil::UpdateVisitor;
}

ViewBase::~ViewBase()
{
	if (m_updateVisitor)
	{
		delete m_updateVisitor;
		m_updateVisitor = nullptr;
	}
}

void ViewBase::Frame()
{
	if (m_firstFrame)
	{
		m_firstFrame = false;
	}

	//������֡��Ϣ
	//Advance();

	//1�¼���ȡ������
	EventTraversal();

	//2���±���
	UpdateTraversal();

	//3��Ⱦ����
	RenderingTraversals();
	
}
