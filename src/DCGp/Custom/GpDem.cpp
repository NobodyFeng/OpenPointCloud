#include <assert.h>
#include <algorithm>

#include "GpDem.h"

//Qt
#include "QtWidgets/QTableWidget"
#include "QtWidgets/QComboBox"
#include "QtWidgets/QPushButton"

#include "Eigen/Dense"

#include "DCCore/DCGL.h"
#include "GpScalarField.h"
#include "DCGp/GpDrawContext.h"

#include "DCCore/Const.h"
#include "DCGp/Common.h"

namespace DcGp
{
	DECLARE_SMARTPTR(DcGpDEMImpl)

	class DcGpDEMImpl
	{
		DECLARE_PHONY_CONSTRUCTOR(DcGpDEMImpl)

		//���㵱ǰʵ����������Сֵ
		void CalculateMyOwnBoundBox()
		{
			if (!m_borderValid)
			{
				//����m_points��������С���ֵm_minPoint��m_maxPoint
				for (auto i = 0; i < m_vertex.size(); ++i)
				{
					m_minPoint[0] = std::min(m_minPoint[0], m_vertex[i][0]);
					m_minPoint[1] = std::min(m_minPoint[1], m_vertex[i][1]);
					m_minPoint[2] = std::min(m_minPoint[2], m_vertex[i][2]);

					m_maxPoint[0] = std::max(m_maxPoint[0], m_vertex[i][0]);
					m_maxPoint[1] = std::max(m_maxPoint[1], m_vertex[i][1]);
					m_maxPoint[2] = std::max(m_maxPoint[2], m_vertex[i][2]);
				}
				//����״ֵ̬
				m_borderValid = true;
			}
		}

	protected:
		DcGpDEMImpl()
			: m_xNumber(100)									//Ĭ��X����ĸ�������
			, m_yNumber(100)									//Ĭ��Y����ĸ�������
			, m_xInterval(1.0)								//Ĭ��X����ĸ������
			, m_yInterval(1.0)								//Ĭ��Y����ĸ������
			, m_showStyle(DcGpDEM::eShowPoint)
			, m_borderValid(false)
			, m_targetTex(0)
		{
			m_minPoint = DCVector3D(DCCore::MAX_POINTCOORD, DCCore::MAX_POINTCOORD, DCCore::MAX_POINTCOORD);
			m_maxPoint = DCVector3D(DCCore::MIN_POINTCOORD, DCCore::MIN_POINTCOORD, DCCore::MIN_POINTCOORD);
			
			m_currentScalarField = DcGpScalarFieldPtr();
			//��ʼ���̱߳�����
			m_heightScalarField = DcGpScalarFieldPtr(new DcGpScalarField("Height"));

			//��ʼ����ԭ����������
			m_distanceScalarField = DcGpScalarFieldPtr(new DcGpScalarField("Distacne"));
		}

	private:
		friend class DcGpDEM;
		//�����Ĵ�С
		GridSize_Type m_xInterval;
		GridSize_Type m_yInterval;
		//����������
		GridCount_Type m_xNumber;
		GridCount_Type m_yNumber;
		//�����Ķ�������
		std::vector<PointCoord> m_vertex;
		//�߳���Ϣ
		DcGpDEM::VertexHeight m_heights;
		//���½�����
		Point_3D m_startPoint;
		//���ӹ�ϵ
		std::vector<std::array<unsigned, 4>> m_links;
		//��ʾģʽ
		DcGpDEM::ShowStyle m_showStyle;
		//���㷨��
		std::vector<DCVector3D> m_Normals;

		//��������ɫ
		std::vector<PointColor> m_rgbColors;

		//��������ɫ
		std::vector<PointColor> m_scalarColors;
		//������
		std::vector<DcGpScalarFieldPtr> m_scalarFields;
		//��ǰʹ�õı�����
		DcGpScalarFieldPtr m_currentScalarField;
		//����ԭ��ı�����
		DcGpScalarFieldPtr m_distanceScalarField;
		//�̱߳�����
		DcGpScalarFieldPtr m_heightScalarField;

		//!\brief �Ƿ��Ѿ�����������Сֵ
		bool m_borderValid;

		//XYZ��Χ
		DCVector3D m_minPoint;
		DCVector3D m_maxPoint;

		//!\brief Ŀ���������
		GLuint m_targetTex;

		//!\brief ��������
		std::vector<DCVector2D> m_texCoords;
	};
}

DcGp::DcGpDEM::DcGpDEM()
	: DcGpEntity()
{
	m_pDcGpDEMImpl = DcGpDEMImpl::CreateObject();
}

DcGp::DcGpDEM::DcGpDEM(QString name)
	: DcGpEntity(name)
{
	m_pDcGpDEMImpl = DcGpDEMImpl::CreateObject();
}

DcGp::DcGpDEM::~DcGpDEM()
{

}

//���Ӷ���
// void DcGpDEM::AddVertex(DEMVertex vertex)
// {
// 	m_vertex = vertex;
// }

//�������½ǵ�����͸�������
void DcGp::DcGpDEM::SetParameter(Point_3D startPoint, 
	GridCount_Type xNumber, 
	GridCount_Type yNumber, 
	GridSize_Type xInterval,
	GridSize_Type yInterval,
	VertexHeight heights,
	bool holdBoder)
{
	m_pDcGpDEMImpl->m_startPoint = startPoint;
	m_pDcGpDEMImpl->m_xNumber = xNumber;
	m_pDcGpDEMImpl->m_yNumber = yNumber;
	m_pDcGpDEMImpl->m_xInterval = xInterval;
	m_pDcGpDEMImpl->m_yInterval = yInterval;

	//�ȸ���heights������ӹ�ϵ��if (heights.find(index) == heights.end())��˵���÷���û����Ч�㣬����Ӷ�Ӧ�����ӹ�ϵ
	for (unsigned i = 0; i < (xNumber + 1) * yNumber; ++i)
	{
		if ((i + 1) % (xNumber + 1) != 0)
		{
			//����iֵȷ��indexֵ��
			Index index = {i % (xNumber + 1), i / (xNumber + 1)};
			if (heights.find(index) == heights.end() && !holdBoder)
			{
				continue;
			}
			std::array<unsigned, 4> link = {i, i + 1, i + xNumber + 2, i + xNumber + 1};
			m_pDcGpDEMImpl->m_links.push_back(link);
		}
	}

	for (unsigned i = 0; i < yNumber + 1; ++i)
	{
		for (unsigned j = 0; j < xNumber + 1; ++j)
		{
			Index index = {j, i};
			PointCoord point;
			if (heights.find(index) == heights.end())
			{
				point[0] = startPoint[0] + xInterval * j;
				point[1] = startPoint[1] + yInterval * i;
				point[2] = startPoint[2];
				heights[index].push_back(point[2]);
			}
			else
			{
				point[0] = startPoint[0] + xInterval * j;
				point[1] = startPoint[1] + yInterval * i;
				point[2] = heights[index][0];
			}
			m_pDcGpDEMImpl->m_vertex.push_back(point);
		}
	}
	m_pDcGpDEMImpl->m_heights = heights;
}

//����ת��
void DcGp::DcGpDEM::ApplyTransform(EigenMatrix4d& transMatrix)
{
	//��ʼ�����ֵ��Сֵ
	m_pDcGpDEMImpl->m_minPoint = DCVector3D(DCCore::MAX_POINTCOORD, DCCore::MAX_POINTCOORD, DCCore::MAX_POINTCOORD);
	m_pDcGpDEMImpl->m_maxPoint = DCVector3D(DCCore::MIN_POINTCOORD, DCCore::MIN_POINTCOORD, DCCore::MIN_POINTCOORD);

	Eigen::Matrix<double, 1, 3> temp;
	for (unsigned i = 0; i != PSize(); ++i)
	{
		/*temp << m_points[3 * i], m_points[3 * i + 1], m_points[3 * i + 2];*/
		temp << m_pDcGpDEMImpl->m_vertex[i][0], 
			m_pDcGpDEMImpl->m_vertex[i][1], 
			m_pDcGpDEMImpl->m_vertex[i][2];

		temp *= transMatrix.block(0, 0, 3, 3).transpose();
		temp += transMatrix.block(0, 3, 3, 1).transpose();
		// 		m_points[3 * i] = temp[0];
		// 		m_points[3 * i + 1] = temp[1];
		// 		m_points[3 * i + 2] = temp[2];
		m_pDcGpDEMImpl->m_vertex[i][0] = temp[0];
		m_pDcGpDEMImpl->m_vertex[i][1] = temp[1];
		m_pDcGpDEMImpl->m_vertex[i][2] = temp[2];


		//���¼������ֵ
		m_pDcGpDEMImpl->m_vertex[0] = std::min(m_pDcGpDEMImpl->m_minPoint[0], m_pDcGpDEMImpl->m_vertex[i][0]);
		m_pDcGpDEMImpl->m_vertex[1] = std::min(m_pDcGpDEMImpl->m_minPoint[1], m_pDcGpDEMImpl->m_vertex[i][1]);
		m_pDcGpDEMImpl->m_vertex[2] = std::min(m_pDcGpDEMImpl->m_minPoint[2], m_pDcGpDEMImpl->m_vertex[i][2]);

		m_pDcGpDEMImpl->m_vertex[0] = std::max(m_pDcGpDEMImpl->m_maxPoint[0], m_pDcGpDEMImpl->m_vertex[i][0]);
		m_pDcGpDEMImpl->m_vertex[1] = std::max(m_pDcGpDEMImpl->m_maxPoint[1], m_pDcGpDEMImpl->m_vertex[i][1]);
		m_pDcGpDEMImpl->m_vertex[2] = std::max(m_pDcGpDEMImpl->m_maxPoint[2], m_pDcGpDEMImpl->m_vertex[i][2]);
	}

	//��Eigen::Matrix4d����ֵ������QMatrix4x4���͵ľ���
	double* matValue = transMatrix.data();

	//m_pDcGpDEMImpl->m_pop = QMatrix4x4(matValue);

	//���¼���߽��
	m_pDcGpDEMImpl->m_borderValid = false;

}

//����
void DcGp::DcGpDEM::DrawMyselfOnly(DcGpDrawContext& context)
{
	if (MACRO_Draw3D(context))
	{
		//�Ƿ���ڶ����
		unsigned n, gonNum = m_pDcGpDEMImpl->m_links.size();
		unsigned pNum = PSize();
		if (pNum == 0)
		{
			return;
		}
		//��ȡ��ʾ����
		glDrawParameters glParams;
		GetDrawingParameters(glParams);
		glParams.showNormals &= bool(MACRO_LightIsEnabled(context));

		//�Ƿ�������ʾ
		bool showWired = (m_pDcGpDEMImpl->m_showStyle == eShowGrid);

		//����ɫ������ɫ
		bool colorMaterialEnabled = false;
		if (glParams.showScalarField || glParams.showColors)
		{
			glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
			glEnable(GL_COLOR_MATERIAL);
			colorMaterialEnabled = true;
		}

		//�Ƿ�׽
		bool pushName = MACRO_DrawEntityNames(context);
		bool pushDemNames = MACRO_DrawTriangleNames(context);

		pushName |= pushDemNames;

		if (pushName)
		{
			//not fast at all!
			if (MACRO_DrawFastNamesOnly(context))
				return;

			glPushName(GetUniqueID());
			//��С��ʾѡ��ģʽ
			glParams.showNormals = false;
			glParams.showColors = false;
			if (glParams.showScalarField);// && m_currentDisplayedScalarField->areNaNValuesShownInGrey())
			glParams.showScalarField = false; //--> we keep it only if SF 'NaN' values are potentially hidden
		}

		//���ƴ�ɫ
		if (glParams.showColors && IsColorOverriden())
		{
			glColor3ub(GetTempColor()[0], GetTempColor()[1], GetTempColor()[2]);
			glParams.showColors = false;
		}
		else
		{
			glColor3fv(context.defaultMat.GetDiffuseFront());
		}

		//�����ƹ�
		if (glParams.showNormals)
		{
			//DGM: Strangely, when Qt::renderPixmap is called, the OpenGL version can fall to 1.0!
			glEnable((QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_Version_1_2 ? GL_RESCALE_NORMAL : GL_NORMALIZE));
			glEnable(GL_LIGHTING);
			context.defaultMat.ApplyGL(true,colorMaterialEnabled);
		}
		//������������
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, &(m_pDcGpDEMImpl->m_vertex[0][0]));

		//�Ƿ�����������ʾ
		if (glParams.showNormals)
		{
			glEnableClientState(GL_NORMAL_ARRAY);
			glNormalPointer(GL_FLOAT, 0, &(m_pDcGpDEMImpl->m_Normals[0][0]));
		}
		if (glParams.showScalarField)
		{
			glEnableClientState(GL_COLOR_ARRAY);
			glColorPointer(3,GL_UNSIGNED_BYTE,3 * sizeof(ColorType), &(m_pDcGpDEMImpl->m_scalarColors[0][0]));
		}

		if (glParams.showColors)
		{
			glEnableClientState(GL_COLOR_ARRAY);
			glColorPointer(3,GL_UNSIGNED_BYTE,3 * sizeof(ColorType), &(m_pDcGpDEMImpl->m_rgbColors[0][0]));
		}

		//�Ƿ�������������
		if (HasTextures())
		{
			//glEnable(GL_LIGHTING);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, m_pDcGpDEMImpl->m_targetTex);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(3, GL_FLOAT,
				2 * sizeof(GL_FLOAT), &(m_pDcGpDEMImpl->m_texCoords[0][0]));
		}

		if (pushDemNames)
		{
			if (pushDemNames)
				glPushName(0);

			//ѭ������n��dem����
			
			for (unsigned i = 0; i < gonNum; ++i)
			{
				//ȡ����ǰ�����Ŷ�Ӧ�����ӹ�ϵ�������ĸ���
				std::array<unsigned, 4> link = GetLink(i);
				if (link.size() != 4)
				{
					return;
				}
				glLoadName(i);
				glBegin(GL_QUADS);
				//�������ӹ�ϵȡ���ĸ�������
				DCVector3D point1 = GetVertex(link[0]);
				DCVector3D point2 = GetVertex(link[1]);
				DCVector3D point3 = GetVertex(link[2]);
				DCVector3D point4 = GetVertex(link[3]);

				
				glVertex3f(point1.x, point1.y, point1.z);
				glVertex3f(point2.x, point2.y, point2.z);
				glVertex3f(point3.x, point3.y, point3.z);
				glVertex3f(point4.x, point4.y, point4.z);
				glEnd();
			}

			if (pushDemNames)
				glPopName();
		}
		else
		{
			//�ж���ʾģʽ
			if(m_pDcGpDEMImpl->m_showStyle == eShowPoint)
			{
				glDrawArrays(GL_POINTS, 0, PSize());
			}
			else if(m_pDcGpDEMImpl->m_showStyle == eShowGrid && gonNum)
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				//��������,ѭ�������ٶȸ���
				glDrawElements(GL_QUADS, 4 * m_pDcGpDEMImpl->m_links.size(), 
					GL_UNSIGNED_INT, &(m_pDcGpDEMImpl->m_links[0][0]));
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}
			else if (m_pDcGpDEMImpl->m_showStyle == eShowFill && gonNum)
			{
				//��������,ѭ�������ٶȸ���
				glDrawElements(GL_QUADS, 4 * m_pDcGpDEMImpl->m_links.size(), 
					GL_UNSIGNED_INT, &(m_pDcGpDEMImpl->m_links[0][0]));
			}
			else if (m_pDcGpDEMImpl->m_showStyle == eShowDefault && gonNum)
			{
				glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT );
				//���Ƶ�һ�����
				glEnable(GL_POLYGON_OFFSET_FILL);
				glPolygonOffset(1.0, 1);
				glDrawElements(GL_QUADS, 4 * m_pDcGpDEMImpl->m_links.size(), 
					GL_UNSIGNED_INT, &(m_pDcGpDEMImpl->m_links[0][0]));
				glDisable(GL_POLYGON_OFFSET_FILL);

				//���Ƶڶ�������Ϊ����ʾ������Ч������Ҫ�ȹرյ���ɫ����
				if (glParams.showScalarField || glParams.showColors)
				{
					glDisableClientState(GL_COLOR_ARRAY);
				}
				if (glParams.showNormals)
				{
					glDisableClientState(GL_NORMAL_ARRAY);
				}
				//������ɫֵ
				glColor3f(1.0f,1.0f,1.0f);

				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				glDrawElements(GL_QUADS, 4 * m_pDcGpDEMImpl->m_links.size(), 
					GL_UNSIGNED_INT, &(m_pDcGpDEMImpl->m_links[0][0]));
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				glPopAttrib();
			}
		}

		//�رն�������,�������飬��ɫ����
		glDisableClientState(GL_VERTEX_ARRAY);
		if (glParams.showNormals)
			glDisableClientState(GL_NORMAL_ARRAY);
		if (glParams.showScalarField || glParams.showColors)
			glDisableClientState(GL_COLOR_ARRAY);

		//�رղ���ģʽ
		if (colorMaterialEnabled)
			glDisable(GL_COLOR_MATERIAL);
		//�رյƹ�
		if (glParams.showNormals)
		{
			glDisable(GL_LIGHTING);
			glDisable((QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_Version_1_2 ? GL_RESCALE_NORMAL : GL_NORMALIZE));
		}

		if (HasTextures())
		{
			//glDisable(GL_LIGHTING);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			glDisable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		if (pushName)
			glPopName();
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
}

//������ʾģʽ
void DcGp::DcGpDEM::SetShowStyle(ShowStyle style)
{
	m_pDcGpDEMImpl->m_showStyle = style;
}

//��ʾ����
void DcGp::DcGpDEM::ShowProperties(QTableWidget* widget)
{
	QTableWidgetItem* tblmItem;
	//��������
	widget->setRowCount(5);
	//��������
	tblmItem = new QTableWidgetItem(tr("Name"));
	widget->setItem(0, 0, tblmItem);
	tblmItem = new QTableWidgetItem(GetName());
	widget->setItem(0, 1, tblmItem);
	//���������
	tblmItem = new QTableWidgetItem(tr("GridNumber"));
	widget->setItem(1, 0, tblmItem);
	tblmItem = new QTableWidgetItem(QString::number(m_pDcGpDEMImpl->m_links.size()));
	widget->setItem(1, 1, tblmItem);

	//������ɫ
	tblmItem = new QTableWidgetItem(tr("Color"));
	widget->setItem(2, 0, tblmItem);

	//������ɫ��ť
	QPushButton* pbtnItem = new QPushButton(tr("Set Color"));
	//�����ź���۵�����
	connect(pbtnItem, SIGNAL(clicked()), this, SLOT(SetColor()));
	widget->setCellWidget(2, 1, pbtnItem);


	//���ģʽ
	tblmItem = new QTableWidgetItem(tr("ShowStyle"));
	widget->setItem(3, 0, tblmItem);

	QComboBox* cmbxItem = new QComboBox();
	cmbxItem->insertItem(eShowGrid, tr("Grid"));
	cmbxItem->insertItem(eShowFill, tr("Fill"));
	cmbxItem->insertItem(eShowDefault, tr("Grid and Fill"));
	//�����ź���۵�����
	connect(cmbxItem, SIGNAL(currentIndexChanged(int)), this, SLOT(ChangeShowStyle(int)));
	widget->setCellWidget(3, 1, cmbxItem);

	//��ʾ�Ĵ���
	tblmItem = new QTableWidgetItem(tr("View"));
	widget->setItem(4, 0, tblmItem);
	//��ȡ��ǰ���������Ĵ���
	QString title;
	if (GetAssociatedWindow())
	{
		title = GetAssociatedWindow()->GetWindowTitle();
	} 
	else
	{
		title = "None";
	}
	tblmItem = new QTableWidgetItem(title);
	widget->setItem(4, 1, tblmItem);
}

//�ı���ʾ��ʽ
void DcGp::DcGpDEM::ChangeShowStyle(int style)
{
	m_pDcGpDEMImpl->m_showStyle = static_cast<ShowStyle>(style);
	GetAssociatedWindow()->Redraw();
}


//�������񶥵�������������
unsigned DcGp::DcGpDEM::GetNumberByIndex(Index index)
{
	return index[0] + index[1] * (m_pDcGpDEMImpl->m_xNumber + 1);
}

//��������������������
DcGp::DcGpDEM::Index DcGp::DcGpDEM::GetIndexByNumber(unsigned number)
{
	Index index;
	index[0] = number / (m_pDcGpDEMImpl->m_xNumber + 1);
	index[1] = number % (m_pDcGpDEMImpl->m_xNumber + 1);
	return index;
}

//�����ṩ����ƽ�̣߳����������
void DcGp::DcGpDEM::CalculateEarthVolumn(double height, double& excavation, double& fill)
{
	double area = m_pDcGpDEMImpl->m_xInterval * m_pDcGpDEMImpl->m_yInterval;

	for (auto i = m_pDcGpDEMImpl->m_links.cbegin(); i != m_pDcGpDEMImpl->m_links.cend(); ++i)
	{
		double gridH = (m_pDcGpDEMImpl->m_vertex[(*i)[0]][2] +
			m_pDcGpDEMImpl->m_vertex[(*i)[1]][2] +
			m_pDcGpDEMImpl->m_vertex[(*i)[2]][2] +
			m_pDcGpDEMImpl->m_vertex[(*i)[3]][2]) / 4 - GetShift()[2];

		//�������̸߳�����ƽ�̣߳�������ڷ���
		if (gridH >= height)
		{
			excavation += (gridH - height) * area;
		}
		else
		{
			fill += (height - gridH) * area;
		}
	}
}

//�Ƿ�ӵ��������
bool DcGp::DcGpDEM::HasNormals() const
{
	return !m_pDcGpDEMImpl->m_Normals.empty();
}

//�Ƿ�ӵ�б�����
bool DcGp::DcGpDEM::HasDisplayedScalarField() const
{
	return (m_pDcGpDEMImpl->m_currentScalarField && !m_pDcGpDEMImpl->m_scalarFields.empty()) || m_pDcGpDEMImpl->m_heightScalarField;
}

void DcGp::DcGpDEM::ShowScalarField(bool state)
{
	DcGpEntity::ShowScalarField(state);
}

//���ĵ�ǰ����������ɫ��
void DcGp::DcGpDEM::ChangeScalarFieldColorScale(QString name)
{
	if (!m_pDcGpDEMImpl->m_currentScalarField)
	{
		return;
	}

	m_pDcGpDEMImpl->m_currentScalarField->ChangeColorScale(name);
	m_pDcGpDEMImpl->m_currentScalarField->Prepare();
	m_pDcGpDEMImpl->m_scalarColors = m_pDcGpDEMImpl->m_currentScalarField->GetColors();
}

//���õ�ǰ������
void DcGp::DcGpDEM::SetCurrentScalarField(unsigned index, unsigned setps, ScalarFieldType type)
{
	switch (type)
	{
	case eScalarField:
		if (index >= 0 && index < ScalarFieldCount())
		{
			m_pDcGpDEMImpl->m_currentScalarField = m_pDcGpDEMImpl->m_scalarFields[index];
		}
		break;
	case eHeightField:
		if (!m_pDcGpDEMImpl->m_heightScalarField)
		{
			return;
		}
		//���ɸ̱߳�����
		for (unsigned i = 0; i != m_pDcGpDEMImpl->m_vertex.size(); ++i)
		{
			m_pDcGpDEMImpl->m_heightScalarField->AddData(m_pDcGpDEMImpl->m_vertex[i].z - GetShift().z);
		}
		m_pDcGpDEMImpl->m_currentScalarField = m_pDcGpDEMImpl->m_heightScalarField;
		break;
	case eDistanceField:
		if (!m_pDcGpDEMImpl->m_distanceScalarField)
		{
			return;
		}
		//���ɾ��������
		for (unsigned i = 0; i != m_pDcGpDEMImpl->m_vertex.size(); ++i)
		{
			m_pDcGpDEMImpl->m_distanceScalarField->
				AddData(sqrt(m_pDcGpDEMImpl->m_vertex[i].x * m_pDcGpDEMImpl->m_vertex[i].x +
				m_pDcGpDEMImpl->m_vertex[i].y * m_pDcGpDEMImpl->m_vertex[i].y + 
				m_pDcGpDEMImpl->m_vertex[i].z * m_pDcGpDEMImpl->m_vertex[i].z));
		}
		m_pDcGpDEMImpl->m_currentScalarField = m_pDcGpDEMImpl->m_distanceScalarField;
		break;
	default:
		break;
	}
	m_pDcGpDEMImpl->m_currentScalarField->SetColorSteps(setps);
	m_pDcGpDEMImpl->m_currentScalarField->Prepare();
	m_pDcGpDEMImpl->m_scalarColors = m_pDcGpDEMImpl->m_currentScalarField->GetColors();
}

void DcGp::DcGpDEM::SetNormals(std::vector<DCVector3D> normals)
{
	m_pDcGpDEMImpl->m_Normals = normals;
}

std::vector<Normal> DcGp::DcGpDEM::GetNormals()
{
	return m_pDcGpDEMImpl->m_Normals;
}

std::vector<PointCoord> DcGp::DcGpDEM::GetVertexs()
{
	return m_pDcGpDEMImpl->m_vertex;
}

std::vector<std::array<unsigned, 4>> DcGp::DcGpDEM::GetLinks()
{
	return m_pDcGpDEMImpl->m_links;
}

DcGp::DcGpBoundBox DcGp::DcGpDEM::GetMyOwnBoundBox()
{
	DcGpBoundBox emptyBox(GetMinPoint(), 
		GetMaxPoint());

	return emptyBox;
}

void DcGp::DcGpDEM::AddColorBarInfo(DcGpDrawContext& context)
{
	if (!m_pDcGpDEMImpl->m_currentScalarField)
	{
		return;
	}

	context.sfColorScaleToDisplay = static_cast<DcGp::DcGpScalarField*>(m_pDcGpDEMImpl->m_currentScalarField.get());
}

//��ȡ����
PointCoord DcGp::DcGpDEM::GetMaxPoint() const
{
	m_pDcGpDEMImpl->CalculateMyOwnBoundBox();

	PointCoord maxPoint(m_pDcGpDEMImpl->m_maxPoint.x, 
		m_pDcGpDEMImpl->m_maxPoint.y, 
		m_pDcGpDEMImpl->m_maxPoint.z);
	return maxPoint;
}

//��ȡ��С��
PointCoord DcGp::DcGpDEM::GetMinPoint() const
{
	m_pDcGpDEMImpl->CalculateMyOwnBoundBox();

	PointCoord minPoint(m_pDcGpDEMImpl->m_minPoint.x, 
		m_pDcGpDEMImpl->m_minPoint.y, 
		m_pDcGpDEMImpl->m_minPoint.z);
	return minPoint;
}

void DcGp::DcGpDEM::AddPoint(DCVector3D vector)
{
	m_pDcGpDEMImpl->m_vertex.push_back(vector);
}

void DcGp::DcGpDEM::AddPoint(DCVector3D vector, unsigned index)
{
	if (index < PSize() && index >= 0)
	{
		m_pDcGpDEMImpl->m_vertex[index] = vector;
	}
}

void DcGp::DcGpDEM::Resize(unsigned size)
{
	m_pDcGpDEMImpl->m_vertex.resize(size);
}

void DcGp::DcGpDEM::AddLink(std::array<unsigned, 4> link)
{
	m_pDcGpDEMImpl->m_links.push_back(link);
}

std::array<unsigned, 4> DcGp::DcGpDEM::GetLink(unsigned index)
{
	return m_pDcGpDEMImpl->m_links.at(index);
}

unsigned DcGp::DcGpDEM::PSize() const
{
	return m_pDcGpDEMImpl->m_vertex.size();
}

unsigned DcGp::DcGpDEM::LinkSize()
{
	return m_pDcGpDEMImpl->m_links.size();
}

PointCoord* DcGp::DcGpDEM::GetVertex(unsigned index) const
{
	if (index >= PSize())
	{
		index = PSize() - 1;
	}

	if (index < 0)
	{
		index = 0;
	}

	return &(m_pDcGpDEMImpl->m_vertex.at(index));
}

PointCoord DcGp::DcGpDEM::GetVertex(unsigned index)
{
	if (index >= PSize())
	{
		index = PSize() - 1;
	}

	if (index < 0)
	{
		index = 0;
	}

	return m_pDcGpDEMImpl->m_vertex.at(index);
}

void DcGp::DcGpDEM::InvalidBoundBox()
{
	m_pDcGpDEMImpl->m_borderValid = false;
}

//��ȡָ���������ɫ
PointColor DcGp::DcGpDEM::GetPointColor(unsigned index) const
{
	if (index >= PSize())
	{
		index = PSize() - 1;
	}

	if (index < 0)
	{
		index = 0;
	}

	return m_pDcGpDEMImpl->m_rgbColors[index];
}

void DcGp::DcGpDEM::AddColor(PointColor color, unsigned index)
{
	if (m_pDcGpDEMImpl->m_rgbColors.empty() && !m_pDcGpDEMImpl->m_vertex.empty())
	{
		m_pDcGpDEMImpl->m_rgbColors.resize(m_pDcGpDEMImpl->m_vertex.size());
	}

	if (index >= 0 && index < m_pDcGpDEMImpl->m_vertex.size())
	{
		m_pDcGpDEMImpl->m_rgbColors[index] = color;
	}
}

//��ӱ�����
void DcGp::DcGpDEM::AddScalarField(DcGpScalarFieldPtr scalarField)
{
	m_pDcGpDEMImpl->m_scalarFields.push_back(scalarField);
}

DcGp::DcGpScalarFieldPtr DcGp::DcGpDEM::GetScalarFieldByIndex(int index)
{
	assert(index >= 0);
	return m_pDcGpDEMImpl->m_scalarFields[index];
}

int DcGp::DcGpDEM::GetScalarFieldIndexByName(QString name)
{
	size_t sfCount = ScalarFieldCount();
	for (size_t i=0; i<sfCount; ++i)
	{
		//we don't accept two SF with the same name!
		if (ScalarFieldNames()[i] == name)
			return static_cast<int>(i);
	}

	return -1;
}

unsigned DcGp::DcGpDEM::ScalarFieldCount() const
{
	return m_pDcGpDEMImpl->m_scalarFields.size();
}

QStringList DcGp::DcGpDEM::ScalarFieldNames() const
{
	QStringList names;
	unsigned number = m_pDcGpDEMImpl->m_scalarFields.size();

	for (unsigned i = 0; i < number; ++i)
	{
		names.push_back(m_pDcGpDEMImpl->m_scalarFields.at(i)->GetName());
	}
	return names;
}

DcGp::DcGpScalarFieldPtr DcGp::DcGpDEM::GetCurrentScalarField()
{
	return m_pDcGpDEMImpl->m_currentScalarField;
}

QMap<QString, double> DcGp::DcGpDEM::GetParameter()
{
	QMap<QString, double> header;
	header["x"] = m_pDcGpDEMImpl->m_startPoint.x;
	header["y"] = m_pDcGpDEMImpl->m_startPoint.y;
	header["z"] = m_pDcGpDEMImpl->m_startPoint.z;
	header["xInterval"] = m_pDcGpDEMImpl->m_xInterval;
	header["yInterval"] = m_pDcGpDEMImpl->m_yInterval;
	header["xNumber"] = m_pDcGpDEMImpl->m_xNumber;
	header["yNumber"] = m_pDcGpDEMImpl->m_yNumber;

	return header;
}

void DcGp::DcGpDEM::UpdateParameter(Point_3D startPoint, GridCount_Type xNumber, GridCount_Type yNumber, GridSize_Type xInterval, GridSize_Type yInterval)
{
	m_pDcGpDEMImpl->m_startPoint = startPoint;
	m_pDcGpDEMImpl->m_xNumber = xNumber;
	m_pDcGpDEMImpl->m_yNumber = yNumber;
	m_pDcGpDEMImpl->m_xInterval = xInterval;
	m_pDcGpDEMImpl->m_yInterval = yInterval;
}

bool DcGp::DcGpDEM::ReserveTheRGBTable()
{
	if(m_pDcGpDEMImpl->m_vertex.empty())
	{
		return false;
	}

	if (m_pDcGpDEMImpl->m_rgbColors.empty())
	{
		m_pDcGpDEMImpl->m_rgbColors.resize(m_pDcGpDEMImpl->m_vertex.size());

		//��ʼ��ֵ
		PointColor default = {255,255,255};
		std::fill(m_pDcGpDEMImpl->m_rgbColors.begin(), m_pDcGpDEMImpl->m_rgbColors.end(), default);
	}

	return true;
}

PointColor& DcGp::DcGpDEM::GetColor(const unsigned index)
{
	assert(index < m_pDcGpDEMImpl->m_vertex.size());
	assert(index >= 0);
	assert(m_pDcGpDEMImpl->m_rgbColors.size() == m_pDcGpDEMImpl->m_vertex.size());
	

	return m_pDcGpDEMImpl->m_rgbColors[index];
}

void DcGp::DcGpDEM::SetImage(QImage& image, const QString& tfwFile)
{
	if (image.isNull())
		return;

	//����tfw�ļ������ȡ����
	bool hasTfw = false;
	TFWParas paras;
	if (!tfwFile.isEmpty())
	{
		paras = GetParasFromTfw(tfwFile);
		hasTfw = true;
	}

	if(!glIsTexture(m_pDcGpDEMImpl->m_targetTex))
	{
		//��������
		BindTargetTexture(image);

		//Ϊÿ������������������
		Point_3D shift = GetShift();
		for (unsigned i = 0; i < PSize(); ++i)
		{
			Point_3D point = Point_3D::FromArray(GetVertex(i).u);
			point -= shift;

			//��ѯ��ǰpoint��Ӧ��image�е��������к�
			int row = 0, column = 0;
			
			if (!hasTfw)
			{
				GetPixelPosition(Point_3D::FromArray(GetMinPoint().u)-shift, Point_3D::FromArray(GetMaxPoint().u)-shift, image.height(), image.width(), point, row, column);
			}
			else
			{
				GetPixelPositionByTFW(paras, point, row, column);
			}

			////����image��size��һ��row��column���õ���������
			DCVector2D texCoord;
			NormalizePixelPos(image.height(), image.width(), row, column, texCoord);

			m_pDcGpDEMImpl->m_texCoords.push_back(texCoord);
		}
	}
}

void DcGp::DcGpDEM::BindTargetTexture(QImage& image)
{
	if (image.isNull())
		return;
	if (m_pDcGpDEMImpl->m_targetTex) 
	{
		glDeleteTextures(1, &m_pDcGpDEMImpl->m_targetTex);
		m_pDcGpDEMImpl->m_targetTex = 0;
	}

	// create texture
	glGenTextures(1, &m_pDcGpDEMImpl->m_targetTex);
	QImage tximg = QGLWidget::convertToGLFormat(image);
	glBindTexture(GL_TEXTURE_2D, m_pDcGpDEMImpl->m_targetTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glTexImage2D(GL_TEXTURE_2D, 0, 3, tximg.width(), tximg.height(),
		0, GL_RGBA, GL_UNSIGNED_BYTE, tximg.bits());

	//glBindTexture(GL_TEXTURE_2D, 0);
} 

void DcGp::DcGpDEM::AddTexCoord(DCVector2D texCoord)
{
	m_pDcGpDEMImpl->m_texCoords.push_back(texCoord);
}

bool DcGp::DcGpDEM::HasTextures()
{
	return m_pDcGpDEMImpl->m_targetTex && !m_pDcGpDEMImpl->m_texCoords.empty();
}

void DcGp::DcGpDEM::CancelTexture()
{
	if (m_pDcGpDEMImpl->m_targetTex)
	{
		glDeleteTextures(1, &m_pDcGpDEMImpl->m_targetTex);
	}
	m_pDcGpDEMImpl->m_targetTex = 0;
	m_pDcGpDEMImpl->m_texCoords.clear();
}