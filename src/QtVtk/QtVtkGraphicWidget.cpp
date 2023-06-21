#include "QtVtk/QtVtkGraphicWidget.h"

#include <TkUtil/MemoryError.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderWindow.h>
#include <iostream>
#include <assert.h>

using namespace TkUtil;
using namespace std;


QtVtkGraphicWidget::QtVtkGraphicWidget (QWidget* parent)
#ifdef VTK_WIDGET
		: QVTKWidget (parent)
#elif VTK_WIDGET2
		: QVTKWidget2 (parent)
#elif VTK_9
		: QVTKOpenGLNativeWidget (parent)
#elif VTK_8
		: QVTKOpenGLWidget (parent)
#endif	// VTK_WIDGET
{
#ifdef VTK_WIDGET
//	cout << __FILE__ << ' ' << __LINE__ << " ============================> QtVtkGraphicWidget::QtVtkGraphicWidget : QVTKWidget constructor." << endl;
#elif VTK_WIDGET2
//	cout << __FILE__ << ' ' << __LINE__ << " ============================> QtVtkGraphicWidget::QtVtkGraphicWidget : QVTKWidget2 constructor." << endl;
#elif VTK_9
//cout << __FILE__ << ' ' << __LINE__ << " ============================> QtVtkGraphicWidget::QtVtkGraphicWidget : QVTKOpenGLWidget constructor." << endl;
/*		if (0 == interactor ( ))
		{	// => VTK 9 ?
			QVTKInteractor*	rwi	= QVTKInteractor::New ( );
			setInteractor (rwi);
			rwi->Initialize ( );
			rwi->Delete ( );	// Still alive, Delete decreases reference count.
		}	// if (0 == interactor ( ))	*/
#elif VTK_8
//	cout << __FILE__ << ' ' << __LINE__ << " ============================> QtVtkGraphicWidget::QtVtkGraphicWidget : QVTKOpenGLWidget constructor." << endl;
#endif	// VTK_WIDGET
}	// QtVtkGraphicWidget::QtVtkGraphicWidget


QtVtkGraphicWidget::QtVtkGraphicWidget (const QtVtkGraphicWidget&)
#ifdef VTK_WIDGET
		: QVTKWidget (0)
#elif VTK_WIDGET2
		: QVTKWidget2 ( )
#elif VTK_9
		: QVTKOpenGLNativeWidget ( )
#elif VTK_8
		: QVTKOpenGLWidget ( )
#endif	// VTK_WIDGET
{
	assert (0 && "QtVtkGraphicWidget copy constructor is not allowed.");
}	// QtVtkGraphicWidget::QtVtkGraphicWidget


QtVtkGraphicWidget& QtVtkGraphicWidget::operator = (const QtVtkGraphicWidget&)
{
	assert (0 && "QtVtkGraphicWidget assignment operator is not allowed.");
	return *this;
}	// QtVtkGraphicWidget::operator =


QtVtkGraphicWidget::~QtVtkGraphicWidget ( )
{
}	// QtVtkGraphicWidget::~QtVtkGraphicWidget


vtkRenderWindow* QtVtkGraphicWidget::getRenderWindow ( )
{
#ifdef VTK_WIDGET
	return QVTKWidget::GetRenderWindow ( );
#elif VTK_WIDGET2
	return QVTKWidget2::GetRenderWindow ( );
#elif VTK_9
	return QVTKOpenGLNativeWidget::renderWindow ( );
#elif VTK_8
	return QVTKOpenGLWidget::GetRenderWindow ( );
#endif	// VTK_WIDGET
}	// QtVtkGraphicWidget::getRenderWindow


void QtVtkGraphicWidget::setRenderWindow (vtkRenderWindow* window)
{
#ifdef VTK_WIDGET
	QVTKWidget::SetRenderWindow (window);
#elif VTK_WIDGET2
	QVTKWidget2::SetRenderWindow (dynamic_cast<vtkGenericOpenGLRenderWindow*>(window));
#elif VTK_9
	QVTKOpenGLNativeWidget::setRenderWindow (dynamic_cast<vtkGenericOpenGLRenderWindow*>(window));
#elif VTK_8
	QVTKOpenGLWidget::SetRenderWindow (dynamic_cast<vtkGenericOpenGLRenderWindow*>(window));
#endif	// VTK_WIDGET
}	// QtVtkGraphicWidget::setRenderWindow


QVTKInteractor* QtVtkGraphicWidget::getInteractor ( )
{
#ifdef VTK_WIDGET
	return dynamic_cast<QVTKInteractor*>(QVTKWidget::GetInteractor ( ));
#elif VTK_WIDGET2
	return dynamic_cast<QVTKInteractor*>(QVTKWidget2::GetInteractor ( ));
#elif VTK_9
	return QVTKOpenGLNativeWidget::interactor ( );
#elif VTK_8
	return dynamic_cast<QVTKInteractor*>(QVTKOpenGLWidget::GetInteractor ( ));
#endif	// VTK_WIDGET
}	// QtVtkGraphicWidget::getInteractor


void QtVtkGraphicWidget::setInteractor (QVTKInteractor* interactor)
{
	if (0 != getRenderWindow ( ))
		getRenderWindow ( )->SetInteractor (interactor);
}	// QtVtkGraphicWidget::setInteractor


#if (VTK_MAJOR_VERSION > 9) || ((VTK_MAJOR_VERSION == 9) && (VTK_MINOR_VERSION >= 2))

void QtVtkGraphicWidget::SetRenderWindow (vtkRenderWindow* window)	// v 8.1.1
{
	QVTKOpenGLNativeWidget::setRenderWindow (window);
}	// QtVtkGraphicWidget::SetRenderWindow


vtkRenderWindow* QtVtkGraphicWidget::GetRenderWindow ( )	// v 8.1.1
{
	QVTKOpenGLNativeWidget::renderWindow ( );
}	// QtVtkGraphicWidget::GetRenderWindow

#endif


void QtVtkGraphicWidget::enterEvent (QEvent* event)
{
	setFocus ( );
	
#ifdef VTK_WIDGET
	QVTKWidget::enterEvent (event);
#elif VTK_WIDGET2
	QVTKWidget2::enterEvent (event);
#elif VTK_9
	QVTKOpenGLNativeWidget::enterEvent (event);
#elif VTK_8
	QVTKOpenGLWidget::enterEvent (event);
#endif	// VTK_WIDGET
}	// QtVtkGraphicWidget::enterEvent


void QtVtkGraphicWidget::focusInEvent (QFocusEvent* event)
{
	repaint ( );
}	// QtVtkGraphicWidget::focusInEvent 


QtVtkGraphicWidget* QtVtkGraphicWidget::create (QWidget* parent, vtkInteractorStyle* interactorStyle, bool allowStereoMode)
{	// A PRIORI FONCTION OBSOLETE, mais code intéressant ...
	QtVtkGraphicWidget*	graphicWidget	= 0;
#ifdef USE_VTK_WIDGET
	assert (0 == graphicWidget);
#ifdef VTK_WIDGET2
	QVTKWidget2*			qvtkWidget	= new QtVtkGraphicWidget (parent);
#elif VTK_WIDGET
	QVTKWidget*				qvtkWidget	= new QtVtkGraphicWidget (parent);
#elif VTK_9
	QVTKOpenGLNativeWidget*	qvtkWidget	= new QtVtkGraphicWidget (parent);
#elif VTK_8
	QVTKOpenGLWidget*		qvtkWidget	= new QtVtkGraphicWidget (parent);
#else
#	error ("ABSENCE DE SPECIFICATION DU TYPE DE WIDGET GRAPHIQUE");
#endif	// VTK_WIDGET2
	CHECK_NULL_PTR_ERROR (qvtkWidget)
	// La bonne utilisation de la transparence en VTK requiert Qt::WA_TranslucentBackground qui active le canal alpha.
	qvtkWidget->setAttribute (Qt::WA_TranslucentBackground, true);
	
// setAutomaticImageCacheEnabled : intérêt très limité. Présente les inconvénients des caches capturés sur carte graphique,
// à savoir que les fenêtres qui recouvrent la vue sont elles mêmes capturées. Les gains de temps escomptés, en revanche,
// ne sont pas là : curieusement, la vue semble recalculée lorsque la fenêtre graphique est démasquée.
// qvtkWidget->setAutomaticImageCacheEnabled (true);
// Effet de bord de setAutomaticImageCacheEnabled : fenêtre non raffraichie lorsqu'elle est démasquée.
// qvtkWidget->setAutomaticImageCacheEnabled (true);
// Pour pouvoir bénéficier de la stéréo on instancie la vtkRenderWindow.
// Dans le cas où c'est fait par le QVTKWidget, ça n'est plus possible.
// vtkRenderWindow*	window		=	qvtkWidget->GetRenderWindow ( );
#if defined(VTK_WIDGET2) || defined(VTK_8) || defined(VTK_9)
	vtkGenericOpenGLRenderWindow*	window		=	vtkGenericOpenGLRenderWindow::New ( );
#else
	vtkRenderWindow*				window		=	vtkRenderWindow::New ( );
#endif	// VTK_WIDGET2
	CHECK_NULL_PTR_ERROR (window)
	if (true == allowStereoMode)
	{
		window->StereoCapableWindowOn ( );
		window->SetStereoTypeToCrystalEyes ( );
	}	// if (true == allowStereoMode)

#if VTK_MAJOR_VERSION < 9
	qvtkWidget->SetRenderWindow (window);
	vtkRenderWindowInteractor*	interactor	= qvtkWidget->GetInteractor ( );
#else 	// VTK_MAJOR_VERSION < 9
	qvtkWidget->setRenderWindow (window);
	vtkRenderWindowInteractor*	interactor	= qvtkWidget->interactor ( );
#endif	// VTK_MAJOR_VERSION < 9
	
	graphicWidget	= dynamic_cast<QtVtkGraphicWidget*>(qvtkWidget);
	CHECK_NULL_PTR_ERROR (graphicWidget)
#endif	// USE_VTK_WIDGET
	CHECK_NULL_PTR_ERROR (window)
	CHECK_NULL_PTR_ERROR (interactor)
//	interactor->SetDesiredUpdateRate (3);

#ifndef USE_VTK_WIDGET
	if (true == allowStereoMode)
	{
		window->StereoCapableWindowOn ( );
		window->SetStereoTypeToCrystalEyes ( );
	}	// if (true == allowStereoMode)
#endif	// USE_VTK_WIDGET

	if (0 !=interactorStyle)
		interactor->SetInteractorStyle (interactorStyle);
	interactor->SetRenderWindow (window);
//cout << "DESIRED = " << interactor->GetDesiredUpdateRate ( ) << " STILL : " << interactor->GetStillUpdateRate ( ) << endl;
//	interactor->SetDesiredUpdateRate : débit souhaité lors des interactions.
//	interactor->SetDesiredUpdateRate (20.);
// interactor->SetStillUpdateRate : débit au repos, sans interactions. A forcer un débit trop fort on prend le risque de se 
// retrouver en permanence avec un affichage dégradé, type LOD avec nuage de points lors des interactions.
//	interactor->SetStillUpdateRate (.1);
// Valeurs par défaut avec VTK 5.1.0 : Desired = 15, Still = 0.0001 (=> peut être très très lent)
#ifndef USE_VTK_WIDGET
	interactor->Delete ( );				interactor		= 0;
#endif	// USE_VTK_WIDGET

	window->LineSmoothingOn ( );
#ifndef USE_VTK_WIDGET
	// Avec le widget de VTK le passage en plein écran provoque une triangularisation visible des faces ("arêtes" blanches).
	window->PolygonSmoothingOn ( );
#endif	// USE_VTK_WIDGET

#ifdef VTK_WIDGET
	// On force la fenetre a effacer le contenu d'un eventuel cache :
	window->Render ( );
#endif	// VTK_WIDGET

	// SetAlphaBitPlanes : à faire après un premier rendu qui appel
	// OpenGLInitState et positionne l'attribut AlphaBitPlanes.
	window->SetAlphaBitPlanes (8);	// Bonne gestion transparence
	window->SetMultiSamples (0);	// Bonne gestion transparence

	return graphicWidget;
}	// QtVtkGraphicWidget::create


string QtVtkGraphicWidget::implementationInfos ( )
{
#ifdef VTK_WIDGET
	return string ("QVTKWidget");
#	elif VTK_WIDGET2
	return string ("QVTKWidget2");
#	elif VTK_9
	return string ("QVTKOpenGLNativeWidget");
#	elif VTK_8
	return string ("QVTKOpenGLWidget");
#	else
#	error ("ABSENCE DE SPECIFICATION DU TYPE DE WIDGET GRAPHIQUE");
	return string ("Implémentation non précisée");
#endif	// VTK_WIDGET
}	// QtVtkGraphicWidget::implementationInfos
