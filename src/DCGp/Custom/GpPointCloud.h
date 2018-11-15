#ifndef DCGP_POINTCLOUD_H
#define DCGP_POINTCLOUD_H

#include "DCGp/DcGpDLL.h"

//c++��׼��
#include <vector>
#include <array>

//Qt
#include "QtGui/QMatrix4x4"

//Dc
#include "DCCore/DCDefs.h"

//Common
#include "DCCore/BasicTypes.h"

#include "DCGp/GpBasicTypes.h"
#include "GpScalarField.h"
#include "DCGp/GpEntity.h"

#include "DCUtil/AbstractEntityVisitor.h"

namespace DcFilter
{
	class BlockedGridToolkit;
}

namespace DcGp
{
	class EntityVisitor;
	class DcGpBoundBox;
	
	DECLARE_SMARTPTR(DcGpScalarField)
	DECLARE_SMARTPTR(DcGpPointCloud)
	DECLARE_SMARTPTR(DcGpPointCloudImpl)
	/*
		* ����ͼԪ
	*/
	class DCGP_API DcGpPointCloud : public DcGpEntity
	{
		Q_OBJECT

		DECLARE_PHONY_CONSTRUCTOR(DcGpPointCloud)
		DECLARE_IMPLEMENT(DcGpPointCloud)
		GET_CLASSNAME(DcGpPointCloud)

	public:

		static DcGp::DcGpEntity* CreateEntity()
		{
			return new DcGpPointCloud("PointCloud");
		}

		DcGpPointCloud(QString name);
		DcGpPointCloud(QString name, long num);
		DcGpPointCloud(DcGpPointCloud* associatedCloud);
		virtual ~DcGpPointCloud();

		virtual void Accept(DCUtil::AbstractEntityVisitor& nv);

		//����ͼ��
		virtual void DrawMyselfOnly(DcGpDrawContext& context);
		virtual void FastDrawMyselfOnly(DcGpDrawContext& context);
		virtual void DrawWithNamesMyselfOnly(DcGpDrawContext& context, bool drawSample = false);
		//��ӵ�
		void AddPoint(DCVector3D vector);

		//���÷�����
		virtual void SetNormals(std::vector<DCVector3D> normals);
		//��ȡ������ֵ
		virtual	std::vector<Normal> GetNormals();
		//������ɫ
		void AddColor(PointColor color);

		//��ӱ�����
		void AddScalarField(DcGpScalarFieldPtr scalarField);

		//��ȡָ���ı�����
		DcGpScalarFieldPtr GetScalarFieldByIndex(int index);

		//��ʾ������
		virtual void ShowScalarField(bool state);

		//���õ�ǰ����������ɫ��
		virtual void ChangeScalarFieldColorScale(QString name);

		//������������
		virtual unsigned ScalarFieldCount() const;

		//������������
		virtual QStringList ScalarFieldNames() const;

		//���õ�ǰ������
		void SetCurrentScalarField(unsigned index, unsigned setps, ScalarFieldType type = eScalarField);

		virtual int GetCurrentScalarFieldIndex();
		DcGpScalarFieldPtr GetCurrentScalarField();

		//�Ƿ�ӵ����ɫ
		virtual bool HasColors() const;

		//�Ƿ�ӵ�б�����
		virtual bool HasScalarFields() const;

		//�Ƿ�ӵ��������
		virtual bool HasNormals() const;

		//��ȡָ����
		DCVector3D GetPoint(unsigned index) const;

		//��ȡ����
		PointCoord GetMaxPoint() const;
		//��ȡ��С��
		PointCoord GetMinPoint() const;

		//��ȡָ����Ŀɼ���
		uchar GetPointVisibility(unsigned index) const;

		//��ȡָ���������ɫ
		PointColor GetPointColor(unsigned index) const;

		//��ȡָ�����ѡ��״̬
		bool GetPointChoosed(unsigned index) const;

		//����ָ�����ѡ��״̬
		void SetPointChoosed(unsigned index, bool status);

		//����ѡȡ��ɫ
		void EnableChoosedColors();

		//ͣ��ѡȡ��ɫ
		void DisableChoosedColors();

		//���ò�����ʾ����
		//����һ���Ƿ�����
		//����������ʾ�����ݣ������true����ʾ��ʾ״̬Ϊtrue�ģ�������ʾ״̬Ϊfalse�ģ�
		void EnableVisibiltyTable(bool status, bool flag = true);

		//��ȡ���е�
		std::vector<DCVector3D> GetPoints();
		std::vector<DCVector3D>& GetPointsRef();
		//�������е�
		void SetPoints(std::vector<DCVector3D>& points);

		//����ָ������������ֵ
		DCVector3D GetDataByIndex(unsigned index);
		//��ȡ������
		unsigned long Size() const;
		unsigned long GetChunk();

		//����ת��
		void ApplyTransform(EigenMatrix4d& transMatrix);

		//��ȡ���Ʋ���
		virtual void GetDrawingParameters(glDrawParameters& params) const;

		//ˢ��ѡȡ��ɫ
		virtual void RefreshChoosedColor();

		//ȡ��ѡȡ
		void CancelChoosed();

		//ɾ����ѡ�Ĳ���
		void DeleteChoosed();

		bool IsChoosed();

		//
		virtual DcGpBoundBox GetDisplayBoundBox();
		//��ȡ�߽��
		virtual DcGpBoundBox GetMyOwnBoundBox();

		//���浽�ļ�
		bool SaveToFile(QFile& file) const;

		//�ͷŸ���Ա�����ж�����ڴ�
		void AutoAdjust();

		//������ɫ�����Ĵ�С
		void SetColorSize(long num);

		int GetPointSize();

		//��ȡrgbֵ
		std::vector<PointColor> GetRGBColor();
		std::vector<PointColor>& GetRGBColorRef();

		//����RGB��ֵ
		void SetRGBColor(std::vector<PointColor> rgbColor);

		//!brief ���ø�����ɫ������ʾ״̬
		void SetSfColorBarVisibility(bool status /*!<[in] �����״ֵ̬ */);

		//!\brief ����context��sfColorScaleToDisplay����ֵ
		void AddColorBarInfo(DcGpDrawContext& context /*<[in] �����Ļ��Ʋ��� */);

		//!\brief ����pop
		void SetPop(QMatrix4x4 matrix);
		//!\brief ��ȡpop
		QMatrix4x4 GetPop();

		//!\brief ��ȡѡ��ĵ���
		void GetChoosedPoints(std::vector<DCVector3D>& points);

		//DcFilter::BlockedGridToolkit* GetBlocked();


		//����ָ����Ŀɼ���
		void SetPointVisibility(unsigned index, uchar visibility);

		//!\brief �ж��Ƿ��е��Ʊ�ѡȡ
		bool HasChoosed();

		//��ȡָ������ķ�����
		DCVector3D GetPointNormal(unsigned index) const;

		//���ָ������ķ�����
		void AddNormal(DCVector3D normal);

		//���÷����������Ĵ�С
		void SetNormalSize(long num);

		//��ȡָ�����Ƶı�������Ӧ��������
		int GetScalarFieldIndexByName(QString name);

	protected:
		DcGpPointCloud();

	public slots:
		void SetPointSize(int size);
	};

}
#endif // POINTCLOUD_H
