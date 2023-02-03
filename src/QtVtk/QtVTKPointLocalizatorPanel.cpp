#include "QtVtk/QtVTKPointLocalizatorPanel.h"

#include <QtUtil/QtConfiguration.h>
#include <TkUtil/MemoryError.h>

#include <QFormLayout>
#include <QLabel>
#include <QValidator>

#include <vtkCamera.h>
#include <vtkRenderWindow.h>

#include <assert.h>


USING_STD
USING_UTIL

static const Charset	charset ("àéèùô");

int		QtVTKPointLocalizatorPanel::numberOfCharacters	= 18;
double	QtVTKPointLocalizatorPanel::zoomMin				= 0.2;
double	QtVTKPointLocalizatorPanel::zoomMax				= 5.;
double	QtVTKPointLocalizatorPanel::zoomComul			= 1.3;


QtVTKPointLocalizatorPanel::QtVTKPointLocalizatorPanel (
			QWidget* parent, const string& title, vtkRenderer& renderer,
			const string& xTitle, const string& yTitle, const string& zTitle,
            double x, double y, double z, double dx, double dy, double dz)
	: QWidget (parent),
	  _dx (dx < 0. ? -dx : dx), _dy (dy < 0. ? -dy : dy),
	  _dz (dz < 0. ? -dz : dz),
	  _xTextField (0), _yTextField (0), _zTextField (0),
	  _centerViewCheckBox (0), _zoomSlider (0),
	  _renderer (&renderer), _pointActor (0)
{
	QFormLayout*	layout	= new QFormLayout (this);
//	layout->setContentsMargins (0, 0, 0, 0);
	setLayout (layout);

	_xTextField				= new QtDoubleTextField (this);
	_xTextField->setVisibleColumns (numberOfCharacters);
	_xTextField->setMinimumSize (_xTextField->sizeHint ( ));
	layout->addRow (xTitle.c_str ( ), _xTextField);
	_yTextField				= new QtDoubleTextField (this);
	_yTextField->setVisibleColumns (numberOfCharacters);
	_yTextField->setMinimumSize (_yTextField->sizeHint ( ));
	layout->addRow (yTitle.c_str ( ), _yTextField);
	_zTextField				= new QtDoubleTextField (this);
	_zTextField->setVisibleColumns (numberOfCharacters);
	_zTextField->setMinimumSize (_zTextField->sizeHint ( ));
	layout->addRow (zTitle.c_str ( ), _zTextField);

	_xTextField->setText (QString::number (x));
	_yTextField->setText (QString::number (y));
	_zTextField->setText (QString::number (z));
	connect (_xTextField, SIGNAL(returnPressed ( )), this,
	         SLOT (updatePointActorCallback ( )));
	connect (_yTextField, SIGNAL(returnPressed ( )), this,
	         SLOT (updatePointActorCallback ( )));
	connect (_zTextField, SIGNAL(returnPressed ( )), this,
	         SLOT (updatePointActorCallback ( )));

	_centerViewCheckBox	= new QCheckBox ("Centrer la vue sur le point", this);
	layout->addRow (_centerViewCheckBox);
	connect (_centerViewCheckBox, SIGNAL(clicked ( )), this,
	         SLOT (centerViewCallback ( )));
	_zoomSlider	= new QSlider (Qt::Horizontal, this);
	_zoomSlider->setRange (1, 100);
	int	one	= defaultSliderValue (1, 100, zoomMin, zoomMax, zoomComul);
	_zoomSlider->setValue (one);
	_zoomSlider->setToolTip (
						"Ajuste la taille du localisateur graphique (croix)");
	layout->addRow (_zoomSlider);
	connect (_zoomSlider, SIGNAL(valueChanged (int)), this,
	         SLOT (crossZoomCallback ( )));

	adjustSize ( );

	setDisplayed (true);
}	// QtVTKPointLocalizatorPanel::QtVTKPointLocalizatorPanel


QtVTKPointLocalizatorPanel::QtVTKPointLocalizatorPanel (
											const QtVTKPointLocalizatorPanel&)
	: QWidget (0), 
	  _dx (0.1), _dy (0.1), _dz (0.1),
	  _xTextField (0), _yTextField (0), _zTextField (0),
	  _centerViewCheckBox (0), _zoomSlider (0),
	  _renderer (0), _pointActor (0)
{
	assert (0 && "QtVTKPointLocalizatorPanel copy constructor is forbidden.");
}	// QtVTKPointLocalizatorPanel::QtVTKPointLocalizatorPanel (const QtVTKPointLocalizatorPanel&)


QtVTKPointLocalizatorPanel& QtVTKPointLocalizatorPanel::operator = (
											const QtVTKPointLocalizatorPanel&)
{
	assert (0 && "QtVTKPointLocalizatorPanel assignment operator is forbidden.");
	return *this;
}	// QtVTKPointLocalizatorPanel::operator =


QtVTKPointLocalizatorPanel::~QtVTKPointLocalizatorPanel ( )
{
	setDisplayed (false);
}	// QtVTKPointLocalizatorPanel::~QtVTKPointLocalizatorPanel


void QtVTKPointLocalizatorPanel::setDisplayed (bool display)
{
	if (true == display)
	{
		if (0 == _pointActor)
		{
			_pointActor	= vtkECMAxesActor::New ( );
			getRenderer ( ).AddViewProp (_pointActor);
			if (0 != getRenderer ( ).GetRenderWindow ( ))
				getRenderer ( ).GetRenderWindow ( )->Render ( );
		}	// if (0 == _pointActor)

		updatePointActorCallback ( );
	}
	else
	{
		if (0 != _pointActor)
		{
			getRenderer ( ).RemoveViewProp (_pointActor);
			_pointActor->Delete ( );
			_pointActor	= 0;
			if (0 != getRenderer ( ).GetRenderWindow ( ))
				getRenderer ( ).GetRenderWindow ( )->Render ( );
		}	// if (0 != _pointActor)
	}
}	// QtVTKPointLocalizatorPanel::setDisplayed


void QtVTKPointLocalizatorPanel::setX (double x)
{
	CHECK_NULL_PTR_ERROR (_xTextField)
	_xTextField->setValue (x);
}	// QtVTKPointLocalizatorPanel::setX


double QtVTKPointLocalizatorPanel::getX ( ) const
{
	CHECK_NULL_PTR_ERROR (_xTextField)
	return _xTextField->getValue ( );
}	// QtVTKPointLocalizatorPanel::getX


void QtVTKPointLocalizatorPanel::setY (double y)
{
	CHECK_NULL_PTR_ERROR (_yTextField)
	_yTextField->setValue (y);
}	// QtVTKPointLocalizatorPanel::setY


double QtVTKPointLocalizatorPanel::getY ( ) const
{
	CHECK_NULL_PTR_ERROR (_yTextField)
	return _yTextField->getValue ( );
}	// QtVTKPointLocalizatorPanel::getY


void QtVTKPointLocalizatorPanel::setZ (double z)
{
	CHECK_NULL_PTR_ERROR (_zTextField)
	_zTextField->setValue (z);
}	// QtVTKPointLocalizatorPanel::setZ


double QtVTKPointLocalizatorPanel::getZ ( ) const
{
	CHECK_NULL_PTR_ERROR (_zTextField)
	return _zTextField->getValue ( );
}	// QtVTKPointLocalizatorPanel::getZ


const QtDoubleTextField& QtVTKPointLocalizatorPanel::getXTextField ( ) const
{
	CHECK_NULL_PTR_ERROR (_xTextField)
	return *_xTextField;
}	// QtVTKPointLocalizatorPanel::getXTextField


QtDoubleTextField& QtVTKPointLocalizatorPanel::getXTextField ( )
{
	CHECK_NULL_PTR_ERROR (_xTextField)
	return *_xTextField;
}	// QtVTKPointLocalizatorPanel::getXTextField


const QtDoubleTextField& QtVTKPointLocalizatorPanel::getYTextField ( ) const
{
	CHECK_NULL_PTR_ERROR (_yTextField)
	return *_yTextField;
}	// QtVTKPointLocalizatorPanel::getYTextField


QtDoubleTextField& QtVTKPointLocalizatorPanel::getYTextField ( )
{
	CHECK_NULL_PTR_ERROR (_yTextField)
	return *_yTextField;
}	// QtVTKPointLocalizatorPanel::getYTextField


const QtDoubleTextField& QtVTKPointLocalizatorPanel::getZTextField ( ) const
{
	CHECK_NULL_PTR_ERROR (_zTextField)
	return *_zTextField;
}	// QtVTKPointLocalizatorPanel::getZTextField


QtDoubleTextField& QtVTKPointLocalizatorPanel::getZTextField ( )
{
	CHECK_NULL_PTR_ERROR (_zTextField)
	return *_zTextField;
}	// QtVTKPointLocalizatorPanel::getZTextField


bool QtVTKPointLocalizatorPanel::centerOnPoint ( ) const
{
	CHECK_NULL_PTR_ERROR (_centerViewCheckBox)
	return Qt::Checked == _centerViewCheckBox->checkState ( ) ? true : false;
}	// QtVTKPointLocalizatorPanel::centerOnPoint


vtkECMAxesActor& QtVTKPointLocalizatorPanel::getPointActor ( )
{
	CHECK_NULL_PTR_ERROR (_pointActor)
	return *_pointActor;
}	// QtVTKPointLocalizatorPanel::getPointActor


vtkRenderer& QtVTKPointLocalizatorPanel::getRenderer ( )
{
	CHECK_NULL_PTR_ERROR (_renderer)
	return *_renderer;
}	// QtVTKPointLocalizatorPanel::getRenderer


int QtVTKPointLocalizatorPanel::defaultSliderValue (
	int minValue, int maxValue, double zoomMin, double zoomMax, double comul)
{
	// 0.5 : l'objet est centré au milieu de la boite, et la distance à
	// l'extrémité de la boite est donc diminuée de 50%.
	if ((fabs (zoomMax - zoomMin) > 1e-12) && (fabs (comul) > 1e-12))
		return 0.5 * (100 - 1) / (zoomMax - zoomMin) / comul;

	return abs (maxValue - minValue) / 2;
}	// QtVTKPointLocalizatorPanel::defaultSliderValue


double QtVTKPointLocalizatorPanel::zoomValue (
	int sliderValue, double objectBounds [6], double targetBounds [6],
	int minValue, int maxValue, double zoomMin, double zoomMax)
{
	const int range	= abs (maxValue - minValue);
	if (0 == range)
		return 1.;

	double			scale		= fabs (sliderValue - minValue) / (double)range;
	const double	objectDiag	=
		(objectBounds[1]-objectBounds[0])*(objectBounds[1]-objectBounds[0]) +
		(objectBounds[3]-objectBounds[2])*(objectBounds[3]-objectBounds[2]) +	
		(objectBounds[5]-objectBounds[4])*(objectBounds[5]-objectBounds[4]);
	const double	targetDiag	=
		(targetBounds[1]-targetBounds[0])*(targetBounds[1]-targetBounds[0]) +
		(targetBounds[3]-targetBounds[2])*(targetBounds[3]-targetBounds[2]) +	
		(targetBounds[5]-targetBounds[4])*(targetBounds[5]-targetBounds[4]);
	if (objectDiag > 1e-12)
		scale	= (zoomMax-zoomMin) * scale * 2 * sqrt (targetDiag/objectDiag);
	else
		scale	= (zoomMax-zoomMin) * scale * 2;

	return scale;
}	// QtVTKPointLocalizatorPanel::zoomValue


void QtVTKPointLocalizatorPanel::updatePointActorCallback ( )
{
	assert (0 != _zoomSlider);
	try
	{
		// Y a t'il un zoom à appliquer ?
		const int		val		= _zoomSlider->value ( );
		const int		min		= _zoomSlider->minimum ( );
		const int		max		= _zoomSlider->maximum ( );
		const double	scale	= min == max ?
								  1. : fabs (val - min) / fabs (max - min);
		const double	x	= getX ( );
		const double	y	= getY ( );
		const double	z	= getZ ( );
		getPointActor ( ).SetPosition (
			x - scale * _dx, x + scale * _dx, y - scale * _dy, y + scale * _dy,
			z - scale * _dz, z + scale * _dz);
		if (false == centerOnPoint ( ))
		{
			if (0 != getRenderer( ).GetRenderWindow( ))
				getRenderer ( ).GetRenderWindow ( )->Render ( );
		}	// if (false == centerOnPoint ( ))
		else
			centerViewCallback ( );

	}
	catch (const Exception& exc)
	{

		UTF8String	mess (charset);
		mess << "Impossibilité de matérialiser le point dans la scène "
		     << "graphique : " << exc.getFullMessage ( );
		ConsoleOutput::cout ( ) << mess << co_endl;

	}
	catch (...)
	{

		UTF8String	mess (charset);
		mess << "Impossibilité de matérialiser le point dans la scène : "
	     << "graphique erreur non documentée.";
		ConsoleOutput::cout ( ) << mess << co_endl;

	}
}	// QtVTKPointLocalizatorPanel::updatePointActorCallback


void QtVTKPointLocalizatorPanel::centerViewCallback ( )
{
	try
	{
		if (false == centerOnPoint ( ))
			return;

		vtkCamera*		camera	= getRenderer ( ).GetActiveCamera ( );
		if (0 == camera)
			return;

		const double	x	= getX ( );
		const double	y	= getY ( );
		const double	z	= getZ ( );
		camera->SetFocalPoint (x, y, z);
		camera->ComputeViewPlaneNormal ( );
		getRenderer ( ).ResetCameraClippingRange ( );
		if (0 != getRenderer ( ).GetRenderWindow ( ))
			getRenderer ( ).GetRenderWindow ( )->Render ( );
	}
	catch (const Exception& exc)
	{

		UTF8String	mess (charset);
		mess << "Impossibilité de matérialiser le point dans la scène "
		     << "graphique : " << exc.getFullMessage ( );
		ConsoleOutput::cout ( ) << mess << co_endl;

	}
	catch (...)
	{

		UTF8String	mess (charset);
		mess << "Impossibilité de matérialiser le point dans la scène "
		     << "graphique : erreur non documentée.";
		ConsoleOutput::cout ( ) << mess << co_endl;

	}
}	// QtVTKPointLocalizatorPanel::centerViewCallback


void QtVTKPointLocalizatorPanel::crossZoomCallback ( )
{
	updatePointActorCallback ( );
}	// QtVTKPointLocalizatorPanel::crossZoomCallback


