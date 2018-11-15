#include "TrackballManipulator.h"

//qt
#include "QtWidgets/QApplication"
#include "QtGui/QMouseEvent"
#include "QtGui/QKeyEvent"

//DCgp
#include "DCGp/GpEntity.h"
#include "DCGp/Custom/GpBoundBox.h"

#include "DCCore/Const.h"
#include "DCCore/Logger.h"

#include "DCCore/Vector4D.h"

#include "DCCore/Matrix2.h"
#include "DCCore/Matrix3.h"
#include "DCCore/Matrix4.h"

//DCView
#include "DCView/View.h"
#include "DCView/GLToolkit.h"
#include "DCView/Parameters.h"
#include "DCView/Camera.h"

using namespace DCGa;

TrackballManipulator::TrackballManipulator(QString name)
	: DCCore::GUIEventHandle(name)
	, m_RotationSpeed(1.0f)
	, m_TranslationSpeed(1.0f)
	, m_ZoomSpeed(1.0f)
{
	SetValid(true, 0);
}

TrackballManipulator::~TrackballManipulator()
{

}

void TrackballManipulator::SetValid(bool isValid, DCView::View* view)
{
	DCCore::GUIEventHandle::SetValid(isValid);

	if (!view)
	{
		return;
	}
	view->SetContinuousUpdate(false);
}

void TrackballManipulator::mousePressEvent(QMouseEvent* event, DCView::View* view)
{
	if (view->GetMatrixPiple() == DCView::eOpenGL)
	{
		//ƽ��
		{
			//��ȡ��ϼ�
			DCView::GlobalParameters::CompositeKey key = 
				DCView::GlobalParameters::GetCompositeKey(DCView::GlobalParameters::eTranslateOperation);

			if ((event->buttons() & key.buttons) 
				&& ((QApplication::keyboardModifiers() & key.keys) 
				|| (QApplication::keyboardModifiers() == key.keys)))
			{
				//��¼���λ��
				m_lastPosOfMouse = event->pos();
				//m_lodActivated = true;

				//�������ͼ��
				QApplication::setOverrideCursor(QCursor(Qt::SizeAllCursor));
			}
		}

		//��ת
		{
			//��ȡ��ϼ�
			DCView::GlobalParameters::CompositeKey key = 
				DCView::GlobalParameters::GetCompositeKey(DCView::GlobalParameters::eRotateOperation);

			if ((event->buttons() & key.buttons) 
				&& ((QApplication::keyboardModifiers() & key.keys) 
				|| (QApplication::keyboardModifiers() == key.keys)))
			{
				//��¼���ĵ�ǰ����
				m_lastOrientationOfMouse = view->MouseOrientation(event->x(), event->y());
			}
		}

		//��¼���λ��
		m_lastPosOfMouse = event->pos();
	}
	else
	{
		if ( view->GetCamera() == NULL )
			return;
		
		m_Pivot = view->GetViewportParameters().pivotPos;

		//��¼���λ��
		m_lastPosOfMouse = event->pos();

		m_StartMatrix = view->GetCamera()->modelingMatrix();

		m_StartCameraPos = view->GetCamera()->modelingMatrix().getT();
		m_StartPivot = m_Pivot;
	}
}

void TrackballManipulator::mouseMoveEvent(QMouseEvent* event, DCView::View* view)
{
	
	//�������Ƴ����壬�򲻽����κβ���
	if (event->x() <= 1 
		|| event->y() <= 1 
		|| event->x() >= view->width() - 1 
		|| event->y() >= view->height() - 1)
	{
		return;
	}

	//�������λ�õ��ƶ���
	int iDetX = event->x() - m_lastPosOfMouse.x();
	int iDetY = event->y() - m_lastPosOfMouse.y();

	float dis = sqrt(float(iDetX * iDetX + iDetY * iDetY));

	if (abs(iDetX)  > abs(iDetY))
	{
		//! ��xΪ׼
		if (iDetX < 0)
		{
			dis = dis * (-1);
		}
	}
	else
	{
		//!��yΪ׼
		if (iDetY < 0)
		{
			dis = dis * (-1);
		}
	}

	

	if (view->GetMatrixPiple() == DCView::eOpenGL)
	{
		//ƽ��
		{
			//��ȡ��ϼ�
			DCView::GlobalParameters::CompositeKey key = 
				DCView::GlobalParameters::GetCompositeKey(DCView::GlobalParameters::eTranslateOperation);

			if ((event->buttons() & key.buttons)
				&& ((QApplication::keyboardModifiers() & key.keys) 
				|| (QApplication::keyboardModifiers() == key.keys)))
			{
				//���ش�С
				float pixSize = view->ComputeActualPixelSize();
				//�ƶ�����
				DCView::Vector_3 v(static_cast<float>(iDetX) * pixSize, -static_cast<float>(iDetY) * pixSize, 0);

				if (view->GetViewportParameters().objectCentric)
				{
					//inverse displacement in object-based mode
					v *= -1.0;
				}	
				view->SetInterActionMode(DCView::View::ePan);
				//�ƶ����
				view->MoveCamera(v);

				view->Refresh();
			}
		}

		//!����
		//��ȡ��ϼ�
		DCView::GlobalParameters::CompositeKey key = 
			DCView::GlobalParameters::GetCompositeKey(DCView::GlobalParameters::eTranslateOperation);
		if (event->buttons()== Qt::RightButton)
		{
			//͸��ͶӰģʽ
			if (view->GetViewportParameters().projectionMode == DCView::eProjPerspective)
			{
				//�Ƕȵ�����ת������
				float pixSize = view->ComputeActualPixelSize();
				view->MoveCamera(DCView::Vector_3(0.0f,0.0f,-(dis) * pixSize));


				view->SetInterActionMode(DCView::View::eZoom);
				view->Refresh();
			}
			//����ͶӰģʽ
			else
			{
				//�������Ų���
				static const float fFactor = 20.0f;
				float fScale = pow(1.1f, ( dis) / fFactor);

				//����
				view->Scale(fScale);

				view->SetInterActionMode(DCView::View::eZoom);
				view->Refresh();
			}
		}
		

		//��ת
		{
			//��ȡ��ϼ�
			DCView::GlobalParameters::CompositeKey key = 
				DCView::GlobalParameters::GetCompositeKey(DCView::GlobalParameters::eRotateOperation);

			if ((event->buttons() & key.buttons) 
				&& ((QApplication::keyboardModifiers() & key.keys) 
				|| (QApplication::keyboardModifiers() == key.keys)))
			{
				//��¼��ǰ��귽��
				m_currentOrientationOfMouse = view->MouseOrientation(event->x(), event->y());

				//��ת����
				DCView::Matrix_4_4 matrixRotate = DCView::GLToolkit::CalculateRotationMatrix(m_lastOrientationOfMouse,
					m_currentOrientationOfMouse);

				//������һ�ε���귽��
				m_lastOrientationOfMouse = m_currentOrientationOfMouse;

				//����FBO
				//m_bUpdatedFBO = true;

				//��ת��ͼ
				view->RotateView(matrixRotate);
				//������άģʽ
				view->SetInterActionMode(DCView::View::eRotate);
				view->EnablePivot(true);

				//���������ʽ
				QApplication::changeOverrideCursor(QCursor(Qt::ClosedHandCursor));

				view->Refresh();
			}
		}

		m_lastPosOfMouse = event->pos();
	}
	else
	{
		if ( view->GetCamera() == NULL )
			return;

		//ƽ��
		{
			//��ȡ��ϼ�
			DCView::GlobalParameters::CompositeKey key = 
				DCView::GlobalParameters::GetCompositeKey(DCView::GlobalParameters::eTranslateOperation);

			if ((event->buttons() & key.buttons)
				&& ((QApplication::keyboardModifiers() & key.keys) 
				|| (QApplication::keyboardModifiers() == key.keys)))
			{
				if (view->GetProjection() == DCView::eProjPerspective)
				{
					float tx = (m_lastPosOfMouse.x() - event->x())  / 400.0f;
					float ty = -(m_lastPosOfMouse.y() - event->y()) / 400.0f;
					tx *= m_TranslationSpeed;
					ty *= m_TranslationSpeed;
					float distance = (m_StartCameraPos - m_Pivot).Length();
					DCVector3D up    = view->GetCamera()->modelingMatrix().getY();
					DCVector3D right = view->GetCamera()->modelingMatrix().getX();
					m_Pivot = m_StartPivot + up*distance*ty + right*distance*tx;
					view->SetInterActionMode(DCView::View::ePan);
					view->UpdateCameraPos(m_StartCameraPos + up*distance*ty + right*distance*tx);

					view->Refresh();
				}
				else
				{
					//���ش�С
					float pixSize = view->ComputeActualPixelSize();
					//�ƶ�����
					DCView::Vector_3 v(static_cast<float>(iDetX) * pixSize, -static_cast<float>(iDetY) * pixSize, 0);

					if (view->GetViewportParameters().objectCentric)
					{
						//inverse displacement in object-based mode
						v *= -1.0;
					}	
					view->SetInterActionMode(DCView::View::ePan);
					//�ƶ����
					view->UpdateCameraPos(m_StartCameraPos + v);

					view->Refresh();
				}
				
			}
		}

		//��ת
		{
			//��ȡ��ϼ�
			DCView::GlobalParameters::CompositeKey key = 
				DCView::GlobalParameters::GetCompositeKey(DCView::GlobalParameters::eRotateOperation);

			if ((event->buttons() & key.buttons) 
				&& ((QApplication::keyboardModifiers() & key.keys) 
				|| (QApplication::keyboardModifiers() == key.keys)))
			{
				DCCore::mat4 rt = trackballRotation(event->x(),event->y(), view);
				view->GetCamera()->setModelingMatrix( DCCore::mat4::getTranslation(m_Pivot) * rt * DCCore::mat4::getTranslation(-m_Pivot) * m_StartMatrix );
				m_StartMatrix = view->GetCamera()->modelingMatrix();
				
				DCView::Matrix_4_4 matrixRotate;
				for (int i = 0; i != 16; ++i)
				{
					matrixRotate.data()[i] = rt.ptr()[i];
				}
				view->RotateView(matrixRotate);

				//������ת����
				view->SetInterActionMode(DCView::View::eRotate);
				view->EnablePivot(true);

				//���������ʽ
				QApplication::changeOverrideCursor(QCursor(Qt::ClosedHandCursor));

				m_lastPosOfMouse = event->pos();

				view->DisableModelView();

				view->Refresh();
			}
		}

		//!����

	}
}

void TrackballManipulator::mouseReleaseEvent(QMouseEvent* event, DCView::View* view)
{
	//�������״̬
	QApplication::restoreOverrideCursor();

	//ͣ����ת����
	//view->EnablePivot(false);
	view->SetInterActionMode(DCView::View::eNoInter);

	if (view)
	{
		view->Refresh();
	}
	
}

void TrackballManipulator::keyPressEvent(QKeyEvent* event, DCView::View* view)
{

}

void TrackballManipulator::wheelEvent(QWheelEvent* event, DCView::View* view)
{
	
	if (view->GetMatrixPiple() == DCView::eOpenGL)
	{
		//view->SetInterActionMode(DCView::View::eZoom);
		//�������ת���ĽǶ�
		float fDegree = static_cast<float>(event->delta()) / 8.0f;
		view->RotatingWheel(fDegree);
		view->Refresh();
	}
	else
	{
		if (view->GetProjection() == DCView::eProjPerspective)
		{
			m_StartCameraPos = view->GetCamera()->modelingMatrix().getT();

			//�������ת���ĽǶ�
			float fDegree = static_cast<float>(event->delta()) / 8.0f;
			float t = fDegree * view->ComputeActualPixelSize() / 50.0;
			t *= m_ZoomSpeed;
			float distance = (m_StartCameraPos - m_Pivot).Length();
			DCVector3D camera_pos = m_StartCameraPos - view->GetCamera()->modelingMatrix().getZ()*t*distance;
			DCCore::mat4 m = view->GetCamera()->modelingMatrix();
			m.setT(camera_pos);
			view->GetCamera()->setModelingMatrix(m);
			view->UpdateCameraPos(camera_pos);
		}
		else 
		{
			//�������ת���ĽǶ�
			float fDegree = static_cast<float>(event->delta()) / 8.0f;

			view->RotatingWheel(fDegree);
		}
		
		view->DisableModelView();
	}

}

DCCore::mat4 TrackballManipulator::trackballRotation(int x, int y, DCView::View* view)
{
	if( x==m_lastPosOfMouse.x()  && y==m_lastPosOfMouse.y() )
		return DCCore::mat4();

	DCVector3D a = computeVector(m_lastPosOfMouse.x(), m_lastPosOfMouse.y(), view);
	DCVector3D b = computeVector(x, y, view);
	DCVector3D n = cross(a, b);
	n.normalize();
	a.normalize();
	b.normalize();
	float dot_a_b = dot(a,b);
	dot_a_b = clamp(dot_a_b,(float)-1.0,(float)+1.0);
	float alpha = acos(dot_a_b) * (-1);
	alpha = alpha * m_RotationSpeed;
	DCVector3D nc =  view->GetCamera()->modelingMatrix().get3x3() * n;

	nc.normalize();
	return DCCore::mat4::getRotation(alpha*(float)DC_RAD_TO_DEG, nc);
}
//-----------------------------------------------------------------------------
DCVector3D TrackballManipulator::computeVector(int x, int y, DCView::View* view)
{
	DCVector3D c(view->GetCamera()->viewport()->width() / 2.0f, view->GetCamera()->viewport()->height() / 2.0f, 0);

	float sphere_x = view->GetCamera()->viewport()->width()  * 0.5f;
	float sphere_y = view->GetCamera()->viewport()->height() * 0.5f;

	DCVector3D v((float)x,(float)y,0);
	v -= c;
	v.x /= sphere_x;
	v.y /= sphere_y;
	v.y = -v.y;
	//if (v.length() > 1.0f)
	//  v.normalize();

	float z2 = 1.0f - v.x*v.x - v.y*v.y;
	if (z2 < 0) 
		z2 = 0;
	v.z = sqrt( z2 );
	v.Normalize();
	return v;
}