#ifndef DCGP_MATERIAL_H
#define DCGP_MATERIAL_H

#include "DCGp/DcGpDLL.h"

//Qt
#include "QString"
class QImage;

#include "DCCore/DCDefs.h"

namespace DcGp
{
	DECLARE_SMARTPTR(DcGpMaterial)
	DECLARE_SMARTPTR(DcGpMaterialImpl)

	class DCGP_API DcGpMaterial
	{
		DECLARE_IMPLEMENT(DcGpMaterial)

	public:
		DcGpMaterial();
		//���캯��
		DcGpMaterial(QString name);

		//!���ƹ���
		DcGpMaterial(const DcGpMaterial& mtl);

		//! ���������
		void SetDiffuse(const float color[4]);

		//! ���ù�����(both front - 100% - and back - 80%)
		void SetShininess(float val);

		//! ����͸���� (all colors)
		void SetTransparency(float val);

		//! Ӧ�ò��� (OpenGL)
		void ApplyGL(bool lightEnabled, bool skipDiffuse) const;

		QString GetName() const;
		void SetName(QString name);

		//��ͼ
		QImage GetTexture() const;
		void SetTexture(QImage texture);

		float* GetDiffuseFront() const;
		void SetDiffuseFront(const float diffuse[4]);

		float* GetDiffuseBack() const;
		void SetDiffuseBack(const float diffuse[4]);

		float* GetAmbient() const;
		void SetAmbient(const float ambient[4]);

		float* GetSpecular() const;
		void SetSpecular(const float specular[4]);

		float* GetEmission() const;
		void SetEmission(const float emission[4]);

		float GetsShininessFront() const;
		void SetShininessFront(float shininessFront);

		float GetsShininessBack() const;
		void SetShininessBack(float shininessBack);

		unsigned GetTextureId() const;
		void SetTextureId(unsigned id);
	};
}

#endif // DCMATERIAL_H
