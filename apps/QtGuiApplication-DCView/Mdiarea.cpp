/*!
 * \file mdiarea.cpp
 * \brief MdiArea file.
 * \author Martial TOLA, CNRS-Lyon, LIRIS UMR 5205
 * \date 2010
 */
#include "Mdiarea.h"

#include "QMimeData"
#include "QPainter"
#include "QFileInfo"
#include "QUrl"

#include "MyClass.h"
#include "DCView/Viewer.h"

MdiArea::MdiArea(QWidget *parent) : QMdiArea(parent)
{
	bType = bNone;
}

void MdiArea::paintEvent(QPaintEvent *paintEvent)
{
	QMdiArea::paintEvent(paintEvent);

	// and now only paint your image here
	QPainter painter(viewport());
 
	painter.fillRect(paintEvent->rect(), QColor(23, 74, 124));
	//painter.drawImage(paintEvent->rect()/*QPoint(0, 0)*/, QImage("./mepp_background.bmp"));
 
	painter.end();
}

void MdiArea::dragEnterEvent(QDragEnterEvent *event)
{
	if (event->mimeData()->hasUrls())	//hasFormat("text/uri-list")
	{
		event->acceptProposedAction();

// ---------------------------------------------------
#ifdef __linux__
	bType=bLeft;
	if (event->mouseButtons() & Qt::RightButton)
		bType=bRight;
#endif
// ---------------------------------------------------
	}
}

void MdiArea::dropEvent(QDropEvent *event)
{
	int res = 0;
	MPViewer::Viewer* viewer = NULL;
	QList<QUrl> urls = event->mimeData()->urls();

// ---------------------------------------------------
#ifdef __APPLE__
	bType=bLeft;
	if (event->keyboardModifiers() & Qt::MetaModifier)
		bType=bRight;
#endif
#ifdef _MSC_VER
	bType=bLeft;
	if (event->mouseButtons() & Qt::RightButton)
		bType=bRight;
#endif
// ---------------------------------------------------

	for (int i=0; i<urls.size(); i++)
	{
		QFileInfo fi(urls[i].toLocalFile());
		QString ext = fi.suffix();

		if ( (ext.toLower()=="off") || (ext.toLower()=="obj") || (ext.toLower()=="txt") || (ext.toLower()=="ply")
#ifdef WITH_ASSIMP
				|| (ext.toLower()=="dae") || (ext.toLower()=="3ds") || (ext.toLower()=="lwo")
#endif
				)
		{
			viewer = static_cast<MPViewer::Viewer*>(m_mw->activeMdiChild());
			if (m_mw->activeMdiChild() == 0)
				res = m_mw->loadFile(urls[i].toLocalFile(), viewer);
			else
			{
				if (bType == bLeft)
					res = m_mw->loadFile(urls[i].toLocalFile(), viewer);
				else if (bType == bRight)
				{
	#ifdef __linux__
					if (event->keyboardModifiers() & Qt::MetaModifier)
	#else
					if (event->keyboardModifiers() & Qt::AltModifier)
	#endif
						res = m_mw->addFile(viewer, urls[i].toLocalFile());
					else
						res = m_mw->addFile(viewer, urls[i].toLocalFile());
				}
			}

			if (res)
				break;

			//m_mw->writeSettings();
			//m_mw->readSettings();
		}
	}
	if (viewer)
		viewer->updateGL();;

	event->acceptProposedAction();
}