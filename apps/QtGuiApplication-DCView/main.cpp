//C++
#include <cstdlib>
#include "GL/glut.h"

//Qt
#include <QtWidgets/QApplication>

#include "myclass.h"


int main(int argc, char *argv[])
{
	//��ʼ��freeglut
	glutInit(&argc, argv);

	QApplication a(argc, argv);
	MyClass w;
	w.showMaximized();
	return a.exec();
}
