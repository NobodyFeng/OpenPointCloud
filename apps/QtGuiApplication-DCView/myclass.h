#ifndef MYCLASS_H
#define MYCLASS_H

//DCGUI
#include "DCGui/AuxMainWindow.h"

//Qt
#include "QMdiSubWindow"

class MdiArea;

namespace MPViewer
{
	class Viewer;
}
class MyClass : public DCGui::AuxMainWindow
{
	Q_OBJECT

public:
	MyClass(QWidget *parent = 0, Qt::WindowFlags flags = 0);
	~MyClass();

	//! ����֧���Ϸ��ļ��ӿ����
	
	//! ���ص�ǰ����Ĵ���
	QWidget *activeMdiChild();

	int loadFile(const QString &fileName, MPViewer::Viewer* viewer);
	int addFile(MPViewer::Viewer*viewer, const QString &fileName);
private:
	//�ര������
	MdiArea* m_pMdiArea;
};

#endif // MYCLASS_H
