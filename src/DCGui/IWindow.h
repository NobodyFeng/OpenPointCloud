//##########################################################################
//#                                                                        #
//#                            DCLW                                        #
//#                                                                        #
//#  This library is free software; you can redistribute it and/or modify  #
//#  it under the terms of the GNU Lesser General Public License(LGPL) as  #
//#  published by the Free Software Foundation; version 2 of the License.  #
//#                                                                        #
//#  This library is distributed in the hope that it will be useful,       #
//#  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
//#  GNU Lesser General Public License for more details.                   #
//#                                                                        #
//#          COPYRIGHT: DCLW             																	 #
//#                                                                        #
//##########################################################################

/*==========================================================================
*�����ܸ�����IWindow��Ҫ���������Ϊ���࣬���ز��������ɲ˵�����������
������壻����̨�Լ������У����Դ��ڵ�
*
*���ߣ�dclw         ʱ�䣺$time$
*�ļ�����$safeitemname$
*�汾��V4.1.0
*
*�޸��ߣ�          ʱ�䣺
*�޸�˵����
*===========================================================================
*/

#ifndef DCGUI_IWINDOW_H
#define DCGUI_IWINDOW_H

//Qt
#include "QMainWindow"

#include "dcguiDLL.h"

namespace DCGui
{
	class UI_IWindow;
	class DCGUI_EXPORT IWindow : public QMainWindow
	{
		Q_OBJECT
	public:
		IWindow(QWidget *parent = 0, Qt::WindowFlags flags = 0);
		virtual ~IWindow();

		/******************************�ӿ�****************************************/
		QMenuBar* GetMenuBar() const;

		bool ConfigInit(QMainWindow* pInter);
		void ConfigFinish(QMainWindow* pInter);

		//! ��ȡmenu��ͷ�Ĳ˵���Ŀ����
		int GetRootMenuNum() const;

		//! ��ȡmenu��ͷ�Ĳ˵���ָ��ID��Ӧ��action����������˵���ʱ����Ϊ����˵�λ��ʹ��
		QAction* GetAction(const int ID);

		QString GetTitle();
		QString GetOrg();
	private:
		//��������
		void ConfigSplash();
		void ConfigStyleSheet();

		void ConfigUI();
		void ConfigTitleBar();
		void ConfigMenu();

		//setting ����
		//! ����������ò���
		void SaveSettings();

		//! ���ؼ�¼������
		void LoadSettings();

		private slots:
			//void on_actionOpen_triggered();

	private:
		/***********************************·������(dir�����Ķ��������ָ���)*****************************************************/
		QString m_appDir;

		/***********************************UI*************************************************/
		UI_IWindow* m_pUI;

		//�����ƼӰ汾��
		QString m_org;
		QString m_title;
	};
}

#endif // IWINDOW_H
