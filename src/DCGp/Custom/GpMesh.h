#ifndef DCGP_MESH_H
#define DCGP_MESH_H

#include "DCGp/DcGpDLL.h"

//C++
#include <vector>
#include <map>

//Qt
class QString;

//Common
#include "DCCore/Const.h"


#include "DCGp/GpEntity.h"
#include "DCGp/Custom/GpBoundBox.h"
#include "GpScalarField.h"
#include "DCGp/GpDrawContext.h"

#include "DCGp/GpBasicTypes.h"

class QImage;

namespace DcGp
{
	DECLARE_SMARTPTR(DcGpScalarField)
	DECLARE_SMARTPTR(DcGpMesh)
	DECLARE_SMARTPTR(DcGpMeshImpl)

	class DCGP_API DcGpMesh : public DcGpEntity
	{
		Q_OBJECT

		DECLARE_PHONY_CONSTRUCTOR(DcGpMesh)
		DECLARE_IMPLEMENT(DcGpMesh)
		GET_CLASSNAME(DcGpMesh)

	public:
		struct FilterPara
		{
			float maxEdge;
			double maxAngle;
			double minAngel;

			FilterPara(float _maxEdge = 5.0, double _maxAngle = 90.0, double _minAngle = 5.0)
			{
				maxEdge = _maxEdge;
				maxAngle = _maxAngle;
				minAngel = _minAngle;
			}
		};

		static DcGp::DcGpEntity* CreateEntity()
		{
			return new DcGpMesh("Mesh");
		}

		//���캯��
		DcGpMesh(QString name); //���붥�����ݶ���ָ��
		DcGpMesh(QString name, std::vector<DCVector3D> vertices); //���붥�����ݶ���ָ��
		virtual ~DcGpMesh();

		//��Ա����
		void AddPoint(DCVector3D point);
		void AddNormal(DCVector3D normal);

		//! ��Ӷ�����ɫ
		void AddVexColor(PointColor color);
		void AddTriangle(unsigned index1, unsigned index2, unsigned index3); //���������ε�3����������

		//����mesh
		virtual void DrawMyselfOnly(DcGpDrawContext& context);
		
		virtual DcGpBoundBox GetDisplayBoundBox();
		//��ȡ�߽��
		virtual DcGpBoundBox GetMyOwnBoundBox();

		//��ȡ���ж���
		std::vector<PointCoord> GetVertexs();
		LinkContainer GetLinks();

		//�������ӹ�ϵ
		void SetLinks(LinkContainer links);
		//���ö�������
		void SetPoints(std::vector<DCVector3D> points);
		//�������˹�ϵ
		void SetTopologys(std::vector<std::array<long, 3>> topologys);

		//��ȡָ����
		DCVector3D GetPoint(unsigned index) const;
		//��ȡ������ֵ
		virtual	std::vector<Normal> GetNormals();
		//���÷�����
		virtual void SetNormals(std::vector<DCVector3D> normals);
		//�Ƿ�ӵ��������
		virtual bool HasNormals() const;

		//���õ�ǰ������
		void SetCurrentScalarField(unsigned index, unsigned setps, ScalarFieldType type = eScalarField);
		//���ĵ�ǰ����������ɫ��
		virtual void ChangeScalarFieldColorScale(QString name);
		//�Ƿ�ӵ�б�����
		virtual bool HasScalarFields() const;
		//��ʾ������
		virtual void ShowScalarField(bool state);

		//������������
		virtual unsigned ScalarFieldCount() const;

		//������������
		virtual QStringList ScalarFieldNames() const;

		//��ʾ������Ϣ
		void ShowProperties(QTableWidget* widget);

		//!\brief ���������������
		//bool BuildDelaunay(std::vector<DCVector2D> &point2ds);

		//!\brief ������˹�ϵ
		void AddTopology(long neighbor1, long neighbor2, long neighbor3);

		//!\brief ��ȡ���˹�ϵ
		std::vector<std::array<long, 3>> GetTopologys();

		//!\brief ���ļ��м��أ������Ƿ����ƫ�����Լ�ƫ����ֵ
		virtual bool LoadFromFile(const QString& fileName /*!< [in] �ļ���*/,
								  bool coordinatesShiftEnabled  /*!< [in] �Ƿ����ƫ����*/,
			                      double* coordinatesShift = 0  /*!< [in] ƫ��������ָ��*/);

		//!\brief ����txt��ʽ
		bool LoadWrl(const QString& fileName /*!< [in] �ļ���*/,
			              bool coordinatesShiftEnabled = false /*!< [in] �Ƿ����ƫ����*/,
			              double* coordinatesShift = 0 /*!< [in] ƫ��������ָ��*/);

		//!\brief��ȡ����
		PointCoord GetMaxPoint() const;
		//!\brief��ȡ��С��
		PointCoord GetMinPoint() const;

	    void SetMaxPoint(PointCoord pt);
		//!\brief������С��
		void SetMinPoint(PointCoord pt);

		//! Sets a particular point compressed normal
		/** WARNING: normals must be enabled.
		**/
		void SetPointNormalIndex(unsigned pointIndex, NormalType norm);

		//! Sets a particular point normal (shortcut)
		/** WARNING: normals must be enabled.
			Normal is automatically compressed before storage.
		**/
		void SetPointNormal(unsigned pointIndex, const DCVector3D& N);

		LinkContainer& GetLinks() const;

		void GetVertexs(std::vector<DCVector3D>& points);

		//��ȡָ�����ѡ��״̬
		//bool GetPointSelected(unsigned index) const;

		//����ָ�����ѡ��״̬
		//void SetPointSelected(unsigned index, bool status);

		//����ѡȡ��ɫ
		void EnableSelectedColors();

		//ͣ��ѡȡ��ɫ
		void DisableSelectedColors();

		//��size
		unsigned PSize();

		//ȡ��ѡȡ
		void CancelSelected();

		//ɾ����ѡ�Ĳ���
		void DeleteSelected();

		Link GetLink(unsigned index);

		void Filter(FilterPara para);

		DCVector3D& GetPoint(unsigned index);

		void AddTriangle(Link tri); //��������������

		//!\brief ����ָ���������Ż�ȡ���˹�ϵ
		std::array<long, 3> GetTopology(unsigned index);

		unsigned LinkSize();

		bool HasSelected();

		//��ȡָ��mesh��ѡ��״̬
		bool GetMeshSelected(unsigned index) const;

		//����ָ��mesh��ѡ��״̬
		void SetMeshSelected(unsigned index, bool status);

		//��ȡѡ�е�������,���������
		std::vector<unsigned> GetSelectMeshs();

		bool SaveToFile(QFile& file) const;

		//��ӱ�����
		void AddScalarField(DcGpScalarFieldPtr scalarField);

		//��ȡָ���ı�����
		DcGpScalarFieldPtr GetScalarFieldByIndex(int index);

		//��ȡָ���������ɫ
		PointColor GetPointColor(unsigned index) const;

		//��ȡָ������ķ�����
		DCVector3D GetPointNormal(unsigned index) const;

		//���ݹ���������ͼƬ��ȡͼƬ����ͼƬ
		void MapTexBySetTextImgFile(QString& fileName, const QString& tfwFile = "", const QString& suffix = "jpg");

		void LoadGLTextureByQImage(QImage& image, const QString& suffix = "jpg");

		//!\ �����������
		void AddTexCoord(DCVector2D texCoord);

		//!\ �����������
		bool HasTextures();

		void CancelTexture();

		ShowStyle GetShowStyle();

		void SetShowStyle(ShowStyle showStyle);

		//����ת��
		void ApplyTransform(EigenMatrix4d& transMatrix);

		//��������������ļ���,�����ڶ�ȡģ���ļ���ʱ�򣬽��ļ��д洢������·����������
		void SetTexImgFile(const QString& texFileName);

		void SetMaterial(DcGp::DcGpMaterial* mat);
	private:
		//!\brief ����context��sfColorScaleToDisplay����ֵ
		void AddColorBarInfo(DcGpDrawContext& context /*<[in] �����Ļ��Ʋ��� */);


	protected:
		DcGpMesh();

	private slots:
		void ChangeShowStyle(int style);
	};
}

#endif // DCMESH_H
