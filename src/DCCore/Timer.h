#ifndef DCCORE_TIMER_H
#define DCCORE_TIMER_H

#include "DCCoreDLL.h"

//Qt
#include <QElapsedTimer>

namespace DCCore
{

//����ļ�ʱ��
//�����ڳ�������ʱ���г�ʼ��
	class DCCORE_EXPORT Timer
{
public:

    //��ʼ����̬ʵ��
	//����������������֮ǰ
    static void Init();

    //�����Գ�ʼ����ʼ��ʱ��(��)
    static int Second();
    //�����Գ�ʼ����ʼ��ʱ��(����)
    static int MilliSecond();

protected:
	Timer();
};

}
#endif //_DCC_TIMER_H_
