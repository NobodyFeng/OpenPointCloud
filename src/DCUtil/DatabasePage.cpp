#include "DatabasePage.h"
#include "OpenThreads/Thread"
#include "OpenThreads/ScopedLock"

//DCCore
#include "DCCore/Logger.h"
#include "DCCore/GUIEventHandle.h"
#include "DCCore/IoPlugin.h"
#include "DCCore/IoInterface.h"

using namespace DCUtil;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  DatabaseThread
//
DatabasePage::DatabaseThread::DatabaseThread(DatabasePage* pager, Mode mode, const std::string& name) :
	_done(false),
	_active(false),
	_pager(pager),
	_mode(mode),
	_name(name)
{
}



DatabasePage::DatabaseThread::~DatabaseThread()
{
	cancel();
}

int DatabasePage::DatabaseThread::cancel()
{
	int result = 0;

	if (isRunning())
	{
		setDone(true);

		switch (_mode)
		{
		case(HANDLE_ALL_REQUESTS):
			//_pager->_fileRequestQueue->release();
			break;
		case(HANDLE_NON_HTTP):
			//_pager->_fileRequestQueue->release();
			break;
		case(HANDLE_ONLY_HTTP):
			//_pager->_httpRequestQueue->release();
			break;
		}

		// then wait for the the thread to stop running.
		while (isRunning())
		{

			OpenThreads::Thread::YieldCurrentThread();
		}

	}

	return result;

}

void DatabasePage::DatabaseThread::run()
{
	RequestQueue* readQueue;

	readQueue = _pager->m_fileRequestQueue;

	do 
	{
		readQueue->block();

		DatabaseRequest* request = new DatabaseRequest;
		Point_3D shift;
		{
			OpenThreads::ScopedLock<OpenThreads::Mutex> lock(readQueue->_requestMutex);
			if (!readQueue->_requestList.isEmpty())
			{
				request = readQueue->takeFirst();
				shift = request->m_parentNode->GetShift();
				
			}
		}
	
		
		QString fileName = request->m_fileName;
		if (!fileName.isEmpty())
		{
			QString extension = QFileInfo(fileName).suffix();
			//������չ��������IO�ļ����еĲ����ѡ����ʵ�io�����������ļ�
			QDir dir(DCCore::GetIOPluginPath());

			DCCore::IoPlugin* currentIo = 0;

			bool hasPlugin = DCCore::GetIOPlugin(dir, extension, currentIo);
			if (hasPlugin && currentIo)
			{
				double coordinatesShift[3] = { shift.x,shift.y,shift.z };
				bool shiftable = true;
				DcGp::DcGpEntity* entity = currentIo->LoadFile(fileName, 0, false, &shiftable, coordinatesShift, nullptr);

				OpenThreads::ScopedLock<OpenThreads::Mutex> drLock(_pager->m_dr_mutex);
				request->m_loadedNode = entity;

				OpenThreads::ScopedLock<OpenThreads::Mutex> listLock(
					_pager->m_dataToMergeQueue->_requestMutex);
				_pager->m_dataToMergeQueue->_requestList.push_back(request);
				request = 0;
			}

			
		}
		bool firstTime = true;
		if (firstTime)
		{
			// do a yield to get round a peculiar thread hang when testCancel() is called 
			// in certain circumstances - of which there is no particular pattern.
			YieldCurrentThread();
			firstTime = false;
		}

	} while (!_done);
}


DatabasePage::DatabasePage()
	: DCUtil::DatabaseRequestHandler()
{
	m_startThreadCalled = false;

	m_databaseThreads = new DatabaseThread(this, DatabaseThread::HANDLE_NON_HTTP, "HANDLE_NON_HTTP");

	m_dataToMergeQueue = new RequestQueue(this);
	m_fileRequestQueue = new RequestQueue(this);

	if (m_startThreadCalled)
	{

		m_databaseThreads->startThread();
	}
}

DatabasePage::~DatabasePage()
{
	if (m_databaseThreads)
	{
		delete m_databaseThreads;
		m_databaseThreads = nullptr;
	}
}

void DatabasePage::UpdateScene()
{
	QList<DatabaseRequest* > localFileLoadedList;

	// �̱߳�����
	m_dataToMergeQueue->swap(localFileLoadedList);

	for (QList<DatabaseRequest* >::iterator itr = localFileLoadedList.begin();
		itr != localFileLoadedList.end();
		++itr)
	{
		DatabaseRequest* databaseRequest = *itr;

		DcGp::DcGpEntity* parent = databaseRequest->m_parentNode;
		if (parent)
		{

			if (databaseRequest->m_loadedNode)
			{
				parent->AddChild(databaseRequest->m_loadedNode);
			}
			

		}
	}
}


//! ���߳� ��ҳ�ڵ����ʱ�����
void DatabasePage::RequestNodeFile(const QString& fileName, DcGp::DcGpEntity* parNode, boost::shared_ptr<DatabaseRequest>& dbRequest)
{
	//!���븸�ڵ�
	DcGp::DcGpEntity* parent = parNode;


	//! �Ƿ������Ѷ�������ҵ����м���������foundEntryΪtrue
	bool foundEntry = false;

	
	//!ͨ���жϴ�������dbRequest������dbRequest���ڣ���˵���Ѿ���ӵ���������
	//! ��Ȼ�Ѿ���ӵ��������ˣ���Ҫ�ж�һ���Ƿ��ڶ������Ѿ�û�����̼߳�������ˡ�
	if (dbRequest)
	{
		bool reQueue = false;   //!���·��������

		//! ����������
		OpenThreads::ScopedLock<OpenThreads::Mutex> drLock(m_dr_mutex);

		//!�ж����������valid״̬,trueû���ڶ�ȡ�߳��ж�ȡ��
		if (!(dbRequest)->Valid())
		{
			//dbRequest = 0;
		}
		else
		{
			foundEntry = true;

			//!�ж��Ƿ��ڶ������ѱ�����
			OpenThreads::ScopedLock<OpenThreads::Mutex> lock(m_fileRequestQueue->_requestMutex);


			if (reQueue)
			{
				m_fileRequestQueue->_requestList.push_back(dbRequest.get());
				m_fileRequestQueue->updateBlock();
			}
		}

	}

	//! ��һ�νڵ���ص�ʱ�򣬴�����newRequest�����������dbRequest��ֵ�ˣ�
	//!��ֵ�ˣ������д�߳��ˣ����ǲ������Ѿ���ȡ������ˡ�������Ҫ
	//! �ڵڶ��ζ�ͬһ�ڵ�����ʱ��ʹ���ϱߵ�reQueue�����ж��Ƿ���Ҫ���¼��ء�
	if (!foundEntry)
	{
		//! w�ļ����м�������������,���´���һ��DatabaseRequest
		OpenThreads::ScopedLock<OpenThreads::Mutex> drLock(m_dr_mutex);

		//! ����һ��request
		boost::shared_ptr<DCUtil::DatabaseRequest > newRequest(new DCUtil::DatabaseRequest);
		dbRequest = newRequest;
		newRequest->m_fileName = fileName;
		newRequest->m_parentNode = parent;
		newRequest->m_valid = true;

		//! w�ļ����м�������������,���´���һ��DatabaseRequest
		OpenThreads::ScopedLock<OpenThreads::Mutex> lock(m_fileRequestQueue->_requestMutex);
		m_fileRequestQueue->_requestList.push_back(newRequest.get());
		m_fileRequestQueue->updateBlock();
	}


	if (!m_startThreadCalled)
	{
		//OpenThreads::ScopedLock<OpenThreads::Mutex> lock(m_run_mutex);

		if (!m_startThreadCalled)
		{
			m_startThreadCalled = true;
			m_done = false;

			if (!m_databaseThreads)
			{
				m_databaseThreads = new DatabaseThread(this, DatabaseThread::HANDLE_NON_HTTP, "HANDLE_NON_HTTP");
			}

			
			m_databaseThreads->startThread();
			
		}
	}
}


/****************************************************************************************************/
