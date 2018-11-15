#include "DCGp/Custom/GpPointCloud.h"

//C++��׼��
#include <algorithm>
#include <string>
#include <memory>
#include <cmath>
#include <fstream>
#include <iostream>	
#include <array>

//Qt
#include "QtWidgets/QTableWidget"
#include "QtWidgets/QSpinBox"
#include "QTextStream"
#include "QtWidgets/QPushButton"
#include "QFileInfo"
#include "QCoreApplication"

//common
#include "DCCore/Logger.h"
#include "DCCore/Const.h"
#include "DCCore/CoordinatesShiftManager.h"

#include "DCCore/vsShaderLib.h"

#include "DCCore/DCGL.h"
#include "DCGp/GpBasicDevice.h"
#include "DCGp/Custom/GpBoundBox.h"
#include "DCGp/GpDrawContext.h"

//#include "DCFilters/BlockedGridToolkit.h"

//#include "DCUtil/AbstractEntityVisitor.h"

#include "impl/GpPointCloudImpl.hxx"

//default material for clouds (with normals)
#define DC_DEFAULT_CLOUD_AMBIENT_COLOR		DcGp::Color::BRIGHT
#define DC_DEFAULT_CLOUD_SPECULAR_COLOR		DcGp::Color::BRIGHT
#define DC_DEFAULT_CLOUD_DIFFUSE_COLOR		DcGp::Color::BRIGHT
#define DC_DEFAULT_CLOUD_EMISSION_COLOR		DcGp::Color::NIGHT
#define DC_DEFAULT_CLOUD_SHININESS			50.0f
//����ÿ�����ݴ�С(�����)
#define DC_CHUNK_COUNT 100000.0f

const unsigned MAX_LOD_NUM = 200000;

DcGp::DcGpPointCloud::DcGpPointCloud()
	: DcGpEntity()
{
	m_pDcGpPointCloudImpl = DcGpPointCloudImpl::CreateObject();
	ShowName(true);
}

DcGp::DcGpPointCloud::DcGpPointCloud(QString name)
	: DcGpEntity(name)
{
	m_pDcGpPointCloudImpl = DcGpPointCloudImpl::CreateObject();
	m_pDcGpPointCloudImpl->m_qIntface = this;
	ShowName(true);
}

DcGp::DcGpPointCloud::DcGpPointCloud(QString name, long num)
	: DcGpEntity(name)
{
	m_pDcGpPointCloudImpl = DcGpPointCloudImpl::CreateObject();
	m_pDcGpPointCloudImpl->m_qIntface = this;

	m_pDcGpPointCloudImpl->m_points.resize(num);
	m_pDcGpPointCloudImpl->m_pointsVisibility.resize(num);
	m_pDcGpPointCloudImpl->m_pointsChoosedState.resize(num);
	std::fill(m_pDcGpPointCloudImpl->m_pointsVisibility.begin(), 
		m_pDcGpPointCloudImpl->m_pointsVisibility.end(), DCCore::POINT_VISIBLE);
	std::fill(m_pDcGpPointCloudImpl->m_pointsChoosedState.begin(), 
		m_pDcGpPointCloudImpl->m_pointsChoosedState.end(), false);

	ShowName(true);
}

DcGp::DcGpPointCloud::DcGpPointCloud(DcGpPointCloud* associatedCloud)
	: DcGpEntity("")
{
	m_pDcGpPointCloudImpl = DcGpPointCloudImpl::CreateObject();
	m_pDcGpPointCloudImpl->m_qIntface = this;
    m_pDcGpPointCloudImpl->m_indexs.clear();

	ShowName(true);
}

DcGp::DcGpPointCloud::~DcGpPointCloud()
{
	/*if (m_pDcGpPointCloudImpl->m_blockedGridToolkit)
	{
		delete m_pDcGpPointCloudImpl->m_blockedGridToolkit;
		m_pDcGpPointCloudImpl->m_blockedGridToolkit = nullptr;
	}*/
}

void DcGp::DcGpPointCloud::SetNormals(std::vector<DCVector3D> normals)
{
	m_pDcGpPointCloudImpl->m_Normals = normals;
}

std::vector<Normal> DcGp::DcGpPointCloud::GetNormals()
{
	return m_pDcGpPointCloudImpl->m_Normals;
}

DcGp::DcGpScalarFieldPtr DcGp::DcGpPointCloud::GetScalarFieldByIndex(int index)
{
	assert(index >= 0);
	return m_pDcGpPointCloudImpl->m_scalarFields[index];
}

int DcGp::DcGpPointCloud::GetScalarFieldIndexByName(QString name)
{
	size_t sfCount = ScalarFieldCount();
	for (size_t i=0; i < sfCount; ++i)
	{
		//ȷ������������Ψһ
		if (ScalarFieldNames()[i] == name)
		{
			return static_cast<int>(i);
		}
	}

	return -1;
}

unsigned DcGp::DcGpPointCloud::ScalarFieldCount() const
{
	return m_pDcGpPointCloudImpl->m_scalarFields.size();
}

QStringList DcGp::DcGpPointCloud::ScalarFieldNames() const
{
	QStringList names;
	unsigned number = m_pDcGpPointCloudImpl->m_scalarFields.size();

	for (unsigned i = 0; i < number; ++i)
	{
		names.push_back(m_pDcGpPointCloudImpl->m_scalarFields.at(i)->GetName());
	}
	return names;
}


std::vector<DCVector3D> DcGp::DcGpPointCloud::GetPoints()
{
	return m_pDcGpPointCloudImpl->m_points;
}

std::vector<DCVector3D>& DcGp::DcGpPointCloud::GetPointsRef()
{
	return m_pDcGpPointCloudImpl->m_points;
}

void DcGp::DcGpPointCloud::SetPoints(std::vector<DCVector3D>& points)
{
	m_pDcGpPointCloudImpl->m_points = points;
	
	//��ʼ��״ֵ̬
	m_pDcGpPointCloudImpl->m_pointsVisibility.resize(points.size());
	std::fill(m_pDcGpPointCloudImpl->m_pointsVisibility.begin(), m_pDcGpPointCloudImpl->m_pointsVisibility.end(), DCCore::POINT_VISIBLE);

	m_pDcGpPointCloudImpl->m_pointsChoosedState.resize(points.size());
	std::fill(m_pDcGpPointCloudImpl->m_pointsChoosedState.begin(), m_pDcGpPointCloudImpl->m_pointsChoosedState.end(), false);
}

unsigned long DcGp::DcGpPointCloud::Size() const
{
	return m_pDcGpPointCloudImpl->m_points.size();
}

unsigned long DcGp::DcGpPointCloud::GetChunk()
{
	return ceil(Size() / DC_CHUNK_COUNT);
}

void DcGp::DcGpPointCloud::SetColorSize(long num)
{
	m_pDcGpPointCloudImpl->m_rgbColors.reserve(num);
}

//
DCVector3D DcGp::DcGpPointCloud::GetDataByIndex(unsigned index)
{
	if (index >= Size())
	{
		index = Size() - 1;
	}

	return m_pDcGpPointCloudImpl->m_points[index];
}

//��ӵ�(������)
void DcGp::DcGpPointCloud::AddPoint(DCVector3D vector)
{
	m_pDcGpPointCloudImpl->m_points.push_back(vector);
	m_pDcGpPointCloudImpl->m_pointsVisibility.push_back(DCCore::POINT_VISIBLE);
	m_pDcGpPointCloudImpl->m_pointsChoosedState.push_back(false);
}

//��ȡָ����
//������ڵ�ʱ�ſɵ���
DCVector3D DcGp::DcGpPointCloud::GetPoint(unsigned index) const
{
	assert(index < Size() && index >= 0);

	if (index >= Size())
	{
		index = Size() - 1;
	}

	if (index < 0)
	{
		index = 0;
	}

	return m_pDcGpPointCloudImpl->m_points[index];
}

void DcGp::DcGpPointCloud::FastDrawMyselfOnly(DcGpDrawContext& context)
{

	//����������أ��򷵻��ϲ�����
	if (!IsVisible())
	{
		return;
	}

	//�Ƿ���ڵ����ݣ����򷵻��ϲ㺯��
	if (m_pDcGpPointCloudImpl->m_points.empty())
		return;

	//�ж��Ƿ��ǻ�����ά����
	if (MACRO_Draw3D(context))
	{
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		if (!m_pDcGpPointCloudImpl->m_fastDrawShader.getProgramIndex())
		{
			//! ��ʼ��һ��shader program���������������Ⱦʱ������ص�����
			char* byteGlVersion  = (char*)glGetString(GL_VERSION);
			if (byteGlVersion[0] < '3' )
			{
				return;
			}
			else
			{
				m_pDcGpPointCloudImpl->SetupFastDrawShaders();
			}

		}

		if (m_pDcGpPointCloudImpl->m_fastDrawShader.getProgramIndex())
		{
			glUseProgram(m_pDcGpPointCloudImpl->m_fastDrawShader.getProgramIndex());

			//! Ϊblock����ֵ
			double matrixProjection[16];
			double matrixView[16];
			glGetDoublev(GL_PROJECTION_MATRIX, matrixProjection);
			glGetDoublev(GL_MODELVIEW_MATRIX, matrixView);

			DCCore::mat4 matPro;
			DCCore::mat4 matView;
			for (int i = 0; i != 16; ++i)
			{
				matPro.ptr()[i] = matrixProjection[i];
				matView.ptr()[i] = matrixView[i];
			}

			DCCore::mat4 pvm = matPro * matView;

			m_pDcGpPointCloudImpl->m_fastDrawShader.setUniform("pvm", pvm.ptr());
			float isSingleColor = 0;
			m_pDcGpPointCloudImpl->m_fastDrawShader.setUniform("isSingleColor", &isSingleColor);
		}


		//��ȡ��ʾ����
		glDrawParameters glParams;
		GetDrawingParameters(glParams);
		glParams.showNormals &= bool(MACRO_LightIsEnabled(context));	//�Ƿ�ʹ�ù���


		//! ���㴿ɫֵ
		std::vector<float > singleColor;
		if (glParams.showColors && IsColorOverriden())
		{
			singleColor.push_back(GetTempColor()[0]);
			singleColor.push_back(GetTempColor()[1]);
			singleColor.push_back(GetTempColor()[2]);
			glParams.showColors = false;
		}
		else
		{
			singleColor.push_back(context.pointsDefaultCol[0]);
			singleColor.push_back(context.pointsDefaultCol[1]);
			singleColor.push_back(context.pointsDefaultCol[2]);
		}


		// L.O.D.
		unsigned numberOfPoints = Size();
		unsigned sample = 1;//��׼����ʾȫ����
		unsigned totalSamplePoints = numberOfPoints;
		if (numberOfPoints >= DCCore::MAX_FACE_NUM)
		{
			sample = static_cast<int>(ceil(static_cast<float>(numberOfPoints) / DCCore::MAX_FACE_NUM));
		}

		if (m_pDcGpPointCloudImpl->m_pointSize != 0)
			glPointSize((GLfloat)m_pDcGpPointCloudImpl->m_pointSize);


		//! ��Ⱦ���ƣ���ΪΪshader�����˿ɼ��Ա��������Բ���ʲô���ֻҪ���ƴ���
		//! ���㡢��ɫ�����������ɡ������ǲ���ģ���ɫ�Ǳ仯�ģ���ɫ������ɫ��ѡȡɫ�����������ǲ���ģ����ڻ��߲����ڣ�
		//!����alpha����
		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GREATER, 0.0f);
		

		//! 1��ÿ֡�ж��㲻��
		glEnableVertexAttribArray(VSShaderLib::VERTEX_COORD_ATTRIB);
		glVertexAttribPointer(VSShaderLib::VERTEX_COORD_ATTRIB, 3, GL_FLOAT, 0, sample * 3 * sizeof(PointCoordinateType), 
			&(m_pDcGpPointCloudImpl->m_points[0][0]));

		//! 2��ÿ֡�ж���ɼ��Բ��䣬���ݶ���Ŀɼ��Ա�����shader��
		glEnableVertexAttribArray(VSShaderLib::VERTEX_ATTRIB1);
		glVertexAttribPointer(VSShaderLib::VERTEX_ATTRIB1, 1, GL_INT, 0, sample * 1 * sizeof(int), 
			&(m_pDcGpPointCloudImpl->m_pointsVisibility[0]));

		//! 3��ȷ����ɫֵ��ôѡ��ֻ���ڴ�ɫ�����ɫ������ɫ���֣�
		if (glParams.showColors)
		{
			glEnableVertexAttribArray(VSShaderLib::VERTEX_ATTRIB2);
			
			if (IsChoosed())
			{
				glVertexAttribPointer(VSShaderLib::VERTEX_ATTRIB2, 3, GL_UNSIGNED_BYTE, 0, sample * 3 * sizeof(ColorType), 
					&(m_pDcGpPointCloudImpl->m_choosedColors[0][0]));
			}
			else
			{
				glVertexAttribPointer(VSShaderLib::VERTEX_ATTRIB2, 3, GL_UNSIGNED_BYTE, 0, sample * 3 * sizeof(ColorType), 
					&(m_pDcGpPointCloudImpl->m_rgbColors[0][0]));
			}
		}
		else if (glParams.showScalarField) 
		{
			glEnableVertexAttribArray(VSShaderLib::VERTEX_ATTRIB2);
			if (IsChoosed())
			{
				glVertexAttribPointer(VSShaderLib::VERTEX_ATTRIB2, 3, GL_UNSIGNED_BYTE, 0, sample * 3 * sizeof(ColorType), 
					&(m_pDcGpPointCloudImpl->m_choosedColors[0][0]));
			}
			else
			{
				glVertexAttribPointer(VSShaderLib::VERTEX_ATTRIB2, 3, GL_UNSIGNED_BYTE, 0, sample * 3 * sizeof(ColorType), 
					&(m_pDcGpPointCloudImpl->m_scalarColors[0][0]));
			}
		}
		else
		{
			if (IsChoosed())
			{
				float isSingleColor = 0;
				m_pDcGpPointCloudImpl->m_fastDrawShader.setUniform("isSingleColor", &isSingleColor);
				
				glEnableVertexAttribArray(VSShaderLib::VERTEX_ATTRIB2);
				glVertexAttribPointer(VSShaderLib::VERTEX_ATTRIB2, 3, GL_UNSIGNED_BYTE, 0, sample * 3 * sizeof(ColorType), 
					&(m_pDcGpPointCloudImpl->m_choosedColors[0][0]));
			}
			else
			{
				//! ��ɫ�������ڴ�ɫ������ɫ���飬���Բ���uniformʵ��
				float isSingleColor = 1;
				m_pDcGpPointCloudImpl->m_fastDrawShader.setUniform("isSingleColor", &isSingleColor);

				//!���봿ɫֵ
				m_pDcGpPointCloudImpl->m_fastDrawShader.setUniform("singleColor", &singleColor[0]);
			}
			
		}

		
		//! ��Ⱦ
		long lodNum = Size();
		if (sample > 1)
		{
			float numFloat = static_cast<float >(lodNum) / sample;
			lodNum = static_cast<unsigned>(floor(numFloat));
		}
		glDrawArrays(GL_POINTS, 0, lodNum);


		glDisableVertexAttribArray(VSShaderLib::VERTEX_COORD_ATTRIB);
		glDisableVertexAttribArray(VSShaderLib::VERTEX_ATTRIB1);
		glDisableVertexAttribArray(VSShaderLib::VERTEX_ATTRIB2);

		
		glDisable(GL_ALPHA_TEST);

		
		glPopAttrib();
		glUseProgram(0);
	}
}

void DcGp::DcGpPointCloud::DrawWithNamesMyselfOnly(DcGpDrawContext& context, bool drawSample /* = false */)
{
	
}

//���Ƶ���
void DcGp::DcGpPointCloud::DrawMyselfOnly(DcGpDrawContext& context)
{	
	//����������أ��򷵻��ϲ�����
	if (!IsVisible())
	{
		return;
	}

	//�Ƿ���ڵ����ݣ����򷵻��ϲ㺯��
	if (m_pDcGpPointCloudImpl->m_points.empty())
		return;

	//�ж��Ƿ��ǻ�����ά����
	if (MACRO_Draw3D(context))
	{
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		if (!m_pDcGpPointCloudImpl->m_generalDrawShader.getProgramIndex())
		{
			//! ��ʼ��һ��shader program���������������Ⱦʱ������ص�����
			char* byteGlVersion  = (char*)glGetString(GL_VERSION);
			if (byteGlVersion[0] < '3' )
			{
				return;
			}
			else
			{
				m_pDcGpPointCloudImpl->SetupGeneralDrawShaders();
			}

		}

		if (m_pDcGpPointCloudImpl->m_generalDrawShader.getProgramIndex())
		{
			glUseProgram(m_pDcGpPointCloudImpl->m_generalDrawShader.getProgramIndex());

			//! Ϊblock����ֵ
			double matrixProjection[16];
			double matrixView[16];
			glGetDoublev(GL_PROJECTION_MATRIX, matrixProjection);
			glGetDoublev(GL_MODELVIEW_MATRIX, matrixView);

			DCCore::mat4 matPro;
			DCCore::mat4 matView;
			for (int i = 0; i != 16; ++i)
			{
				matPro.ptr()[i] = matrixProjection[i];
				matView.ptr()[i] = matrixView[i];
			}

			DCCore::mat4 pvm = matPro * matView;

			m_pDcGpPointCloudImpl->m_generalDrawShader.setUniform("pvm", pvm.ptr());
			float isSingleColor = 0;
			m_pDcGpPointCloudImpl->m_generalDrawShader.setUniform("isSingleColor", &isSingleColor);
		}


		//��ȡ��ʾ����
		glDrawParameters glParams;
		GetDrawingParameters(glParams);
		glParams.showNormals &= bool(MACRO_LightIsEnabled(context));	//�Ƿ�ʹ�ù���


		//! ���㴿ɫֵ
		std::vector<float > singleColor;
		if (glParams.showColors && IsColorOverriden())
		{
			singleColor.push_back(GetTempColor()[0]);
			singleColor.push_back(GetTempColor()[1]);
			singleColor.push_back(GetTempColor()[2]);
			glParams.showColors = false;
		}
		else
		{
			singleColor.push_back(context.pointsDefaultCol[0]);
			singleColor.push_back(context.pointsDefaultCol[1]);
			singleColor.push_back(context.pointsDefaultCol[2]);
		}


		// L.O.D.
		unsigned numberOfPoints = Size();
		unsigned decimStep = 0;

		if (m_pDcGpPointCloudImpl->m_pointSize != 0)
			glPointSize((GLfloat)m_pDcGpPointCloudImpl->m_pointSize);


		//! ��Ⱦ���ƣ���ΪΪshader�����˿ɼ��Ա��������Բ���ʲô���ֻҪ���ƴ���
		//! ���㡢��ɫ�����������ɡ������ǲ���ģ���ɫ�Ǳ仯�ģ���ɫ������ɫ��ѡȡɫ�����������ǲ���ģ����ڻ��߲����ڣ�
		//!����alpha����
		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GREATER, 0.0f);

		int sample = 0;

		

		//! 1��ÿ֡�ж��㲻��
		glEnableVertexAttribArray(VSShaderLib::VERTEX_COORD_ATTRIB);
		glVertexAttribPointer(VSShaderLib::VERTEX_COORD_ATTRIB, 3, GL_FLOAT, 0, sample * 3 * sizeof(PointCoordinateType), 
			&(m_pDcGpPointCloudImpl->m_points[0][0]));

		//! 2��ÿ֡�ж���ɼ��Բ��䣬���ݶ���Ŀɼ��Ա�����shader��
		glEnableVertexAttribArray(VSShaderLib::VERTEX_ATTRIB1);
		glVertexAttribPointer(VSShaderLib::VERTEX_ATTRIB1, 1, GL_INT, 0, sample * 1 * sizeof(int), 
			&(m_pDcGpPointCloudImpl->m_pointsVisibility[0]));

		//! 3��ȷ����ɫֵ��ôѡ��ֻ���ڴ�ɫ�����ɫ������ɫ���֣�
		if (glParams.showColors)
		{
			glEnableVertexAttribArray(VSShaderLib::VERTEX_ATTRIB2);
			
			if (IsChoosed())
			{
				glVertexAttribPointer(VSShaderLib::VERTEX_ATTRIB2, 3, GL_UNSIGNED_BYTE, 0, sample * 3 * sizeof(ColorType), 
					&(m_pDcGpPointCloudImpl->m_choosedColors[0][0]));
			}
			else
			{
				glVertexAttribPointer(VSShaderLib::VERTEX_ATTRIB2, 3, GL_UNSIGNED_BYTE, 0, sample * 3 * sizeof(ColorType), 
					&(m_pDcGpPointCloudImpl->m_rgbColors[0][0]));
			}
		}
		else if (glParams.showScalarField) 
		{
			glEnableVertexAttribArray(VSShaderLib::VERTEX_ATTRIB2);
			if (IsChoosed())
			{
				glVertexAttribPointer(VSShaderLib::VERTEX_ATTRIB2, 3, GL_UNSIGNED_BYTE, 0, sample * 3 * sizeof(ColorType), 
					&(m_pDcGpPointCloudImpl->m_choosedColors[0][0]));
			}
			else
			{
				glVertexAttribPointer(VSShaderLib::VERTEX_ATTRIB2, 3, GL_UNSIGNED_BYTE, 0, sample * 3 * sizeof(ColorType), 
					&(m_pDcGpPointCloudImpl->m_scalarColors[0][0]));
			}
		}
		else
		{
			if (IsChoosed())
			{
				float isSingleColor = 0;
				m_pDcGpPointCloudImpl->m_generalDrawShader.setUniform("isSingleColor", &isSingleColor);
				
				glEnableVertexAttribArray(VSShaderLib::VERTEX_ATTRIB2);
				glVertexAttribPointer(VSShaderLib::VERTEX_ATTRIB2, 3, GL_UNSIGNED_BYTE, 0, sample * 3 * sizeof(ColorType), 
					&(m_pDcGpPointCloudImpl->m_choosedColors[0][0]));
			}
			else
			{
				//! ��ɫ�������ڴ�ɫ������ɫ���飬���Բ���uniformʵ��
				float isSingleColor = 1;
				m_pDcGpPointCloudImpl->m_generalDrawShader.setUniform("isSingleColor", &isSingleColor);

				//!���봿ɫֵ
				m_pDcGpPointCloudImpl->m_generalDrawShader.setUniform("singleColor", &singleColor[0]);
			}
			
		}

		
		//! ��Ⱦ
		//glDrawArrays(GL_POINTS, 0, numberOfPoints);

		//���ƶ�������ʱ�������ѭ�������������ݶ�������ÿ�δ���100000����
		if (GetChunk() > 1)
		{
			for (long i = 0; i != GetChunk(); ++i)
			{
				if (i == GetChunk()-1)
				{
					glDrawArrays(GL_POINTS, DC_CHUNK_COUNT * i, Size() - DC_CHUNK_COUNT * i);
				}
				else
				{
					glDrawArrays(GL_POINTS, DC_CHUNK_COUNT * i, DC_CHUNK_COUNT);
				}
			}
		} 
		else
		{
			glDrawArrays(GL_POINTS, 0, Size());
		}

		glDisableVertexAttribArray(VSShaderLib::VERTEX_COORD_ATTRIB);
		glDisableVertexAttribArray(VSShaderLib::VERTEX_ATTRIB1);
		glDisableVertexAttribArray(VSShaderLib::VERTEX_ATTRIB2);

		
		glDisable(GL_ALPHA_TEST);

		
		glPopAttrib();
		glUseProgram(0);
	}
	else if (MACRO_Draw2D(context))
	{
		if (MACRO_Foreground(context) && !context.sfColorScaleToDisplay)
		{
			if (IsSfColorbarVisible()  && ScalarFieldShown())
			{
				//drawScale(context);
				AddColorBarInfo(context);
			}
		}
	}
	//�����ı�
	//context._win->Display3DLabel( "Qt", GetPoint(0) + DCVector3D(context.pickedPointsTextShift), DCColor::MAGENTA, QFont());
}

//���õ��С
void DcGp::DcGpPointCloud::SetPointSize(int size)
{
	m_pDcGpPointCloudImpl->m_pointSize = size;
}

//����ת��
void DcGp::DcGpPointCloud::ApplyTransform(EigenMatrix4d& transMatrix)
{
	//��ʼ�����ֵ��Сֵ
	m_pDcGpPointCloudImpl->m_minPoint = DCVector3D(DCCore::MAX_POINTCOORD, DCCore::MAX_POINTCOORD, DCCore::MAX_POINTCOORD);
	m_pDcGpPointCloudImpl->m_maxPoint = DCVector3D(DCCore::MIN_POINTCOORD, DCCore::MIN_POINTCOORD, DCCore::MIN_POINTCOORD);

	Eigen::Matrix<double, 1, 3> eshift;
	Point_3D shift = GetShift();

	eshift << shift[0], shift[1], shift[2];

	Eigen::Matrix<double, 1, 3> temp;
	for (unsigned i = 0; i != Size(); ++i)
	{
		temp << m_pDcGpPointCloudImpl->m_points[i][0], 
				m_pDcGpPointCloudImpl->m_points[i][1], 
				m_pDcGpPointCloudImpl->m_points[i][2];

		temp = (temp - eshift) * transMatrix.block(0, 0, 3, 3).transpose() + eshift;
		temp += transMatrix.block(0, 3, 3, 1).transpose();
		m_pDcGpPointCloudImpl->m_points[i][0] = temp[0];
		m_pDcGpPointCloudImpl->m_points[i][1] = temp[1];
		m_pDcGpPointCloudImpl->m_points[i][2] = temp[2];

		//���¼������ֵ
		m_pDcGpPointCloudImpl->m_minPoint[0] = std::min(m_pDcGpPointCloudImpl->m_minPoint[0], m_pDcGpPointCloudImpl->m_points[i][0]);
		m_pDcGpPointCloudImpl->m_minPoint[1] = std::min(m_pDcGpPointCloudImpl->m_minPoint[1], m_pDcGpPointCloudImpl->m_points[i][1]);
		m_pDcGpPointCloudImpl->m_minPoint[2] = std::min(m_pDcGpPointCloudImpl->m_minPoint[2], m_pDcGpPointCloudImpl->m_points[i][2]);

		m_pDcGpPointCloudImpl->m_maxPoint[0] = std::max(m_pDcGpPointCloudImpl->m_maxPoint[0], m_pDcGpPointCloudImpl->m_points[i][0]);
		m_pDcGpPointCloudImpl->m_maxPoint[1] = std::max(m_pDcGpPointCloudImpl->m_maxPoint[1], m_pDcGpPointCloudImpl->m_points[i][1]);
		m_pDcGpPointCloudImpl->m_maxPoint[2] = std::max(m_pDcGpPointCloudImpl->m_maxPoint[2], m_pDcGpPointCloudImpl->m_points[i][2]);
	}

	//��Eigen::Matrix4d����ֵ������QMatrix4x4���͵ľ���
	//double* matValue = transMatrix.data();

	//m_pDcGpPointCloudImpl->m_pop = QMatrix4x4(matValue);

	//���¼���߽��
	m_pDcGpPointCloudImpl->SetBorderInValid();
}

//��ȡ��ʾ����
void DcGp::DcGpPointCloud::GetDrawingParameters(glDrawParameters& params) const
{
	//��ɫ��д
	if (IsColorOverriden())
	{
		params.showColors = true;
		params.showNormals = HasNormals() &&  NormalsShown();
		params.showScalarField = false;
	}
	else
	{
		params.showScalarField = HasScalarFields() && ScalarFieldShown();
		params.showNormals = HasNormals() &&  NormalsShown();

		params.showColors = !params.showScalarField && HasColors() && ColorsShown();
	}
}

DcGp::DcGpBoundBox DcGp::DcGpPointCloud::GetDisplayBoundBox()
{
	return GetMyOwnBoundBox();
}

//
DcGp::DcGpBoundBox DcGp::DcGpPointCloud::GetMyOwnBoundBox()
{
	DcGpBoundBox emptyBox(GetMinPoint(), 
							GetMaxPoint());

	return emptyBox;
}

//��ȡָ����Ŀɼ���
uchar DcGp::DcGpPointCloud::GetPointVisibility(unsigned index) const
{
	if (index < Size() && index >= 0)
	{
		return m_pDcGpPointCloudImpl->m_pointsVisibility[index];
	}
	return DCCore::POINT_HIDDEN;
}

//��ȡָ���������ɫ
PointColor DcGp::DcGpPointCloud::GetPointColor(unsigned index) const
{
	if (index >= Size())
	{
		index = Size() - 1;
	}

	if (index < 0)
	{
		index = 0;
	}

	return m_pDcGpPointCloudImpl->m_rgbColors[index];

}

//�����ɫ
void DcGp::DcGpPointCloud::AddColor(PointColor color)
{
	if (!HasColors())
	{
		ShowColors(true);
	}

	m_pDcGpPointCloudImpl->m_rgbColors.push_back(color);

}

//��ȡָ�����ѡ��״̬
bool DcGp::DcGpPointCloud::GetPointChoosed(unsigned index) const
{
	return m_pDcGpPointCloudImpl->m_pointsChoosedState[index];
}

//����ָ�����ѡ��״̬
void DcGp::DcGpPointCloud::SetPointChoosed(unsigned index, bool status)
{

	if (GetPointVisibility(index) == DCCore::POINT_VISIBLE)
	{
		m_pDcGpPointCloudImpl->m_pointsChoosedState[index] = status;

		//����ѡȡ�����ɫ
		if (status)
		{
			m_pDcGpPointCloudImpl->m_choosedColors[index][0] = 255;
			m_pDcGpPointCloudImpl->m_choosedColors[index][1] = 0;
			m_pDcGpPointCloudImpl->m_choosedColors[index][2] = 0;
		}
		else
		{
			if (HasColors() && GetColorMode() == eTrueLinearColor)
			{
				m_pDcGpPointCloudImpl->m_choosedColors[index] = m_pDcGpPointCloudImpl->m_rgbColors[index];
			}
			else if (GetColorMode() == eFalseRangeColor ||
				GetColorMode() == eFalseHeightColor ||
				GetColorMode() == eFalseScalarColor)
			{
				m_pDcGpPointCloudImpl->m_choosedColors[index] = m_pDcGpPointCloudImpl->m_scalarColors[index];
			}
			else
			{
				m_pDcGpPointCloudImpl->m_choosedColors[index][0] = GetTempColor()[0];
				m_pDcGpPointCloudImpl->m_choosedColors[index][1] = GetTempColor()[1];
				m_pDcGpPointCloudImpl->m_choosedColors[index][2] = GetTempColor()[2];
			}
		}
	}
}

//����ѡȡ��ɫ
void DcGp::DcGpPointCloud::EnableChoosedColors()
{
	if (!m_pDcGpPointCloudImpl->m_choosedColors.empty())
	{
		return;
	}

	if (!m_pDcGpPointCloudImpl->m_rgbColors.empty())
	{
		m_pDcGpPointCloudImpl->m_choosedColors = m_pDcGpPointCloudImpl->m_rgbColors;
	}
	else
	{
		PointColor color = {GetTempColor()[0], GetTempColor()[1], GetTempColor()[2]};
		m_pDcGpPointCloudImpl->m_choosedColors.resize(Size());
		std::fill_n(m_pDcGpPointCloudImpl->m_choosedColors.begin(), Size(), color);
	}
}

//ͣ��ѡȡ��ɫ
void DcGp::DcGpPointCloud::DisableChoosedColors()
{
	m_pDcGpPointCloudImpl->m_choosedColors.clear();
}

//
void DcGp::DcGpPointCloud::EnableVisibiltyTable(bool status, bool flag /* = false */)
{
	m_pDcGpPointCloudImpl->m_visibilityTable = status;
	m_pDcGpPointCloudImpl->m_visibilityType = flag;

	if (m_pDcGpPointCloudImpl->m_visibilityTable)
	{
		if (!m_pDcGpPointCloudImpl->m_visibilityType)   //����ѡ��ĵ�
		{
			//���µ�Ŀɼ��ԣ�Ȼ��ȡ�����ѡ��״̬
			unsigned num = Size();
			for (unsigned i = 0; i != num; ++i)
			{
				if (GetPointChoosed(i))  //��ǰ��ѡ��
				{
					SetPointVisibility(i, DCCore::POINT_HIDDEN);
				}
			}
		}
		else  //����δ��ѡ��ĵ�
		{
			//���µ�Ŀɼ��ԣ�Ȼ��ȡ�����ѡ��״̬
			unsigned num = Size();
			for (unsigned i = 0; i != num; ++i)
			{
				if (!GetPointChoosed(i))  //��ǰδ��ѡ��
				{
					SetPointVisibility(i, DCCore::POINT_HIDDEN);
				}
			}
		}
		CancelChoosed();
	}
	else  //��ʾ���У����ÿɼ��Ա�
	{
		m_pDcGpPointCloudImpl->m_pointsVisibility.resize(Size());
		std::fill(m_pDcGpPointCloudImpl->m_pointsVisibility.begin(), m_pDcGpPointCloudImpl->m_pointsVisibility.end(), DCCore::POINT_VISIBLE);
		m_pDcGpPointCloudImpl->m_visibilityTable = false;
		CancelChoosed();
	}
}

bool DcGp::DcGpPointCloud::IsChoosed()
{
	//������ѡ�񣬶���������ѡȡ��ɫ��������ѡȡ״̬
	//return (IsSelected() && !m_pDcGpPointCloudImpl->m_choosedColors.empty());
	return (!m_pDcGpPointCloudImpl->m_choosedColors.empty());
}

//ɾ����ѡ�Ĳ���
void DcGp::DcGpPointCloud::DeleteChoosed()
{
	std::vector<DCVector3D> points;
	std::vector<PointColor> rgbColors;
	std::vector<PointColor> selectColors;
	std::vector<int> pointsVisibility;  //��ʾ��

	for (unsigned i = 0; i < Size(); ++i)
	{
		if (m_pDcGpPointCloudImpl->m_pointsChoosedState[i] == false)
		{
			points.push_back(m_pDcGpPointCloudImpl->m_points[i]);
			selectColors.push_back(m_pDcGpPointCloudImpl->m_choosedColors[i]);
			if (HasColors())
			{
				rgbColors.push_back(m_pDcGpPointCloudImpl->m_rgbColors[i]);
			}

			//ȡ����ǰ��ʾ״̬��������
			pointsVisibility.push_back(GetPointVisibility(i));

			//���±�����ֵ
			
		}
	}
	m_pDcGpPointCloudImpl->m_points = points;
	m_pDcGpPointCloudImpl->m_choosedColors = selectColors;
	m_pDcGpPointCloudImpl->m_rgbColors = rgbColors;
	m_pDcGpPointCloudImpl->m_pointsVisibility = pointsVisibility;
	m_pDcGpPointCloudImpl->m_pointsChoosedState.resize(Size());
	std::fill_n(m_pDcGpPointCloudImpl->m_pointsChoosedState.begin(), Size(), false);
	
	//m_pDcGpPointCloudImpl->m_visibilityTable = false;
	m_pDcGpPointCloudImpl->SetBorderInValid();
}

//ȡ��ѡȡ
void DcGp::DcGpPointCloud::CancelChoosed()
{
	if (!IsChoosed())
	{
		return;
	}
	//ȡ��ÿ�����ѡȡ״̬
	std::fill_n(m_pDcGpPointCloudImpl->m_pointsChoosedState.begin(), Size(), false);

	//���ÿ�����ѡȡ��ɫ
	if (HasColors() && GetColorMode() == eTrueLinearColor)
	{
		m_pDcGpPointCloudImpl->m_choosedColors = m_pDcGpPointCloudImpl->m_rgbColors;
	}
	else if (GetColorMode() == eFalseRangeColor ||
			GetColorMode() == eFalseHeightColor ||
			GetColorMode() == eFalseScalarColor)
	{
		m_pDcGpPointCloudImpl->m_choosedColors = m_pDcGpPointCloudImpl->m_scalarColors;
	}
	else
	{
		PointColor color = {GetTempColor()[0], GetTempColor()[1], GetTempColor()[2]};
		std::fill_n(m_pDcGpPointCloudImpl->m_choosedColors.begin(), Size(), color);
	}
}

//ˢ��ѡȡ��ɫ
void DcGp::DcGpPointCloud::RefreshChoosedColor()
{
	if (m_pDcGpPointCloudImpl->m_choosedColors.size() == 0)
	{
		return;
	}

	switch (GetColorMode())
	{
	case eAmplitudeLinearColor:
		break;
	case eFalseRangeColor:
		m_pDcGpPointCloudImpl->m_choosedColors = m_pDcGpPointCloudImpl->m_scalarColors;
		break;
	case eFalseHeightColor:
		m_pDcGpPointCloudImpl->m_choosedColors = m_pDcGpPointCloudImpl->m_scalarColors;
		break;
	case eFalseScalarColor:
		m_pDcGpPointCloudImpl->m_choosedColors = m_pDcGpPointCloudImpl->m_scalarColors;
		break;
	case eTrueLinearColor:
		if (HasColors())
		{
			//�����������ʵ��ɫ
			for (unsigned i = 0; i < Size(); ++i)
			{
				if (false == m_pDcGpPointCloudImpl->m_pointsChoosedState[i])
				{
					m_pDcGpPointCloudImpl->m_choosedColors[i] = m_pDcGpPointCloudImpl->m_rgbColors[i];
				}
			}
		}
		else
		{
			//����û����ʵ��ɫ
			for (unsigned i = 0; i < Size(); ++i)
			{
				if (false == m_pDcGpPointCloudImpl->m_pointsChoosedState[i])
				{
					m_pDcGpPointCloudImpl->m_choosedColors[i] = GetTempColor();
				}
			}
		}
		break;
	case eTrueLogarithmicColor:
		break;
	case eSingleColor:
		for (unsigned i = 0; i < Size(); ++i)
		{
			if (false == m_pDcGpPointCloudImpl->m_pointsChoosedState[i])
			{
				m_pDcGpPointCloudImpl->m_choosedColors[i] = GetTempColor();
			}
		}
		break;
	case eDeviationColor:
		break;
	default:
		break;
	}
}

//�Ƿ�ӵ����ɫ
bool DcGp::DcGpPointCloud::HasColors() const
{
	return !m_pDcGpPointCloudImpl->m_rgbColors.empty();
}

//�Ƿ�ӵ��������
bool DcGp::DcGpPointCloud::HasNormals() const
{
	return !m_pDcGpPointCloudImpl->m_Normals.empty();
}

//�Ƿ�ӵ�б�����
bool DcGp::DcGpPointCloud::HasScalarFields() const
{
	return (!m_pDcGpPointCloudImpl->m_scalarFields.empty());
}

//��ӱ�����
void DcGp::DcGpPointCloud::AddScalarField(DcGpScalarFieldPtr scalarField)
{
	m_pDcGpPointCloudImpl->m_scalarFields.push_back(scalarField);
}

void DcGp::DcGpPointCloud::ShowScalarField(bool state)
{
	DcGpEntity::ShowScalarField(state);
}

//���ĵ�ǰ����������ɫ��
void DcGp::DcGpPointCloud::ChangeScalarFieldColorScale(QString name)
{
	if (!m_pDcGpPointCloudImpl->m_currentScalarField)
	{
		return;
	}

	m_pDcGpPointCloudImpl->m_currentScalarField->ChangeColorScale(name);
	m_pDcGpPointCloudImpl->m_currentScalarField->Prepare();
	m_pDcGpPointCloudImpl->m_scalarColors = m_pDcGpPointCloudImpl->m_currentScalarField->GetColors();
}

//���õ�ǰ������
void DcGp::DcGpPointCloud::SetCurrentScalarField(unsigned index, unsigned setps, ScalarFieldType type)
{
	if (index >= 0 && index < ScalarFieldCount())
	{
		m_pDcGpPointCloudImpl->m_currentScalarField = m_pDcGpPointCloudImpl->m_scalarFields[index];
	}
	else
	{
		return;
	}
	
	//���¸̺߳;����������ֵ
	m_pDcGpPointCloudImpl->UpdateScalarFieldData();

	m_pDcGpPointCloudImpl->m_currentScalarField->SetColorSteps(setps);
	m_pDcGpPointCloudImpl->m_currentScalarField->Prepare();
	m_pDcGpPointCloudImpl->m_scalarColors = m_pDcGpPointCloudImpl->m_currentScalarField->GetColors();
}

//���浽�ļ�
bool DcGp::DcGpPointCloud::SaveToFile(QFile& file) const
{
	if (file.open(QIODevice::WriteOnly))
	{
		QTextStream out(&file);
		//���ø�ʽ����
		out.setRealNumberNotation(QTextStream::FixedNotation);
		out.setRealNumberPrecision(3);
		//��ȡƫ��������ֵ
		Point_3D shift = GetShift();
		for (unsigned i = 0; i < Size(); ++i)
		{
			out << m_pDcGpPointCloudImpl->m_points[i][0] - shift[0] << ' ' 
				<< m_pDcGpPointCloudImpl->m_points[i][1] - shift[1] << ' ' 
				<< m_pDcGpPointCloudImpl->m_points[i][2] - shift[2] << endl;
		}
		file.close();
		return true;
	}
	return false;
}

//�Զ���������Ա���������õ��ڴ�
void DcGp::DcGpPointCloud::AutoAdjust()
{
	//������������ڴ�
	m_pDcGpPointCloudImpl->m_points.shrink_to_fit();

	//���������ʾ״̬
	m_pDcGpPointCloudImpl->m_pointsVisibility.shrink_to_fit();

	//�������ѡ��״̬���ڴ�
	m_pDcGpPointCloudImpl->m_pointsChoosedState.shrink_to_fit();

	//������ɫ���ڴ�

	//���ɫ
	m_pDcGpPointCloudImpl->m_rgbColors.shrink_to_fit();

	//ѡȡ��ɫ
	m_pDcGpPointCloudImpl->m_choosedColors.shrink_to_fit();

	//������ɫ
	m_pDcGpPointCloudImpl->m_scalarColors.shrink_to_fit();

	//�����������ڴ�
	m_pDcGpPointCloudImpl->m_scalarFields.shrink_to_fit();
}

//��ȡ����
PointCoord DcGp::DcGpPointCloud::GetMaxPoint() const
{
	m_pDcGpPointCloudImpl->CalculateMyOwnBoundBox();

	PointCoord maxPoint(m_pDcGpPointCloudImpl->m_maxPoint.x, 
						m_pDcGpPointCloudImpl->m_maxPoint.y, 
						m_pDcGpPointCloudImpl->m_maxPoint.z);
	return maxPoint;
}

//��ȡ��С��
PointCoord DcGp::DcGpPointCloud::GetMinPoint() const
{
	m_pDcGpPointCloudImpl->CalculateMyOwnBoundBox();

	PointCoord minPoint(m_pDcGpPointCloudImpl->m_minPoint.x, 
						m_pDcGpPointCloudImpl->m_minPoint.y, 
						m_pDcGpPointCloudImpl->m_minPoint.z);
	return minPoint;
}

int DcGp::DcGpPointCloud::GetPointSize()
{
	return m_pDcGpPointCloudImpl->m_pointSize;
}

void DcGp::DcGpPointCloud::SetRGBColor(std::vector<PointColor> rgbColor)
{
	m_pDcGpPointCloudImpl->m_rgbColors = rgbColor;
}

std::vector<PointColor> DcGp::DcGpPointCloud::GetRGBColor()
{
	return m_pDcGpPointCloudImpl->m_rgbColors;
}

std::vector<PointColor>& DcGp::DcGpPointCloud::GetRGBColorRef()
{
	return m_pDcGpPointCloudImpl->m_rgbColors;
}

void DcGp::DcGpPointCloud::SetSfColorBarVisibility(bool status)
{
	DcGpEntity::SetSfColorBarVisibility(status);
}

void DcGp::DcGpPointCloud::AddColorBarInfo(DcGpDrawContext& context)
{
	if (!m_pDcGpPointCloudImpl->m_currentScalarField)
	{
		return;
	}
	
	context.sfColorScaleToDisplay = static_cast<DcGp::DcGpScalarField*>(m_pDcGpPointCloudImpl->m_currentScalarField.get());
}

void DcGp::DcGpPointCloud::SetPop(QMatrix4x4 matrix)
{
	m_pDcGpPointCloudImpl->m_pop = matrix;
}

QMatrix4x4 DcGp::DcGpPointCloud::GetPop()
{
	return m_pDcGpPointCloudImpl->m_pop;
}


void DcGp::DcGpPointCloud::GetChoosedPoints(std::vector<DCVector3D>& points)
{
	for (unsigned i = 0; i < Size(); ++i)
	{
		if (m_pDcGpPointCloudImpl->m_pointsChoosedState[i] == true)
		{
			points.push_back(m_pDcGpPointCloudImpl->m_points[i]);
		}
	}
}

void DcGp::DcGpPointCloud::SetPointVisibility(unsigned index, uchar visibility)
{
	m_pDcGpPointCloudImpl->m_visibilityTable = true;
	m_pDcGpPointCloudImpl->m_pointsVisibility[index] = visibility;
}

bool DcGp::DcGpPointCloud::HasChoosed()
{
	std::vector<bool >::iterator it = std::find(m_pDcGpPointCloudImpl->m_pointsChoosedState.begin(), m_pDcGpPointCloudImpl->m_pointsChoosedState.end(), true);
	if (it == m_pDcGpPointCloudImpl->m_pointsChoosedState.end())
	{
		return false;
	}
	return true;
}

DCVector3D DcGp::DcGpPointCloud::GetPointNormal(unsigned index) const
{
	if (index >= m_pDcGpPointCloudImpl->m_Normals.size())
	{
		index = m_pDcGpPointCloudImpl->m_Normals.size() - 1;
	}

	if (index < 0)
	{
		index = 0;
	}

	return m_pDcGpPointCloudImpl->m_Normals[index];
}

//��ӵ�(������)
void DcGp::DcGpPointCloud::AddNormal(DCVector3D normal)
{
	m_pDcGpPointCloudImpl->m_Normals.push_back(normal);
}

void DcGp::DcGpPointCloud::SetNormalSize(long num)
{
	m_pDcGpPointCloudImpl->m_Normals.reserve(num);
}

DcGp::DcGpScalarFieldPtr DcGp::DcGpPointCloud::GetCurrentScalarField()
{
	return m_pDcGpPointCloudImpl->m_currentScalarField;
}

int DcGp::DcGpPointCloud::GetCurrentScalarFieldIndex()
{
	if (!m_pDcGpPointCloudImpl->m_currentScalarField)
	{
		return -1;
	}

	return ScalarFieldNames().indexOf(m_pDcGpPointCloudImpl->m_currentScalarField->GetName());
}

void DcGp::DcGpPointCloud::Accept(DCUtil::AbstractEntityVisitor& nv)
{
	nv.Apply(*this);
}

//DcFilter::BlockedGridToolkit* DcGp::DcGpPointCloud::GetBlocked()
//{
//	if (!m_pDcGpPointCloudImpl->m_blockedGridToolkit)
//	{
//		m_pDcGpPointCloudImpl->m_blockedGridToolkit = new DcFilter::BlockedGridToolkit();
//		m_pDcGpPointCloudImpl->m_blockedGridToolkit->SetInputCloud(GetPointsRef());
//		m_pDcGpPointCloudImpl->m_blockedGridToolkit->SetResolution(2, 2);
//		m_pDcGpPointCloudImpl->m_blockedGridToolkit->BlockingGridFromInputCloud();
//	}
//
//	return m_pDcGpPointCloudImpl->m_blockedGridToolkit;
//}