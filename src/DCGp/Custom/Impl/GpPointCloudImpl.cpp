#include "GpPointCloudImpl.hxx"

#include "QCoreApplication"



#include "../GpPointCloud.h"

DcGp::DcGpPointCloudImpl::~DcGpPointCloudImpl()
{
	glDeleteProgram(m_generalDrawShader.getProgramIndex());
}

GLuint DcGp::DcGpPointCloudImpl::SetupFastDrawShaders() 
{

	// Shader for models
	m_fastDrawShader.init();
	m_fastDrawShader.loadShader(VSShaderLib::VERTEX_SHADER, 
		(QCoreApplication::applicationDirPath().append( "/Support/Shaders").append("/fastDraw.vert").toStdString() ));
	m_fastDrawShader.loadShader(VSShaderLib::FRAGMENT_SHADER, 
		(QCoreApplication::applicationDirPath().append("/Support/Shaders").append("/fastDraw.frag").toStdString() ));

	// set semantics for the shader variables
	m_fastDrawShader.setProgramOutput(0,"outputF");
	m_fastDrawShader.setVertexAttribName(VSShaderLib::VERTEX_COORD_ATTRIB, "position");
	m_fastDrawShader.setVertexAttribName(VSShaderLib::VERTEX_ATTRIB1, "isVisible");


	m_fastDrawShader.prepareProgram();

	auto aa = m_fastDrawShader.isProgramValid();

	printf("InfoLog for fastDraw Shader\n%s\n\n", m_fastDrawShader.getAllInfoLogs().c_str());

	return(m_fastDrawShader.isProgramValid());
} 

GLuint DcGp::DcGpPointCloudImpl::SetupGeneralDrawShaders() 
{

	// Shader for models
	m_generalDrawShader.init();
	m_generalDrawShader.loadShader(VSShaderLib::VERTEX_SHADER,
		(QCoreApplication::applicationDirPath().append("/Support/Shaders").append("/generalDraw.vert").toStdString() ));
	m_generalDrawShader.loadShader(VSShaderLib::FRAGMENT_SHADER, 
		(QCoreApplication::applicationDirPath().append("/Support/Shaders").append("/generalDraw.frag").toStdString() ));

	// set semantics for the shader variables
	m_generalDrawShader.setProgramOutput(0,"outputF");

	m_generalDrawShader.prepareProgram();

	auto aa = m_generalDrawShader.isProgramValid();

	printf("InfoLog for GeneralDraw Shader\n%s\n\n", m_generalDrawShader.getAllInfoLogs().c_str());

	return(m_generalDrawShader.isProgramValid());
}

void DcGp::DcGpPointCloudImpl::SetBorderInValid()
{
	m_borderValid = false;

	m_minPoint = DCVector3D(DCCore::MAX_POINTCOORD, DCCore::MAX_POINTCOORD, DCCore::MAX_POINTCOORD);
	m_maxPoint = DCVector3D(DCCore::MIN_POINTCOORD, DCCore::MIN_POINTCOORD, DCCore::MIN_POINTCOORD);
}

//���㵱ǰʵ����������Сֵ
void DcGp::DcGpPointCloudImpl::CalculateMyOwnBoundBox()
{
	if (!m_borderValid)
	{
		//����m_points��������С���ֵm_minPoint��m_maxPoint
		for (auto i = 0; i < m_points.size(); ++i)
		{
			m_minPoint[0] = std::min(m_minPoint[0], m_points[i][0]);
			m_minPoint[1] = std::min(m_minPoint[1], m_points[i][1]);
			m_minPoint[2] = std::min(m_minPoint[2], m_points[i][2]);

			m_maxPoint[0] = std::max(m_maxPoint[0], m_points[i][0]);
			m_maxPoint[1] = std::max(m_maxPoint[1], m_points[i][1]);
			m_maxPoint[2] = std::max(m_maxPoint[2], m_points[i][2]);
		}
		//����״ֵ̬
		m_borderValid = true;
	}
}

DcGp::DcGpPointCloudImpl::DcGpPointCloudImpl()
	: m_qIntface(nullptr)
	, m_pointSize(1)
	, m_minPoint(DCCore::MAX_POINTCOORD, DCCore::MAX_POINTCOORD, DCCore::MAX_POINTCOORD)
	, m_maxPoint(DCCore::MIN_POINTCOORD, DCCore::MIN_POINTCOORD, DCCore::MIN_POINTCOORD)
	, m_visibilityTable(false)
	, m_visibilityType(true)
	, m_borderValid(false)
	//, m_blockedGridToolkit(nullptr)
{
	m_currentScalarField = DcGpScalarFieldPtr();
	//��ʼ���̱߳�����
	DcGpScalarFieldPtr heightScalarField = DcGpScalarFieldPtr(new DcGpScalarField(QObject::tr("Height")));
	m_scalarFields.push_back(heightScalarField);
	//��ʼ����ԭ����������
	DcGpScalarFieldPtr distanceScalarField = DcGpScalarFieldPtr(new DcGpScalarField(QObject::tr("Distacne")));
	m_scalarFields.push_back(distanceScalarField);

}


void DcGp::DcGpPointCloudImpl::UpdateScalarFieldData()
{
	int num = m_scalarFields.size();
	unsigned long pNum = m_points.size();

	for (int i = 0; i != num; ++i)
	{
		//�ú������µ������ݣ���������ͱ�����data������ȵ�ʱ��Ͳ���Ҫ����
		if (pNum == m_scalarFields[i]->GetDatas().size())
		{
			continue;
		}

		QString scalarName = m_scalarFields[i]->GetName();
		if (scalarName == QObject::tr("Height"))
		{
			m_scalarFields[i]->GetDatas().clear();
			for (unsigned j = 0; j != pNum; ++j)
			{
				m_scalarFields[i]->AddData(m_points[j].z - m_qIntface->GetShift().z);
			}
		}
		else if (scalarName == QObject::tr("Distacne"))
		{
			m_scalarFields[i]->GetDatas().clear();

			for (unsigned j = 0; j != pNum; ++j)
			{
				m_scalarFields[i]->AddData(sqrt(m_points[j].x * m_points[j].x 
					+ m_points[j].y * m_points[j].y 
					+ m_points[j].z * m_points[j].z));
			}
		}
	}
}