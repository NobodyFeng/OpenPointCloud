//C++
#include <set>

//Qt
#include "QFontMetrics"
#include "QObject"

//DcGp
#include "DCGp/Custom/GpScalarField.h"
#include "DCGp/GpBasicTypes.h"

//�Զ���
#include "ViewRenderingTool.h"
#include "Viewer.h"

#define max(a,b) (((a) > (b)) ? (a) : (b))

using namespace MPViewer;

void ViewRenderingTool::DrawColorBar(const DcGp::DcGpDrawContext& context )
{
	DcGp::DcGpScalarField* scalarField = context.sfColorScaleToDisplay;

	if (!scalarField || !scalarField->GetColorScale())
	{
		return;
	}

	//�жϵ�ǰcontext�еĴ����Ƿ����
	Viewer* win = static_cast<Viewer* >(context._win);
	if (!win)
	{
		return;
	}

	//����set����,���뵱ǰ����������С�����ֵ
	std::set<double> keyValues = scalarField->GetScaleRange();
	//keyValues.insert(238.0);
	//keyValues.insert(239.0);

	//��������
	QFontMetrics strMetrics(win->font());

	//����Ĭ����ɫ
	const unsigned char tc[3] = {255,255,255};
	const unsigned char* textColor = DcGp::Color::DEFAULTLABELCOLOR;

	//histogram?
	//const::ccScalarField::Histogram histogram = sf->getHistogram();
	//bool showHistogram = (ccGui::Parameters().colorScaleShowHistogram && !logScale && histogram.maxValue != 0 && histogram.size() > 1);

	//��ʾ��������
	const int strHeight = strMetrics.height();
	const int scaleWidth = 50;
	const int scaleMaxHeight = (keyValues.size() > 1 ? max(context.glH-140,2*strHeight) : scaleWidth); //if 1 value --> we draw a cube

	//���Ļ�ƽ��ͶӰ
	int halfW = (context.glW>>1);  //����λ�������൱�� /2
	int halfH = (context.glH>>1);

	//���Ͻ�ƫ����
	const int xShift = 20 + (false ? scaleWidth/2 : 0);
	const int yShift = halfH - max(context.glH - 140,2 * strHeight) / 2 - 10;


	glPushAttrib(GL_LINE_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_LINE_SMOOTH);
	glDisable(GL_DEPTH_TEST);

	std::vector<double> sortedKeyValues(keyValues.begin(),keyValues.end());
	double maxRange = sortedKeyValues.back()-sortedKeyValues.front();

	const ColorType* lineColor = textColor;

	//������ɫ��
	{
		//���½���ʼ������
		int x = halfW-xShift-scaleWidth;
		int y = halfH-yShift-scaleMaxHeight;

		if (keyValues.size() > 1)
		{
			glLineWidth(1.0f);
			glBegin(GL_LINES);
			for (int j=0; j<scaleMaxHeight; ++j)
			{
				double value = sortedKeyValues.front() + ((double)j * maxRange) / (double)scaleMaxHeight;
				const RGBAColor col = scalarField->GetColor(static_cast<ScalarType>(value));
				ColorType color[4] = {col[0], col[1], col[2], col[3]};
				glColor3ubv(!col.empty() ? color : DcGp::Color::LIGHTGREY);

				glVertex2i(x,y+j);
				glVertex2i(x+scaleWidth,y+j);
			}
			glEnd();
		}

		//�߿�����
		glLineWidth(2.0);
		glColor3ubv(lineColor);
		glBegin(GL_LINE_LOOP);
		glVertex2i(x,y);
		glVertex2i(x+scaleWidth,y);
		glVertex2i(x+scaleWidth,y+scaleMaxHeight);
		glVertex2i(x,y+scaleMaxHeight);
		glEnd();
	}

	//���ƿ̶Ⱥ����֣�����
	{
		VlabelSet labelList;

		//��ʼ�̶ȱ�ǩ
		labelList.push_back(Vlabel(0,0,strHeight,sortedKeyValues.front()));

		//������ֹ�̶ȱ�ǩ
		if (sortedKeyValues.size() > 1)
		{
			labelList.push_back(Vlabel(scaleMaxHeight,scaleMaxHeight-strHeight,scaleMaxHeight,sortedKeyValues.back()));
		}
		
		//�ݹ鴴����ǩ
		if (sortedKeyValues.size() > 1)
		{
			//�ı�label���
			const int minGap = strHeight * 2;

			//��ʱ����
			unsigned drawnLabelsBefore = 0; //����ѭ��
			unsigned drawnLabelsAfter = labelList.size(); 

			//����ֱ��û���µı�ǩ����
			while (drawnLabelsAfter > drawnLabelsBefore)
			{
				drawnLabelsBefore = drawnLabelsAfter;

				VlabelSet::iterator it1 = labelList.begin();
				VlabelSet::iterator it2 = it1; it2++;
				for (; it2 != labelList.end(); ++it2)
				{
					if (it1->yMax + 2*minGap < it2->yMin)
					{
						//�����ǩ
						double val = (it1->val + it2->val)/2.0;
						int yScale = static_cast<int>((val-sortedKeyValues[0]) * (double)scaleMaxHeight / maxRange);

						//���뵽�����еĺ���λ�ã���˳���ŷ�
						labelList.insert(it2,Vlabel(yScale,yScale-strHeight/2,yScale+strHeight/2,val));
					}
					it1 = it2;
				}

				drawnLabelsAfter = labelList.size();
			}
		}

		//���Ʊ�ǩ
		glColor3f(0,1,0);

		//���Ʋ���ʾ����������
		//double index = scalarField->GetName().toDouble();
		//QString sfName = QObject::tr("Scalar Field #%1").arg(index);
		QString sfName = scalarField->GetName();
		if (!sfName.isEmpty())
		{
			//Ԥ����һЩ��϶
			win->Display2DLabel(sfName, context.glW-xShift+10, context.glH-yShift+strMetrics.height(), 4 | 8);
		}

		//��ʾ����
		const unsigned precision = 3;
		//format
		const char format = ('f');
		//tick
		const int tickSize = 4;

		//for labels
		const int x = context.glW-xShift-scaleWidth-2*tickSize-1;
		const int y = context.glH-yShift-scaleMaxHeight;
		//for ticks
		const int xTick = halfW-xShift-scaleWidth-tickSize-1;
		const int yTick = halfH-yShift-scaleMaxHeight;

		for (VlabelSet::iterator it = labelList.begin(); it != labelList.end(); ++it)
		{
			VlabelSet::iterator itNext = it; itNext++;
			//position
			unsigned char align = 4;
			if (it == labelList.begin())
				align |= 8;
			else if (itNext == labelList.end())
				align |= 32;
			else
				align |= 16;

			double value = it->val;

			win->Display2DLabel(QString::number(value,format,precision), x, y+it->yPos, align);
			glBegin(GL_LINES);
			glVertex2f(xTick,yTick+it->yPos);
			glVertex2f(xTick+tickSize,yTick+it->yPos);
			glEnd();
		}
	}

	glPopAttrib();
}