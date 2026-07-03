/**
 * \file		QtVtkTransformationPanel.cpp
 * \author		Charles PIGNEROL, CEA/DAM/DCLC
 * \date		20/01/2026
 */
#include "QtVtk/QtVtkTransformationPanel.h"
#include "QtVtk/QtVTKPointLocalizatorPanel.h"

#include <QtUtil/QtConfiguration.h>
#include <QtUtil/QtStringHelper.h>
#include <TkUtil/InternalError.h>
#include <TkUtil/MemoryError.h>
#include <QtUtil/QtAutoWaitingCursor.h>
#include <QtUtil/QtUnicodeHelper.h>

#include <assert.h>
#include <math.h>
#include <float.h>		// FLT_MAX
#include <values.h>		// FLT_MAX sous Linux ...

#include <QLayout>
#include <QLabel>
#include <QValidator>
#include <QMessageBox> 

#include <vtkRenderer.h>
#include <vtkRenderWindow.h>


USING_STD
USING_UTIL


static const	Charset	charset ("àéèùô");
USE_ENCODING_AUTODETECTION


// ===========================================================================
//                    QtVtkExtrinsicTransformationPanel
// ===========================================================================

QtVtkExtrinsicTransformationPanel::QtVtkExtrinsicTransformationPanel (QWidget* parent, const UTF8String& appTitle, double xoy, double xoz, double yoz, bool translationFirst, double dx, double dy, double dz)
	: QWidget (parent), _appTitle (appTitle), _translationFirstCheckBox (0), _xOyAngleTextField (0), _xOzAngleTextField (0), _yOzAngleTextField (0), _translationPanel (0)
{
	QtAutoWaitingCursor		waitingCursor (true);

	// Creation de l'ihm :
	QVBoxLayout*	layout	= new QVBoxLayout (this);
	layout->setSpacing (QtVtkTransformationPanel::spacing);
#ifdef QT_5
	layout->setMargin (QtVtkTransformationPanel::margin);
#else	// QT_5
	layout->setContentsMargins (QtVtkTransformationPanel::margin, QtVtkTransformationPanel::margin, QtVtkTransformationPanel::margin, QtVtkTransformationPanel::margin);
#endif	// QT_5
	layout->setSizeConstraint (QLayout::SetMinimumSize);

	// Translation en premier ? :
	_translationFirstCheckBox	= new QCheckBox (QSTR ("Translation en premier"), this);
	connect (_translationFirstCheckBox, SIGNAL(clicked ( )), this, SLOT(transformationModifiedCallback ( )));
	QString	tip (QSTR ("Coché, la translation est effectuée avant les rotations."));
	_translationFirstCheckBox->setChecked (translationFirst);
	_translationFirstCheckBox->setToolTip (tip);
	layout->addWidget (_translationFirstCheckBox);
	
	// Les rotations :
	QtGroupBox*		xOyGroupBox	= new QtGroupBox (this);
	QHBoxLayout*	qhLayout	= new QHBoxLayout (xOyGroupBox);
	xOyGroupBox->setLayout (qhLayout);
//	xOyGroupBox->setMargin (QtVtkTransformationPanel::margin);
//	xOyGroupBox->setSpacing (QtVtkTransformationPanel::spacing);
	qhLayout->setContentsMargins (QtConfiguration::margin, QtConfiguration::margin, QtConfiguration::margin, QtConfiguration::margin);
	qhLayout->setSpacing (QtConfiguration::spacing);
	layout->addWidget (xOyGroupBox);
	QLabel*	angleLabel	= new QLabel (QSTR ("Angle de rotation autour de Oz :"), xOyGroupBox);
	qhLayout->addWidget (angleLabel);
	_xOyAngleTextField	= new QtTextField (QString::number (xoy), xOyGroupBox);
	_xOyAngleTextField->setValidator (new QDoubleValidator (-180., 180., 4, _xOyAngleTextField));
	qhLayout->addWidget (_xOyAngleTextField);
	connect (_xOyAngleTextField, SIGNAL (returnPressed ( )), this, SLOT (transformationModifiedCallback ( )));
	_xOyAngleTextField->setFixedSize (_xOyAngleTextField->sizeHint ( ));
	_xOyAngleTextField->setToolTip (QSTR ("Zone de saisie de l'angle de rotation autour de l'axe Oz de la transformation dans le plan xOy."));
	qhLayout->addWidget (new QLabel (QSTR ("degrés"), xOyGroupBox));
	qhLayout->addStretch (200);

	QtGroupBox*		yOzGroupBox	= new QtGroupBox (this);
	qhLayout	= new QHBoxLayout (yOzGroupBox);
	yOzGroupBox->setLayout (qhLayout);
//	yOzGroupBox->setMargin (QtVtkTransformationPanel::margin);
//	yOzGroupBox->setSpacing (QtVtkTransformationPanel::spacing);
	qhLayout->setContentsMargins (QtConfiguration::margin, QtConfiguration::margin, QtConfiguration::margin, QtConfiguration::margin);
	qhLayout->setSpacing (QtConfiguration::spacing);
	layout->addWidget (yOzGroupBox);
	angleLabel	= new QLabel (QSTR ("Angle de rotation autour de Ox :"), yOzGroupBox);
	qhLayout->addWidget (angleLabel);
	_yOzAngleTextField	= 	new QtTextField (QString::number (yoz), yOzGroupBox);
	_yOzAngleTextField->setValidator (new QDoubleValidator (-180., 180., 4, _yOzAngleTextField));
	qhLayout->addWidget (_yOzAngleTextField);
	connect (_yOzAngleTextField, SIGNAL (returnPressed ( )), this, SLOT (transformationModifiedCallback ( )));
	_yOzAngleTextField->setFixedSize (_yOzAngleTextField->sizeHint ( ));
	_yOzAngleTextField->setToolTip (QSTR ("Zone de saisie de l'angle de rotation autour de l'axe Ox de la transformation dans le plan yOz."));
	qhLayout->addWidget (new QLabel (QSTR ("degrés"), yOzGroupBox));
	qhLayout->addStretch (200);

	QtGroupBox*		xOzGroupBox	= new QtGroupBox (this);
	qhLayout	= new QHBoxLayout (xOzGroupBox);
	xOzGroupBox->setLayout (qhLayout);
//	xOzGroupBox->setMargin (QtVtkTransformationPanel::margin);
//	xOzGroupBox->setSpacing (QtVtkTransformationPanel::spacing);
	qhLayout->setContentsMargins (QtConfiguration::margin, QtConfiguration::margin, QtConfiguration::margin, QtConfiguration::margin);
	qhLayout->setSpacing (QtConfiguration::spacing);
	layout->addWidget (xOzGroupBox);
	angleLabel	= new QLabel (QSTR ("Angle de rotation autour de Oy :"), xOzGroupBox);
	qhLayout->addWidget (angleLabel);
	_xOzAngleTextField	= 	new QtTextField (QString::number (xoz), xOzGroupBox);
	_xOzAngleTextField->setValidator (new QDoubleValidator (-180., 180., 4, _xOzAngleTextField));
	qhLayout->addWidget (_xOzAngleTextField);
	connect (_xOzAngleTextField, SIGNAL (returnPressed ( )), this, SLOT (transformationModifiedCallback ( )));
	_xOzAngleTextField->setFixedSize (_xOzAngleTextField->sizeHint ( ));
	_xOzAngleTextField->setToolTip (QSTR ("Zone de saisie de l'angle de rotation autour de l'axe Oy de la transformation dans le plan xOz."));
	qhLayout->addWidget (new QLabel (QSTR ("degrés"), xOzGroupBox));
	qhLayout->addStretch (200);
	
	// La translation :
	_translationPanel	= new Qt3DDataPanel (this, "Translation :", true, "x : ", "y : ", "z : ", dx, -DBL_MAX, DBL_MAX, dy, -DBL_MAX, DBL_MAX, dz, -DBL_MAX, DBL_MAX, false);
	if (0 != _translationPanel->layout ( ))
	{
		_translationPanel->layout ( )->setContentsMargins (QtConfiguration::margin, QtConfiguration::margin, QtConfiguration::margin, QtConfiguration::margin);
		_translationPanel->layout ( )->setSpacing (QtVtkTransformationPanel::spacing);
	}
	connect (_translationPanel->getXTextField ( ), SIGNAL (returnPressed ( )), this, SLOT (transformationModifiedCallback ( )));
	connect (_translationPanel->getYTextField ( ), SIGNAL (returnPressed ( )), this, SLOT (transformationModifiedCallback ( )));
	connect (_translationPanel->getZTextField ( ), SIGNAL (returnPressed ( )), this, SLOT (transformationModifiedCallback ( )));
	layout->addWidget (_translationPanel);
	_translationPanel->setToolTip (QSTR ("Zone de saisie de la translation pré ou post-rotations."));

	layout->activate ( );
	setMinimumSize (layout->sizeHint ( ));
}	// QtVtkExtrinsicTransformationPanel::QtVtkExtrinsicTransformationPanel


QtVtkExtrinsicTransformationPanel::QtVtkExtrinsicTransformationPanel (const QtVtkExtrinsicTransformationPanel&)
{
	assert (0 && "QtVtkExtrinsicTransformationPanel copy constructor is forbidden.");
}	// QtVtkExtrinsicTransformationPanel::QtVtkExtrinsicTransformationPanel (const QtVtkExtrinsicTransformationPanel&)


QtVtkExtrinsicTransformationPanel& QtVtkExtrinsicTransformationPanel::operator = (const QtVtkExtrinsicTransformationPanel&)
{
	assert (0 && "QtVtkExtrinsicTransformationPanel assignment operator is forbidden.");
	return *this;
}	// QtVtkExtrinsicTransformationPanel::operator =


QtVtkExtrinsicTransformationPanel::~QtVtkExtrinsicTransformationPanel ( )
{
}	// QtVtkExtrinsicTransformationPanel::~QtVtkExtrinsicTransformationPanel


void QtVtkExtrinsicTransformationPanel::setXOYAngle (double angle)
{
	assert ((0 != _xOyAngleTextField) && "QtVtkExtrinsicTransformationPanel::setXOYAngle : null angle text field.");
	_xOyAngleTextField->setText (QString::number (angle));
}	// QtVtkExtrinsicTransformationPanel::setXOYAngle


double QtVtkExtrinsicTransformationPanel::getXOYAngle ( ) const
{
	assert ((0 != _xOyAngleTextField) && "QtVtkExtrinsicTransformationPanel::getXOYAngle : null angle text field.");
	bool	ok		= true;
	double	value	= _xOyAngleTextField->text ( ).toDouble (&ok);

	if (false == ok)
	{
		UTF8String	errorMsg (charset);
		errorMsg << _appTitle << " : valeur invalide de l'angle xOy (" << _xOyAngleTextField->text ( ).toStdString ( ) << ")";
		throw Exception (errorMsg);
	}	// if (false == ok)

	return value;
}	// QtVtkExtrinsicTransformationPanel::getXOYAngle


void QtVtkExtrinsicTransformationPanel::setXOZAngle (double angle)
{
	assert ((0 != _xOzAngleTextField) && "QtVtkExtrinsicTransformationPanel::setXOZAngle : null angle text field.");
	_xOzAngleTextField->setText (QString::number (angle));
}	// QtVtkExtrinsicTransformationPanel::setXOZAngle


double QtVtkExtrinsicTransformationPanel::getXOZAngle ( ) const
{
	assert ((0 != _xOzAngleTextField) && "QtVtkExtrinsicTransformationPanel::getXOZAngle : null sectionRaangledius text field.");
	bool	ok		= true;
	double	value	= _xOzAngleTextField->text ( ).toDouble (&ok);

	if (false == ok)
	{
		UTF8String	errorMsg (charset);
		errorMsg << _appTitle << " : valeur invalide de l'angle xOz (" << _xOzAngleTextField->text ( ).toStdString ( ) << ")";
		throw Exception (errorMsg);
	}	// if (false == ok)

	return value;
}	// QtVtkExtrinsicTransformationPanel::getXOZAngle


void QtVtkExtrinsicTransformationPanel::setYOZAngle (double angle)
{
	assert ((0 != _yOzAngleTextField) && "QtVtkExtrinsicTransformationPanel::setYOZAngle : null angle text field.");
	_yOzAngleTextField->setText (QString::number (angle));
}	// QtVtkExtrinsicTransformationPanel::setYOZAngle


double QtVtkExtrinsicTransformationPanel::getYOZAngle ( ) const
{
	assert ((0 != _yOzAngleTextField) && "QtVtkExtrinsicTransformationPanel::getYOZAngle : null angle text field.");
	bool	ok		= true;
	double	value	= _yOzAngleTextField->text ( ).toDouble (&ok);

	if (false == ok)
	{
		UTF8String	errorMsg (charset);
		errorMsg << _appTitle << " : valeur invalide de l'angle yOz (" << _yOzAngleTextField->text ( ).toStdString ( ) << ")";
		throw Exception (errorMsg);
	}	// if (false == ok)

	return value;
}	// QtVtkExtrinsicTransformationPanel::getYOZAngle


void QtVtkExtrinsicTransformationPanel::setTranslationFirst (bool translationFirst)
{
	assert ((0 != _translationFirstCheckBox) && "QtVtkExtrinsicTransformationPanel::setTranslationFirst : null coordinate system checkbox.");
	_translationFirstCheckBox->setChecked (translationFirst);
	transformationModifiedCallback ( ) ;
}	// QtVtkExtrinsicTransformationPanel::setTranslationFirst


bool QtVtkExtrinsicTransformationPanel::isTranslationFirst ( ) const
{
	assert ((0 != _translationFirstCheckBox) && "QtVtkExtrinsicTransformationPanel::isTranslationFirst : null coordinate system checkbox.");

	return _translationFirstCheckBox->isChecked ( );
}	// QtVtkExtrinsicTransformationPanel::isTranslationFirst


void QtVtkExtrinsicTransformationPanel::setTranslation (double dx, double dy, double dz)
{
	assert ((0 != _translationPanel) && "QtVtkExtrinsicTransformationPanel::setTranslation : null translation panel.");
	_translationPanel->setX (dx);
	_translationPanel->setY (dy);
	_translationPanel->setZ (dz);
}	// QtVtkExtrinsicTransformationPanel::setTranslation


void QtVtkExtrinsicTransformationPanel::getTranslation (double& dx, double& dy, double& dz) const
{
	assert ((0 != _translationPanel) && "QtVtkExtrinsicTransformationPanel::getTranslation : null translation panel.");
	dx	= _translationPanel->getX ( );
	dy	= _translationPanel->getY ( );
	dz	= _translationPanel->getZ ( );
}	// QtVtkExtrinsicTransformationPanel::getTranslation


vtkTransform* QtVtkExtrinsicTransformationPanel::getTransformation ( ) const
{
	vtkTransform*	transformation	= vtkTransform::New ( );
	assert (0 != transformation);

	// Passage en mode PostMultiply pour le repère extrinsèque (global)
	transformation->PostMultiply ( );

	const bool	translationFirst	= isTranslationFirst ( );
	double	dx	= 0., dy	= 0., dz	= 0.;
	getTranslation (dx, dy, dz);

	// Application des transformations dans l'ordre exact de l'énoncé
	if (true == translationFirst)
		transformation->Translate (dx, dy, dz);
		
	// Rotations autour de Oz, puis Oz, puis Oy :
	transformation->RotateZ (getXOYAngle ( ));
	transformation->RotateX (getYOZAngle ( ));
	transformation->RotateY (getXOZAngle ( ));

	if (false == translationFirst)
		transformation->Translate (dx, dy, dz);

	return transformation;
}	// QtVtkExtrinsicTransformationPanel::getTransformation


UTF8String QtVtkExtrinsicTransformationPanel::getTransformationDescription ( ) const
{
	UTF8String	description (charset);
	
	const bool	translationFirst	= isTranslationFirst ( );
	double	dx	= 0., dy	= 0., dz	= 0.;
	getTranslation (dx, dy, dz);
	if (true == translationFirst)
	{
		description << "Transformation extrinsèque. Translation de (" << dx << ", " << dy << ", " << dz 
		            << ") suivie de rotations autour de Oz = " << getXOYAngle ( ) << " degrés, autour de Ox = " << getYOZAngle ( ) 
		            << " degrés, autour de Oy = " << getXOZAngle ( ) << " degrés.";
	}	// if (true == translationFirst)
	else
	{
		description << "Transformation extrinsèque. Rotations autour de Oz = " << getXOYAngle ( ) << " degrés, autour de Ox = " << getYOZAngle ( ) 
		            << " degrés, autour de Oy = " << getXOZAngle ( ) << " degrés, suivie d'une translation de (" << dx << ", " << dy << ", " << dz << ")";
	}
	
	return description;
}	// QtVtkExtrinsicTransformationPanel::getTransformationDescription


void QtVtkExtrinsicTransformationPanel::transformationModifiedCallback ( )
{
	emit (transformationChanged ( ));
}	// QtVtkExtrinsicTransformationPanel::transformationModifiedCallback


// ===========================================================================
//                    QtVtkIntrinsicTransformationPanel
// ===========================================================================

QtVtkIntrinsicTransformationPanel::QtVtkIntrinsicTransformationPanel (
	QWidget* parent, const IN_UTIL UTF8String& appTitle, vtkRenderer* renderer, vtkFloatingPointType bounds [6], bool displayTrihedrons, double phi, double theta, double omega, bool translationFirst, double dx, double dy, double dz)
	: QWidget (parent), _appTitle (appTitle), _translationFirstCheckBox (0), _translationPanel (0), _phiAngleTextField (0), _thetaAngleTextField (0), _omegaAngleTextField (0),
	  _trihedronCheckBox (0), _globalTrihedron (0), _localTrihedron (0), _renderer (renderer)
{
	memcpy (_bounds, bounds, 6 * sizeof (vtkFloatingPointType));

	QtAutoWaitingCursor		waitingCursor (true);

	// Creation de l'ihm :
	QVBoxLayout*	layout	= new QVBoxLayout (this);
	layout->setSpacing (QtVtkTransformationPanel::spacing);
#ifdef QT_5
	layout->setMargin (QtVtkTransformationPanel::margin);
#else	// QT_5
	layout->setContentsMargins (QtVtkTransformationPanel::margin, QtVtkTransformationPanel::margin, QtVtkTransformationPanel::margin, QtVtkTransformationPanel::margin);
#endif	// QT_5
	layout->setSizeConstraint (QLayout::SetMinimumSize);

	// Translation en premier ? :
	_translationFirstCheckBox	= new QCheckBox (QSTR ("Translation en premier"), this);
	connect (_translationFirstCheckBox, SIGNAL(clicked ( )), this, SLOT(transformationModifiedCallback ( )));
	QString	tip (QSTR ("Coché, la translation est effectuée avant les rotations."));
	_translationFirstCheckBox->setChecked (translationFirst);
	_translationFirstCheckBox->setToolTip (tip);
	layout->addWidget (_translationFirstCheckBox);

	// La translation :
	_translationPanel	= new Qt3DDataPanel (this, "Translation :", true, "x : ", "y : ", "z : ", dx, -DBL_MAX, DBL_MAX, dy, -DBL_MAX, DBL_MAX, dz, -DBL_MAX, DBL_MAX, false);
	if (0 != _translationPanel->layout ( ))
	{
		_translationPanel->layout ( )->setContentsMargins (QtConfiguration::margin, QtConfiguration::margin, QtConfiguration::margin, QtConfiguration::margin);
		_translationPanel->layout ( )->setSpacing (QtVtkTransformationPanel::spacing);
	}
	connect (_translationPanel->getXTextField ( ), SIGNAL (returnPressed ( )), this, SLOT (transformationModifiedCallback ( )));
	connect (_translationPanel->getYTextField ( ), SIGNAL (returnPressed ( )), this, SLOT (transformationModifiedCallback ( )));
	connect (_translationPanel->getZTextField ( ), SIGNAL (returnPressed ( )), this, SLOT (transformationModifiedCallback ( )));
	layout->addWidget (_translationPanel);
	_translationPanel->setToolTip (QSTR ("Zone de saisie de la translation de la transformation intrinsèque."));

	// Les rotations :
	QtGroupBox*		phiGroupBox	= new QtGroupBox (this);
	QHBoxLayout*	qhLayout	= new QHBoxLayout (phiGroupBox);
	phiGroupBox->setLayout (qhLayout);
//	phiGroupBox->setMargin (QtVtkTransformationPanel::margin);
//	phiGroupBox->setSpacing (QtVtkTransformationPanel::spacing);
	qhLayout->setContentsMargins (QtConfiguration::margin, QtConfiguration::margin, QtConfiguration::margin, QtConfiguration::margin);
	qhLayout->setSpacing (QtConfiguration::spacing);
	layout->addWidget (phiGroupBox);
	QLabel*	angleLabel	= new QLabel (QSTR ("Angle ") + QtStringHelper::phiMin ( ), phiGroupBox);
	qhLayout->addWidget (angleLabel);
	_phiAngleTextField	= new QtTextField (QString::number (phi), phiGroupBox);
	_phiAngleTextField->setValidator (new QDoubleValidator (-180., 180., 4, _phiAngleTextField));
	qhLayout->addWidget (_phiAngleTextField);
	connect (_phiAngleTextField, SIGNAL (returnPressed ( )), this, SLOT (transformationModifiedCallback ( )));
	_phiAngleTextField->setFixedSize (_phiAngleTextField->sizeHint ( ));
	qhLayout->addWidget (new QLabel (QSTR ("degrés"), phiGroupBox));
	_phiAngleTextField->setToolTip (QSTR ("Zone de saisie de l'angle de rotation ") + QtStringHelper::phiMin ( ) + QSTR (" autour de l'axe Oy dans le repère local."));
	angleLabel->setToolTip (QSTR ("Zone de saisie de l'angle de rotation ") + QtStringHelper::phiMin ( ) + QSTR (" autour de l'axe Oy dans le repère local."));
	qhLayout->addStretch (200);
	
	QtGroupBox*		thetaGroupBox	= new QtGroupBox (this);
	qhLayout	= new QHBoxLayout (thetaGroupBox);
	thetaGroupBox->setLayout (qhLayout);
//	thetaGroupBox->setMargin (QtVtkTransformationPanel::margin);
//	thetaGroupBox->setSpacing (QtVtkTransformationPanel::spacing);
	qhLayout->setContentsMargins (QtConfiguration::margin, QtConfiguration::margin, QtConfiguration::margin, QtConfiguration::margin);
	qhLayout->setSpacing (QtConfiguration::spacing);
	layout->addWidget (thetaGroupBox);
	angleLabel	= new QLabel (QSTR ("Angle ") + QtStringHelper::thetaMaj ( ), thetaGroupBox);
	qhLayout->addWidget (angleLabel);
	_thetaAngleTextField	= new QtTextField (QString::number (theta), thetaGroupBox);
	_thetaAngleTextField->setValidator (new QDoubleValidator (-180., 180., 4, _thetaAngleTextField));
	qhLayout->addWidget (_thetaAngleTextField);
	connect (_thetaAngleTextField, SIGNAL (returnPressed ( )), this, SLOT (transformationModifiedCallback ( )));
	_thetaAngleTextField->setFixedSize (_thetaAngleTextField->sizeHint ( ));
	qhLayout->addWidget (new QLabel (QSTR ("degrés"), thetaGroupBox));
	_thetaAngleTextField->setToolTip (QSTR ("Zone de saisie de l'angle de rotation ") + QtStringHelper::thetaMaj ( ) + QSTR (" autour de l'axe O'z' dans le repère local."));
	angleLabel->setToolTip (QSTR ("Zone de saisie de l'angle de rotation ") + QtStringHelper::thetaMaj ( ) + QSTR (" autour de l'axe O'z' dans le repère local."));
	qhLayout->addStretch (200);
	
	QtGroupBox*		omegaGroupBox	= new QtGroupBox (this);
	qhLayout	= new QHBoxLayout (omegaGroupBox);
	omegaGroupBox->setLayout (qhLayout);
//	omegaGroupBox->setMargin (QtVtkTransformationPanel::margin);
//	omegaGroupBox->setSpacing (QtVtkTransformationPanel::spacing);
	qhLayout->setContentsMargins (QtConfiguration::margin, QtConfiguration::margin, QtConfiguration::margin, QtConfiguration::margin);
	qhLayout->setSpacing (QtConfiguration::spacing);
	layout->addWidget (omegaGroupBox);
	angleLabel	= new QLabel (QSTR ("Angle ") + QtStringHelper::omegaMaj ( ), omegaGroupBox);
	qhLayout->addWidget (angleLabel);
	_omegaAngleTextField	= new QtTextField (QString::number (omega), omegaGroupBox);
	_omegaAngleTextField->setValidator (new QDoubleValidator (-180., 180., 4, _omegaAngleTextField));
	qhLayout->addWidget (_omegaAngleTextField);
	connect (_omegaAngleTextField, SIGNAL (returnPressed ( )), this, SLOT (transformationModifiedCallback ( )));
	_omegaAngleTextField->setFixedSize (_omegaAngleTextField->sizeHint ( ));
	qhLayout->addWidget (new QLabel (QSTR ("degrés"), omegaGroupBox));
	_omegaAngleTextField->setToolTip (QSTR ("Zone de saisie de l'angle de rotation ") + QtStringHelper::omegaMin ( ) + QSTR (" autour de l'axe O'x' dans le repère local."));
	angleLabel->setToolTip (QSTR ("Zone de saisie de l'angle de rotation ") + QtStringHelper::omegaMin ( ) + QSTR (" autour de l'axe O'x' dans le repère local."));
	qhLayout->addStretch (200);

	// Afficher les trièdres ?
	_trihedronCheckBox	= new QCheckBox (QSTR ("Afficher les trièdres"), this);
	connect (_trihedronCheckBox, SIGNAL(clicked ( )), this, SLOT(displayTrihedronsCallback ( )));
	_trihedronCheckBox->setChecked (displayTrihedrons);
	_trihedronCheckBox->setToolTip (QSTR ("Cochée des trièdres montrant le repère avant et après transformation seront affichés dans la fenêtre graphique."));
	layout->addWidget (_trihedronCheckBox);
	
	layout->activate ( );
	setMinimumSize (layout->sizeHint ( ));
}	// QtVtkIntrinsicTransformationPanel::QtVtkIntrinsicTransformationPanel


QtVtkIntrinsicTransformationPanel::QtVtkIntrinsicTransformationPanel (const QtVtkIntrinsicTransformationPanel&)
{
	assert (0 && "QtVtkIntrinsicTransformationPanel copy constructor is forbidden.");
}	// QtVtkIntrinsicTransformationPanel::QtVtkIntrinsicTransformationPanel (const QtVtkIntrinsicTransformationPanel&)


QtVtkIntrinsicTransformationPanel& QtVtkIntrinsicTransformationPanel::operator = (const QtVtkIntrinsicTransformationPanel&)
{
	assert (0 && "QtVtkIntrinsicTransformationPanel assignment operator is forbidden.");
	return *this;
}	// QtVtkIntrinsicTransformationPanel::operator =


QtVtkIntrinsicTransformationPanel::~QtVtkIntrinsicTransformationPanel ( )
{
	assert (0 != _trihedronCheckBox);
	_trihedronCheckBox->setChecked (false);
}	// QtVtkIntrinsicTransformationPanel::~QtVtkIntrinsicTransformationPanel


void QtVtkIntrinsicTransformationPanel::setPhiAngle (double angle)
{
	assert ((0 != _phiAngleTextField) && "QtVtkIntrinsicTransformationPanel::setPhiAngle : null angle text field.");
	_phiAngleTextField->setText (QString::number (angle));
}	// QtVtkIntrinsicTransformationPanel::setPhiAngle


double QtVtkIntrinsicTransformationPanel::getPhiAngle ( ) const
{
	assert ((0 != _phiAngleTextField) && "QtVtkIntrinsicTransformationPanel::getPhiAngle : null angle text field.");
	bool	ok		= true;
	double	value	= _phiAngleTextField->text ( ).toDouble (&ok);

	if (false == ok)
	{
		UTF8String	errorMsg (charset);
		errorMsg << _appTitle << " : valeur invalide de l'angle phi (" << _phiAngleTextField->text ( ).toStdString ( ) << ")";
		throw Exception (errorMsg);
	}	// if (false == ok)

	return value;
}	// QtVtkIntrinsicTransformationPanel::getPhiAngle


void QtVtkIntrinsicTransformationPanel::setThetaAngle (double angle)
{
	assert ((0 != _thetaAngleTextField) && "QtVtkIntrinsicTransformationPanel::setThetaAngle : null angle text field.");
	_thetaAngleTextField->setText (QString::number (angle));
}	// QtVtkIntrinsicTransformationPanel::setThetaAngle


double QtVtkIntrinsicTransformationPanel::getThetaAngle ( ) const
{
	assert ((0 != _thetaAngleTextField) && "QtVtkIntrinsicTransformationPanel::getThetaAngle : null sectionRaangledius text field.");
	bool	ok		= true;
	double	value	= _thetaAngleTextField->text ( ).toDouble (&ok);

	if (false == ok)
	{
		UTF8String	errorMsg (charset);
		errorMsg << _appTitle << " : valeur invalide de l'angle theta (" << _thetaAngleTextField->text ( ).toStdString ( ) << ")";
		throw Exception (errorMsg);
	}	// if (false == ok)

	return value;
}	// QtVtkIntrinsicTransformationPanel::getThetaAngle


void QtVtkIntrinsicTransformationPanel::setOmegaAngle (double angle)
{
	assert ((0 != _omegaAngleTextField) && "QtVtkIntrinsicTransformationPanel::setOmegaAngle : null angle text field.");
	_omegaAngleTextField->setText (QString::number (angle));
}	// QtVtkIntrinsicTransformationPanel::setOmegaAngle


double QtVtkIntrinsicTransformationPanel::getOmegaAngle ( ) const
{
	assert ((0 != _omegaAngleTextField) && "QtVtkIntrinsicTransformationPanel::getOmegaAngle : null angle text field.");
	bool	ok		= true;
	double	value	= _omegaAngleTextField->text ( ).toDouble (&ok);

	if (false == ok)
	{
		UTF8String	errorMsg (charset);
		errorMsg << _appTitle << " : valeur invalide de l'angle omega (" << _omegaAngleTextField->text ( ).toStdString ( ) << ")";
		throw Exception (errorMsg);
	}	// if (false == ok)

	return value;
}	// QtVtkIntrinsicTransformationPanel::getOmegaAngle


void QtVtkIntrinsicTransformationPanel::setTranslationFirst (bool translationFirst)
{
	assert ((0 != _translationFirstCheckBox) && "QtVtkIntrinsicTransformationPanel::setTranslationFirst : null coordinate system checkbox.");
	_translationFirstCheckBox->setChecked (translationFirst);
	transformationModifiedCallback ( ) ;
}	// QtVtkIntrinsicTransformationPanel::setTranslationFirst


bool QtVtkIntrinsicTransformationPanel::isTranslationFirst ( ) const
{
	assert ((0 != _translationFirstCheckBox) && "QtVtkIntrinsicTransformationPanel::isTranslationFirst : null coordinate system checkbox.");

	return _translationFirstCheckBox->isChecked ( );
}	// QtVtkIntrinsicTransformationPanel::isTranslationFirst


void QtVtkIntrinsicTransformationPanel::setTranslation (double x, double y, double z)
{
	assert ((0 != _translationPanel) && "QtVtkIntrinsicTransformationPanel::setTranslation : null translation panel.");
	_translationPanel->setX (x);
	_translationPanel->setY (y);
	_translationPanel->setZ (z);
}	// QtVtkIntrinsicTransformationPanel::setTranslation


void QtVtkIntrinsicTransformationPanel::getTranslation (double& x, double& y, double& z) const
{
	assert ((0 != _translationPanel) && "QtVtkIntrinsicTransformationPanel::getTranslation : null translation panel.");
	x	= _translationPanel->getX ( );
	y	= _translationPanel->getY ( );
	z	= _translationPanel->getZ ( );
}	// QtVtkIntrinsicTransformationPanel::getTranslation


vtkTransform* QtVtkIntrinsicTransformationPanel::getTransformation ( ) const
{
	vtkTransform*	transform	= vtkTransform::New ( );	// PreMultiply
	assert (0 != transform);

	// Passage en mode PreMultiply pour le repère intrinsèque (local)
	transform->PreMultiply ( );

	const bool	translationFirst	= isTranslationFirst ( );
	double	dx	= 0., dy	= 0., dz	= 0.;
	getTranslation (dx, dy, dz);

	if (true == translationFirst)
		transform->Translate (dx, dy, dz);
	transform->RotateY (getPhiAngle ( ));
	transform->RotateZ (getThetaAngle ( ));
	transform->RotateX (getOmegaAngle ( ));
	if (false == translationFirst)
		transform->Translate (dx, dy, dz);

	return transform;
}	// QtVtkIntrinsicTransformationPanel::getTransformation


UTF8String QtVtkIntrinsicTransformationPanel::getTransformationDescription ( ) const
{
	UTF8String	description (charset);

	double	x	= 0., y	= 0., z	= 0.;
	getTranslation (x, y, z);
	description << "Transformation intrinsèque. Phi = " << getPhiAngle ( ) << " degrés, thêta = " << getThetaAngle ( ) << " degrés, oméga = " 
		        << getOmegaAngle ( ) << " degrés, centrée en (" << x << ", " << y << ", " << z << ")";


	const bool	translationFirst	= isTranslationFirst ( );
	double		dx	= 0., dy	= 0., dz	= 0.;
	getTranslation (dx, dy, dz);
	if (true == translationFirst)
	{
		description << "Transformation intrinsèque. Translation de (" << dx << ", " << dy << ", " << dz 
		            << ") suivie de rotations phi (autour de Oy) = " << getPhiAngle ( ) << " degrés, thêta (autour de Oz) = " << getThetaAngle ( ) 
		            << " degrés, omega (autour de Ox) = " << getOmegaAngle ( ) << " degrés.";
	}	// if (true == translationFirst)
	else
	{
		description << "Transformation intrinsèque. Rotations phi (autour de Oy) = " << getPhiAngle ( ) << " degrés, thêta (autour de Oz) = " << getThetaAngle ( ) 
		            << " degrés, omega (autour de Ox) = " << getOmegaAngle ( ) << " degrés, suivie d'une translation de (" << dx << ", " << dy << ", " << dz << ") dans le repère local.";
	}	// else if (true == translationFirst)

	return description;
}	// QtVtkIntrinsicTransformationPanel::getTransformationDescription


bool QtVtkIntrinsicTransformationPanel::displayTrihedron ( ) const
{
	assert (0 != _trihedronCheckBox);
	return _trihedronCheckBox->isChecked ( );
}	// QtVtkIntrinsicTransformationPanel::displayTrihedron


void QtVtkIntrinsicTransformationPanel::showEvent (QShowEvent* event)
{
	QWidget::showEvent (event);
	
	if ((0 != _renderer) && (true == displayTrihedron ( )))
	{
		if ((0 == _globalTrihedron) || (0 == _localTrihedron))
			displayTrihedronsCallback ( );
		else
		{
			if (0 != _globalTrihedron)
				_renderer->AddActor (_globalTrihedron);
			if (0 != _localTrihedron)
				_renderer->AddActor (_localTrihedron);
			if (0 != _renderer->GetRenderWindow ( ))
				_renderer->GetRenderWindow ( )->Render ( );
		}	// else if ((0 != _renderer) && (true == displayTrihedron ( )))
	}	// if ((0 != _renderer) && (true == displayTrihedron ( )))
}	// QtVtkIntrinsicTransformationPanel::showEvent


void QtVtkIntrinsicTransformationPanel::hideEvent (QHideEvent* event)
{
	if ((0 != _renderer) && (true == displayTrihedron ( )))
	{
		if (0 != _globalTrihedron)
			_renderer->RemoveActor (_globalTrihedron);
		if (0 != _localTrihedron)
			_renderer->RemoveActor (_localTrihedron);
		if (0 != _renderer->GetRenderWindow ( ))
			_renderer->GetRenderWindow ( )->Render ( );
	}	// if ((0 != _renderer) && (true == displayTrihedron ( )))

	QWidget::hideEvent (event);
}	// QtVtkIntrinsicTransformationPanel::hideEvent


void QtVtkIntrinsicTransformationPanel::transformationModifiedCallback ( )
{
	if ((0 != _renderer) && (true == displayTrihedron ( )))
	{
		if (0 != _localTrihedron)
		{
			vtkSmartPointer<vtkTransform>	transform (getTransformation ( ));
			_localTrihedron->SetTransform (transform);
			double	center [3]	= { 0., 0., 0. };
			if (0 != transform)
				transform->GetPosition (center);
			double	scale [3]	= {
				_localTrihedron->GetXAxisArrowActor ( ).GetScale ( )[0],
				_localTrihedron->GetYAxisArrowActor ( ).GetScale ( )[1],
				_localTrihedron->GetZAxisArrowActor ( ).GetScale ( )[2],
				};
			double	xcoords [3]	= { scale [0] * 1., 0.1, 0. };
			transform->MultiplyPoint (xcoords, xcoords);
			_localTrihedron->GetXAxisLabelActor2D ( )->GetPositionCoordinate ( )->SetCoordinateSystemToWorld ( );
			_localTrihedron->GetXAxisLabelActor2D ( )->GetPositionCoordinate ( )->SetValue (
				xcoords [0] + center [0], xcoords [1] + center [1], xcoords [2] + center [2]);
			double	ycoords [3]	= { 0.1, scale [1], 0. };
			transform->MultiplyPoint (ycoords, ycoords);
			_localTrihedron->GetYAxisLabelActor2D ( )->GetPositionCoordinate ( )->SetCoordinateSystemToWorld ( );
			_localTrihedron->GetYAxisLabelActor2D ( )->GetPositionCoordinate ( )->SetValue (
				ycoords [0], ycoords [1], ycoords [2]);	// center a priori inutile pour y ...
			double	zcoords [3]	= { 0., 0.1, scale [2] };
			transform->MultiplyPoint (zcoords, zcoords);
			_localTrihedron->GetZAxisLabelActor2D ( )->GetPositionCoordinate ( )->SetCoordinateSystemToWorld ( );
			_localTrihedron->GetZAxisLabelActor2D ( )->GetPositionCoordinate ( )->SetValue (
				zcoords [0] + center [0], zcoords [1] + center [1], zcoords [2] + center [2]);
		}	// if (0 != _localTrihedron)
		if (0 != _renderer->GetRenderWindow ( ))
			_renderer->GetRenderWindow ( )->Render ( );
	}	// if ((0 != _renderer) && (true == displayTrihedron ( )))

	emit (transformationChanged ( ));
}	// QtVtkIntrinsicTransformationPanel::transformationModifiedCallback


static vtkTrihedron* createTrihedron (bool global, vtkFloatingPointType bounds [6])
{
	// Les trièdres doivent dépasser légèrement de la zone d'intérêt :
	double	xscale	= 1.25 * (bounds [1] - bounds [0]);
	double	yscale	= 1.25 * (bounds [3] - bounds [2]);
	double	zscale	= 1.25 * (bounds [5] - bounds [4]);
	// On peut arriver du 2D : si les 3 sont ~nuls on est mort !
	if (fabs (yscale) < 1E-9)	yscale	= std::max (xscale, zscale);
	if (fabs (zscale) < 1E-9)	zscale	= std::max (xscale, yscale);
	if (fabs (xscale) < 1E-9)	xscale	= std::max (yscale, zscale);
	
	vtkTrihedron*	trihedron	= vtkTrihedron::New ( );
	const int labelOffset	= 2;
	if (true == global)
	{
		trihedron->SetAxisLabels ("x", "y", "z");
		trihedron->SetLabelsOffsets (-labelOffset, -labelOffset, -labelOffset);
	}
	else
	{
		trihedron->SetAxisLabels ("x'", "y'", "z'");
		trihedron->SetLabelsOffsets (labelOffset, labelOffset, labelOffset);
	}	// else if (true == global)
		
	trihedron->SetLabel2D (true);
	assert (0 != trihedron->GetXAxisLabelActor2D ( ));
	assert (0 != trihedron->GetYAxisLabelActor2D ( ));
	assert (0 != trihedron->GetZAxisLabelActor2D ( ));
	// Rem CP : c'est sans effet si je passe des valeurs != à SetScale pour x, y et z. Pourquoi ???
	trihedron->GetXAxisArrowActor ( ).SetScale (xscale, xscale, xscale);
	trihedron->GetYAxisArrowActor ( ).SetScale (yscale, yscale, yscale);
	trihedron->GetZAxisArrowActor ( ).SetScale (zscale, zscale, zscale);
	trihedron->GetXAxisLabelActor2D ( )->GetPositionCoordinate ( )->SetValue (xscale * 1., 0.1, 0.);
	trihedron->GetYAxisLabelActor2D ( )->GetPositionCoordinate ( )->SetValue (0.1, yscale * 1., 0.);
	trihedron->GetZAxisLabelActor2D ( )->GetPositionCoordinate ( )->SetValue (0., 0.1, zscale * 1.);
	trihedron->GetXAxisArrowSource ( ).SetShaftRadius (QtVtkTransformationPanel::shaftRadius / xscale);
	trihedron->GetXAxisArrowSource ( ).SetTipRadius (QtVtkTransformationPanel::tipRadius / xscale);
	trihedron->GetXAxisArrowSource ( ).SetTipLength (QtVtkTransformationPanel::tipLength / xscale);
	trihedron->GetYAxisArrowSource ( ).SetShaftRadius (QtVtkTransformationPanel::shaftRadius / yscale);
	trihedron->GetYAxisArrowSource ( ).SetTipRadius (QtVtkTransformationPanel::tipRadius / yscale);
	trihedron->GetYAxisArrowSource ( ).SetTipLength (QtVtkTransformationPanel::tipLength / yscale);
	trihedron->GetZAxisArrowSource ( ).SetShaftRadius (QtVtkTransformationPanel::shaftRadius / zscale);
	trihedron->GetZAxisArrowSource ( ).SetTipRadius (QtVtkTransformationPanel::tipRadius / zscale);
	trihedron->GetZAxisArrowSource ( ).SetTipLength (QtVtkTransformationPanel::tipLength / zscale);

	return trihedron;
}	// createTrihedron


void QtVtkIntrinsicTransformationPanel::displayTrihedronsCallback ( )
{
	if (0 != _renderer)
	{
		if (true == displayTrihedron ( ))
		{	
			if (0 == _globalTrihedron)
				_globalTrihedron	= createTrihedron (true, _bounds);
			if (0 == _localTrihedron)
				_localTrihedron		= createTrihedron (false, _bounds);
			
			_renderer->AddActor (_globalTrihedron);
			_renderer->AddActor (_localTrihedron);
			transformationModifiedCallback ( );
		}	// if (true == displayTrihedron ( ))
		else
		{
			if (0 != _globalTrihedron)
			{
				_renderer->RemoveActor (_globalTrihedron);
				_globalTrihedron->Delete ( );
				_globalTrihedron	= 0;
			}	// if (0 != _globalTrihedron)
			if (0 != _localTrihedron)
			{
				_renderer->RemoveActor (_localTrihedron);
				_localTrihedron->Delete ( );
				_localTrihedron	= 0;
			}	// if (0 != _localTrihedron)
			if (0 != _renderer->GetRenderWindow ( ))
				_renderer->GetRenderWindow ( )->Render ( );
		}	// else if if (true == displayTrihedron ( ))
	}	// if (0 != _renderer)
}	// QtVtkIntrinsicTransformationPanel::displayTrihedronsCallback


// ===========================================================================
//                         QtVtkTransformationPanel
// ===========================================================================

double	QtVtkTransformationPanel::shaftRadius	= 0.01, QtVtkTransformationPanel::tipRadius	= 0.03, QtVtkTransformationPanel::tipLength	= 0.1;
int		QtVtkTransformationPanel::margin		= QtConfiguration::margin, 							QtVtkTransformationPanel::spacing	= QtConfiguration::spacing;

QtVtkTransformationPanel::QtVtkTransformationPanel (
	QWidget* parent, const UTF8String& appTitle, vtkRenderer* renderer, vtkFloatingPointType bounds [6], const vtkTransformHelper::vtkSimpleTransformMemento& memento)
	: QWidget (parent), _appTitle (appTitle), _extrinsicCheckBox (0), _contextualHelpLabel (0), _imageLabel (0),_extrinsicPanel (0), _intrinsicPanel (0)
{
	QtAutoWaitingCursor		waitingCursor (true);

	// Creation de l'ihm :
	QHBoxLayout*	mainLayout	= new QHBoxLayout (this);
	QVBoxLayout*	layout		= new QVBoxLayout ( );
	mainLayout->addLayout (layout);
	layout->setSpacing (QtVtkTransformationPanel::spacing);
#ifdef QT_5
	layout->setMargin (QtVtkTransformationPanel::margin);
#else	// QT_5
	layout->setContentsMargins (QtVtkTransformationPanel::margin, QtVtkTransformationPanel::margin, QtVtkTransformationPanel::margin, QtVtkTransformationPanel::margin);
#endif	// QT_5
	layout->setSizeConstraint (QLayout::SetMinimumSize);

	// L'aide associée :
	_contextualHelpLabel	= new QLabel ("Aide contextuelle", this);
	layout->addWidget (_contextualHelpLabel);

	// Type de transformation :
	_extrinsicCheckBox	= new QCheckBox (QSTR ("Transformation extrinsèque"), this);
	connect (_extrinsicCheckBox, SIGNAL(clicked ( )), this, SLOT(transformationTypeModifiedCallback ( )));
	QString	tip (QSTR ("Coché, la transformation est extrinsèque, c.a.d. dans le repère global, les axes ne suivent pas les rotations."));
	_extrinsicCheckBox->setToolTip (tip);
	layout->addWidget (_extrinsicCheckBox);
	
	// Les panneaux de transformation :
	_extrinsicPanel	= new QtVtkExtrinsicTransformationPanel (this, _appTitle);
	connect (_extrinsicPanel, SIGNAL (transformationChanged ( )), this, SLOT (transformationModifiedCallback ( )));
	layout->addWidget (_extrinsicPanel);
	_intrinsicPanel	= new QtVtkIntrinsicTransformationPanel (this, _appTitle, renderer, bounds, (bool)memento.userData);
	connect (_intrinsicPanel, SIGNAL (transformationChanged ( )), this, SLOT (transformationModifiedCallback ( )));
	layout->addWidget (_intrinsicPanel);

	// La représentation de la transformation :
	_imageLabel	= new QLabel ("", this);
	mainLayout->addWidget (_imageLabel);
	
	// On affiche le bon panneau :
	_extrinsicCheckBox->setChecked (memento.isExtrinsic);
	if (true == memento.isExtrinsic)
	{
		_extrinsicPanel->setXOYAngle (memento.xoy);
		_extrinsicPanel->setXOZAngle (memento.xoz);
		_extrinsicPanel->setYOZAngle (memento.yoz);
		_extrinsicPanel->setTranslationFirst (memento.translationFirst);
		_extrinsicPanel->setTranslation (memento.dx, memento.dy, memento.dz);
	}	// if (true == memento.isExtrinsic)
	else 
	{
		_intrinsicPanel->setTranslationFirst (memento.translationFirst);
		_intrinsicPanel->setTranslation (memento.dx, memento.dy, memento.dz);
		_intrinsicPanel->setPhiAngle (memento.xoz);
		_intrinsicPanel->setThetaAngle (memento.xoy);
		_intrinsicPanel->setOmegaAngle (memento.yoz);
	}	// else if (true == memento.isExtrinsic)

	transformationTypeModifiedCallback ( );
	mainLayout->activate ( );
}	// QtVtkTransformationPanel::QtVtkTransformationPanel


vtkTransformHelper::vtkSimpleTransformMemento QtVtkTransformationPanel::getMemento ( ) const
{
	assert (0 != _extrinsicPanel);
	assert (0 != _intrinsicPanel);

	vtkTransformHelper::vtkSimpleTransformMemento	memento;
	memento.isExtrinsic	= isExtrinsic ( );

	if (true == memento.isExtrinsic)
	{
		memento.xoy					= _extrinsicPanel->getXOYAngle ( );
		memento.xoz					= _extrinsicPanel->getXOZAngle ( );
		memento.yoz					= _extrinsicPanel->getYOZAngle ();
		memento.translationFirst	= _extrinsicPanel->isTranslationFirst ( );
		_extrinsicPanel->getTranslation (memento.dx, memento.dy, memento.dz);
	}	// if (true == memento.isExtrinsic)
	else
	{
		memento.xoz					= _intrinsicPanel->getPhiAngle ( );
		memento.xoy					= _intrinsicPanel->getThetaAngle ( );
		memento.yoz					= _intrinsicPanel->getOmegaAngle ( );
		memento.translationFirst	= _intrinsicPanel->isTranslationFirst ( );
		_intrinsicPanel->getTranslation (memento.dx, memento.dy, memento.dz);
	}	// else if (true == memento.isExtrinsic)

	return memento;
}	// QtVtkTransformationPanel::getMemento


QtVtkTransformationPanel::QtVtkTransformationPanel (const QtVtkTransformationPanel&)
{
	assert (0 && "QtVtkTransformationPanel copy constructor is forbidden.");
}	// QtVtkTransformationPanel::QtVtkTransformationPanel (const QtVtkTransformationPanel&)


QtVtkTransformationPanel& QtVtkTransformationPanel::operator = (const QtVtkTransformationPanel&)
{
	assert (0 && "QtVtkTransformationPanel assignment operator is forbidden.");
	return *this;
}	// QtVtkTransformationPanel::operator =


QtVtkTransformationPanel::~QtVtkTransformationPanel ( )
{
}	// QtVtkTransformationPanel::~QtVtkTransformationPanel


void QtVtkTransformationPanel::setExtrinsic (bool extrinsic)
{
	assert ((0 != _extrinsicCheckBox) && "QtVtkTransformationPanel::setExtrinsic : null extrinsic checkbox.");
	_extrinsicCheckBox->setChecked (extrinsic);
	transformationModifiedCallback ( ) ;
}	// QtVtkTransformationPanel::setExtrinsic


bool QtVtkTransformationPanel::isExtrinsic ( ) const
{
	assert ((0 != _extrinsicCheckBox) && "QtVtkTransformationPanel::isExtrinsic : null extrinsic checkbox.");

	return _extrinsicCheckBox->isChecked ( );
}	// QtVtkTransformationPanel::isExtrinsic


vtkTransform* QtVtkTransformationPanel::getTransformation ( ) const
{
	assert (0 != _extrinsicPanel);
	assert (0 != _intrinsicPanel);
	return true == isExtrinsic ( ) ? _extrinsicPanel->getTransformation ( ) : _intrinsicPanel->getTransformation ( );
}	// QtVtkTransformationPanel::getTransformation


UTF8String QtVtkTransformationPanel::getTransformationDescription ( ) const
{
	assert (0 != _extrinsicPanel);
	assert (0 != _intrinsicPanel);
	return true == isExtrinsic ( ) ? _extrinsicPanel->getTransformationDescription ( ) : _intrinsicPanel->getTransformationDescription ( );
}	// QtVtkTransformationPanel::getTransformationDescription


const QtVtkExtrinsicTransformationPanel& QtVtkTransformationPanel::getExtrinsicPanel ( ) const
{
	assert (0 != _extrinsicPanel);
	return *_extrinsicPanel;
}	// QtVtkTransformationPanel::getExtrinsicPanel


const QtVtkIntrinsicTransformationPanel& QtVtkTransformationPanel::getIntrinsicPanel ( ) const
{
	assert (0 != _intrinsicPanel);
	return *_intrinsicPanel;
}	// QtVtkTransformationPanel::getIntrinsicPanel


void QtVtkTransformationPanel::transformationTypeModifiedCallback ( )
{
	assert (0 != _intrinsicPanel);
	assert (0 != _extrinsicPanel);
	assert (0 != _contextualHelpLabel);
	assert (0 != _imageLabel);
	const bool	extrinsic	= isExtrinsic ( );
	if (true == extrinsic)
	{
		_intrinsicPanel->setVisible (false);
		_extrinsicPanel->setVisible (true);
		_contextualHelpLabel->setText (QSTR ("Transformation extrinsèque (dans le repère global) :\nles axes ne bougent pas."));
		QPixmap	pixmap (":/images/extrinsic.png");
		QSize	s	= pixmap.size ( );
		pixmap	= pixmap.scaledToHeight (0.5 * s.height ( ), Qt::SmoothTransformation);
		_imageLabel->setPixmap (pixmap);
	}	// if (true == extrinsic)
	else
	{
		_extrinsicPanel->setVisible (false);
		_intrinsicPanel->setVisible (true);
		_contextualHelpLabel->setText (QSTR ("Transformation intrinsèque (dans le repère local) :\nles axes sont soumis à chaque étape de la transformation."));
		QPixmap	pixmap (":/images/intrinsic_2.png");
		QSize	s	= pixmap.size ( );
		pixmap	= pixmap.scaledToHeight (1.5 * _intrinsicPanel->height ( ), Qt::SmoothTransformation);
		_imageLabel->setPixmap (pixmap);
	}	// else if (true == extrinsic)
	resize (sizeHint ( ));

	emit (transformationTypeChanged ( ));
}	// QtVtkTransformationPanel::transformationTypeModifiedCallback


void QtVtkTransformationPanel::transformationModifiedCallback ( )
{
	emit (transformationChanged ( ));
}	// QtVtkTransformationPanel::transformationModifiedCallback

