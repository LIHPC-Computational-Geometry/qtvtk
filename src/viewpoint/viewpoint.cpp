/**
 * Petite application test mettant en scène une chambre d'expérience avec des
 * pointages laser.
 */
#include "QtViewPointWindow.h"
#include <QtVtk/QtVtk.h>

#include <vtkInteractorStyleTrackball.h>
#include <TkUtil/Exception.h>

#include <QApplication>
#include <QSurfaceFormat>

#include <iostream>


using namespace TkUtil;
using namespace std;


int main (int argc, char* argv[])
{
	try
	{
		QApplication*	application	= new QApplication (argc, argv);
#if (VTK_MAJOR_VERSION >= 9)
		QSurfaceFormat fmt;
		fmt.setRenderableType(QSurfaceFormat::OpenGL);
		fmt.setVersion(3, 2);
		fmt.setProfile(QSurfaceFormat::CoreProfile);
		fmt.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
		fmt.setRedBufferSize(8);
		fmt.setGreenBufferSize(8);
		fmt.setBlueBufferSize(8);
		fmt.setDepthBufferSize(24);
		fmt.setStencilBufferSize(8);
		fmt.setAlphaBufferSize(0);
		fmt.setStereo(false);
		fmt.setSamples(0);//vtkOpenGLRenderWindow::GetGlobalMaximumNumberOfMultiSamples());
		QSurfaceFormat::setDefaultFormat (fmt);
#endif	// #if (VTK_MAJOR_VERSION >= 9)
		QtVtk::initialize ( );
		QtViewPointWindow*	mainWindow	= new QtViewPointWindow ( );
		mainWindow->show ( );
		return application->exec ( );
	}
	catch (const Exception& e)
	{
		cerr << "ERREUR : " << e.getFullMessage ( ) << endl;
		return -1;
	}
	catch (...)
	{
		cerr << "ERREUR non documentée." << endl;
		return -2;
	}
}	// main
