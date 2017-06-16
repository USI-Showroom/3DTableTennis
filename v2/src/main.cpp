#include <gl\glew.h>
#include <QApplication>
#include <QGLFormat>
#include "GLRender.h"
#include "Defines.h"

#include <qglviewer.h>

//int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

	QGLFormat format;
	QGLContext context(format);

#ifdef STEREO_PING_PONG
    format.setStereo(true);
    //format.setOverlay(true);
    format.setAlpha(true);
#endif
	format.setSampleBuffers(true);	
	format.setSamples(8);
    QGLFormat::setDefaultFormat(format);
	
	context.create();
	context.makeCurrent();

	/*
		_AG:
		setActiveWindow inpujt is a QWidget

		bug: GLRender -> QWidget
	*/

	QWidget *viewer = new GLRender();
    app.setActiveWindow( viewer );
    viewer->show();

	return app.exec();
}
//! [0]
