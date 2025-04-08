/**
 * Petite application test mettant en scène une chambre d'expérience avec des
 * pointages laser.
 */
#include "QtViewPointWindow.h"
#include <QtVtk/QtVtk.h>

#include <vtkInteractorStyleTrackball.h>
#include <TkUtil/Exception.h>

#include <QApplication>

#include <iostream>


using namespace TkUtil;
using namespace std;


int main (int argc, char* argv[])
{
	try
	{
		QApplication*	application	= new QApplication (argc, argv);
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
