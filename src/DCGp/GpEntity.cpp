//DCCore
#include "DCCore/DCGL.h"
#include "DCCore/Logger.h"

#include "DCGp/GpEntity.h"
#include "DCGp/GpBasicDevice.h"
#include "DCGp/Custom/GpBoundBox.h"
#include "DCGp/GpDrawContext.h"

#include "DCUtil/AbstractEntityVisitor.h"


namespace DcGp
{
	DECLARE_SMARTPTR(DcGpEntityImpl)

	class DcGpEntityImpl
	{
		DECLARE_PHONY_CONSTRUCTOR(DcGpEntityImpl)

	public:
		void SetInterface(DcGpEntityPtr pInterface)
		{
			m_pInterface = pInterface;
		}

		~DcGpEntityImpl()
		{}

	protected:
		DcGpEntityImpl()
		{}

	private:
		friend class DcGpEntity;

		DcGpEntity* m_parent;
		DcGpEntityPtr m_pParent;
		DcGpEntityPtr m_pInterface;

		//�����Ƿ�ƫ��
		bool m_isShifted;
		//����ƫ����
		Point_3D m_shift;

		DcGpEntity::SelectionBehavior m_selectionBehavior;

		Container m_children;
		ContainerPtr m_pChildren;

		//��ʾ��Ϣ
		QString m_toolTip;
	};
}

DcGp::DcGpEntity::DcGpEntity()
	: DcGpDrawableObject()
{
	m_pDcGpEntityImpl = DcGpEntityImpl::CreateObject();
	//m_pDcGpEntityImpl->SetInterface(DcGpEntityPtr(this));

	m_pDcGpEntityImpl->m_selectionBehavior = eSelectionAABoundBox;
	m_pDcGpEntityImpl->m_parent = nullptr;
	m_pDcGpEntityImpl->m_pParent = DcGpEntityPtr();
	m_pDcGpEntityImpl->m_isShifted = false;

	//��ʼ���������ֵ
	m_pDcGpEntityImpl->m_shift[0] =0;
	m_pDcGpEntityImpl->m_shift[1] =0;
	m_pDcGpEntityImpl->m_shift[2] =0;
}

DcGp::DcGpEntity::DcGpEntity(QString name)
	: DcGpDrawableObject(name)
{
	m_pDcGpEntityImpl = DcGpEntityImpl::CreateObject();
	//m_pDcGpEntityImpl->SetInterface(DcGpEntityPtr(this));

	m_pDcGpEntityImpl->m_selectionBehavior = eSelectionAABoundBox;
	m_pDcGpEntityImpl->m_parent = nullptr;
	m_pDcGpEntityImpl->m_pParent = DcGpEntityPtr();
	m_pDcGpEntityImpl->m_isShifted = false;

	//��ʼ���������ֵ
	m_pDcGpEntityImpl->m_shift[0] =0;
	m_pDcGpEntityImpl->m_shift[1] =0;
	m_pDcGpEntityImpl->m_shift[2] =0;
}

DcGp::DcGpEntity::~DcGpEntity()
{
	//����ɾ�������Ӷ���
	DeleteAllChildren();

	//delete m_pDcGpEntityImpl.get();
}

void DcGp::DcGpEntity::SetSelectionBehavior(SelectionBehavior mode)
{
	m_pDcGpEntityImpl->m_selectionBehavior = mode;
}

DcGp::DcGpEntity::SelectionBehavior DcGp::DcGpEntity::GetSelectionBehavior() const
{
	return m_pDcGpEntityImpl->m_selectionBehavior;
}

void DcGp::DcGpEntity::DrawWithNames(DcGpDrawContext& context, bool drawSample /* = false */)
{
	if (!IsEnabled())
		return;

	//are we currently drawing objects in 2D or 3D?
	bool draw3D = MACRO_Draw3D(context);

	//the entity must be either visible and selected, and of course it should be displayed in this context
	bool drawInThisContext = ((IsVisible() || IsSelected()) && GetAssociatedWindow() == context._win);

	//Ӧ��3d����ʱ�ԡ�ת��(ֻ��ʾ)
	if (draw3D && IsTransformed())
	{
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glMultMatrixf(GetTransMat().Data());
	}

	//draw entity
	if (IsVisible() && drawInThisContext)
	{
		if (( !IsSelected() || !MACRO_SkipSelected(context) ) &&
			( IsSelected() || !MACRO_SkipUnselected(context) ))
		{
			//apply default color (in case of)
			glColor3ubv(context.pointsDefaultCol);

			DrawWithNamesMyselfOnly(context, drawSample);
		}
	}

	//����ʵ����Ӷ���

#ifdef USING_SMARTPTR
	for (auto it = m_pDcGpEntityImpl->m_pChildren.begin(); it != m_pDcGpEntityImpl->m_pChildren.end(); ++it)
	{
		(*it)->DrawWithNames(context);
	}
#else
	for (auto it = m_pDcGpEntityImpl->m_children.begin(); it != m_pDcGpEntityImpl->m_children.end(); ++it)
	{
		(*it)->DrawWithNames(context, drawSample);
	}
#endif

	if (draw3D && IsTransformed())
		glPopMatrix();
}

void DcGp::DcGpEntity::FastDraw3D(DcGpDrawContext& context)
{
	if (!IsEnabled())
		return;

	//are we currently drawing objects in 2D or 3D?
	bool draw3D = MACRO_Draw3D(context);

	//the entity must be either visible and selected, and of course it should be displayed in this context
	bool drawInThisContext = ((IsVisible() || IsSelected()) && GetAssociatedWindow() == context._win);

	//Ӧ��3d����ʱ�ԡ�ת��(ֻ��ʾ)
	if (draw3D && IsTransformed())
	{
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glMultMatrixf(GetTransMat().Data());
	}

	//draw entity
	if (IsVisible() && drawInThisContext)
	{
		if (( !IsSelected() || !MACRO_SkipSelected(context) ) &&
			( IsSelected() || !MACRO_SkipUnselected(context) ))
		{
			//apply default color (in case of)
			glColor3ubv(context.pointsDefaultCol);

			FastDrawMyselfOnly(context);
		}
	}

	//����ʵ����Ӷ���

#ifdef USING_SMARTPTR
	for (auto it = m_pDcGpEntityImpl->m_pChildren.begin(); it != m_pDcGpEntityImpl->m_pChildren.end(); ++it)
	{
		(*it)->FastDraw3D(context);
	}
#else
	for (auto it = m_pDcGpEntityImpl->m_children.begin(); it != m_pDcGpEntityImpl->m_children.end(); ++it)
	{
		(*it)->FastDraw3D(context);
	}
#endif

	//if the entity is currently selected, we draw its bounding-box
	//�����ǰʵ�屻ѡ�У�����Ʊ߽��
	if (IsSelected() && draw3D && drawInThisContext && !MACRO_DrawNames(context))
	{
		switch (m_pDcGpEntityImpl->m_selectionBehavior)
		{
		case eSelectionAABoundBox:
			DrawBoundBox(context.bbDefaultCol);
			break;
		case eSelectionFitBoundBox:
			{
				DcGpMatrix trans;
				DcGpBoundBox box = GetFitBoundBox(trans);
				if (box.IsValid())
				{
					glMatrixMode(GL_MODELVIEW);
					glPushMatrix();
					glMultMatrixf(trans.Data());
					box.Draw(context.bbDefaultCol);
					glPopMatrix();
				}
			}
			break;
		case eSelectionIgnored:
			break;
		default:
			assert(false);
		}
	}

	if (draw3D && IsTransformed())
		glPopMatrix();
}

void DcGp::DcGpEntity::Draw3DName(DcGpDrawContext& context)
{
	if(!context._win) 
	{
		return;
	}

	DcGp::DcGpBoundBox box = GetBoundBox();

	//! ת���߽�����ĵ��������꣬Ȼ�����name
	const double* MM = context._win->GetModelViewMatrixd();
	const double* MP = context._win->GetProjectionMatrixd();
	int VP[4];
	context._win->GetViewportArray(VP);

	GLdouble xp,yp,zp;
	DCVector3D C = box.GetCenter();
	gluProject(C.x,C.y,C.z,MM,MP,VP,&xp,&yp,&zp);

	QFont font;// = context._win->GetDisplayedTextFont();
	context._win->Display2DLabel(	GetName(),
		static_cast<int>(xp),
		static_cast<int>(yp),
		DcGpBasicDevice::eHMidAlign | DcGpBasicDevice::eVMidAlign ,
		0.75f,
		0,
		&font);
}

//���������������
void DcGp::DcGpEntity::Draw(DcGpDrawContext& context)
{
	if (!IsEnabled())
		return;

	//��ά����
	bool draw3D = MACRO_Draw3D(context);

	//�����ǿɼ��� ���߱�ѡ�� �������ڵ�ǰ������
	bool drawInThisContext = ((IsVisible() || IsSelected()));

	//Ӧ��3d����ʱ�ԡ�ת��(ֻ��ʾ)
	if (draw3D && IsTransformed())
	{
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glMultMatrixf(GetTransMat().Data());
	}

	//����
	if (IsVisible() && drawInThisContext)
	{
		if (( !IsSelected() || !MACRO_SkipSelected(context) ) &&
			( IsSelected() || !MACRO_SkipUnselected(context) ))
		{
			//Ĭ��ɫ
			glColor3ubv(context.pointsDefaultCol);

			DrawMyselfOnly(context);

			//! �Ƿ����Ӵ��л���ʵ������
			if (IsShownName() && MACRO_Draw2D(context))
				Draw3DName(context);
		}
	}

	//����ʵ����Ӷ���
	
#ifdef USING_SMARTPTR
	for (auto it = m_pDcGpEntityImpl->m_pChildren.begin(); it != m_pDcGpEntityImpl->m_pChildren.end(); ++it)
	{
		(*it)->Draw(context);
	}
#else
	for (auto it = m_pDcGpEntityImpl->m_children.begin(); it != m_pDcGpEntityImpl->m_children.end(); ++it)
	{
		(*it)->Draw(context);
	}
#endif

	//�����ǰʵ�屻ѡ�У�����Ʊ߽��
	if (IsSelected() && draw3D && drawInThisContext && !MACRO_DrawNames(context))
	{
		switch (m_pDcGpEntityImpl->m_selectionBehavior)
		{
		case eSelectionAABoundBox:
			DrawBoundBox(context.bbDefaultCol);
			break;
		case eSelectionFitBoundBox:
			{
				DcGpMatrix trans;
				DcGpBoundBox box = GetFitBoundBox(trans);
				if (box.IsValid())
				{
					glMatrixMode(GL_MODELVIEW);
					glPushMatrix();
					glMultMatrixf(trans.Data());
					box.Draw(context.bbDefaultCol);
					glPopMatrix();
				}
			}
			break;
		case eSelectionIgnored:
			break;
		default:
			assert(false);
		}
	}

 	if (draw3D && IsTransformed())
 		glPopMatrix();

}

void DcGp::DcGpEntity::FastDrawMyselfOnly(DcGpDrawContext& context)
{

}

void DcGp::DcGpEntity::DrawMyselfOnly(DcGpDrawContext& context)
{

}

void DcGp::DcGpEntity::DrawWithNamesMyselfOnly(DcGpDrawContext& context, bool drawSample /* = false */)
{

}

//
DcGp::DcGpBoundBox DcGp::DcGpEntity::GetBoundBox(bool relative/* =true */, 
												bool withGLfeatures/* =false */, 
												const DcGpBasicDevice* window/* =0 */)
{
	DcGpBoundBox box;

	if (!window || GetAssociatedWindow() == window)
	{
		box = (withGLfeatures ? GetDisplayBoundBox() : GetMyOwnBoundBox());
	}

	
#ifdef USING_SMARTPTR
	for (auto it = m_pDcGpEntityImpl->m_pChildren.begin(); it != m_pDcGpEntityImpl->m_pChildren.end();++it)
	{
		if ((*it)->IsEnabled())
		{
			box += ((*it)->GetBoundBox(false, withGLfeatures, window));
		}
	}
#else
	for (auto it = m_pDcGpEntityImpl->m_children.begin(); it != m_pDcGpEntityImpl->m_children.end();++it)
	{
		if ((*it)->IsEnabled())
		{
			box += ((*it)->GetBoundBox(false, withGLfeatures, window));
		}
	}
#endif

	//apply GL transformation afterwards!
	if (!window || GetAssociatedWindow() == window)
	{
		if (box.IsValid() && !relative && IsTransformed())
		{
			box *= GetTransMat();
		}
	}

	return box;
}

DcGp::DcGpBoundBox DcGp::DcGpEntity::GetFitBoundBox(DcGpMatrix& trans)
{
	DcGpBoundBox box;
	return box;
}

//
void DcGp::DcGpEntity::LinkWindow(DcGpBasicDevice* window)
{
	SetAssociatedWindow(window);
	
#ifdef USING_SMARTPTR
	for(auto i = m_pDcGpEntityImpl->m_pChildren.begin(); i != m_pDcGpEntityImpl->m_pChildren.end(); ++i)
	{
		(*i)->LinkWindow(window);
	}
#else
	for(auto i = m_pDcGpEntityImpl->m_children.begin(); i != m_pDcGpEntityImpl->m_children.end(); ++i)
	{
		(*i)->LinkWindow(window);
	}
#endif

}

DcGp::DcGpBoundBox DcGp::DcGpEntity::GetMyOwnBoundBox()
{
	return DcGpBoundBox();
}

DcGp::DcGpBoundBox DcGp::DcGpEntity::GetDisplayBoundBox()
{
	//by default, this is the same bbox as the "geometrical" one
	return GetMyOwnBoundBox();
}

//
int DcGp::DcGpEntity::GetIndex() const
{
#ifdef USING_SMARTPTR
	return (m_pDcGpEntityImpl->m_pParent ? m_pDcGpEntityImpl->m_pParent->GetChildIndex(this) : -1);
#else
	return (m_pDcGpEntityImpl->m_parent ? m_pDcGpEntityImpl->m_parent->GetChildIndex(this) : -1);
#endif
}

//�Ƿ�ƫ��
bool DcGp::DcGpEntity::HasShifted() const
{
	return m_pDcGpEntityImpl->m_isShifted;
}

//����ƫ��
void DcGp::DcGpEntity::SetShift(Point_3D shift)
{
	m_pDcGpEntityImpl->m_shift = shift;
	m_pDcGpEntityImpl->m_isShifted = true;
}

//��ȡƫ����
Point_3D DcGp::DcGpEntity::GetShift() const
{
	return m_pDcGpEntityImpl->m_shift;
}

void DcGp::DcGpEntity::DoubleClicked(unsigned selectedID, unsigned subSelectedID)
{
	DCCore::Logger::Message(tr("Double Clicked And Picked!"));
	return;
}

//

void DcGp::DcGpEntity::AddChildPtr(DcGpEntityPtr pChild, bool dependant)
{
	m_pDcGpEntityImpl->m_pChildren.push_back(pChild);
	if (dependant)
	{
		pChild->SetParentPtr(DcGpEntityPtr(this));
		pChild->LinkWindow(GetAssociatedWindow());
	}
}

void DcGp::DcGpEntity::AddItemPtr(DcGpDrawableObjectPtr pItem)
{

}

DcGp::DcGpEntityPtr DcGp::DcGpEntity::GetChildPtr(unsigned index)
{
	if (index >= 0)
	{
		return m_pDcGpEntityImpl->m_pChildren[index];
	}

	return DcGpEntityPtr();
}

DcGp::DcGpEntityPtr DcGp::DcGpEntity::GetParentPtr()
{
	return m_pDcGpEntityImpl->m_pParent;
}

void DcGp::DcGpEntity::SetParentPtr(DcGpEntityPtr pParent)
{
	m_pDcGpEntityImpl->m_pParent = pParent;
}

void DcGp::DcGpEntity::RemoveChildPtr(const DcGpEntityPtr pChild)
{
	int pos = GetChildIndexPtr(pChild);

	if (pos>=0)
		RemoveChildPtr(pos);
}

void DcGp::DcGpEntity::RemoveChildPtr(int pos)
{
	assert(pos>=0 && unsigned(pos)<m_pDcGpEntityImpl->m_pChildren.size());

	m_pDcGpEntityImpl->m_pChildren.erase(m_pDcGpEntityImpl->m_pChildren.begin()+pos);
}

void DcGp::DcGpEntity::DeleteAllChildrenPtr()
{
	//�Ƴ����е��Ӷ��󣬲�delete
	while(!m_pDcGpEntityImpl->m_pChildren.empty())
	{
		DcGpEntityPtr child = m_pDcGpEntityImpl->m_pChildren.back();
		m_pDcGpEntityImpl->m_pChildren.pop_back();
	}
}

DcGp::DcGpEntityPtr DcGp::DcGpEntity::FindPtr(unsigned uniqueID)
{
	ContainerPtr toTest;
	toTest.push_back(DcGpEntityPtr(this));

	while (!toTest.empty())
	{
		DcGpEntityPtr obj = toTest.back();
		toTest.pop_back();

		if (obj->GetUniqueID() == uniqueID)
			return obj;

		for (unsigned i=0;i<obj->GetChildrenNumberPtr();++i)
			toTest.push_back(obj->GetChildPtr(i));
	}

	return DcGpEntityPtr();
}

int DcGp::DcGpEntity::GetChildIndexPtr(const DcGpEntityPtr child) const
{
	for (unsigned i = 0; i< m_pDcGpEntityImpl->m_pChildren.size(); ++i)
	{
		if (m_pDcGpEntityImpl->m_pChildren[i] == child)
			return (int)i;
	}

	return -1;
}

DcGp::ContainerPtr DcGp::DcGpEntity::GetChildrenPtr() const
{
	return m_pDcGpEntityImpl->m_pChildren;
}

unsigned DcGp::DcGpEntity::GetChildrenNumberPtr() const
{
	return m_pDcGpEntityImpl->m_pChildren.size();
}

void DcGp::DcGpEntity::SetParent(DcGpEntity* parent)
{
	m_pDcGpEntityImpl->m_parent = parent;
}

//
DcGp::DcGpEntity* DcGp::DcGpEntity::Find(unsigned uniqueID)
{
	//now, we are going to test each object in the database!
	//(any better idea ?)
	/*Container toTest;
	toTest.push_back(this);

	while (!toTest.empty())
	{
		DcGpEntity* obj = toTest.back();
		toTest.pop_back();

		if (obj->GetUniqueID() == uniqueID)
			return obj;

		for (unsigned i=0;i<obj->GetChildrenNumber();++i)
			toTest.push_back(obj->GetChild(i));
	}

	return NULL;*/
	//found the right item?
	if (GetUniqueID() == uniqueID)
		return this;

	//otherwise we are going to test all children recursively
	for (unsigned i=0; i<GetChildrenNumber(); ++i)
	{
		DcGpEntity* match = GetChild(i)->Find(uniqueID);
		if (match)
			return match;
	}

	return 0;
}

DcGp::DcGpEntity* DcGp::DcGpEntity::GetParent() const
{
	return m_pDcGpEntityImpl->m_parent;
}

//ָ�����������
int DcGp::DcGpEntity::GetChildIndex(const DcGpEntity* child) const
{
	for (unsigned i = 0; i< m_pDcGpEntityImpl->m_children.size(); ++i)
	{
		if (m_pDcGpEntityImpl->m_children[i] == child)
			return (int)i;
	}

	return -1;
}

void DcGp::DcGpEntity::RemoveChild(const DcGpEntity* anObject, bool isDelete/* = true*/)
{
	assert(anObject);

	int pos = GetChildIndex(anObject);

	if (pos>=0)
		RemoveChild(pos, isDelete);
}

void DcGp::DcGpEntity::RemoveChild(int pos, bool isDelete/* = true*/)
{
	assert(pos>=0 && unsigned(pos)<m_pDcGpEntityImpl->m_children.size());

	DcGp::DcGpEntity* child = m_pDcGpEntityImpl->m_children[pos];

	m_pDcGpEntityImpl->m_children.erase(m_pDcGpEntityImpl->m_children.begin()+pos);

	if (isDelete)
	{
		delete child;
		child = nullptr;
	}
}

void DcGp::DcGpEntity::DeleteAllChildren()
{
	//�Ƴ����е��Ӷ��󣬲�delete
	while(!m_pDcGpEntityImpl->m_children.empty())
	{
		DcGpEntity* child = m_pDcGpEntityImpl->m_children.back();
		m_pDcGpEntityImpl->m_children.pop_back();

		delete child;
		child = nullptr;
	}
}

//��ȡ�Ӷ���
DcGp::DcGpEntity* DcGp::DcGpEntity::GetChild(unsigned index)
{

	if (index >= 0)
	{
		return m_pDcGpEntityImpl->m_children[index];
	}
	
	return nullptr;
}

bool LookupName(DcGp::DcGpEntity* pEntity, QString entityName)
{
	//���Ƚڵ�
	DcGp::Container childs = pEntity->GetChildren();

	//�����ڵ����������֣��鿴�Ƿ������ֵ
	int count = childs.size();

	for (auto i = 0; i < count; ++i)
	{
		//��ȡ��ǰ�ڵ�
		DcGp::DcGpEntity* nowitem = childs[i];
		//��ȡ��ǰ�ڵ�����
		QString nowName = nowitem->GetName();

		if (nowName == entityName)
		{
			return true;
		}
	}
	return false;
}

//���ͼԪ����
void DcGp::DcGpEntity::AddChild(DcGpEntity* child, bool dependant/* = true */)
{
	assert(child);

	//! �ж��ӽڵ����Ƿ���ͬ�������������
	QString nowName = child->GetName();
	QString newName = nowName;
	unsigned count = 0;

	while(LookupName(this, newName))
	{
		newName = nowName + QString("-%1").arg(++count);
	}

	child->SetName(newName);

	m_pDcGpEntityImpl->m_children.push_back(child);
	child->SetParent(this);
	if (dependant)
	{
		child->LinkWindow(GetAssociatedWindow());
	}
}

void DcGp::DcGpEntity::AddItem(DcGpDrawableObject* item)
{

}

unsigned DcGp::DcGpEntity::GetChildrenNumber() const
{
	return m_pDcGpEntityImpl->m_children.size();
}

//
DcGp::Container DcGp::DcGpEntity::GetChildren() const
{
	return m_pDcGpEntityImpl->m_children;
}

bool DcGp::DcGpEntity::SaveToFile(QFile& file) const
{
	return false;
}

//��ȡ��ʾ��Ϣ
QString DcGp::DcGpEntity
	::GetToolTip() const
{
	return m_pDcGpEntityImpl->m_toolTip;
}

//������ʾ��Ϣ
void DcGp::DcGpEntity
	::SetToolTip(const QString& strToolTip)
{
	m_pDcGpEntityImpl->m_toolTip = GetName() + QString(" %1").arg(strToolTip);
}

void DcGp::DcGpEntity::Accept(DCUtil::AbstractEntityVisitor& nv)
{
		nv.Apply(*this);
}

void DcGp::DcGpEntity::Ascend(DCUtil::AbstractEntityVisitor& nv)
{
	//std::for_each(m_parents.begin(),_parents.end(),NodeAcceptOp(nv));
}

void DcGp::DcGpEntity::Traverse(DCUtil::AbstractEntityVisitor& nv)
{
	//����ÿһ�����ӣ����շ�����
	for (auto it = m_pDcGpEntityImpl->m_children.begin(); it != m_pDcGpEntityImpl->m_children.end(); ++it)
	{
		(*it)->Accept(nv);
	}
}

bool DcGp::DcGpEntity::IsAncestorOf(const DcGpEntity* anObject) const
{
	assert(anObject);
	DcGpEntity* parent = anObject->GetParent();
	if (!parent)
		return false;

	if (parent == this)
		return true;

	return IsAncestorOf(parent);

}