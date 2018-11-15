//----------------------------------------------------------------------
//author:wly
//time:
//dsc:��ʻģʽ������
//
//----------------------------------------------------------------------


#ifndef DCGA_FLATMANIPULATOR_H
#define DCGA_FLATMANIPULATOR_H

//Qt
#include "QPoint"

#include "dcga_global.h"

#include "DCCore/GUIEventHandle.h"

#include "DCCore/BasicTypes.h"

#include "DCView/Common.h"

namespace DCView
{
	class View;
}

namespace DcGp
{
	class DcGpEntity;
}

namespace DCGa
{
	class DCGA_EXPORT FlatManipulator : public DCCore::GUIEventHandle
	{
		Q_OBJECT
	public:

		FlatManipulator(QString name);
		~FlatManipulator();

		//��갴��
		virtual void mousePressEvent(QMouseEvent* event, DCView::View* view);
		//����ƶ�
		virtual void mouseMoveEvent(QMouseEvent* event, DCView::View* view);
		//���̰����¼�
		virtual void keyPressEvent(QKeyEvent* event, DCView::View* view);

		void mouseReleaseEvent(QMouseEvent* event, DCView::View* view);

		//ת������
		virtual void wheelEvent(QWheelEvent* event, DCView::View* view);

	private:

	private:

	};
}

#endif // DCGA_TRACKBALLMANIPULATOR_H
