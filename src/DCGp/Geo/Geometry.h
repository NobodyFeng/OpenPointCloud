//----------------------------------------------------------------------
//author:wly
//time:
//dsc:����һ�����������Ŀ���Ǳ��漸���嶥��������Ϣ��������һ��
//��װ��opegnGLͼԪ���ͣ�ʵ��ͼԪ����Ⱦ
//----------------------------------------------------------------------
#ifndef MPGP_GEOMETRY_H
#define MPGP_GEOMETRY_H

#include "DCCore/BasicTypes.h"

#include "DCGp/GpEntity.h"

#include "State.h"

#include "DCGp/DcGpDLL.h"
namespace DcGp
{
	class DcGpBoundBox;
};
namespace DCGp
{
	class PrimitiveSet;
	class DCGP_API Geometry : public DcGp::DcGpEntity
	{
		Q_OBJECT
		GET_CLASSNAME(Geometry)
	public:
		Geometry(QString name);
		Geometry(QString name, std::vector<DCVector3D> vertexes);

		virtual ~Geometry();

		//!---------------------------------��Ⱦ���--------------------------------------------------------
		virtual void DrawMyselfOnly(DcGp::DcGpDrawContext& context);
		virtual void FastDrawMyselfOnly(DcGp::DcGpDrawContext& context);
		virtual void DrawWithNames(DcGp::DcGpDrawContext& context, bool drawSample = false);

		//! ��ȡ�߽��
		virtual DcGp::DcGpBoundBox GetMyOwnBoundBox();

		//! ���úͻ�ȡͼԪ��Ⱦ״̬����
		void SetState(State state);
		State& GetState();
        
        //! ������ɫ
		void SetColor(RGBColor colors);

		//���õ�ǰ����ʱ��ɫ
		//����һ����ɫֵ
		//���������Ƿ��Զ�������ʱ��ɫ
		virtual void SetTempColor(const RGBColor color, bool autoActivate = true);

		//! Ψһɫ
		virtual const RGBColor GetTempColor() const;

		//!--------------------------------Set/Get property-----------------------------------------
		//! ���һ��ͼԪ
		bool AddPrimitiveSet(PrimitiveSet* primitiveset);

		//! ���һ����λ������
		void AddVertex(DCVector3D vertex);

		//! ���һ����ɫ����
		void AddColorf(DCVector3D colorf);

		void AddTexCoord(DCVector2D texCoord);

		//! ���ض���
		std::vector<DCVector3D>& GetVertexes();

		//! ��������
		std::vector<DCVector2D >& GetTexCoords();

		//! ���ض�����Ŀ
		int GetVertexesSize();

		//! ȡ��Сֵ��
		DCVector3D GetMinPoint();

		//! ȡ���ֵ��
		DCVector3D GetMaxPoint();

		DCVector3D& GetVertex(const long index);

		std::vector<PrimitiveSet* > GetPrimitive();
		//! ���ñ߽��
		void SetBorderInValid();
	private:
		//��������ı߽��
		void CalcMyOwnBoundBox();
		
	private:
		//!------------------------------�����������Ϣ������λ�á�������ɫ��----------------------------------------
		std::vector<DCVector3D > m_vertexs;

		std::vector<DCVector3D > m_normals;

		RGBColor m_colors;

		std::vector<DCVector3D > m_secondaryColors;

		std::vector<DCVector2D > m_texCoords;

		std::vector<unsigned int > m_indexs;

		//!------------------------------ͼԪ����----------------------------------------
		std::vector<PrimitiveSet* > m_primitveSets;

		//!-----------------------------��Ⱦ֧��--------------------------------------------------
		//! ͼԪ��Ⱦstate
		State m_state;

		bool m_useVertexBufferObjects;

		//! �߽��
		DCVector3D m_minPoint;
		DCVector3D m_maxPoint;

		bool m_borderValid;
	};
}

#endif // GEOMETRY_H
