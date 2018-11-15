#ifndef DC_DEFS_H
#define DC_DEFS_H

//Boost
#include "boost/shared_ptr.hpp"

//��������ָ��
#ifndef DECLARE_SMARTPTR
#define DECLARE_SMARTPTR(ClassName)\
class ClassName;\
	typedef boost::shared_ptr<ClassName> ClassName##Ptr;
#endif

//����װ��
 #ifndef SMARTPTR_DYNAMIC_CAST
 #define SMARTPTR_DYNAMIC_CAST(ClassName)\
 	boost::dynamic_pointer_cast<ClassName>
 #endif

//����α���캯��
#ifndef DECLARE_PHONY_CONSTRUCTOR
#define DECLARE_PHONY_CONSTRUCTOR(ClassName)\
public:\
	static ClassName##Ptr CreateObject()\
	{\
	return ClassName##Ptr(new ClassName);\
	}
#endif

//����ʵ��
#ifndef DECLARE_IMPLEMENT
#define DECLARE_IMPLEMENT(ClassName)\
private:\
	ClassName##ImplPtr m_p##ClassName##Impl;
#endif

//��������������
#ifndef GET_CLASSNAME
#define GET_CLASSNAME(ClassName)\
public:\
	virtual QString GetClassname()\
	{\
	return #ClassName;\
	}
#endif

#endif