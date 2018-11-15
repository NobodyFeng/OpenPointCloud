//----------------------------------------------------------------------
//author:wly
//time:
//dsc:������װ��꽻���¼�����
//
//----------------------------------------------------------------------

#ifndef DCVIEW_GUIEVENTHANDLE_H
#define DCVIEW_GUIEVENTHANDLE_H

#include <QObject>
#include "QtWidgets/QAction"

#include "DCCoreDLL.h"

class QMouseEvent;
class QKeyEvent;
class QWheelEvent;

namespace DCView
{
	class View;
}

namespace DCCore
{
	
	class DCCORE_EXPORT GUIEventHandle : public QObject
	{
		Q_OBJECT

	public:
		GUIEventHandle(QString name);
		~GUIEventHandle();

		//��갴��
		virtual void mousePressEvent(QMouseEvent* event, DCView::View* view) = 0;
		//����ƶ�
		virtual void mouseMoveEvent(QMouseEvent* event, DCView::View* view) = 0;
		//���̰����¼�
		virtual void keyPressEvent(QKeyEvent* event, DCView::View* view) = 0;

		//!����ɿ�
		virtual void mouseReleaseEvent(QMouseEvent* event, DCView::View* view);

		//ת������
		virtual void wheelEvent(QWheelEvent* event, DCView::View* view);

		//! ���˫��
		virtual void mouseDoubleClickEvent(QMouseEvent* event, DCView::View* view);

		//! ��������
		virtual void updateEvent(DCView::View* view);
	
		virtual void SetValid(bool isValid, DCView::View* view = 0);

		void SetName(QString name);
		QString GetName();

		bool GetValid();

		//�趨�󶨵Ķ���
		void SetAction(QAction* boundAction);

		//��ȡ�󶨵Ķ���
		QAction* GetAction();
	private:
		//! ����״̬
		bool m_isValid;

		//! ���������֣�unique
		QString m_uniqueName;

		//! �󶨵Ķ�������
		QAction* m_BoundAction;
	};
}

#endif // DCVIEW_GUIEVENTHANDLE_H
