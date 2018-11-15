#ifndef DC_VIEW_GLFILTER_H
#define DC_VIEW_GLFILTER_H

#include "ViewDLL.h"

//boost
#include "boost/shared_ptr.hpp"

//Qt
#include "QtCore/QString"

namespace DCView
	{
		class GLFilter;
		typedef boost::shared_ptr<GLFilter> GLFilterPtr;

		class DC_VIEW_API GLFilter
		{
		public:
			struct ViewportParameters
			{
				//! Default constructor
				ViewportParameters()
					: zoom(1.0)
					, perspectiveMode(false)
					, zNear(0.0)
					, zFar(1.0)
				{}

				double zoom;
				//�Ƿ�͸��ģʽ
				bool perspectiveMode;
				//��ƽ���͸��ģʽ
				double zNear;
				//Զƽ���͸��ģʽ
				double zFar;
			};
		public:
			//��������
			virtual ~GLFilter() {}

			//��¡����
			virtual GLFilterPtr Clone() const = 0;

			//��ʼ��GLFilter
			virtual bool Init(int width, int height, const QString& shadersPath, QString& error) = 0;

			//��filterӦ�õ�������
			virtual void Shade(unsigned texDepth, unsigned texColor, ViewportParameters& parameters) = 0;

			//����ID
			virtual unsigned Texture() const = 0;

			//filter����
			inline QString Name() const { return m_name; }

		protected:
			//���캯��
			GLFilter(const QString& strName)
				: m_name(strName){}

		private:
			//����
			QString m_name;
		};
	}

#endif
