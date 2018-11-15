#ifndef DCGP_DRAW_CONTEXT_H
#define DCGP_DRAW_CONTEXT_H

//c++
#include "cstdint"

//#include "DCCore/DCGL.h"
#include "DCGp/GpBasicDevice.h"
#include "DCGp/Custom/GpMaterial.h"

namespace DcGp
{
	class DcGpScalarField;
	class DcGpBasicDevice;
	//��άʵ��Ļ��Ʋ���
	class glDrawParameters
	{
	public:
		//! ��ʾ������
		bool showScalarField;
		//�Ƿ���ʾ��ɫ
		bool showColors;
		//�Ƿ���ʾ����
		bool showNormals;
	};

	//����ʾ������
	class DcGpDrawContext
	{
	public:
		uint16_t flags;       //����ѡ��(see below)
		int glW;                    //Opengl���ڿ��
		int glH;                    //Opengl���ڸ߶�
		DcGpBasicDevice* _win;   //Opengl����ָ��

		//Ĭ�ϲ���
		DcGpMaterial defaultMat;
		float defaultMeshFrontDiff[4];
		float defaultMeshBackDiff[4];
		unsigned char pointsDefaultCol[3];
		unsigned char textDefaultCol[3];
		unsigned char labelDefaultCol[3];
		unsigned char bbDefaultCol[3];

		//�Ż���ѡ��
		bool decimateCloudOnMove;
		bool decimateMeshOnMove;

		//��ɫ���
		DcGpScalarField* sfColorScaleToDisplay;

		//�㲶׽
		float pickedPointsRadius;
		float pickedPointsTextShift;

		//�ı�
		unsigned dispNumberPrecision;

		//��ǩ
		unsigned labelsTransparency;

		DcGpDrawContext()
			: flags(0)
			, glW(0)
			, glH(0)
			, _win(0)
			, defaultMat()
			, decimateCloudOnMove(true)
			, decimateMeshOnMove(true)
			, sfColorScaleToDisplay(nullptr)
			, pickedPointsRadius(4)
			, pickedPointsTextShift(0.0)
			, dispNumberPrecision(6)
			, labelsTransparency(100)
		{}
	};

	// ���Ʊ�ʶ
#define DC_DRAW_2D                              0x0001
#define DC_DRAW_3D                              0x0002
#define DC_DRAW_FOREGROUND                      0x0004
#define DC_LIGHT_ENABLED                        0x0008
#define DC_SKIP_UNSELECTED                      0x0010
#define DC_SKIP_SELECTED                        0x0020
#define DC_SKIP_ALL                             0x0030	
#define DC_DRAW_ENTITY_NAMES                    0x0040
#define DC_DRAW_POINT_NAMES                     0x0080
#define DC_DRAW_TRI_NAMES						0x0100
#define DC_DRAW_FAST_NAMES_ONLY					0x0200
#define DC_DRAW_ANY_NAMES						0x03C0
#define DC_LOD_ACTIVATED                        0x0400
#define DC_VIRTUAL_TRANS_ENABLED                0x0800

	//���Ʊ�ʶ
#define MACRO_Draw2D(context) (context.flags & DC_DRAW_2D)							//�Ƿ��ǻ��ƶ�ά����
#define MACRO_Draw3D(context) (context.flags & DC_DRAW_3D)							//�Ƿ��ǻ�����ά����
#define MACRO_DrawPointNames(context) (context.flags & DC_DRAW_POINT_NAMES)
#define MACRO_DrawTriangleNames(context) (context.flags & DC_DRAW_TRI_NAMES)
#define MACRO_DrawEntityNames(context) (context.flags & DC_DRAW_ENTITY_NAMES)
#define MACRO_DrawNames(context) (context.flags & DC_DRAW_ANY_NAMES)
#define MACRO_DrawFastNamesOnly(context) (context.flags & DC_DRAW_FAST_NAMES_ONLY)
#define MACRO_SkipUnselected(context) (context.flags & DC_SKIP_UNSELECTED)
#define MACRO_SkipSelected(context) (context.flags & DC_SKIP_SELECTED)
#define MACRO_LightIsEnabled(context) (context.flags & DC_LIGHT_ENABLED)			//�Ƿ�ʹ�õƹ�
#define MACRO_Foreground(context) (context.flags & DC_DRAW_FOREGROUND)
#define MACRO_LODActivated(context) (context.flags & DC_LOD_ACTIVATED)
#define MACRO_VirtualTransEnabled(context) (context.flags & DC_VIRTUAL_TRANS_ENABLED)
}

#endif