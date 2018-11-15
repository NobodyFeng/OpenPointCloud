#ifndef PDBIO_H
#define PDBIO_H

/*#include <vld.h>*/

//Qt
#include "QString"
#include "QStringList"
#include "QThread"
#include "QChar"

//IO
#include "DCCore/IoPlugin.h"

namespace DcGp
{
	class PageLodEntity;
}

//����ÿ������
enum DataTypePerColumn
{
	IGNORED = 0,
	COORD_X,
	COORD_Y,
	COORD_Z,
	RGB_RED,
	RGB_GREEN,
	RGB_BLUE,
	GRAY,
	SCALAR,
	NORMAL_X,
	NORMAL_Y,
	NORMAL_Z
};


struct DataColumnType
{
	int xCoordIndex;
	int yCoordIndex;
	int zCoordIndex;
	int rgbRedIndex;
	int rgbGreenIndex;
	int rgbBlueIndex;
	int grayIndex;
	QList<unsigned> scalarIndex;
	int xNormalIndex;
	int yNormalIndex;
	int zNormalIndex;

	//�Ƿ��������ֵ
	bool m_hasRGBColor;
	//�Ƿ��������ֵ
	bool m_hasNormals;
	//��Ч��������
	unsigned m_validTypeNumbers;


	//��ʼ��
	DataColumnType()
		: xCoordIndex(-1)
		, yCoordIndex(-1)
		, zCoordIndex(-1)
		, rgbRedIndex(-1)
		, rgbGreenIndex(-1)
		, rgbBlueIndex(-1)
		, grayIndex(-1)
		, m_validTypeNumbers(0)
		, m_hasNormals(false)
		, m_hasRGBColor(false)
		, xNormalIndex(-1)
		, yNormalIndex(-1)
		, zNormalIndex(-1)
	{};

	//�Ƿ�ӵ��RGB��ɫ
	bool HasRGBColors() const
	{
		return m_hasRGBColor;
	}
	//�Ƿ�ӵ������
	bool HasNormals() const
	{
		return m_hasNormals;
	}
	//��ȡ��Ч����������
	unsigned ValidTypeNumber() const
	{
		return m_validTypeNumbers;
	}
};

//����һ��ÿ�����ݵĽṹ;
struct PerLineData
{
	//X��Y��Z����;
	double xCoord;
	double yCoord;
	double zCoord;

	//RGBֵ;
	int rgbRed;
	int rgbGreen;
	int rgbBlue;

	//�Ҷ�ֵ;
	int gray;

	//δ�������;
	QStringList undefineScalar;

	//���Ե�ֵ;
	QStringList ignoreValue;
};


namespace DcIo
{
class DCCORE_EXPORT PdbIO : public DCCore::IoPlugin
{
	Q_OBJECT
public:
	PdbIO();

//#define SUFFIX "txt";

	static IoPlugin* Instance(const char* extension);

	static QStringList SUPPORT_EXTENSION;

	QString GetName();

	virtual DcGp::DcGpEntity* LoadFile(const QString& filename,
		                                DCCore::CallBackPos* cb = 0, 
										bool alwaysDisplayLoadDialog = true,
										bool* coordinatesShiftEnabled = 0,
										double* coordinatesShift = 0,
										QString kind = ""
										);

	void SetBoxInfo(DcGp::PageLodEntity* node, QString qFilename, bool isRootFile/* = false*/) const;
	DcGp::PageLodEntity* LoadCloudFromFormatedAsciiFile(const QString& fileName,
													const DataColumnType type,
													QChar separator,
													unsigned approximateNumberOfLines,
													qint64 fileSize,
													unsigned maxCloudSize,
													 DCCore::CallBackPos* cb = 0, 
													int sampleRate = 1,
													unsigned skipLines=0,
													bool alwaysDisplayLoadDialog=true,
													bool* coordinatesShiftEnabled=0,
													double* coordinatesShift=0);

	virtual bool SaveToFile(DcGp::DcGpEntity* entity, QString filename);

	virtual void UnloadInstance();
protected:
	~PdbIO();

signals:
	void Notify(unsigned);
private:
	static DCCore::IoPlugin* s_instance;
};

}

DCCORE_EXPORT DCCore::IoPlugin* LoadPlugin(const char* extension);
DCCORE_EXPORT void UnloadPlugin();
#endif // PDBIO_H
