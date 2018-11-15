
//Common
#include "DCCore/Const.h"

#include "DCCore/DCDefs.h"

#include "DCGp/GpBasicDevice.h"
#include "../GpBoundBox.h"
#include "DCGp/GpDrawContext.h"
#include "../GpScalarField.h"

#include "DCCore/DCGL.h"

#include "DCGp/GpBasicTypes.h"

namespace DcGp
{
	DECLARE_SMARTPTR(DcGpMeshImpl)
	//! DcGpMesh ������ʵ��
	/**
		* ��Ҫ����GpMesh�е�����
	*/
	class DcGpMeshImpl
	{
		DECLARE_PHONY_CONSTRUCTOR(DcGpMeshImpl)

		//���㵱ǰʵ����������Сֵ
		void CalculateMyOwnBoundBox();

		bool LoadGLTextureByDevil(int* width = 0, int* height = 0);

		void UpdateScalarFieldData();
			 
	protected:
		DcGpMeshImpl();

	private:
		friend class DcGpMesh;

		std::vector<DCVector3D> m_points;
		unsigned m_globalIterator;

		//XYZ��Χ
		DCVector3D m_minPoint;
		DCVector3D m_maxPoint;

		//���ӹ�ϵ
		DcGp::LinkContainer m_links;

		//���˹�ϵ
		std::vector<std::array<long, 3>> m_topologys;

		//��ʾģʽ
		DcGp::ShowStyle m_showStyle;
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

		//!\brief ����������������
		unsigned m_numberOfTriangles;

		//!\brief �Ƿ��Ѿ�����������Сֵ
		bool m_borderValid;

		//����ѹ������
		std::vector<NormalType> m_comPressNormal;

		//ѡȡ״̬�µ���ɫ
		std::vector<PointColor> m_selectedColors;

		//�Ƿ�ֻ��ʾ��������
		bool m_visibilityTable;

		//mesh��ѡ��״̬
		std::vector<bool> m_meshsSelected;

		//!\brief Ŀ���������
		GLuint m_targetTex;

		//!\brief ��������
		std::vector<DCVector2D> m_texCoords;

		//!\brief �󶨵�Ӱ���ļ���
		QString m_teximgFileName;

		//! Ĭ�ϲ���
		DcGpMaterial* m_material; 
	};
}