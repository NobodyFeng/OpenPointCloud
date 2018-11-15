#ifndef GP_POINT_CLOUD_IMPL_H
#define GP_POINT_CLOUD_IMPL_H

//Qt
#include "QtGui/QMatrix4x4"

//DcCore
#include "DCCore/DCDefs.h"
#include "DCCore/DCGL.h"
#include "DCCore/vsShaderLib.h"

#include "DCGp/GpBasicDevice.h"
#include "DCGp/Custom/GpBoundBox.h"
#include "DCGp/GpDrawContext.h"
#include "DCGp/Custom/GpScalarField.h"

//#include "DCFilters/BlockedGridToolkit.h"

namespace DcGp
{
	DECLARE_SMARTPTR(DcGpPointCloudImpl)
	class DcGpPointCloudImpl
	{
		DECLARE_PHONY_CONSTRUCTOR(DcGpPointCloudImpl)
	public:
		~DcGpPointCloudImpl();

		GLuint SetupFastDrawShaders();

		GLuint SetupGeneralDrawShaders();

		void SetBorderInValid();

		//���㵱ǰʵ����������Сֵ
		void CalculateMyOwnBoundBox();

		void UpdateScalarFieldData();

	protected:
		DcGpPointCloudImpl();

	private:
		friend class DcGpPointCloud;

		//������
		std::vector<DCVector3D> m_points;
		//�����ʾ״̬
		std::vector<int> m_pointsVisibility;
		//���ѡ��״̬
		std::vector<bool> m_pointsChoosedState;
		//�Ƿ�ֻ��ʾ��������
		bool m_visibilityTable;
		//������ʾ�ķ�ʽ
		bool m_visibilityType;
		std::vector<DCVector3D> m_Normals;
		//��������ɫ
		std::vector<PointColor> m_rgbColors;
		//��������ɫ
		std::vector<PointColor> m_scalarColors;
		//ѡȡ״̬�µ���ɫ
		std::vector<PointColor> m_choosedColors;
		//������
		std::vector<DcGpScalarFieldPtr> m_scalarFields;

		//��ǰʹ�õı�����
		DcGpScalarFieldPtr m_currentScalarField;
		//�������
		std::vector<unsigned long> m_indexs;
		//���С
		unsigned m_pointSize;
		//XYZ��Χ
		DCVector3D m_minPoint;
		DCVector3D m_maxPoint;

		unsigned int m_nVBOVertices;

		//�Ƿ��Ѿ�����������Сֵ
		bool m_borderValid;

		//���Ƶ�ǰת������
		QMatrix4x4 m_pop;

		//�����ṹ
		//DcFilter::BlockedGridToolkit* m_blockedGridToolkit;

		DcGpPointCloud* m_qIntface;

		//! ��תƽ������ʱ���ϡ����ʹ�õ�shader
		VSShaderLib m_fastDrawShader;

		//! ��ͨ�����Ⱦ����
		VSShaderLib m_generalDrawShader;
	};
}

#endif