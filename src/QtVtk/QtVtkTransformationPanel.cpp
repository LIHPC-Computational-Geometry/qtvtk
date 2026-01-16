/**
 * \file		QtVtkTransformationPanel.cpp
 * \author		Charles PIGNEROL, CEA/DAM/DCLC
 * \date		16/01/2026
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
	layout->setSpacing (QtConfiguration::spacing);
#ifdef QT_5
	layout->setMargin (QtConfiguration::margin);
#else	// QT_5
	layout->setContentsMargins (QtConfiguration::margin, QtConfiguration::margin, QtConfiguration::margin, QtConfiguration::margin);
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
	xOyGroupBox->setMargin (QtConfiguration::margin);
	xOyGroupBox->setSpacing (QtConfiguration::spacing);
	layout->addWidget (xOyGroupBox);
	QLabel*	angleLabel	= new QLabel (QSTR ("Angle de rotation dans le plan xOy:"), xOyGroupBox);
	qhLayout->addWidget (angleLabel);
	_xOyAngleTextField	= new QtTextField (QString::number (xoy), xOyGroupBox);
	_xOyAngleTextField->setValidator (new QDoubleValidator (-180., 180., 0, _xOyAngleTextField));
	qhLayout->addWidget (_xOyAngleTextField);
	connect (_xOyAngleTextField, SIGNAL (returnPressed ( )), this, SLOT (transformationModifiedCallback ( )));
	_xOyAngleTextField->setFixedSize (_xOyAngleTextField->sizeHint ( ));
	qhLayout->addStretch (200);
	_xOyAngleTextField->setToolTip (QSTR ("Zone de saisie de l'angle de rotation de la transformation dans le plan xOy."));

	QtGroupBox*		xOzGroupBox	= new QtGroupBox (this);
	qhLayout	= new QHBoxLayout (xOzGroupBox);
	xOzGroupBox->setLayout (qhLayout);
	xOzGroupBox->setMargin (QtConfiguration::margin);
	xOzGroupBox->setSpacing (QtConfiguration::spacing);
	layout->addWidget (xOzGroupBox);
	angleLabel	= new QLabel (QSTR ("Angle de rotation dans le plan xOz:"), xOzGroupBox);
	qhLayout->addWidget (angleLabel);
	_xOzAngleTextField	= 	new QtTextField (QString::number (xoz), xOzGroupBox);
	_xOzAngleTextField->setValidator (new QDoubleValidator (-180., 180., 0, _xOzAngleTextField));
	qhLayout->addWidget (_xOzAngleTextField);
	connect (_xOzAngleTextField, SIGNAL (returnPressed ( )), this, SLOT (transformationModifiedCallback ( )));
	_xOzAngleTextField->setFixedSize (_xOzAngleTextField->sizeHint ( ));
	qhLayout->addStretch (200);
	_xOzAngleTextField->setToolTip (QSTR ("Zone de saisie de l'angle de rotation de la transformation dans le plan xOz."));

	QtGroupBox*		yOzGroupBox	= new QtGroupBox (this);
	qhLayout	= new QHBoxLayout (yOzGroupBox);
	yOzGroupBox->setLayout (qhLayout);
	yOzGroupBox->setMargin (QtConfiguration::margin);
	yOzGroupBox->setSpacing (QtConfiguration::spacing);
	layout->addWidget (yOzGroupBox);
	angleLabel	= new QLabel (QSTR ("Angle de rotation dans le plan yOz:"), yOzGroupBox);
	qhLayout->addWidget (angleLabel);
	_yOzAngleTextField	= 	new QtTextField (QString::number (yoz), yOzGroupBox);
	_yOzAngleTextField->setValidator (new QDoubleValidator (-180., 180., 0, _yOzAngleTextField));
	qhLayout->addWidget (_yOzAngleTextField);
	connect (_yOzAngleTextField, SIGNAL (returnPressed ( )), this, SLOT (transformationModifiedCallback ( )));
	_yOzAngleTextField->setFixedSize (_yOzAngleTextField->sizeHint ( ));
	qhLayout->addStretch (200);
	_yOzAngleTextField->setToolTip (QSTR ("Zone de saisie de l'angle de rotation de la transformation dans le plan yOz."));
	
	// La translation :
	_translationPanel	= new Qt3DDataPanel (this, "Translation :", true, "x : ", "y : ", "z : ", dx, -DBL_MAX, DBL_MAX, dy, -DBL_MAX, DBL_MAX, dz, -DBL_MAX, DBL_MAX, false);
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
	vtkTransform*	transformation	= vtkTransform::New ( );	// PreMultiply
	assert (0 != transformation);
	
	const bool	translationFirst	= isTranslationFirst ( );
	double	dx	= 0., dy	= 0., dz	= 0.;
	getTranslation (dx, dy, dz);
	if (false == translationFirst)
		transformation->Translate (dx, dy, dz);
	transformation->RotateY (getXOZAngle ( ));
	transformation->RotateX (getYOZAngle ( ));
	transformation->RotateZ (getXOYAngle ( ));
	if (true == translationFirst)
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
		            << ") suivie de rotations autour de Oy = " << getXOZAngle ( ) << " degrés, autour de Ox = " << getYOZAngle ( ) 
		            << " degrés, autour de Oz = " << getXOYAngle ( ) << " degrés.";
	}	// if (true == translationFirst)
	else
	{
		description << "Transformation extrinsèque. Rotations autour de Oy = " << getXOZAngle ( ) << " degrés, autour de Ox = " << getYOZAngle ( ) 
		            << " degrés, autour de Oz = " << getXOYAngle ( ) << " degrés, suivie d'une translation de (" << dx << ", " << dy << ", " << dz << ")";
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
	QWidget* parent, const UTF8String& appTitle, vtkRenderer* renderer, vtkFloatingPointType bounds [6], bool displayTrihedrons, double x, double y, double z, double phi, double theta, double omega)
	: QWidget (parent), _appTitle (appTitle), _centerPanel (0), _phiAngleTextField (0), _thetaAngleTextField (0), _omegaAngleTextField (0),
	  _trihedronCheckBox (0), _globalTrihedron (0), _localTrihedron (0), _renderer (renderer)
{
	memcpy (_bounds, bounds, 6 * sizeof (vtkFloatingPointType));

	QtAutoWaitingCursor		waitingCursor (true);

	// Creation de l'ihm :
	QVBoxLayout*	layout	= new QVBoxLayout (this);
	layout->setSpacing (QtConfiguration::spacing);
#ifdef QT_5
	layout->setMargin (QtConfiguration::margin);
#else	// QT_5
	layout->setContentsMargins (QtConfiguration::margin, QtConfiguration::margin, QtConfiguration::margin, QtConfiguration::margin);
#endif	// QT_5
	layout->setSizeConstraint (QLayout::SetMinimumSize);

	// La centre :
	_centerPanel	= new Qt3DDataPanel (this, "Centre :", true, "x : ", "y : ", "z : ", x, -DBL_MAX, DBL_MAX, y, -DBL_MAX, DBL_MAX, z, -DBL_MAX, DBL_MAX, false);
	connect (_centerPanel->getXTextField ( ), SIGNAL (returnPressed ( )), this, SLOT (transformationModifiedCallback ( )));
	connect (_centerPanel->getYTextField ( ), SIGNAL (returnPressed ( )), this, SLOT (transformationModifiedCallback ( )));
	connect (_centerPanel->getZTextField ( ), SIGNAL (returnPressed ( )), this, SLOT (transformationModifiedCallback ( )));
	layout->addWidget (_centerPanel);
	_centerPanel->setToolTip (QSTR ("Zone de saisie duu centre de la transformation intrinsèque."));

	// Les rotations :
	QtGroupBox*		phiGroupBox	= new QtGroupBox (this);
	QHBoxLayout*	qhLayout	= new QHBoxLayout (phiGroupBox);
	phiGroupBox->setLayout (qhLayout);
	phiGroupBox->setMargin (QtConfiguration::margin);
	phiGroupBox->setSpacing (QtConfiguration::spacing);
	layout->addWidget (phiGroupBox);
	QLabel*	angleLabel	= new QLabel (QSTR ("Angle ") + QtStringHelper::phiMin ( ), phiGroupBox);
	qhLayout->addWidget (angleLabel);
	_phiAngleTextField	= new QtTextField (QString::number (phi), phiGroupBox);
	_phiAngleTextField->setValidator (new QDoubleValidator (-180., 180., 0, _phiAngleTextField));
	qhLayout->addWidget (_phiAngleTextField);
	connect (_phiAngleTextField, SIGNAL (returnPressed ( )), this, SLOT (transformationModifiedCallback ( )));
	_phiAngleTextField->setFixedSize (_phiAngleTextField->sizeHint ( ));
	qhLayout->addStretch (200);
	_phiAngleTextField->setToolTip (QSTR ("Zone de saisie de l'angle de rotation ") + QtStringHelper::phiMin ( ) + QSTR (" autour de l'axe Oy dans le repère local."));
	angleLabel->setToolTip (QSTR ("Zone de saisie de l'angle de rotation ") + QtStringHelper::phiMin ( ) + QSTR (" autour de l'axe Oy dans le repère local."));
	
	QtGroupBox*		thetaGroupBox	= new QtGroupBox (this);
	qhLayout	= new QHBoxLayout (thetaGroupBox);
	thetaGroupBox->setLayout (qhLayout);
	thetaGroupBox->setMargin (QtConfiguration::margin);
	thetaGroupBox->setSpacing (QtConfiguration::spacing);
	layout->addWidget (thetaGroupBox);
	angleLabel	= new QLabel (QSTR ("Angle ") + QtStringHelper::thetaMaj ( ), thetaGroupBox);
	qhLayout->addWidget (angleLabel);
	_thetaAngleTextField	= new QtTextField (QString::number (theta), thetaGroupBox);
	_thetaAngleTextField->setValidator (new QDoubleValidator (-180., 180., 0, _thetaAngleTextField));
	qhLayout->addWidget (_thetaAngleTextField);
	connect (_thetaAngleTextField, SIGNAL (returnPressed ( )), this, SLOT (transformationModifiedCallback ( )));
	_thetaAngleTextField->setFixedSize (_thetaAngleTextField->sizeHint ( ));
	qhLayout->addStretch (200);
	_thetaAngleTextField->setToolTip (QSTR ("Zone de saisie de l'angle de rotation ") + QtStringHelper::thetaMaj ( ) + QSTR (" autour de l'axe Oz dans le repère local."));
	angleLabel->setToolTip (QSTR ("Zone de saisie de l'angle de rotation ") + QtStringHelper::thetaMaj ( ) + QSTR (" autour de l'axe Oz dans le repère local."));
	
	QtGroupBox*		omegaGroupBox	= new QtGroupBox (this);
	qhLayout	= new QHBoxLayout (omegaGroupBox);
	omegaGroupBox->setLayout (qhLayout);
	omegaGroupBox->setMargin (QtConfiguration::margin);
	omegaGroupBox->setSpacing (QtConfiguration::spacing);
	layout->addWidget (omegaGroupBox);
	angleLabel	= new QLabel (QSTR ("Angle ") + QtStringHelper::omegaMaj ( ), omegaGroupBox);
	qhLayout->addWidget (angleLabel);
	_omegaAngleTextField	= new QtTextField (QString::number (omega), omegaGroupBox);
	_omegaAngleTextField->setValidator (new QDoubleValidator (-180., 180., 0, _omegaAngleTextField));
	qhLayout->addWidget (_omegaAngleTextField);
	connect (_omegaAngleTextField, SIGNAL (returnPressed ( )), this, SLOT (transformationModifiedCallback ( )));
	_omegaAngleTextField->setFixedSize (_omegaAngleTextField->sizeHint ( ));
	qhLayout->addStretch (200);
	_omegaAngleTextField->setToolTip (QSTR ("Zone de saisie de l'angle de rotation ") + QtStringHelper::omegaMin ( ) + QSTR (" autour de l'axe Ox dans le repère local."));
	angleLabel->setToolTip (QSTR ("Zone de saisie de l'angle de rotation ") + QtStringHelper::omegaMin ( ) + QSTR (" autour de l'axe Ox dans le repère local."));

	// Afficher les trièdres ?
	_trihedronCheckBox	= new QCheckBox (QSTR ("Afficher les trièdres"), this);
	connect (_trihedronCheckBox, SIGNAL(clicked ( )), this, SLOT(displayTrihedronsCallback ( )));
	_trihedronCheckBox->setChecked (displayTrihedrons);
	_trihedronCheckBox->setToolTip (QSTR ("Cochée des trièdres montrant le repère avant et après transformation seront affichés dans la fenêtre graphhique."));
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


void QtVtkIntrinsicTransformationPanel::setCenter (double x, double y, double z)
{
	assert ((0 != _centerPanel) && "QtVtkIntrinsicTransformationPanel::setTranslation : null translation panel.");
	_centerPanel->setX (x);
	_centerPanel->setY (y);
	_centerPanel->setZ (z);
}	// QtVtkIntrinsicTransformationPanel::setCenter


void QtVtkIntrinsicTransformationPanel::getCenter (double& x, double& y, double& z) const
{
	assert ((0 != _centerPanel) && "QtVtkIntrinsicTransformationPanel::getCenter : null translation panel.");
	x	= _centerPanel->getX ( );
	y	= _centerPanel->getY ( );
	z	= _centerPanel->getZ ( );
}	// QtVtkIntrinsicTransformationPanel::getCenter


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


vtkTransform* QtVtkIntrinsicTransformationPanel::getTransformation ( ) const
{
	vtkTransform*	transformation	= vtkTransform::New ( );	// PreMultiply
	assert (0 != transformation);
	
	double	x	= 0., y	= 0., z	= 0.;
	getCenter (x, y, z);
	transformation->Translate (x, y, z);	// Modification extrinsèque pour centrage des rotations intrinsèques
	transformation->PostMultiply ( );
	// RotateY(phi) → RotateZ(theta) → RotateX(omega) : chaque rotation s’applique dans le repère local courant, 
	// méthode standard pour les angles de Tait-Bryan intrinsèques (ZYX) en robotique/aéronautique. Dixit mistral.ai.
	// Pour ce il faut inverser l'ordre des rotations par rapport à la même transformation mais à repère constant.
	transformation->RotateX (getOmegaAngle ( ));
	transformation->RotateZ (getThetaAngle ( ));
	transformation->RotateY (getPhiAngle ( ));

	return transformation;
}	// QtVtkIntrinsicTransformationPanel::getTransformation


UTF8String QtVtkIntrinsicTransformationPanel::getTransformationDescription ( ) const
{
	UTF8String	description (charset);

	double	x	= 0., y	= 0., z	= 0.;
	getCenter (x, y, z);
	description << "Transformation intrinsèque. Phi = " << getPhiAngle ( ) << " degrés, thêta = " << getThetaAngle ( ) << " degrés, oméga = " 
		        << getOmegaAngle ( ) << " degrés, centrée en (" << x << ", " << y << ", " << z << ")";
	
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
			_localTrihedron->SetTransform (getTransformation ( ));
		if (0 != _renderer->GetRenderWindow ( ))
			_renderer->GetRenderWindow ( )->Render ( );
	}	// if ((0 != _renderer) && (true == displayTrihedron ( )))

	emit (transformationChanged ( ));
}	// QtVtkIntrinsicTransformationPanel::transformationModifiedCallback


static vtkTrihedron* createTrihedron (bool global, vtkFloatingPointType bounds [6])
{
	// Les trièdres doivent dépasser légèrement de la zone d'intérêt :
	const double	xscale	= 1.25 * (bounds [1] - bounds [0]);
	const double	yscale	= 1.25 * (bounds [3] - bounds [2]);
	const double	zscale	= 1.25 * (bounds [5] - bounds [4]);
			
	vtkTrihedron*	trihedron	= vtkTrihedron::New ( );
	if (true == global)
	{
		trihedron->SetAxisLabels ("    x", "    y", "    z");
		trihedron->SetLabelsOffsets (-15, -15, -15);
	}
	else
	{
		trihedron->SetAxisLabels ("x'    ", "y'    ", "z'    ");
		trihedron->SetLabelsOffsets (-15, -15, -15);
	}	// else if (true == global)
		
	trihedron->SetLabel2D (true);
	// Rem CP : c'est sans effet si je passe des valeurs != à SetScale pour x, y et z. Pourquoi ???
	trihedron->GetXAxisArrowActor ( ).SetScale (xscale, xscale, xscale);
	trihedron->GetYAxisArrowActor ( ).SetScale (yscale, yscale, yscale);
	trihedron->GetZAxisArrowActor ( ).SetScale (zscale, zscale, zscale);
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
/* CP : En attente, pour fonctionnement avec un slider type QtExpRoomToMeshTransformationPanel::applyTransformations
 			static const double	trihedronBounds [6]	= { 0., 1., 0., 1., 0., 1. };			
			const double	zoomMin	= 0.8, zoomMax	= 1.5;
			const int		min	= 0, max	= 100, val	= (max - min) / 2;		// slider
			const double	scale	= QtVTKPointLocalizatorPanel::zoomValue (val, (double*)trihedronBounds, _bounds, min, max, zoomMin, zoomMax);
*/
			
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
//              QtVtkTransformationPanel::TransformationMemento
// ===========================================================================

QtVtkTransformationPanel::TransformationMemento::TransformationMemento ( )
	: isExtrinsic (true), xoy (0.), xoz (0.), yoz (0.), dx (0.), dy (0.), dz (0.), translationFirst (true), x (0.), y (0.), z (0.), phi (0.), theta (0.), omega (0.), displayTrihedron (true)
{
}	// TransformationMemento::TransformationMemento


QtVtkTransformationPanel::TransformationMemento::TransformationMemento (const QtVtkTransformationPanel::TransformationMemento& m)
	: isExtrinsic (m.isExtrinsic), xoy (m.xoy), xoz (m.xoz), yoz (m.yoz), dx (m.dx), dy (m.dy), dz (m.dz), translationFirst (m.translationFirst),
	  x (m.x), y (m.y), z (m.z), phi (m.phi), theta (m.theta), omega (m.omega), displayTrihedron (m.displayTrihedron)
{
}	// TransformationMemento::TransformationMemento


QtVtkTransformationPanel::TransformationMemento& QtVtkTransformationPanel::TransformationMemento::operator = (const QtVtkTransformationPanel::TransformationMemento& m)
{
	isExtrinsic			= m.isExtrinsic;
	xoy					= m.xoy;
	xoz					= m.xoz;
	yoz					= m.yoz;
	dx					= m.dx;
	dy					= m.dy;
	dz					= m.dz;
	translationFirst	= m.translationFirst;
	x					= m.x;
	y					= m.y;
	z					= m.z;
	phi					= m.phi;
	theta				= m.theta;
	omega				= m.omega;
	displayTrihedron	= m.displayTrihedron;

	return *this;
}	// TransformationMemento::operator =


// ===========================================================================
//                         QtVtkTransformationPanel
// ===========================================================================

double	QtVtkTransformationPanel::shaftRadius	= 0.01, QtVtkTransformationPanel::tipRadius	= 0.03, QtVtkTransformationPanel::tipLength	= 0.1;

QtVtkTransformationPanel::QtVtkTransformationPanel (
	QWidget* parent, const UTF8String& appTitle, vtkRenderer* renderer, vtkFloatingPointType bounds [6], const QtVtkTransformationPanel::TransformationMemento& memento)
	: QWidget (parent), _appTitle (appTitle), _extrinsicCheckBox (0), _contextualHelpLabel (0), _imageLabel (0),_extrinsicPanel (0), _intrinsicPanel (0)
{
	QtAutoWaitingCursor		waitingCursor (true);

	// Creation de l'ihm :
	QHBoxLayout*	mainLayout	= new QHBoxLayout (this);
	QVBoxLayout*	layout		= new QVBoxLayout ( );
	mainLayout->addLayout (layout);
	layout->setSpacing (QtConfiguration::spacing);
#ifdef QT_5
	layout->setMargin (QtConfiguration::margin);
#else	// QT_5
	layout->setContentsMargins (QtConfiguration::margin, QtConfiguration::margin, QtConfiguration::margin, QtConfiguration::margin);
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
	_intrinsicPanel	= new QtVtkIntrinsicTransformationPanel (this, _appTitle, renderer, bounds, memento.displayTrihedron);
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
		_intrinsicPanel->setCenter (memento.x, memento.y, memento.z);
		_intrinsicPanel->setPhiAngle (memento.phi);
		_intrinsicPanel->setThetaAngle (memento.theta);
		_intrinsicPanel->setOmegaAngle (memento.omega);
	}	// else if (true == memento.isExtrinsic)

	transformationTypeModifiedCallback ( );
	mainLayout->activate ( );
}	// QtVtkTransformationPanel::QtVtkTransformationPanel


QtVtkTransformationPanel::TransformationMemento QtVtkTransformationPanel::getMemento ( ) const
{
	assert (0 != _extrinsicPanel);
	assert (0 != _intrinsicPanel);

	QtVtkTransformationPanel::TransformationMemento	memento;
	memento.isExtrinsic	= isExtrinsic ( );
	
	memento.xoy					= _extrinsicPanel->getXOYAngle ( );
	memento.xoz					= _extrinsicPanel->getXOZAngle ( );
	memento.yoz					= _extrinsicPanel->getYOZAngle ();
	memento.translationFirst	= _extrinsicPanel->isTranslationFirst ( );
	_extrinsicPanel->getTranslation (memento.dx, memento.dy, memento.dz);
	_intrinsicPanel->getCenter (memento.x, memento.y, memento.z);
	memento.phi					= _intrinsicPanel->getPhiAngle ( );
	memento.theta				= _intrinsicPanel->getThetaAngle ( );
	memento.omega				= _intrinsicPanel->getOmegaAngle ( );

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
	const bool	extrinsic	= isExtrinsic ( );
	if (true == extrinsic)
	{
		_intrinsicPanel->setVisible (false);
		_extrinsicPanel->setVisible (true);
		QPixmap	pixmap (":/images/extrinsic.png");
		QSize	s	= pixmap.size ( );
		pixmap	= pixmap.scaledToHeight (0.5 * s.height ( ), Qt::SmoothTransformation);
		_imageLabel->setPixmap (pixmap);
	}	// if (true == extrinsic)
	else
	{
		_extrinsicPanel->setVisible (false);
		_intrinsicPanel->setVisible (true);
		QPixmap	pixmap (":/images/intrinsic.png");
		QSize	s	= pixmap.size ( );
		pixmap	= pixmap.scaledToHeight (0.5 * s.height ( ), Qt::SmoothTransformation);
		_imageLabel->setPixmap (pixmap);
	}	// else if (true == extrinsic)

	emit (transformationChanged ( ));
}	// QtVtkTransformationPanel::transformationTypeModifiedCallback


void QtVtkTransformationPanel::transformationModifiedCallback ( )
{
	emit (transformationChanged ( ));
}	// QtVtkTransformationPanel::transformationModifiedCallback


