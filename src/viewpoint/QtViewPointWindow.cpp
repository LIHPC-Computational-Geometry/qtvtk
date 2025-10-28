#include "QtViewPointWindow.h"
#include <QtVtk/QtVtkViewPointToolBar.h>
#include <QtVtk/QtVTKPrintHelper.h>
#include <vtkInteractorStyleSwitch.h>
#include <TkUtil/Exception.h>
#include <TkUtil/MemoryError.h>
#include <TkUtil/Process.h>
#include <QtUtil/QtMessageBox.h>
#include <QtUtil/QtUnicodeHelper.h>
#include <QtUtil/QtCoordinatesDialog.h>
#include <VtkContrib/vtkTrihedron.h>
#include <VtkContrib/vtkTrihedronCommand.h>
#ifndef VTK_WIDGET
#include <vtkGenericOpenGLRenderWindow.h>
#endif  // VTK_WIDGET

#include <QApplication>
#include <QInputDialog>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>

#include <vtkRenderer.h>
#include <vtkPolyDataMapper.h>
#include <vtkCylinderSource.h>
#include <vtkSphereSource.h>
#include <vtkTransform.h>
#include <vtkTransformFilter.h>

#include <iostream>

using namespace TkUtil;
using namespace std;

static const TkUtil::Charset	charset ("àéèùô");

USE_ENCODING_AUTODETECTION


QtViewPointWindow::QtViewPointWindow ( )
	: QMainWindow ( ), _graphicWindow (0), _renderer (0), _renderWindow (0), _actors ( ), _surfaces ( )
{
	createGui ( );
	createMenu ( );
}	// QtViewPointWindow::QtViewPointWindow


void QtViewPointWindow::createGui ( )
{
	_graphicWindow	= new QtVtkGraphicWidget (this);
	_graphicWindow->setAttribute (Qt::WA_TranslucentBackground, true);	// Bonne gestion transparence
	setCentralWidget (_graphicWindow);
	_renderer		= vtkRenderer::New ( );
#ifndef VTK_WIDGET
	_renderWindow	= vtkGenericOpenGLRenderWindow::New ( );
#else  // VTK_WIDGET
	_renderWindow	= vtkRenderWindow::New ( );
	_renderWindow->Render ( );				// Pour bonne gestion transparence
#endif  // VTK_WIDGET
	_renderWindow->SetAlphaBitPlanes (1);	// Bonne gestion transparence
	_renderWindow->SetMultiSamples (0);		// Bonne gestion transparence
	_renderer->SetUseDepthPeeling (true);	// Bonne gestion transparence
	_renderer->SetBackground (1., 1., 1.);	// Blanc
//_renderer->SetBackground (0., 0., 0.);	// Noir
	_renderWindow->AddRenderer (_renderer);
	vtkRenderer*	trihedronRenderer	= vtkRenderer::New ( );
	trihedronRenderer->InteractiveOff ( );
	trihedronRenderer->SetViewport (0, 0, 0.2, 0.2);
	trihedronRenderer->SetLayer (1);
	vtkTrihedronCommand*	trihedronCommand	= vtkTrihedronCommand::New ( );
	_renderer->AddObserver (vtkCommand::StartEvent, trihedronCommand);
	trihedronCommand->SetRenderer (trihedronRenderer);
	vtkTrihedron*	trihedron	= vtkTrihedron::New ( );
	trihedron->PickableOff ( );
	trihedronRenderer->AddActor (trihedron);
	_renderWindow->SetNumberOfLayers (2);
	_renderWindow->AddRenderer (trihedronRenderer);
	_graphicWindow->SetRenderWindow (_renderWindow);
	vtkInteractorStyleSwitch*	interactorStyle	= vtkInteractorStyleSwitch::New ( );
	_graphicWindow->getInteractor ( )->SetInteractorStyle (interactorStyle);
	_graphicWindow->getInteractor ( )->SetRenderWindow (_renderWindow);
	_graphicWindow->setMinimumSize (800, 600);
	_graphicWindow->resize (800, 600);
}	// QtViewPointWindow::createGui


void QtViewPointWindow::createMenu ( )
{
	addToolBar (Qt::TopToolBarArea, new QtVtkViewPointToolBar (this, *_renderer->GetActiveCamera ( ), _renderer));
	QMenuBar*	menubar		= menuBar ( );

	QMenu*		fileMenu	= new QMenu ("&Fichier", menubar);
	menubar->addMenu (fileMenu);
	fileMenu->setToolTipsVisible (true);
	QAction*	action	= new QAction ("Im&primer ...", this);
	connect (action, SIGNAL (triggered( )), this, SLOT(printCallback ( )));
	fileMenu->addAction (action);
	action	= new QAction ("Im&primer dans un fichier ...", this);
	connect (action, SIGNAL (triggered( )), this, SLOT(printToFileCallback ( )));
	fileMenu->addAction (action);
	fileMenu->addSeparator ( );
	action	= new QAction ("&Quitter", this);
	connect (action, SIGNAL (triggered ( )), this, SLOT (quitCallback ( )));
	fileMenu->addAction (action);

	QMenu*	objectMenu	= new QMenu ("&Objets", menubar);
	menubar->addMenu (objectMenu);
	objectMenu->setToolTipsVisible (true);
	action	= new QAction ("S&upprimer", this);
	connect (action, SIGNAL (triggered ( )), this, SLOT (removeObjectsCallback ( )));
	objectMenu->addAction (action);
	action	= new QAction (QSTR ("Sp&hère ..."), this);
	connect (action, SIGNAL (triggered ( )), this, SLOT (sphereCallback ( )));
	objectMenu->addAction (action);
	action	= new QAction ("C&ylindre ...", this);
	connect (action, SIGNAL (triggered ( )), this, SLOT (cylinderCallback ( )));
	objectMenu->addAction (action);
}	// QtViewPointWindow::createMenu


void QtViewPointWindow::printCallback ( )
{
	try
	{
		raise ( );	// Eviter qu'une fenêtre recouvre celle-ci
		CHECK_NULL_PTR_ERROR (_renderWindow)
		QtVTKPrintHelper::PrinterStatus	status	= QtVTKPrintHelper::print (*_renderWindow, this);
	}
	catch (const Exception& e)
	{
		QtMessageBox::displayErrorMessage (this, windowTitle ( ).toStdString ( ), e.getFullMessage ( ));
	}
	catch (...)
	{
		QtMessageBox::displayErrorMessage (this, windowTitle ( ).toStdString ( ), "Erreur non documentée");
	}
}	// QtViewPointWindow::printCallback


void QtViewPointWindow::printToFileCallback ( )
{
	try
	{
		raise ( );	// Eviter qu'une fen^etre recouvre celle-ci
		CHECK_NULL_PTR_ERROR (_renderWindow)
		QtVTKPrintHelper::PrinterStatus	status	= QtVTKPrintHelper::printToFile (*_renderWindow, this);
	}
	catch (const Exception& e)
	{
		QtMessageBox::displayErrorMessage (this, windowTitle ( ).toStdString ( ), e.getFullMessage ( ));
	}
	catch (...)
	{
		QtMessageBox::displayErrorMessage (this, windowTitle ( ).toStdString ( ), "Erreur non documentée");
	}
}	// QtViewPointWindow::printToFileCallback


void QtViewPointWindow::quitCallback ( )
{
	QApplication::exit (0);
}	// QtViewPointWindow::quitCallback


void QtViewPointWindow::removeObjectsCallback ( )
{
	try
	{
		CHECK_NULL_PTR_ERROR (_renderer)
		CHECK_NULL_PTR_ERROR (_renderWindow)
		for (vector<vtkActor*>::iterator it = _actors.begin ( );
		     _actors.end ( ) != it; it++)
		{
			_renderer->RemoveActor (*it);
			(*it)->Delete ( );
		}
		for (vector<vtkPolyData*>::iterator itpd = _surfaces.begin ( );
		     _surfaces.end ( ) != itpd; itpd++)
			(*itpd)->Delete ( );

		_actors.clear ( );
		_surfaces.clear ( );
	}
	catch (const Exception& e)
	{
		QtMessageBox::displayErrorMessage (this, windowTitle ( ).toStdString ( ), e.getFullMessage ( ));
	}
	catch (...)
	{
		QtMessageBox::displayErrorMessage (this, windowTitle ( ).toStdString ( ), "Erreur non documentée");
	}
}	// QtViewPointWindow::removeObjectsCallback


void QtViewPointWindow::sphereCallback ( )
{
	try
	{
		CHECK_NULL_PTR_ERROR (_renderer)
		CHECK_NULL_PTR_ERROR (_renderWindow)
		// Mériterait une boite de dialogue dédiée :
		double	x	= 0.,	y	= 0.,	z	= 0., radius	= 1.;
		QtCoordinatesDialog	coordsDialog (this, "Centre de la sphère", "X :", "Y :", "Z :", x, y, z, false, true, true, true);
		if (QDialog::Rejected == coordsDialog.exec ( ))
			return;
		coordsDialog.getCoordinates (x, y, z);
		bool	ok	= true;
		radius	= QInputDialog::getDouble (this, QSTR ("Rayon de la sphère"), "Rayon :", radius, 0., 9.99E9, 10, &ok);
		if (false == ok)
			return;
		vtkSphereSource*	sphere	= vtkSphereSource::New ( );
		sphere->SetCenter (x, y, z);
		sphere->SetThetaResolution (360 / 4);
		sphere->SetPhiResolution (360 / 4);
		sphere->SetRadius (radius);
		vtkPolyDataMapper*	mapper	= vtkPolyDataMapper::New ( );
		vtkActor*			actor	= vtkActor::New ( );
		mapper->SetInputConnection (sphere->GetOutputPort ( ));
		actor->SetMapper (mapper);
		_renderer->AddActor (actor);
		_renderer->ResetCamera ( );
		_renderWindow->Render ( );
		_actors.push_back (actor);
		vtkPolyData*	surface	= sphere->GetOutput ( );
		sphere->Update ( );
		surface->Register (0);
		_surfaces.push_back (surface);
		mapper->Delete ( );			mapper	= 0;
		sphere->Delete ( );			sphere	= 0;
	}
	catch (const Exception& e)
	{
		QtMessageBox::displayErrorMessage (this, windowTitle ( ).toStdString ( ), e.getFullMessage ( ));
	}
	catch (...)
	{
		QtMessageBox::displayErrorMessage (this, windowTitle ( ).toStdString ( ), "Erreur non documentée");
	}
}	// QtViewPointWindow::sphereCallback


void QtViewPointWindow::cylinderCallback ( )
{
	try
	{
		CHECK_NULL_PTR_ERROR (_renderer)
		CHECK_NULL_PTR_ERROR (_renderWindow)
		// Mériterait une boite de dialogue dédiée :
		double	x	= 0.,	y	= 0.,	z	= 0., radius	= 1.,	height	= 1;
		QtCoordinatesDialog	coordsDialog (this, "Centre du cylindre", "X :", "Y :", "Z :", x, y, z, false, true, true, true);
		if (QDialog::Rejected == coordsDialog.exec ( ))
			return;
		coordsDialog.getCoordinates (x, y, z);
		bool	ok	= true;
		height	= QInputDialog::getDouble (this, "Hauteur du cylindre", "Hauteur :", height, 0., 9.99E9, 10, &ok);
		if (false == ok)
			return;
		radius	= QInputDialog::getDouble (this, "Rayon du cylindre", "Rayon :", radius, 0., 9.99E9, 10, &ok);
		if (false == ok)
			return;
		vtkCylinderSource*	cylinder	= vtkCylinderSource::New ( );
		cylinder->SetCenter (x, y, z);
		cylinder->SetResolution (360);
		cylinder->SetHeight (height);
		cylinder->SetRadius (radius);
		cylinder->CappingOff ( );
		// Par défaut le cylindre est le long de l'axe des Y, on le veut le long des X :
		vtkTransformFilter*	filter		= vtkTransformFilter::New ( );
		vtkTransform*		transform	= vtkTransform::New ( );
		transform->RotateZ (90.);
		filter->SetTransform (transform);
		filter->SetInputData (cylinder->GetOutput ( ));
		cylinder->Update ( );
		vtkPolyDataMapper*	mapper	= vtkPolyDataMapper::New ( );
		vtkActor*			actor	= vtkActor::New ( );
		mapper->SetInputConnection (filter->GetOutputPort ( ));
		actor->SetMapper (mapper);
		_renderer->AddActor (actor);
		_renderer->ResetCamera ( );
		_renderWindow->Render ( );
		_actors.push_back (actor);
		vtkPolyData*	surface	= filter->GetPolyDataOutput  ( );
		surface->Register (0);
		_surfaces.push_back (surface);
		mapper->Delete ( );			mapper		= 0;
		cylinder->Delete ( );		cylinder	= 0;
	}
	catch (const Exception& e)
	{
		QtMessageBox::displayErrorMessage (this, windowTitle ( ).toStdString ( ), e.getFullMessage ( ));
	}
	catch (...)
	{
		QtMessageBox::displayErrorMessage (this, windowTitle ( ).toStdString ( ), "Erreur non documentée");
	}
}	// QtViewPointWindow::cylinderCallback
