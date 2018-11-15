#ifndef DCGP_ENTITY_H
#define DCGP_ENTITY_H

#include "DCGp/DcGpDLL.h"

//C++
#include <vector>

//Qt
#include "QVector"

#include "DCCore/DCDefs.h"
#include "GpDrawableObject.h"
//#include "DCUtil/AbstractEntityVisitor.h"

namespace DCUtil
{
	class AbstractEntityVisitor;
}

namespace DcGp
{
	
	class DcGpBasicDevice;
	class DcGpBoundBox;

	DECLARE_SMARTPTR(DcGpEntity)
	DECLARE_SMARTPTR(DcGpEntityImpl)

	typedef QVector<DcGpEntity*> Container;
	typedef std::vector<DcGpEntityPtr> ContainerPtr;

	class DCGP_API DcGpEntity : public DcGpDrawableObject
	{
		DECLARE_PHONY_CONSTRUCTOR(DcGpEntity)
		DECLARE_IMPLEMENT(DcGpEntity)
		GET_CLASSNAME(DcGpEntity)

		Q_OBJECT
		public:
			DcGpEntity(QString name);

			~DcGpEntity();

			void AddChildPtr(DcGpEntityPtr pChild, bool dependant = true);
			void AddItemPtr(DcGpDrawableObjectPtr pItem);
			DcGpEntityPtr GetChildPtr(unsigned index);
			DcGpEntityPtr GetParentPtr();
			void SetParentPtr(DcGpEntityPtr pParent);
			void RemoveChildPtr(const DcGpEntityPtr pChild);
			void RemoveChildPtr(int pos);
			void DeleteAllChildrenPtr();
			DcGpEntityPtr FindPtr(unsigned uniqueID);
			int GetChildIndexPtr(const DcGpEntityPtr child) const;
			ContainerPtr GetChildrenPtr() const;
			unsigned GetChildrenNumberPtr() const;
			//�����ͼ������
			void AddChild(DcGpEntity* child, bool dependant = true);
			//���ͼԪ����
			void AddItem(DcGpDrawableObject* item);
			//��ȡ������
			DcGpEntity* GetParent() const;
			//���ø�����
			void SetParent(DcGpEntity* parent);
			//�Ƴ�ָ�����Ӷ���
			void RemoveChild(const DcGpEntity* object, bool isDelete = true);
			//�Ƴ�ָ�����������Ӷ��󣬲���ִ��delete
			void RemoveChild(int pos, bool isDelete = true);
			//�Ƴ���ɾ�����е��Ӷ���
			void DeleteAllChildren();
			//
			DcGpEntity* Find(unsigned uniqueID);
			//��ȡ�Ӷ�������
			int GetChildIndex(const DcGpEntity* child) const;
			//��ȡ�Ӷ���
			DcGpEntity* GetChild(unsigned index);
			//
			Container GetChildren() const;
			//��ȡ�Ӷ��������
			unsigned GetChildrenNumber() const;

			//! �жϵ�ǰ�����ǲ��Ǵ������ĸ���
			bool IsAncestorOf(const DcGpEntity* anObject) const;


			//����
			virtual void Draw(DcGpDrawContext& context);
			virtual void FastDraw3D(DcGpDrawContext& context);
			virtual void DrawWithNames(DcGpDrawContext& context, bool drawSample = false);
			//������ʵ�壬�������Ӷ���
			virtual void DrawMyselfOnly(DcGpDrawContext& context);
			virtual void FastDrawMyselfOnly(DcGpDrawContext& context);
			virtual void DrawWithNamesMyselfOnly(DcGpDrawContext& context, bool drawSample = false);

			//��ȡ����
			int GetIndex() const;

			//�̳���DcGpDrawableObject
			virtual DcGpBoundBox GetBoundBox(bool relative=true, bool withGLfeatures=false, const DcGpBasicDevice* window=0);
			virtual DcGpBoundBox GetFitBoundBox(DcGpMatrix& trans);
			virtual DcGpBoundBox GetMyOwnBoundBox();
			virtual DcGpBoundBox GetDisplayBoundBox();

			//���Ӵ���
			virtual void LinkWindow(DcGpBasicDevice* window);

			//! Behavior when selected
			enum SelectionBehavior 
			{ 
				eSelectionAABoundBox		=	0	,
				eSelectionFitBoundBox				,
				eSelectionIgnored
			};
	
			//
			void SetSelectionBehavior(SelectionBehavior mode);

			//!����ѡ��״̬
			SelectionBehavior GetSelectionBehavior() const;

			//�Ƿ�ƫ��
			bool HasShifted() const;
			//��������ƫ��
			void SetShift(Point_3D shift);
			//��ȡƫ��
			Point_3D GetShift() const;

			//DoubleClicked
			virtual void DoubleClicked(unsigned selectedID, unsigned subSelectedID);

			//���浽�ļ�
			virtual bool SaveToFile(QFile& file) const;

			//!brief ������ʾ��Ϣ
			void SetToolTip(const QString& strToolTip);
			//!brief ��ȡ��ʾ��Ϣ
			QString GetToolTip() const;

			//���㷨����
			virtual void ComputerNormal() {};

			virtual void Accept(DCUtil::AbstractEntityVisitor& nv);

			virtual void Ascend(DCUtil::AbstractEntityVisitor& nv);

			virtual void Traverse(DCUtil::AbstractEntityVisitor& nv);

		private:
			void Draw3DName(DcGpDrawContext& context);

		protected:
			DcGpEntity();
		};

}
#endif // DCGRAPHICCONTAINER_H
