#include "QtVtk/QtVtk.h"

#include <TkUtil/InternalError.h>

#include <vtkSmartPointer.h>

#include <assert.h>


// ===========================================================================
//                            Classe QtVtk
// ===========================================================================


static vtkSmartPointer<vtkHardwareSelector>	hardwareSelector;
static vtkSmartPointer<vtkRenderWindow>		hardwareSelectionWindow;
static vtkSmartPointer<vtkRenderer>			hardwareSelectionRenderer;



QtVtk::QtVtk ( )
{
	assert (0 && "QtVtk constructor is not allowed.");
}	// QtVtk::QtVtk


QtVtk::QtVtk (const QtVtk&)
{
	assert (0 && "QtVtk copy constructor is not allowed.");
}	// QtVtk::QtVtk


QtVtk& QtVtk::operator = (const QtVtk&)
{
	assert (0 && "QtVtk assignment operator is not allowed.");
	return *this;
}	// QtVtk::QtVtk


QtVtk::~QtVtk ( )
{
	assert (0 && "QtVtk destructor is not allowed.");
}	// QtVtk::~QtVtk


void QtVtk::initialize ( )
{
//	Q_INIT_RESOURCE (QtVtk);

/*	if (0 != QApplication::desktop ( ))
	{
//		const QRect	geom = QApplication::desktop ( )->screenGeometry ( );
	}	// if (0 != QApplication::desktop ( )) */

}	// QtVtk::initialize


void QtVtk::finalize ( )
{
	if (0 != hardwareSelector)
	{
		hardwareSelector->SetRenderer (0);
		hardwareSelector.TakeReference (0);
	}
	if (0 != hardwareSelectionWindow)
	{
		hardwareSelectionWindow->RemoveRenderer (hardwareSelectionRenderer);
		hardwareSelectionWindow.TakeReference (0);
		hardwareSelectionRenderer.TakeReference (0);
	}	// if (0 != hardwareSelectionWindow)
}	// QtVtk::finalize


TkUtil::Version QtVtk::getVersion ( )
{
	static const TkUtil::Version	version (QT_VTK_VERSION);

	return version;
}   // QtVtk::getVersion


vtkHardwareSelector& QtVtk::getHardwareSelector ( )
{
	if (0 == hardwareSelector)
		hardwareSelector	= vtkHardwareSelector::New ( );

	return *hardwareSelector;
}	// QtVtk::getHardwareSelector


vtkRenderWindow& QtVtk::getHardwareSelectionWindow ( )
{
	if (0 == hardwareSelectionWindow)
	{
		hardwareSelectionRenderer	= vtkRenderer::New ( );
		hardwareSelectionWindow		= vtkRenderWindow::New ( );
		// Following code is mandatory :
		hardwareSelectionRenderer->SetBackground (1., 1., 1.);
		hardwareSelectionWindow->OffScreenRenderingOn ( );
#if VTK_MAJOR_VERSION < 9
		hardwareSelectionWindow->MappedOff ( );
#else	// VTK_MAJOR_VERSION < 9
	hardwareSelectionWindow->SetShowWindow (false);
#endif	// VTK_MAJOR_VERSION < 9
		hardwareSelectionWindow->SetMultiSamples (0);// Turn off anti-aliasing
		hardwareSelectionWindow->AddRenderer (hardwareSelectionRenderer);
	}	// if (0 == hardwareSelectionWindow)

	return *hardwareSelectionWindow;
}	// QtVtk::getHardwareSelectionWindow


vtkRenderer& QtVtk::getHardwareSelectionRenderer ( )
{
	if (0 == hardwareSelectionRenderer)
	{
		hardwareSelectionRenderer	= vtkRenderer::New ( );
		hardwareSelectionWindow		= vtkRenderWindow::New ( );
		hardwareSelectionWindow->AddRenderer (hardwareSelectionRenderer);
		// Following code is mandatory :
		hardwareSelectionRenderer->SetBackground (1., 1., 1.);
		hardwareSelectionWindow->OffScreenRenderingOn ( );
#if VTK_MAJOR_VERSION < 9
		hardwareSelectionWindow->MappedOff ( );
#else	// VTK_MAJOR_VERSION < 9
	hardwareSelectionWindow->SetShowWindow (false);
#endif	// VTK_MAJOR_VERSION < 9
		hardwareSelectionWindow->SetMultiSamples (0);// Turn off anti-aliasing
		hardwareSelectionWindow->AddRenderer (hardwareSelectionRenderer);
	}	// if (0 == hardwareSelectionRenderer)

	return *hardwareSelectionRenderer;
}	// QtVtk::getHardwareSelectionRenderer


