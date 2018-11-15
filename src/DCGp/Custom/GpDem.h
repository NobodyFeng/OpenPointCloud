#ifndef DCGP_DEM_H
#define DCGP_DEM_H

#include "DCGp/DcGpDLL.h"
#include "QMap"

//C++
#include <memory>
#include <map>
#include <array>

//DCCore
#include "DCCore/DCDefs.h"

//common
#include "DCCore/BasicTypes.h"
#include "DCGp/Custom/GpBoundBox.h"

#include "DCGp/GpEntity.h"

//class QTableWidget;
class QImage;

//������С����
typedef double GridSize_Type;
//������������
typedef unsigned GridCount_Type;
//DEM�Ķ�������

namespace DcGp
{
	DECLARE_SMARTPTR(DcGpScalarField)
	DECLARE_SMARTPTR(DcGpDEM)
	DECLARE_SMARTPTR(DcGpDEMImpl)

	class DCGP_API DcGpDEM : public DcGpEntity
	{
		Q_OBJECT

		DECLARE_PHONY_CONSTRUCTOR(DcGpDEM)
		DECLARE_IMPLEMENT(DcGpDEM)
		GET_CLASSNAME(DcGpDEM)

	public:
		static DcGp::DcGpEntity* CreateEntity()
		{
			return new DcGpDEM("Dem");
		}

		typedef std::array<unsigned, 2> Index;
		typedef std::map<Index, std::vector<double>> VertexHeight;

		DcGpDEM(QString name);
		virtual ~DcGpDEM();

		//�������½ǵ�����͸�������
		void SetParameter(Point_3D startPoint, 
			GridCount_Type xNumber, 
			GridCount_Type yNumber,
			GridSize_Type xInterval,
			GridSize_Type yInterval,
			VertexHeight heights,
			bool holdBoder = true);

		//���²���
		void UpdateParameter(Point_3D startPoint, 
							GridCount_Type xNumber, 
							GridCount_Type yNumber,
							GridSize_Type xInterval,
							GridSize_Type yInterval);

		//��ȡ����
		QMap<QString, double> GetParameter();

		//��Ӷ���
		//void AddVertex(DEMVertex vertex);

		//��ʾģʽ
		enum ShowStyle
		{
			eShowPoint      =   0   ,
			eShowGrid		=	1	,
			eShowFill		=	2	,
			eShowDefault	=	3
		};
		//������ʾģʽ
		inline void SetShowStyle(ShowStyle style);
		//��ʾ����
		void ShowProperties(QTableWidget* widget);
		//����������
		void CalculateEarthVolumn(double height, double& excavation, double& fill);

		//���÷�����
		virtual void SetNormals(std::vector<DCVector3D> normals);
		//��ȡ������ֵ
		virtual std::vector<Normal> GetNormals();
		//�Ƿ�ӵ��������
		virtual bool HasNormals() const;

		//��ӱ�����
		void AddScalarField(DcGpScalarFieldPtr scalarField);

		//��ȡָ���ı�����
		DcGpScalarFieldPtr GetScalarFieldByIndex(int index);

		//��ȡָ�����Ƶı�������Ӧ��������
		int GetScalarFieldIndexByName(QString name);

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

		DcGpScalarFieldPtr GetCurrentScalarField();

		//�Ƿ�ӵ�б�����
		virtual bool HasDisplayedScalarField() const;

		//��ȡ����
		std::vector<PointCoord> GetVertexs();

		PointCoord GetVertex(unsigned index);

		PointCoord* GetVertex(unsigned index) const;

		//��ȡ���ӹ�ϵ
		std::vector<std::array<unsigned, 4>> GetLinks();

		//��ȡ�߽��
		virtual DcGpBoundBox GetMyOwnBoundBox();

		//!\brief��ȡ����
		PointCoord GetMaxPoint() const;
		//!\brief��ȡ��С��
		PointCoord GetMinPoint() const;

		//��ӵ�
		void AddPoint(DCVector3D vector);

		//��ӵ㵽ָ����λ��
		void AddPoint(DCVector3D vector, unsigned index);

		//Ϊ����������ռ�
		void Resize(unsigned size);

		//�����
		unsigned PSize() const;

		void InvalidBoundBox();

		//��ȡָ���������ɫ
		PointColor GetPointColor(unsigned index) const;

		//���һ�����ӹ�ϵ
		void AddLink(std::array<unsigned, 4> link);
		std::array<unsigned, 4> GetLink(unsigned index);

		//link num
		unsigned LinkSize();

		//���һ����ɫֵ
		void AddColor(PointColor color, unsigned index);

		//��ʼ����ɫ�ռ�
		bool ReserveTheRGBTable();

		//����ָ����������ɫ������
		PointColor& GetColor(const unsigned index);

		//���ù���������ͼƬ
		void SetImage(QImage& image, const QString& tfwFile = "");

		void BindTargetTexture(QImage& image);

		//!\ �����������
		void AddTexCoord(DCVector2D texCoord);

		//!\ ��������
		bool HasTextures();

		void CancelTexture();

		//����ת��
		void ApplyTransform(EigenMatrix4d& transMatrix);
	private:

		//����
		void DrawMyselfOnly(DcGpDrawContext& context);
		//�������񶥵�������������
		unsigned GetNumberByIndex(Index index);
		//��������������������
		Index GetIndexByNumber(unsigned number);

		//!\brief ����context��sfColorScaleToDisplay����ֵ
		void AddColorBarInfo(DcGpDrawContext& context /*<[in] �����Ļ��Ʋ��� */);
	protected:
		DcGpDEM();	
	private slots:

		void ChangeShowStyle(int style);
	};
}
#endif