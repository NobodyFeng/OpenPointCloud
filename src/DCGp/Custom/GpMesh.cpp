#include "GpMesh.h"
#include <algorithm>

//Qt
#include "QtWidgets/QTableWidget"
#include "QtWidgets/QComboBox"
#include "QtWidgets/QPushButton"
#include "QFileInfo"
#include "QTextStream"

//IL
#include <IL/il.h>

//DcLib
#include "DCCore/Logger.h"

#include "DCCore/DCGL.h"
#include "Impl/NormalVectors.hxx"

#include "DCGp/Common.h"
#include "DCGp/GpDrawContext.h"
#include "DCGp/Custom/GpMaterial.h"
#include "Impl/GpMeshImpl.hxx"

DcGp::DcGpMesh::DcGpMesh()
	: DcGpEntity()
{
	m_pDcGpMeshImpl = DcGpMeshImpl::CreateObject();
}

DcGp::DcGpMesh::DcGpMesh(QString name)
	:DcGpEntity(name)
{
	m_pDcGpMeshImpl = DcGpMeshImpl::CreateObject();
}

DcGp::DcGpMesh::DcGpMesh(QString name, std::vector<DCVector3D> vertices)
	:DcGpEntity(name)
{
	m_pDcGpMeshImpl = DcGpMeshImpl::CreateObject();
	m_pDcGpMeshImpl->m_points = vertices;
}

DcGp::DcGpMesh::~DcGpMesh()
{

}

//���������
void DcGp::DcGpMesh::AddTriangle(unsigned index1, unsigned index2, unsigned index3)
{
	Link polygon = {index1, index2, index3};

	AddTriangle(polygon);
}

//����meshͼ��
void DcGp::DcGpMesh::DrawMyselfOnly(DcGpDrawContext& context)
{
	//handleColorRamp(context);

	//�ж��Ƿ��ǻ�����ά����
	if (MACRO_Draw3D(context))
	{
		//�Ƿ���ڶ����
		unsigned n, gonNum = m_pDcGpMeshImpl->m_links.size();
		if (gonNum == 0)
		{
			return;
		}

		//�ж��Ƿ���Ҫ��ʼ�������������
		if (!HasTextures() && !m_pDcGpMeshImpl->m_teximgFileName.isEmpty())
		{
			//CreateTexture();
			m_pDcGpMeshImpl->LoadGLTextureByDevil();
		}

		//�ж��Ƿ���ʱ���ص�ǰ����,ֻ���Ƶ�
		bool lodEnabled = (gonNum > DCCore::MAX_FACE_NUM && MACRO_LODActivated(context));

		//��ȡ��ʾ����
		glDrawParameters glParams;
		GetDrawingParameters(glParams);
		glParams.showNormals &= bool(MACRO_LightIsEnabled(context));

		//�Ƿ���ʾ����
		glParams.showNormals = (HasNormals() && NormalsShown());

		//�Ƿ�������ʾ
		bool showWired = (m_pDcGpMeshImpl->m_showStyle == eShowDefault);

		//����ɫ������ɫ
		bool colorMaterialEnabled = false;
		if (glParams.showScalarField || glParams.showColors)
		{
			glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
			glEnable(GL_COLOR_MATERIAL);
			colorMaterialEnabled = true;
		}

		//GL name pushing
		bool pushName = MACRO_DrawEntityNames(context);
		//��׽��Ҫ
		bool pushTriNames = MACRO_DrawTriangleNames(context);
		pushName |= pushTriNames;

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

		//ѡ����ɫ
		//���ƴ�ɫ
		if (glParams.showColors && IsColorOverriden())
		{
			glColor3ub(GetTempColor()[0], GetTempColor()[1], GetTempColor()[2]);
			//glParams.showColors = false;
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
			/*if (m_pDcGpMeshImpl->m_material)
			{
				m_pDcGpMeshImpl->m_material->ApplyGL(true,colorMaterialEnabled);
			}*/
			context.defaultMat.ApplyGL(true,colorMaterialEnabled);
		}

		//ѹ��������
		NormalVectors* compressedNormals = 0;
		if (glParams.showNormals)
		{
			compressedNormals = NormalVectors::GetUniqueInstance();
		}

		unsigned vertCount = m_pDcGpMeshImpl->m_points.size();
		unsigned faceCount = m_pDcGpMeshImpl->m_links.size();
		if (glParams.showNormals && !m_pDcGpMeshImpl->m_comPressNormal.empty())
		{
			//���·�������ֵ
			for (unsigned i = 0; i < vertCount; ++i)
			{
				//��ȡÿ�������������������
				//Link index = GetLink(i);
				m_pDcGpMeshImpl->m_Normals[i] = compressedNormals->GetNormal(m_pDcGpMeshImpl->m_comPressNormal[i]) * (-1);
			}
		}
		
		if (pushTriNames)
		{
			if (m_pDcGpMeshImpl->m_visibilityTable)
			{
				
			}
			else
			{
				//��ʼ����ǰ����
				//����ɫ
				const ColorType *col1=0,*col2=0,*col3=0;
				//������ֵ
				const PointCoordinateType *N1=0,*N2=0,*N3=0;
				//��������
				const float *Tx1=0,*Tx2=0,*Tx3=0;

				if (m_pDcGpMeshImpl->m_showStyle == eShowGrid)
				{
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				}

				if (pushTriNames)
					glPushName(0);

				glBegin(GL_TRIANGLES);
				for (unsigned i = 0; i < gonNum; ++i)
				{
					//��ȡ��ǰ������������
					Link index = m_pDcGpMeshImpl->m_links[i];

					if (glParams.showScalarField)
					{
						col1 = &(m_pDcGpMeshImpl->m_scalarColors[index[0]][0]);
						col2 = &(m_pDcGpMeshImpl->m_scalarColors[index[1]][0]);
						col3 = &(m_pDcGpMeshImpl->m_scalarColors[index[2]][0]);
					}
					else if (glParams.showNormals)
					{
						N1 = &(m_pDcGpMeshImpl->m_Normals[index[0]][0]);
						N2 = &(m_pDcGpMeshImpl->m_Normals[index[1]][0]);
						N3 = &(m_pDcGpMeshImpl->m_Normals[index[2]][0]);
					}
					else if (glParams.showColors && IsColorOverriden())
					{
						col1 = &GetTempColor()[0];
						col2 = &GetTempColor()[1];
						col3 = &GetTempColor()[2];
					}
					else
					{
						glColor3fv(context.defaultMat.GetDiffuseFront());
					}
					if (pushTriNames)
					{
						glEnd();
						glLoadName(i);
						glBegin(GL_TRIANGLES);
					}
					//�����жϵ�ǰ�������Ƿ�ѡ����(����һ��ѡ�к�����ѡ��)
					if (GetMeshSelected(i))
					{

						//����1
						glColor3ub(255,0,0);
						glVertex3f(GetPoint(index[0]).x, GetPoint(index[0]).y, GetPoint(index[0]).z);

						//����2
						glColor3ub(255,0,0);
						glVertex3f(GetPoint(index[1]).x, GetPoint(index[1]).y, GetPoint(index[1]).z);

						//����3
						glColor3ub(255,0,0);
						glVertex3f(GetPoint(index[2]).x, GetPoint(index[2]).y, GetPoint(index[2]).z);
					}
					else //����ʵ����ɫ����
					{
						//ѡ����ɫ
						//����1
						if (col1) glColor3ubv(col1);
						if (N1) glNormal3fv(N1);
						glVertex3f(GetPoint(index[0]).x, GetPoint(index[0]).y, GetPoint(index[0]).z);

						//����2
						if (col2) glColor3ubv(col2);
						if (N1)glNormal3fv(N2);
						glVertex3f(GetPoint(index[1]).x, GetPoint(index[1]).y, GetPoint(index[1]).z);

						//����3
						if (col3) glColor3ubv(col3);
						if (N1)glNormal3fv(N3);
						glVertex3f(GetPoint(index[2]).x, GetPoint(index[2]).y, GetPoint(index[2]).z);
					}
				}
				glEnd();

				if (pushTriNames)
					glPopName();
				if (m_pDcGpMeshImpl->m_showStyle == eShowGrid)
				{
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				}
			}
		}
		else
		{
			if (m_pDcGpMeshImpl->m_visibilityTable || (IsSelected() && HasSelected()))
			{
				//��ʼ����ǰ����
				//����ɫ
				const std::array<ColorType, 3> *col1=0,*col2=0,*col3=0;
				//������ֵ
				std::array<PointCoordinateType, 3> *N1=0,*N2=0,*N3=0;
				//��������
				const float *Tx1=0,*Tx2=0,*Tx3=0;

				if (m_pDcGpMeshImpl->m_showStyle == eShowGrid)
				{
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				}

				if (pushTriNames)
					glPushName(0);

				glBegin(GL_TRIANGLES);
				for (unsigned i = 0; i < gonNum; ++i)
				{
					//��ȡ��ǰ������������
					Link index = m_pDcGpMeshImpl->m_links[i];

					if (glParams.showNormals)
					{
						std::array<PointCoordinateType, 3> tempN1 = {m_pDcGpMeshImpl->m_Normals[index[0]][0],m_pDcGpMeshImpl->m_Normals[index[0]][1],m_pDcGpMeshImpl->m_Normals[index[0]][2]};
						N1 = &tempN1;
						std::array<PointCoordinateType, 3> tempN2 = {m_pDcGpMeshImpl->m_Normals[index[1]][0],m_pDcGpMeshImpl->m_Normals[index[1]][1],m_pDcGpMeshImpl->m_Normals[index[1]][2]};
						N2 = &tempN2;
						std::array<PointCoordinateType, 3> tempN3 = {m_pDcGpMeshImpl->m_Normals[index[2]][0],m_pDcGpMeshImpl->m_Normals[index[2]][1],m_pDcGpMeshImpl->m_Normals[index[2]][2]};
						N3 = &tempN3;
					}
					if (glParams.showColors && IsColorOverriden())
					{
						col1 = &GetTempColor();
						col2 = &GetTempColor();
						col3 = &GetTempColor();
					}
					else if (glParams.showScalarField)
					{
						col1 = &m_pDcGpMeshImpl->m_scalarColors[index[0]];
						col2 = &m_pDcGpMeshImpl->m_scalarColors[index[1]];
						col3 = &m_pDcGpMeshImpl->m_scalarColors[index[2]];
					}
					else
					{
						glColor3fv(context.defaultMat.GetDiffuseFront());
					}
					if (pushTriNames)
					{
						glEnd();
						glLoadName(i);
						glBegin(GL_TRIANGLES);
					}
					//�����жϵ�ǰ�������Ƿ�ѡ����(����һ��ѡ�к�����ѡ��)
					if (GetMeshSelected(i))
					{

						if (N1) glNormal3f((*N1)[0],(*N1)[1], (*N1)[2]);
						//����1
						glColor3ub(255,0,0);
						glVertex3f(GetPoint(index[0]).x, GetPoint(index[0]).y, GetPoint(index[0]).z);

						if (N2) glNormal3f((*N2)[0],(*N2)[1], (*N2)[2]);
						//����2
						glColor3ub(255,0,0);
						glVertex3f(GetPoint(index[1]).x, GetPoint(index[1]).y, GetPoint(index[1]).z);

						if (N3) glNormal3f((*N3)[0],(*N3)[1], (*N3)[2]);
						//����3
						glColor3ub(255,0,0);
						glVertex3f(GetPoint(index[2]).x, GetPoint(index[2]).y, GetPoint(index[2]).z);
					}
					else //����ʵ����ɫ����
					{
						//ѡ����ɫ
						//����1
						if (col1) glColor3ub((*col1)[0], (*col1)[1], (*col1)[2]);
						if (N1) glNormal3f((*N1)[0],(*N1)[1], (*N1)[2]);
						glVertex3f(GetPoint(index[0]).x, GetPoint(index[0]).y, GetPoint(index[0]).z);

						//����2
						if (col2) glColor3ub((*col2)[0], (*col2)[1], (*col2)[2]);
						if (N2) glNormal3f((*N2)[0],(*N2)[1], (*N2)[2]);
						glVertex3f(GetPoint(index[1]).x, GetPoint(index[1]).y, GetPoint(index[1]).z);

						//����3
						if (col3) glColor3ub((*col3)[0], (*col3)[1], (*col3)[2]);
						if (N3) glNormal3f((*N3)[0],(*N3)[1], (*N3)[2]);
						glVertex3f(GetPoint(index[2]).x, GetPoint(index[2]).y, GetPoint(index[2]).z);
					}
				}
				glEnd();

				if (pushTriNames)
					glPopName();
				if (m_pDcGpMeshImpl->m_showStyle == eShowGrid)
				{
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				}
			}
			else 
			{
				//������������
				glEnableClientState(GL_VERTEX_ARRAY);
				//glColor3b(255, 0, 0);
				glVertexPointer(3, GL_FLOAT, 0, &(m_pDcGpMeshImpl->m_points[0][0]));
				//glVertexPointer(3, GL_FLOAT, 0, &(points[0][0]));

				//�Ƿ�����������ʾ
				if (glParams.showNormals)
				{
					glEnableClientState(GL_NORMAL_ARRAY);
					glNormalPointer(GL_FLOAT, 0, &(m_pDcGpMeshImpl->m_Normals[0][0]));
				}
				//�Ƿ�������ɫ����
				if (glParams.showScalarField)
				{
					glEnableClientState(GL_COLOR_ARRAY);
					glColorPointer(3, GL_UNSIGNED_BYTE,
						3 * sizeof(ColorType), &(m_pDcGpMeshImpl->m_scalarColors[0][0]));
				}

				//�Ƿ�������������
				if (HasTextures())
				{
					if (!m_pDcGpMeshImpl->m_texCoords.empty())
					{
						//glEnable(GL_LIGHTING);
						glEnable(GL_TEXTURE_2D);
						glBindTexture(GL_TEXTURE_2D, m_pDcGpMeshImpl->m_targetTex);
						glEnableClientState(GL_TEXTURE_COORD_ARRAY);
						glTexCoordPointer(3, GL_FLOAT,
							2 * sizeof(GL_FLOAT), &(m_pDcGpMeshImpl->m_texCoords[0][0]));
					}
					
				}

				//glFrontFace(GL_CW); //����˳��ʱ��
				if (lodEnabled)
				{
					//ֻ���Ƶ�
					glDrawArrays(GL_POINTS, 0, PSize());
				}
				else
				{
					//�ж���ʾģʽ
					if(m_pDcGpMeshImpl->m_showStyle == eShowGrid)
					{
						glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
						//��������,ѭ�������ٶȸ���
						glDrawElements(GL_TRIANGLES, 3 * m_pDcGpMeshImpl->m_links.size(), 
							GL_UNSIGNED_INT, &(m_pDcGpMeshImpl->m_links[0][0]));
						glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					}
					else if (m_pDcGpMeshImpl->m_showStyle == eShowFill)
					{
						//�������,ѭ�������ٶȸ���
						glDrawElements(GL_TRIANGLES, 3 * m_pDcGpMeshImpl->m_links.size(), 
							GL_UNSIGNED_INT, &(m_pDcGpMeshImpl->m_links[0][0]));
						//glDrawArrays(GL_TRIANGLES, 0, faceCount * 3);
					}
					else if (m_pDcGpMeshImpl->m_showStyle == eShowDefault)
					{
						glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT );
						//���Ƶ�һ�����
						glEnable(GL_POLYGON_OFFSET_FILL);
						glPolygonOffset(1.0, 1);
						glDrawElements(GL_TRIANGLES, 3 * m_pDcGpMeshImpl->m_links.size(), 
							GL_UNSIGNED_INT, &(m_pDcGpMeshImpl->m_links[0][0]));
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
						glPolygonMode(GL_FRONT_AND_BACK ,GL_LINE);
						//glPolygonMode(GL_BACK, GL_LINE);
						glDrawElements(GL_TRIANGLES, 3 * m_pDcGpMeshImpl->m_links.size(), 
							GL_UNSIGNED_INT, &(m_pDcGpMeshImpl->m_links[0][0]));
						glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
						glPopAttrib();
					}

				}
			}
		}
		//�رն�������,�������飬��ɫ����
		glDisableClientState(GL_VERTEX_ARRAY);
		if (glParams.showNormals)
			glDisableClientState(GL_NORMAL_ARRAY);
		if (glParams.showScalarField || glParams.showColors)
			glDisableClientState(GL_COLOR_ARRAY);

		if (HasTextures())
		{
			//glDisable(GL_LIGHTING);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			glDisable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		//�رղ���ģʽ
		if (colorMaterialEnabled)
			glDisable(GL_COLOR_MATERIAL);
		//�رյƹ�
		if (glParams.showNormals)
		{
			glDisable(GL_LIGHTING);
			glDisable((QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_Version_1_2 ? GL_RESCALE_NORMAL : GL_NORMALIZE));
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

DcGp::DcGpBoundBox DcGp::DcGpMesh::GetDisplayBoundBox()
{
	return GetMyOwnBoundBox();
}

//
DcGp::DcGpBoundBox DcGp::DcGpMesh::GetMyOwnBoundBox()
{
	DcGpBoundBox emptyBox(GetMinPoint(), 
		                  GetMaxPoint());

	return emptyBox;
}

//��ʾ������Ϣ
void DcGp::DcGpMesh::ShowProperties(QTableWidget* widget)
{
	QTableWidgetItem* tblmItem;
	//��������
	widget->setRowCount(5);
	//��������
	tblmItem = new QTableWidgetItem(tr("Name"));
	widget->setItem(0, 0, tblmItem);
	tblmItem = new QTableWidgetItem(GetName());
	widget->setItem(0, 1, tblmItem);
	//��������
	tblmItem = new QTableWidgetItem(tr("MeshNumber"));
	widget->setItem(1, 0, tblmItem);
	tblmItem = new QTableWidgetItem(QString::number(m_pDcGpMeshImpl->m_links.size()));
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

std::vector<PointCoord> DcGp::DcGpMesh::GetVertexs()
{
	return m_pDcGpMeshImpl->m_points;
}

void DcGp::DcGpMesh::AddPoint(DCVector3D point)
{
	m_pDcGpMeshImpl->m_points.push_back(point);
	//m_pDcGpMeshImpl->m_pointsSelected.push_back(false);
}

void DcGp::DcGpMesh::AddNormal(DCVector3D normal)
{
	m_pDcGpMeshImpl->m_Normals.push_back(normal);
}

void DcGp::DcGpMesh::AddVexColor(PointColor color)
{
	m_pDcGpMeshImpl->m_rgbColors.push_back(color);
}

//�ı���ʾ��ʽ
void DcGp::DcGpMesh::ChangeShowStyle(int style)
{
	m_pDcGpMeshImpl->m_showStyle = static_cast<ShowStyle>(style);
	GetAssociatedWindow()->Redraw();
}

//�Ƿ�ӵ��������
bool DcGp::DcGpMesh::HasNormals() const
{
	return !m_pDcGpMeshImpl->m_Normals.empty();
}

//�Ƿ�ӵ�б�����
bool DcGp::DcGpMesh::HasScalarFields() const
{
	return (!m_pDcGpMeshImpl->m_scalarFields.empty());
}

void DcGp::DcGpMesh::ShowScalarField(bool state)
{
	DcGpEntity::ShowScalarField(state);
}

//���ĵ�ǰ����������ɫ��
void DcGp::DcGpMesh::ChangeScalarFieldColorScale(QString name)
{
	if (!m_pDcGpMeshImpl->m_currentScalarField)
	{
		return;
	}

	m_pDcGpMeshImpl->m_currentScalarField->ChangeColorScale(name);
	m_pDcGpMeshImpl->m_currentScalarField->Prepare();
	m_pDcGpMeshImpl->m_scalarColors = m_pDcGpMeshImpl->m_currentScalarField->GetColors();
}

//���õ�ǰ������
void DcGp::DcGpMesh::SetCurrentScalarField(unsigned index, unsigned setps, ScalarFieldType type)
{
	if (index >= 0 && index < ScalarFieldCount())
	{
		m_pDcGpMeshImpl->m_currentScalarField = m_pDcGpMeshImpl->m_scalarFields[index];
	}
	else
	{
		return;
	}

	//���¸̺߳;����������ֵ
	m_pDcGpMeshImpl->UpdateScalarFieldData();
	
	m_pDcGpMeshImpl->m_currentScalarField->SetColorSteps(setps);
	m_pDcGpMeshImpl->m_currentScalarField->Prepare();
	m_pDcGpMeshImpl->m_scalarColors = m_pDcGpMeshImpl->m_currentScalarField->GetColors();
}

//��ȡָ����
//������ڵ�ʱ�ſɵ���
DCVector3D DcGp::DcGpMesh::GetPoint(unsigned index) const
{
	assert(index < m_pDcGpMeshImpl->m_points.size() && index >= 0);

	if (index >= m_pDcGpMeshImpl->m_points.size())
	{
		index = m_pDcGpMeshImpl->m_points.size() - 1;
	}

	if (index < 0)
	{
		index = 0;
	}

	return m_pDcGpMeshImpl->m_points[index];
}

DcGp::LinkContainer DcGp::DcGpMesh::GetLinks()
{
	return m_pDcGpMeshImpl->m_links;
}

std::vector<Normal> DcGp::DcGpMesh::GetNormals()
{
	return m_pDcGpMeshImpl->m_Normals;
}

void DcGp::DcGpMesh::SetNormals(std::vector<DCVector3D> normals)
{
	m_pDcGpMeshImpl->m_Normals = normals;
}

void DcGp::DcGpMesh::SetPoints(std::vector<DCVector3D> points)
{
	m_pDcGpMeshImpl->m_points = points;

// 	m_pDcGpMeshImpl->m_pointsSelected.resize(points.size());
// 	std::fill(m_pDcGpMeshImpl->m_pointsSelected.begin(), m_pDcGpMeshImpl->m_pointsSelected.end(), false);
}

void DcGp::DcGpMesh::SetLinks(LinkContainer links)
{
	m_pDcGpMeshImpl->m_links = links;

	m_pDcGpMeshImpl->m_meshsSelected.resize(links.size());
	std::fill(m_pDcGpMeshImpl->m_meshsSelected.begin(), m_pDcGpMeshImpl->m_meshsSelected.end(), false);
}

//bool DcGp::DcGpMesh::BuildDelaunay(std::vector<DCVector2D> &point2ds)
//{
//	if (point2ds.empty())
//	{
//		return false;
//	}
//
//	//reset
//	m_pDcGpMeshImpl->m_numberOfTriangles = 0;
//	if (!m_pDcGpMeshImpl->m_links.empty())
//	{
//		m_pDcGpMeshImpl->m_links.clear();
//	}
//
//	//�����ⲿ��triangle
//	triangulateio in;
//	memset(&in, 0, sizeof(triangulateio));
//	in.numberofpoints = point2ds.size();
//	in.pointlist = (float*)(&point2ds[0]);
//
//	try
//	{
//		triangulate("zQNn", &in, &in, 0);
//	}
//	catch(...)
//	{
//		return false;
//	}
//
//	//�����������������Լ����ӹ�ϵ
//	m_pDcGpMeshImpl->m_numberOfTriangles = in.numberoftriangles;
//	if (m_pDcGpMeshImpl->m_numberOfTriangles > 0)
//	{
//		//���������С����
//		int minIndex = ((int*)in.trianglelist)[0];
//		int maxIndex = ((int*)in.trianglelist)[0];
//		for (unsigned i = 0;i<m_pDcGpMeshImpl->m_numberOfTriangles;++i)
//		{
//			if (minIndex > ((int*)in.trianglelist)[i])
//				minIndex = ((int*)in.trianglelist)[i];
//			else if (maxIndex < ((int*)in.trianglelist)[i])
//				maxIndex = ((int*)in.trianglelist)[i];
//			//���links
//			AddTriangle(((int*)in.trianglelist)[3*i], ((int*)in.trianglelist)[3*i+1], ((int*)in.trianglelist)[3*i+2]);
//			//������˹�ϵ
//			AddTopology(((int*)in.neighborlist)[3*i], ((int*)in.neighborlist)[3*i+1], ((int*)in.neighborlist)[3*i+2]);
//		}
//	}
//	
//	return true;
//}

void DcGp::DcGpMesh::AddTopology(long neighbor1, long neighbor2, long neighbor3)
{
	std::array<long, 3> topo = {neighbor1, neighbor2, neighbor3};
	m_pDcGpMeshImpl->m_topologys.push_back(topo);
}

std::vector<std::array<long, 3>> DcGp::DcGpMesh::GetTopologys()
{
	return m_pDcGpMeshImpl->m_topologys;
}

void DcGp::DcGpMesh::AddColorBarInfo(DcGpDrawContext& context)
{
	if (!m_pDcGpMeshImpl->m_currentScalarField)
	{
		return;
	}

	context.sfColorScaleToDisplay = static_cast<DcGp::DcGpScalarField*>(m_pDcGpMeshImpl->m_currentScalarField.get());
}

bool DcGp::DcGpMesh::LoadFromFile(const QString& fileName, bool coordinatesShiftEnabled, double* coordinatesShift /* = 0 */)
{
	//������չ��
	QString extension = QFileInfo(fileName).suffix();

	bool status;
	//������չ���жϵ��ú��ֶ�ȡ��ʽ
	if (extension.toUpper() == "WRL") //����WRL��ʽ�ļ�
	{
		status = LoadWrl(fileName, coordinatesShiftEnabled, coordinatesShift);
	}
	return status;
}

//����WRL��ʽ�ļ�
bool DcGp::DcGpMesh::LoadWrl(const QString& fileName , bool coordinatesShiftEnabled /* = false  */, double* coordinatesShift /* = 0 */ )
{
	QFile file(fileName);
	if (!file.exists())
	{
		DCCore::Logger::Error(QObject::tr("File [%1] doesn\'t exist.").arg(fileName));
		return false;
	}

	//���ļ���
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		return false;
	}

	QTextStream stream(&file);
	//���ж�ȡ
	PointCoord point(0,0,0);
	PointCoord shift(0,0,0);
	bool finish = false;

	QString firstLine = stream.readLine();
	if (!firstLine.toUpper().startsWith("#VRML"))
	{
		DCCore::Logger::Warning(QObject::tr("WRL File Header is Valid?"));
		return false;
	}

	//������
	while(!stream.atEnd())
	{
		QString line = stream.readLine();

		if (line.toUpper() == "COORD COORDINATE {")
		{
			auto m = 4;
		}
		//�����洢������
		if (line.toUpper().startsWith("COORD") && line.toUpper() == "COORD COORDINATE {")
		{
		}
	}

	file.close();

	return true;
}

//��ȡ����
PointCoord DcGp::DcGpMesh::GetMaxPoint() const
{
	m_pDcGpMeshImpl->CalculateMyOwnBoundBox();

	PointCoord maxPoint(m_pDcGpMeshImpl->m_maxPoint.x, 
		m_pDcGpMeshImpl->m_maxPoint.y, 
		m_pDcGpMeshImpl->m_maxPoint.z);
	return maxPoint;
}

//��ȡ��С��
PointCoord DcGp::DcGpMesh::GetMinPoint() const
{
	m_pDcGpMeshImpl->CalculateMyOwnBoundBox();

	PointCoord minPoint(m_pDcGpMeshImpl->m_minPoint.x, 
		m_pDcGpMeshImpl->m_minPoint.y, 
		m_pDcGpMeshImpl->m_minPoint.z);
	return minPoint;
}

void DcGp::DcGpMesh::SetMinPoint(PointCoord pt)
{
	//ȡ pt��m_pDcGpMeshImpl->m_minPoint�е���С��
	m_pDcGpMeshImpl->m_minPoint[0] = std::min(m_pDcGpMeshImpl->m_minPoint[0], pt[0]);
	m_pDcGpMeshImpl->m_minPoint[1] = std::min(m_pDcGpMeshImpl->m_minPoint[1], pt[1]);
	m_pDcGpMeshImpl->m_minPoint[2] = std::min(m_pDcGpMeshImpl->m_minPoint[2], pt[2]);
}

void DcGp::DcGpMesh::SetMaxPoint(PointCoord pt)
{
	//ȡ pt��m_pDcGpMeshImpl->m_maxPoint�е�����
	m_pDcGpMeshImpl->m_maxPoint[0] = std::max(m_pDcGpMeshImpl->m_maxPoint[0], pt[0]);
	m_pDcGpMeshImpl->m_maxPoint[1] = std::max(m_pDcGpMeshImpl->m_maxPoint[1], pt[1]);
	m_pDcGpMeshImpl->m_maxPoint[2] = std::max(m_pDcGpMeshImpl->m_maxPoint[2], pt[2]);
}

DcGp::Link DcGp::DcGpMesh::GetLink(unsigned index)
{
	return m_pDcGpMeshImpl->m_links.at(index);
}

void DcGp::DcGpMesh::SetPointNormalIndex(unsigned pointIndex, NormalType norm)
{
	//assert(m_normals && pointIndex < m_normals->currentSize());
	if (m_pDcGpMeshImpl->m_comPressNormal.empty() && m_pDcGpMeshImpl->m_points.size() > 0)
	{
		m_pDcGpMeshImpl->m_comPressNormal.resize(m_pDcGpMeshImpl->m_points.size());
	}

	m_pDcGpMeshImpl->m_comPressNormal[pointIndex] = norm;
}
void DcGp::DcGpMesh::SetPointNormal(unsigned pointIndex, const DCVector3D& N)
{
	SetPointNormalIndex(pointIndex, NormalVectors::GetNormIndex(N));
}

void DcGp::DcGpMesh::SetTopologys(std::vector<std::array<long, 3>> topologys)
{
	m_pDcGpMeshImpl->m_topologys = topologys;
}

DcGp::LinkContainer& DcGp::DcGpMesh::GetLinks() const
{
	return m_pDcGpMeshImpl->m_links;
}

void DcGp::DcGpMesh::GetVertexs(std::vector<DCVector3D>& points)
{
	points = m_pDcGpMeshImpl->m_points;
}

//����ָ�����ѡ��״̬
// void DcGp::DcGpMesh::SetPointSelected(unsigned index, bool status)
// {
// 	m_pDcGpMeshImpl->m_pointsSelected[index] = status;
// 
// 	//����ѡȡ�����ɫ
// 	if (status)
// 	{
// 		m_pDcGpMeshImpl->m_selectedColors[index][0] = 255;
// 		m_pDcGpMeshImpl->m_selectedColors[index][1] = 0;
// 		m_pDcGpMeshImpl->m_selectedColors[index][2] = 0;
// 	}
// 	else
// 	{
// 		if (GetColorMode() == eFalseRangeColor ||
// 			GetColorMode() == eFalseHeightColor ||
// 			GetColorMode() == eFalseScalarColor)
// 		{
// 			m_pDcGpMeshImpl->m_selectedColors[index] = m_pDcGpMeshImpl->m_scalarColors[index];
// 		}
// 		else
// 		{
// 			m_pDcGpMeshImpl->m_selectedColors[index][0] = GetTempColor()[0];
// 			m_pDcGpMeshImpl->m_selectedColors[index][1] = GetTempColor()[1];
// 			m_pDcGpMeshImpl->m_selectedColors[index][2] = GetTempColor()[2];
// 		}
// 	}
// }

//��ȡָ�����ѡ��״̬
// bool DcGp::DcGpMesh::GetPointSelected(unsigned index) const
// {
// 	return m_pDcGpMeshImpl->m_pointsSelected[index];
// }

//����ѡȡ��ɫ
void DcGp::DcGpMesh::EnableSelectedColors()
{
	if (!m_pDcGpMeshImpl->m_selectedColors.empty())
	{
		return;
	}
	else
	{
		PointColor color = {GetTempColor()[0], GetTempColor()[1], GetTempColor()[2]};
		m_pDcGpMeshImpl->m_selectedColors.resize(GetVertexs().size());
		std::fill_n(m_pDcGpMeshImpl->m_selectedColors.begin(), GetVertexs().size(), color);
	}
}

//ͣ��ѡȡ��ɫ
void DcGp::DcGpMesh::DisableSelectedColors()
{
	m_pDcGpMeshImpl->m_selectedColors.clear();
}

unsigned DcGp::DcGpMesh::PSize()
{
	return m_pDcGpMeshImpl->m_points.size();
}

//ɾ����ѡ�Ĳ���
void DcGp::DcGpMesh::DeleteSelected()
{
	//�������ӹ�ϵ��������ɾ������
	unsigned triNum = m_pDcGpMeshImpl->m_links.size();

	//�ж��Ƿ����ѡ�е�
	bool hasSelect = HasSelected();
	if (!hasSelect)
	{
		return;
	}

	//����һ�����ӹ�ϵ
	LinkContainer links;

	for (unsigned i = 0; i < triNum; ++i)
	{
		//��ȡ��ǰ������������
		Link index = m_pDcGpMeshImpl->m_links[i];
		//�����жϵ�ǰ�������Ƿ�ѡ����(����һ��ѡ�к�����ѡ��)
		if (!GetMeshSelected(i))
		{
			links.push_back(index);
		}
	}

	//������ӹ�ϵ
	m_pDcGpMeshImpl->m_links.clear();
	m_pDcGpMeshImpl->m_links = links;

	//��յ�Ŀɼ���
	CancelSelected();
}

//ȡ��ѡȡ
void DcGp::DcGpMesh::CancelSelected()
{
	//ȡ��ÿ�����ѡȡ״̬
	std::fill_n(m_pDcGpMeshImpl->m_meshsSelected.begin(), LinkSize(), false);

	//���ÿ�����ѡȡ��ɫ
	if (GetColorMode() == eFalseRangeColor ||
		GetColorMode() == eFalseHeightColor ||
		GetColorMode() == eFalseScalarColor)
	{
		m_pDcGpMeshImpl->m_selectedColors = m_pDcGpMeshImpl->m_scalarColors;
	}
	else
	{
		PointColor color = {GetTempColor()[0], GetTempColor()[1], GetTempColor()[2]};
		std::fill_n(m_pDcGpMeshImpl->m_selectedColors.begin(), PSize(), color);
	}

	//������ʾ��
	m_pDcGpMeshImpl->m_visibilityTable = false;
}

double GetAngleDeg(DCVector3D& AB, DCVector3D& AC)
{
	AB.Normalize();
	AC.Normalize();
	double dotprod = AB.Dot(AC);
	if (dotprod<=-1.0)
		return 180.0;
	else if (dotprod>1.0)
		return 0.0;
	return 180.0*acos(dotprod)/M_PI;
}

void DcGp::DcGpMesh::Filter(FilterPara para)
{
	unsigned triNum = m_pDcGpMeshImpl->m_links.size();

	//����һ�����ӹ�ϵ��
	std::map<unsigned, Link> linkTable;

	for (unsigned i = 0; i < triNum; ++i)
	{
		linkTable[i] = GetLink(i);
	}

	//������ӹ�ϵ
	m_pDcGpMeshImpl->m_links.clear();

	//�����ӹ�ϵ����д���
	for (unsigned i = 0; i < triNum; ++i)
	{
		//��߹���
		//��ȡ������������
		DCVector3D point1 = GetPoint(linkTable[i].at(0));
		DCVector3D point2 = GetPoint(linkTable[i].at(1));
		DCVector3D point3 = GetPoint(linkTable[i].at(2));

		//������������12,13,23
		float dis1 = DCVector3D::VDistance(point1, point2);
		float dis2 = DCVector3D::VDistance(point1, point3);
		float dis3 = DCVector3D::VDistance(point2, point3);

		//���������ڽ�ֵ
		DCVector3D point12 = point2 - point1;
		DCVector3D point13 = point3 - point1;
		DCVector3D point23 = point3 - point2;

		//ȡ��ֵ
		DCVector3D nPoint12 = -point12;
		DCVector3D nPoint13 = -point13;
		DCVector3D nPoint23 = -point23;

		double angle1 = GetAngleDeg(point12,point13);
		double angle2 = GetAngleDeg(point23,nPoint12);
		double angle3 = GetAngleDeg(nPoint13,nPoint23); 

		if (dis1 > para.maxEdge || dis2 > para.maxEdge || dis3 > para.maxEdge || 
			angle1 > para.maxAngle || angle2 > para.maxAngle || angle3 > para.maxAngle ||
			angle1 < para.minAngel || angle2 < para.minAngel || angle3 < para.minAngel)  //�߳�����������ֵ,�Ƕ���ֵ
		{
			//ɾ���������Ӽ�¼
			linkTable.erase(i);
		}
	}
	
	//���ݴ�����linkTable������m_links����,ͬʱ�������˹�ϵ����
	//������ʱ���˹�ϵ����

	std::vector<std::array<long, 3>> topoTable;
	std::array<long, 3> topo;

	for (auto it = linkTable.begin(); it != linkTable.end(); ++it)
	{
		AddTriangle(it->second);

		//�ж��Ƿ�������˹�ϵ��������������˹�ϵ��1.����keyֵȡ����Ч�ģ�2.���ж���Ч�����˹�ϵ���Ƿ��в����ڵ���������ֵΪ-1
		if (!m_pDcGpMeshImpl->m_topologys.empty())
		{
			//ȡ����ǰ��Ч�����˹�ϵ���뵽topo��
			topo = GetTopology(it->first);

			//�ж�topo���Ƿ���ĳ����������linkTable��keyֵ��û�и�Ԫ��
			for (int tIndexNum = 0; tIndexNum < topo.size(); ++tIndexNum)
			{
				bool isExist = linkTable.find(topo[tIndexNum]) != linkTable.end();   //����
				if (!isExist)
				{
					topo[tIndexNum] = -1;
				}
			}

			topoTable.push_back(topo);
		}
	}
	if (!topoTable.empty())
	{
		m_pDcGpMeshImpl->m_topologys = topoTable;
	}
}

DCVector3D& DcGp::DcGpMesh::GetPoint(unsigned index)
{
	return m_pDcGpMeshImpl->m_points.at(index);
}

void DcGp::DcGpMesh::AddTriangle(Link tri)
{	
	m_pDcGpMeshImpl->m_links.push_back(tri);

	m_pDcGpMeshImpl->m_meshsSelected.push_back(false);
}

std::array<long, 3> DcGp::DcGpMesh::GetTopology(unsigned index)
{
	if (!m_pDcGpMeshImpl->m_topologys.empty())
	{
		return m_pDcGpMeshImpl->m_topologys.at(index);
	}
}

unsigned DcGp::DcGpMesh::LinkSize()
{
	if (m_pDcGpMeshImpl->m_links.empty())
	{
		return 0;
	}
	else
	{
		return m_pDcGpMeshImpl->m_links.size();
	}
}

bool DcGp::DcGpMesh::HasSelected()
{
	std::vector<bool >::iterator it = std::find(m_pDcGpMeshImpl->m_meshsSelected.begin(), m_pDcGpMeshImpl->m_meshsSelected.end(), true);
	if (it == m_pDcGpMeshImpl->m_meshsSelected.end())
	{
		return false;
	}
	return true;
}

bool DcGp::DcGpMesh::GetMeshSelected(unsigned index) const
{
	return m_pDcGpMeshImpl->m_meshsSelected[index];
}

void DcGp::DcGpMesh::SetMeshSelected(unsigned index, bool status)
{
	m_pDcGpMeshImpl->m_meshsSelected[index] = status;
}

std::vector<unsigned> DcGp::DcGpMesh::GetSelectMeshs()
{
	unsigned triNum = LinkSize();

	std::vector<unsigned> meshNumber;

	for (unsigned i = 0; i < triNum; ++i)
	{
		//�����жϵ�ǰ�������Ƿ�ѡ����(����һ��ѡ�к�����ѡ��)
		if (GetMeshSelected(i))
		{
			meshNumber.push_back(i);
		}
	}
	return meshNumber;
}

bool DcGp::DcGpMesh::SaveToFile(QFile& file) const
{
	//����mesh����Ϊply��ʽ
	QString fileName = file.fileName();
	
	return true;
}

//��ӱ�����
void DcGp::DcGpMesh::AddScalarField(DcGpScalarFieldPtr scalarField)
{
	m_pDcGpMeshImpl->m_scalarFields.push_back(scalarField);
}

DcGp::DcGpScalarFieldPtr DcGp::DcGpMesh::GetScalarFieldByIndex(int index)
{
	assert(index >= 0);
	return m_pDcGpMeshImpl->m_scalarFields[index];
}

//��ȡָ���������ɫ
PointColor DcGp::DcGpMesh::GetPointColor(unsigned index) const
{
	if (index >= m_pDcGpMeshImpl->m_points.size())
	{
		index = m_pDcGpMeshImpl->m_points.size() - 1;
	}

	if (index < 0)
	{
		index = 0;
	}

	return m_pDcGpMeshImpl->m_rgbColors[index];

}

DCVector3D DcGp::DcGpMesh::GetPointNormal(unsigned index) const
{
	if (index >= m_pDcGpMeshImpl->m_Normals.size())
	{
		index = m_pDcGpMeshImpl->m_Normals.size() - 1;
	}

	if (index < 0)
	{
		index = 0;
	}

	return m_pDcGpMeshImpl->m_Normals[index];

}

void DcGp::DcGpMesh::MapTexBySetTextImgFile(QString& fileName, const QString& tfwFile, const QString& suffix)
{
	if (fileName.isEmpty())
		return;
	
	//����tfw�ļ������ȡ����
	bool hasTfw = false;
	TFWParas paras;
	if (!tfwFile.isEmpty())
	{
		paras = GetParasFromTfw(tfwFile);
		hasTfw = true;
	}

	if(!glIsTexture(m_pDcGpMeshImpl->m_targetTex))
	{
		//��������
		//BindTargetTexture(image, suffix);

		//! ����һ devil
		m_pDcGpMeshImpl->m_teximgFileName = fileName;
		int* width = new int[1];
		int* height = new int[1];
		//m_pDcGpMeshImpl->LoadGLTextureByDevil(width, height);

		//������ qimage
		QImage image(fileName);
		LoadGLTextureByQImage(image, suffix);
		*width = image.width();
		*height = image.height();

		//Ϊÿ������������������
		Point_3D shift = GetShift();
		for (unsigned i = 0; i < PSize(); ++i)
		{
			Point_3D point = Point_3D::FromArray(GetPoint(i).u);
			point -= shift;

			//��ѯ��ǰpoint��Ӧ��image�е��������к�
			int row = 0, column = 0;
			if (!hasTfw)
			{
				GetPixelPosition(Point_3D::FromArray(GetMinPoint().u)-shift, Point_3D::FromArray(GetMaxPoint().u)-shift, *height, *width, point, row, column);
			}
			else
			{
				GetPixelPositionByTFW(paras, point, row, column);
			}
			

			////����image��size��һ��row��column���õ���������
			DCVector2D texCoord;
			NormalizePixelPos(*height, *width, row, column, texCoord);

			m_pDcGpMeshImpl->m_texCoords.push_back(texCoord);
		}

		delete width;
		delete height;

		width = 0;
		height = 0;
	}
}

void DcGp::DcGpMesh::LoadGLTextureByQImage(QImage& image, const QString& suffix)
{
	if (image.isNull())
		return;
	if (m_pDcGpMeshImpl->m_targetTex) 
	{
		glDeleteTextures(1, &m_pDcGpMeshImpl->m_targetTex);
		m_pDcGpMeshImpl->m_targetTex = 0;
	}

	// create texture
	glGenTextures(1, &m_pDcGpMeshImpl->m_targetTex);
	QImage tximg = QGLWidget::convertToGLFormat(image);
	glBindTexture(GL_TEXTURE_2D, m_pDcGpMeshImpl->m_targetTex);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	if (suffix.toUpper() == "BMP")
	{
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR_MIPMAP_LINEAR);
		gluBuild2DMipmaps(GL_TEXTURE_2D, 4, tximg.width(), tximg.height(), GL_RGBA, GL_UNSIGNED_BYTE, tximg.bits());
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

		glTexImage2D(GL_TEXTURE_2D, 0, 3, tximg.width(), tximg.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, tximg.bits());
	}
} 

void DcGp::DcGpMesh::AddTexCoord(DCVector2D texCoord)
{
	m_pDcGpMeshImpl->m_texCoords.push_back(texCoord);
}

bool DcGp::DcGpMesh::HasTextures()
{
	return m_pDcGpMeshImpl->m_targetTex;
}

void DcGp::DcGpMesh::CancelTexture()
{
	if (m_pDcGpMeshImpl->m_targetTex)
	{
		glDeleteTextures(1, &m_pDcGpMeshImpl->m_targetTex);
	}
	m_pDcGpMeshImpl->m_targetTex = 0;
	m_pDcGpMeshImpl->m_teximgFileName = "";
	m_pDcGpMeshImpl->m_texCoords.clear();
}

DcGp::ShowStyle DcGp::DcGpMesh::GetShowStyle()
{
	return m_pDcGpMeshImpl->m_showStyle;
}

//������ʾ��ʽ
void DcGp::DcGpMesh::SetShowStyle(ShowStyle showStyle)
{
	m_pDcGpMeshImpl->m_showStyle = showStyle;
	GetAssociatedWindow()->Redraw();
}

unsigned DcGp::DcGpMesh::ScalarFieldCount() const
{
	return m_pDcGpMeshImpl->m_scalarFields.size();
}

QStringList DcGp::DcGpMesh::ScalarFieldNames() const
{
	QStringList names;
	unsigned number = m_pDcGpMeshImpl->m_scalarFields.size();

	for (unsigned i = 0; i < number; ++i)
	{
		names.push_back(m_pDcGpMeshImpl->m_scalarFields.at(i)->GetName());
	}
	return names;
}

//����ת��
void DcGp::DcGpMesh::ApplyTransform(EigenMatrix4d& transMatrix)
{
	//��ʼ�����ֵ��Сֵ
	m_pDcGpMeshImpl->m_minPoint = DCVector3D(DCCore::MAX_POINTCOORD, DCCore::MAX_POINTCOORD, DCCore::MAX_POINTCOORD);
	m_pDcGpMeshImpl->m_maxPoint = DCVector3D(DCCore::MIN_POINTCOORD, DCCore::MIN_POINTCOORD, DCCore::MIN_POINTCOORD);

	Eigen::Matrix<double, 1, 3> temp;
	for (unsigned i = 0; i != PSize(); ++i)
	{
		/*temp << m_points[3 * i], m_points[3 * i + 1], m_points[3 * i + 2];*/
		temp << m_pDcGpMeshImpl->m_points[i][0], 
			m_pDcGpMeshImpl->m_points[i][1], 
			m_pDcGpMeshImpl->m_points[i][2];

		temp *= transMatrix.block(0, 0, 3, 3).transpose();
		temp += transMatrix.block(0, 3, 3, 1).transpose();
		// 		m_points[3 * i] = temp[0];
		// 		m_points[3 * i + 1] = temp[1];
		// 		m_points[3 * i + 2] = temp[2];
		m_pDcGpMeshImpl->m_points[i][0] = temp[0];
		m_pDcGpMeshImpl->m_points[i][1] = temp[1];
		m_pDcGpMeshImpl->m_points[i][2] = temp[2];


		//���¼������ֵ
		m_pDcGpMeshImpl->m_minPoint[0] = std::min(m_pDcGpMeshImpl->m_minPoint[0], m_pDcGpMeshImpl->m_points[i][0]);
		m_pDcGpMeshImpl->m_minPoint[1] = std::min(m_pDcGpMeshImpl->m_minPoint[1], m_pDcGpMeshImpl->m_points[i][1]);
		m_pDcGpMeshImpl->m_minPoint[2] = std::min(m_pDcGpMeshImpl->m_minPoint[2], m_pDcGpMeshImpl->m_points[i][2]);

		m_pDcGpMeshImpl->m_maxPoint[0] = std::max(m_pDcGpMeshImpl->m_maxPoint[0], m_pDcGpMeshImpl->m_points[i][0]);
		m_pDcGpMeshImpl->m_maxPoint[1] = std::max(m_pDcGpMeshImpl->m_maxPoint[1], m_pDcGpMeshImpl->m_points[i][1]);
		m_pDcGpMeshImpl->m_maxPoint[2] = std::max(m_pDcGpMeshImpl->m_maxPoint[2], m_pDcGpMeshImpl->m_points[i][2]);
	}

	//��Eigen::Matrix4d����ֵ������QMatrix4x4���͵ľ���
	double* matValue = transMatrix.data();

	//m_pDcGpMeshImpl->m_pop = QMatrix4x4(matValue);

	//���¼���߽��
	m_pDcGpMeshImpl->m_borderValid = false;

}

void DcGp::DcGpMesh::SetTexImgFile(const QString& texFileName)
{
	if (!texFileName.isEmpty())
	{
		m_pDcGpMeshImpl->m_teximgFileName = texFileName;
	}
}

void DcGp::DcGpMesh::SetMaterial(DcGp::DcGpMaterial* mat)
{
	if (m_pDcGpMeshImpl->m_material)
	{
		delete m_pDcGpMeshImpl->m_material;
		m_pDcGpMeshImpl->m_material = nullptr;
	}
	m_pDcGpMeshImpl->m_material = mat;
}
