/*########################################################################
								DcDs
					��Ȩ����	����������ά�Ƽ����޹�˾
							2014��05��28��

						����Openggl���ڣ�ʵ�ָ�����ʾ

########################################################################*/


#ifndef DCDS_RENDERING_TOOL_H
#define DCDS_RENDERING_TOOL_H

//C++
#include <list>

#include "DCGp/GpDrawContext.h"

namespace MPViewer
{
	//!\brief �ṹ���������ڱ�ǩ�ķ���λ����Ϣ
	struct Vlabel
	{
		int yPos; 		/**< label center pos **/
		int yMin; 		/**< label 'ROI' min **/
		int yMax; 		/**< label 'ROI' max **/
		double val; 	/**< label value **/

		//!\ Ĭ�Ϲ��캯��
		Vlabel(int y, int y1, int y2, double v) : yPos(y), yMin(y1), yMax(y2),val(v) { assert(y2>=y1); }
	};

	//!\brief ��ǩ����
	typedef std::list<Vlabel> VlabelSet;

	//!\brief DcDs�����ռ�

	//!\brief ������Ⱦ����
	class ViewRenderingTool
	{
	public:

		//!\brief ������ɫ��
		static void DrawColorBar(const DcGp::DcGpDrawContext& context /*<[in] �����Ļ��Ʋ���*/);
	};
}

#endif
