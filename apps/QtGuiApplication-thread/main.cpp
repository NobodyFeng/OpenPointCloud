/*******************************************************************
*  Copyright(c) 2012-2017 DCLW
*  All rights reserved.
*
*  �ļ�����:
*  ��Ҫ����: ����c++11�Դ����̹߳�����
*
*  ��������: 20170707
*  ����:     ������
*  ˵��:
*
*  �޸�����:
*  ����:
*  ˵��:
******************************************************************/

#include "QtGuiApplicationthread.h"
#include <QtWidgets/QTreeWidgetItem>


#include <thread>
#include <mutex>

int j = 0;

std::mutex mt;

void Foo()
{
	mt.lock();
	j++;
	mt.unlock();
}

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);


	//! �����߳�
	std::thread td1(Foo);
	td1.join();

	std::thread td2(Foo);
	td2.join();

	QtGuiApplicationthread w;
	w.show();
	return a.exec();
}
