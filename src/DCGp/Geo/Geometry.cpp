#include "Geometry.h"

//C++
#include <math.h>

#include "PrimitiveSet.h"

//DCCore
#include "DCCore/Const.h"

//dcgp
#include "DCGp/Custom/GpBoundBox.h"
#include "DCGp/GpDrawContext.h"
using namespace DCGp;

Geometry::Geometry(QString name)
	: DcGp::DcGpEntity(name)
	, m_borderValid(false)
	, m_state(State())
{
	m_colors[0] = 0;
	m_colors[1] = 255;
	m_colors[2] = 0;
	SetBorderInValid();
}

Geometry::Geometry(QString name, std::vector<DCVector3D> vertexes)
	: DcGp::DcGpEntity(name)
	, m_vertexs(vertexes)
	, m_borderValid(false)
	, m_state(State())
{
	m_colors[0] = 0;
	m_colors[1] = 255;
	m_colors[2] = 0;
	SetBorderInValid();
	SetSelectionBehavior(DcGpEntity::eSelectionIgnored);

	SetToolTip(GetName());
}

Geometry::~Geometry()
{

}

void Geometry::SetState(State state)
{
	m_state = state;
}

void Geometry::SetColor(RGBColor colors)
{
	m_colors[0] = colors[0];
	m_colors[1] = colors[1];
	m_colors[2] = colors[2];
}

State& Geometry::GetState()
{
	return m_state;
}

std::vector<DCVector3D>& Geometry::GetVertexes()
{
	return m_vertexs;
}

int Geometry::GetVertexesSize()
{
	return m_vertexs.size();
}

DCVector3D Geometry::GetMinPoint()
{
	if (!m_borderValid)
	{
		CalcMyOwnBoundBox();
	}
	return m_minPoint;
}

DCVector3D Geometry::GetMaxPoint()
{
	if (!m_borderValid)
	{
		CalcMyOwnBoundBox();
	}
	return m_maxPoint;
}

std::vector<PrimitiveSet* > Geometry::GetPrimitive()

{
	return m_primitveSets;
}

//���õ�ǰ����ʱ��ɫ
//����һ����ɫֵ
//���������Ƿ��Զ�������ʱ��ɫ
void Geometry::SetTempColor(const RGBColor color, bool autoActivate /*= true*/)
{
	m_colors = color;
}


//��ȡ��ʱ��ɫ
const RGBColor Geometry::GetTempColor() const
{
	return m_colors;
}

void Geometry::FastDrawMyselfOnly(DcGp::DcGpDrawContext& context)
{
	DrawMyselfOnly(context);
}

void Geometry::DrawWithNames(DcGp::DcGpDrawContext& context, bool drawSample /* = false */)
{
	if (MACRO_Draw3D(context))
	{
		if (!IsEnabled() || !IsVisible())
		{
			return;
		}
		//! ���ֶ�ջ������ʵ��ʵ�岶׽
		bool pushName = MACRO_DrawEntityNames(context);

		if (pushName)
		{
			glPushName(GetUniqueID());
		}


		//! �Ƿ�֧��ʹ��VBO
		bool usingVBOs = m_useVertexBufferObjects;

		//! �����Ƿ�ʹ��vbo����ȷ����ôʹ��setVertexPointer����set up arrays
		//! osg��ͨ��state��װ��state.setVertexPointer(_vertexData.array.get());
		{
			//ʹ�ö�������
			//���ö�������
			if (!m_vertexs.empty())
			{
				glEnableClientState(GL_VERTEX_ARRAY);
				glVertexPointer(3, GL_FLOAT, 0, &m_vertexs[0][0]);
				//m_state.SetVertexPointer(m_vertexs);
			}

			if (!m_texCoords.empty())
			{
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				glTexCoordPointer(2, GL_FLOAT, 0, &m_texCoords[0][0]);
				//m_state.SetTexCoordPointer(2, GL_FLOAT, 0, &m_texCoords[0][0]);
			}
		}

		glPushAttrib(GL_ALL_ATTRIB_BITS);

		/*if (IsSelected())
		{
		glColor3ub(255, 0,0);
		}
		else*/
		{
			//! ��ɫ��ʾ
			if (!m_colors.empty())
			{
				glColor3ub(m_colors[0],m_colors[1],m_colors[2]);
			}
		}

		//!  ��һ��ִ�������������ú󣬸��ݵ�ǰͼԪ���ͣ����������ͼԪ���ͻ���
		for(unsigned int primitiveSetNum=0; primitiveSetNum!= m_primitveSets.size(); ++primitiveSetNum)
		{
			//����������,�����Կ�.
			//auto aa = m_primitveSets[0]->GetMode();
			//glDrawArrays(m_primitveSets[0]->GetMode(), 0, m_vertexs.size());

			//! ͼԪ����֮ǰҪ������Ӧ��model�����ö�Ӧ�����ԣ�Ȼ����ǻ��ƣ��������������ǰ
			//! Ӧ����������������opengl�� glEnable��osgMode�������ԣ�Attrib����osgPolygonMode��
			//! ��osg�������������������state����

			//! Ӧ����������
			//glPolygonMode(GL_FRONT_AND_BACK,GL_LINE | GL_POLYGON | GL_POINT);
			m_state.Apply();
			m_primitveSets[primitiveSetNum]->Draw(false);
		}

		glPopAttrib();

		if (pushName)
		{
			glPopName();
		}
	}

	//m_state.ApplyDisablingOfVertexAttributes();
	if (!m_vertexs.empty())
	{
		glDisableClientState(GL_VERTEX_ARRAY);
	}

	if (!m_texCoords.empty())
	{
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
}

void Geometry::DrawMyselfOnly(DcGp::DcGpDrawContext& context)
{
	if (MACRO_Draw3D(context))
	{
		//! �Ƿ�֧��ʹ��VBO
		bool usingVBOs = m_useVertexBufferObjects;

		//! �����Ƿ�ʹ��vbo����ȷ����ôʹ��setVertexPointer����set up arrays
		//! osg��ͨ��state��װ��state.setVertexPointer(_vertexData.array.get());
		{
			//ʹ�ö�������
			//���ö�������
			if (!m_vertexs.empty())
			{
				glEnableClientState(GL_VERTEX_ARRAY);
				glVertexPointer(3, GL_FLOAT, 0, &m_vertexs[0][0]);
				//m_state.SetVertexPointer(m_vertexs);
			}
			
			if (!m_texCoords.empty())
			{
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				glTexCoordPointer(2, GL_FLOAT, 0, &m_texCoords[0][0]);
				//m_state.SetTexCoordPointer(2, GL_FLOAT, 0, &m_texCoords[0][0]);
			}
		}

		glPushAttrib(GL_ALL_ATTRIB_BITS);

		if (IsSelected())
		{
			glColor3ub(255, 0,0);
		}
		else
		{
			//! ��ɫ��ʾ
			if (!m_colors.empty())
			{
				glColor3ub(m_colors[0],m_colors[1],m_colors[2]);
			}
		}

		//!  ��һ��ִ�������������ú󣬸��ݵ�ǰͼԪ���ͣ����������ͼԪ���ͻ���
		for(unsigned int primitiveSetNum=0; primitiveSetNum!= m_primitveSets.size(); ++primitiveSetNum)
		{
			//����������,�����Կ�.
			//auto aa = m_primitveSets[0]->GetMode();
			//glDrawArrays(m_primitveSets[0]->GetMode(), 0, m_vertexs.size());

			//! ͼԪ����֮ǰҪ������Ӧ��model�����ö�Ӧ�����ԣ�Ȼ����ǻ��ƣ��������������ǰ
			//! Ӧ����������������opengl�� glEnable��osgMode�������ԣ�Attrib����osgPolygonMode��
			//! ��osg�������������������state����

			//! Ӧ����������
			//glPolygonMode(GL_FRONT_AND_BACK,GL_LINE | GL_POLYGON | GL_POINT);
			m_state.Apply();
			m_primitveSets[primitiveSetNum]->Draw(false);
		}

		glPopAttrib();
	}

	//m_state.ApplyDisablingOfVertexAttributes();
	if (!m_vertexs.empty())
	{
		glDisableClientState(GL_VERTEX_ARRAY);
	}

	if (!m_texCoords.empty())
	{
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
}

bool Geometry::AddPrimitiveSet(PrimitiveSet* primitiveset)
{
	m_primitveSets.push_back(primitiveset);

	return true;
}

void Geometry::AddVertex(DCVector3D vertex)
{
	m_vertexs.push_back(vertex);
}

void Geometry::AddTexCoord(DCVector2D texCoord)
{
	m_texCoords.push_back(texCoord);
}

void Geometry::AddColorf(DCVector3D colorf)
{
	//m_colors.push_back(colorf);
}

std::vector<DCVector2D >& Geometry::GetTexCoords()
{
	return m_texCoords;
}


void Geometry::CalcMyOwnBoundBox()
{
	if (m_borderValid)
	{
		return ;
	}
	std::vector<DCVector3D> bndaryPoint = m_vertexs;

	//����m_points��������С���ֵm_minPoint��m_maxPoint
	for (auto i = 0; i < bndaryPoint.size(); ++i)
	{
		m_minPoint[0] = std::min(m_minPoint[0], bndaryPoint[i][0]);
		m_minPoint[1] = std::min(m_minPoint[1], bndaryPoint[i][1]);
		m_minPoint[2] = std::min(m_minPoint[2], bndaryPoint[i][2]);

		m_maxPoint[0] = std::max(m_maxPoint[0], bndaryPoint[i][0]);
		m_maxPoint[1] = std::max(m_maxPoint[1], bndaryPoint[i][1]);
		m_maxPoint[2] = std::max(m_maxPoint[2], bndaryPoint[i][2]);
	}

	m_borderValid =  true;
}

void Geometry::SetBorderInValid()
{
	m_borderValid = false;

	m_minPoint = DCVector3D(DCCore::MAX_POINTCOORD, DCCore::MAX_POINTCOORD, DCCore::MAX_POINTCOORD);
	m_maxPoint = DCVector3D(DCCore::MIN_POINTCOORD, DCCore::MIN_POINTCOORD, DCCore::MIN_POINTCOORD);

}

DcGp::DcGpBoundBox Geometry::GetMyOwnBoundBox()
{
	DcGp::DcGpBoundBox emptyBox(GetMinPoint(), GetMaxPoint());
	return emptyBox;
}

DCVector3D& Geometry::GetVertex(const long index)
{
	return m_vertexs[index];
}