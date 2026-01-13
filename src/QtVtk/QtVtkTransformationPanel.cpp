/**
 * \file		QtVtkTransformationPanel.cpp
 * \author		Charles PIGNEROL, CEA/DAM/DCLC
 * \date		06/01/2026
 */
#include "QtVtk/QtVtkTransformationPanel.h"

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


USING_STD
USING_UTIL


static const	Charset	charset ("àéèùô");
USE_ENCODING_AUTODETECTION



// ===========================================================================
//                         QtVtkTransformationPanel
// ===========================================================================


QtVtkTransformationPanel::QtVtkTransformationPanel (QWidget* parent, const UTF8String& appTitle, bool cartesian, double xoy, double xoz, double yoz, double dx, double dy, double dz)
	: QWidget (parent), _appTitle (appTitle), _sphericalCheckBox (0), _contextualHelpLabel (0), _xOyAngleLabel (0), _xOzAngleLabel (0), _yOzAngleLabel (0), 
	  _xOyAngleTextField (0), _xOzAngleTextField (0), _yOzAngleTextField (0), _translationPanel (0)
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

	// Système de coordonnées utilisé :
	_sphericalCheckBox	= new QCheckBox (QSTR ("Repère sphérique"), this);
	connect (_sphericalCheckBox, SIGNAL(clicked ( )), this, SLOT(transformationModifiedCallback ( )));
	QString	tip (QSTR ("Coché, la transformation est effectuée en repère sphérique via les angles "));
	tip += QtStringHelper::phiMin ( ) + ", " + QtStringHelper::thetaMaj ( )+ " et " + QtStringHelper::omegaMaj ( );
	_sphericalCheckBox->setChecked (!cartesian);
	_sphericalCheckBox->setToolTip (tip);
	layout->addWidget (_sphericalCheckBox);
	
	// et l'aide associée :
	_contextualHelpLabel	= new QLabel ("Aide contextuelle", this);
	layout->addWidget (_contextualHelpLabel);

	// Les rotations :
	QtGroupBox*		xOyGroupBox	= new QtGroupBox (this);
	QHBoxLayout*	qhLayout	= new QHBoxLayout (xOyGroupBox);
	xOyGroupBox->setLayout (qhLayout);
	xOyGroupBox->setMargin (QtConfiguration::margin);
	xOyGroupBox->setSpacing (QtConfiguration::spacing);
	layout->addWidget (xOyGroupBox);
	_xOyAngleLabel	= new QLabel (QSTR ("Angle de rotation dans le plan xOy:"), xOyGroupBox);
	qhLayout->addWidget (_xOyAngleLabel);
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
	_xOzAngleLabel	= new QLabel (QSTR ("Angle de rotation dans le plan xOz:"), xOzGroupBox);
	qhLayout->addWidget (_xOzAngleLabel);
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
	_yOzAngleLabel	= new QLabel (QSTR ("Angle de rotation dans le plan yOz:"), yOzGroupBox);
	qhLayout->addWidget (_yOzAngleLabel);
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
	_translationPanel->setToolTip (QSTR ("Zone de saisie de la translation post-rotations."));

	layout->activate ( );
	setMinimumSize (layout->sizeHint ( ));

	// On met les bons labels et tooltips :
	transformationModifiedCallback ( );
}	// QtVtkTransformationPanel::QtVtkTransformationPanel


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


void QtVtkTransformationPanel::setCartesianCoordinateSystem (bool cartesian)
{
	assert ((0 != _sphericalCheckBox) && "QtVtkTransformationPanel::setCartesianCoordinateSystem : null coordinate system checkbox.");
	_sphericalCheckBox->setChecked (!cartesian);
	transformationModifiedCallback ( ) ;
}	// QtVtkTransformationPanel::setCartesianCoordinateSystem


bool QtVtkTransformationPanel::isCartesianCoordinateSystem ( ) const
{
	assert ((0 != _sphericalCheckBox) && "QtVtkTransformationPanel::isCartesianCoordinateSystem : null coordinate system checkbox.");

	return !_sphericalCheckBox->isChecked ( );
}	// QtVtkTransformationPanel::isCartesianCoordinateSystem


void QtVtkTransformationPanel::setXOYAngle (double angle)
{
	assert ((0 != _xOyAngleTextField) && "QtVtkTransformationPanel::setXOYAngle : null angle text field.");
	_xOyAngleTextField->setText (QString::number (angle));
}	// QtVtkTransformationPanel::setXOYAngle


double QtVtkTransformationPanel::getXOYAngle ( ) const
{
	assert ((0 != _xOyAngleTextField) && "QtVtkTransformationPanel::getXOYAngle : null angle text field.");
	bool	ok		= true;
	double	value	= _xOyAngleTextField->text ( ).toDouble (&ok);

	if (false == ok)
	{
		UTF8String	errorMsg (charset);
		errorMsg << _appTitle << " : valeur invalide de l'angle xOy (" << _xOyAngleTextField->text ( ).toStdString ( ) << ")";
		throw Exception (errorMsg);
	}	// if (false == ok)

	return value;
}	// QtVtkTransformationPanel::getXOYAngle


void QtVtkTransformationPanel::setXOZAngle (double angle)
{
	assert ((0 != _xOzAngleTextField) && "QtVtkTransformationPanel::setXOZAngle : null angle text field.");
	_xOzAngleTextField->setText (QString::number (angle));
}	// QtVtkTransformationPanel::setXOZAngle


double QtVtkTransformationPanel::getXOZAngle ( ) const
{
	assert ((0 != _xOzAngleTextField) && "QtVtkTransformationPanel::getXOZAngle : null sectionRaangledius text field.");
	bool	ok		= true;
	double	value	= _xOzAngleTextField->text ( ).toDouble (&ok);

	if (false == ok)
	{
		UTF8String	errorMsg (charset);
		errorMsg << _appTitle << " : valeur invalide de l'angle xOz (" << _xOzAngleTextField->text ( ).toStdString ( ) << ")";
		throw Exception (errorMsg);
	}	// if (false == ok)

	return value;
}	// QtVtkTransformationPanel::getXOZAngle


void QtVtkTransformationPanel::setYOZAngle (double angle)
{
	assert ((0 != _yOzAngleTextField) && "QtVtkTransformationPanel::setYOZAngle : null angle text field.");
	_yOzAngleTextField->setText (QString::number (angle));
}	// QtVtkTransformationPanel::setYOZAngle


double QtVtkTransformationPanel::getYOZAngle ( ) const
{
	assert ((0 != _yOzAngleTextField) && "QtVtkTransformationPanel::getYOZAngle : null angle text field.");
	bool	ok		= true;
	double	value	= _yOzAngleTextField->text ( ).toDouble (&ok);

	if (false == ok)
	{
		UTF8String	errorMsg (charset);
		errorMsg << _appTitle << " : valeur invalide de l'angle yOz (" << _yOzAngleTextField->text ( ).toStdString ( ) << ")";
		throw Exception (errorMsg);
	}	// if (false == ok)

	return value;
}	// QtVtkTransformationPanel::getYOZAngle


void QtVtkTransformationPanel::setPhiAngle (double angle)
{
	setXOYAngle (angle);	// Le nom ne correspond pas, c'est la position dans le panneau qui compte ici.
}	// QtVtkTransformationPanel::setPhiAngle


double QtVtkTransformationPanel::getPhiAngle ( ) const
{
	return getXOYAngle ( );
}	// QtVtkTransformationPanel::getPhiAngle


void QtVtkTransformationPanel::setThetaAngle (double angle)
{
	setXOZAngle (angle);	// Le nom ne correspond pas, c'est la position dans le panneau qui compte ici.
}	// QtVtkTransformationPanel::setThetaAngle


double QtVtkTransformationPanel::getThetaAngle ( ) const
{
	return getXOZAngle ( );
}	// QtVtkTransformationPanel::getThetaAngle


void QtVtkTransformationPanel::setOmegaAngle (double angle)
{
	setYOZAngle (angle);	// Le nom ne correspond pas, c'est la position dans le panneau qui compte ici.
}	// QtVtkTransformationPanel::setOmegaAngle


double QtVtkTransformationPanel::getOmegaAngle ( ) const
{
	return getYOZAngle ( );
}	// QtVtkTransformationPanel::getOmegaAngle


void QtVtkTransformationPanel::setTranslation (double dx, double dy, double dz)
{
	assert ((0 != _translationPanel) && "QtVtkTransformationPanel::setTranslation : null translation panel.");
	_translationPanel->setX (dx);
	_translationPanel->setY (dy);
	_translationPanel->setZ (dz);
}	// QtVtkTransformationPanel::setTranslation


void QtVtkTransformationPanel::getTranslation (double& dx, double& dy, double& dz) const
{
	assert ((0 != _translationPanel) && "QtVtkTransformationPanel::getTranslation : null translation panel.");
	dx	= _translationPanel->getX ( );
	dy	= _translationPanel->getY ( );
	dz	= _translationPanel->getZ ( );
}	// QtVtkTransformationPanel::getTranslation


vtkTransform* QtVtkTransformationPanel::getTransformation ( ) const
{
	vtkTransform*	transformation	= vtkTransform::New ( );	// PreMultiply
	assert (0 != transformation);
	
	const bool	spherical	= !isCartesianCoordinateSystem ( );
	double	dx	= 0., dy	= 0., dz	= 0.;
	getTranslation (dx, dy, dz);
	if (true == spherical)
	{
		transformation->Translate (dx, dy, dz);
		// RotateY(phi) → RotateZ(theta) → RotateX(omega) : chaque rotation s’applique dans le repère local courant, 
		// méthode standard pour les angles de Tait-Bryan intrinsèques (ZYX) en robotique/aéronautique. Dixit mistral.ai.
		// Pour ce il faut inverser l'ordre des rotations par rapport à la même transformation mais à repère constant.
		// Mais comme on est (par défaut) en PreMultiply il faut de nouveau inverser l'ordre des rotations !
		transformation->RotateY (getPhiAngle ( ));
		transformation->RotateZ (getThetaAngle ( ));
		transformation->RotateX (getOmegaAngle ( ));
/*
double	ptIn [3]	= { 1., 1., 1. };
double	ptOut [3]	= { 0., 0., 0. };
transformation->TransformPoint (ptIn, ptOut);
cout << "IN (" << ptIn [0] << ", " << ptIn [1] << ", " << ptIn [2] << ") OUT (" << ptOut [0] << ", " << ptOut [1] << ", " << ptOut [2] << ")" << endl;
*/
	}	// if (true == spherical)
	else
	{
		transformation->Translate (dx, dy, dz);
		transformation->RotateY (getXOZAngle ( ));
		transformation->RotateX (getYOZAngle ( ));
		transformation->RotateZ (getXOYAngle ( ));
	}	// else if (true == spherical)

	return transformation;
}	// QtVtkTransformationPanel::getTransformation


UTF8String QtVtkTransformationPanel::getTransformationDescription ( ) const
{
	UTF8String	description (charset);
	
	const bool	spherical	= !isCartesianCoordinateSystem ( );
	double	dx	= 0., dy	= 0., dz	= 0.;
	getTranslation (dx, dy, dz);
	if (true == spherical)
	{
		description << "Transformation en repère sphérique. Phi = " << getPhiAngle ( ) << " degrés, thêta = " << getThetaAngle ( ) << " degrés, oméga = " 
		            << getOmegaAngle ( ) << " degrés, translation de (" << dx << ", " << dy << ", " << dz << ")";
	}	// if (true == spherical)
	else
	{
		description << "Transformation en repère cartésien. Rotations autour de Oy = " << getXOZAngle ( ) << " degrés, autour de Ox = " << getYOZAngle ( ) 
		            << " degrés, autour de Oz = " << getXOYAngle ( ) << " degrés, translation de (" << dx << ", " << dy << ", " << dz << ")";
	}	// else if (true == spherical)
	
	
	return description;
}	// QtVtkTransformationPanel::getTransformationDescription


void QtVtkTransformationPanel::transformationModifiedCallback ( )
{
	assert (0 != _xOyAngleLabel && "QtVtkTransformationPanel::transformationModifiedCallback : null xOy label");
	assert (0 != _xOzAngleLabel && "QtVtkTransformationPanel::transformationModifiedCallback : null xOz label");
	assert (0 != _yOzAngleLabel && "QtVtkTransformationPanel::transformationModifiedCallback : null yOz label");
	assert (0 != _xOyAngleTextField && "QtVtkTransformationPanel::transformationModifiedCallback : null xOy textfield");
	assert (0 != _xOzAngleTextField && "QtVtkTransformationPanel::transformationModifiedCallback : null xOz textfield");
	assert (0 != _yOzAngleTextField && "QtVtkTransformationPanel::transformationModifiedCallback : null yOz textfield");
	try
	{
		QtAutoWaitingCursor		waitingCursor (true);

		const bool	spherical	= !isCartesianCoordinateSystem ( );
		if (true == spherical)
		{
			_contextualHelpLabel->setText (QSTR ("Rotation de ") + QtStringHelper::phiMin ( ) + " puis rotation de " + QtStringHelper::thetaMaj ( ) + " puis rotation de " + QtStringHelper::omegaMaj ( ) + " puis translation.");
			_xOyAngleLabel->setText (QtStringHelper::phiMin ( ) + " (angle dans le plan xOz) :");
			_xOyAngleLabel->setToolTip (QSTR ("Zone de saisie de l'angle de précession de la transformation (dans le plan xOz)."));
			_xOyAngleTextField->setToolTip (QSTR ("Zone de saisie de l'angle de précession de la transformation (dans le plan xOz)."));
			_xOzAngleLabel->setText (QtStringHelper::thetaMaj ( ) + " (angle avec l'axe des abscisses) :");
			_xOzAngleLabel->setToolTip (QSTR ("Zone de saisie de l'angle de nutation du tore (par rapport à l'axe des abscisses)."));
			_xOzAngleTextField->setToolTip (QSTR ("Zone de saisie de l'angle de nutation du tore (par rapport à l'axe des abscisses)."));
			_yOzAngleLabel->setText (QtStringHelper::omegaMaj ( ) + " (angle de rotation autour de l'axe des abscisses) :");
			_yOzAngleLabel->setToolTip (QSTR ("Zone de saisie de l'angle autour de l'axe des abscisses."));
			_yOzAngleTextField->setToolTip (QSTR ("Zone de saisie de l'angle autour de l'axe des abscisses."));
		}
		else
		{
			_contextualHelpLabel->setText (QSTR ("Rotation autour de Oz, puis autour de Ox, puis autour de Oy, puis translation."));
			_xOyAngleLabel->setText (QSTR ("Angle de rotation dans le plan xOy :"));
			_xOyAngleLabel->setToolTip (QSTR ("Zone de saisie de l'angle de rotation de la transformation dans le plan xOy."));
			_xOyAngleTextField->setToolTip (QSTR ("Zone de saisie de l'angle de rotation de la transformation dans le plan xOy."));
			_xOzAngleLabel->setText (QSTR ("Angle de rotation dans le plan xOz :"));
			_xOzAngleLabel->setToolTip (QSTR ("Zone de saisie de l'angle de rotation de la transformation dans le plan xOy."));
			_xOzAngleTextField->setToolTip (QSTR ("Zone de saisie de l'angle de rotation de la transformation dans le plan xOz."));
			_yOzAngleLabel->setText (QSTR ("Angle de rotation dans le plan yOz :"));
			_yOzAngleLabel->setToolTip (QSTR ("Zone de saisie de l'angle de rotation de la transformation dans le plan yOz."));
			_yOzAngleTextField->setToolTip (QSTR ("Zone de saisie de l'angle de rotation de la transformation dans le plan yOz."));
		}

		getXOYAngle ( );
		getXOZAngle ( );
		getYOZAngle ( );
		double	dx	= 0., dy	= 0., dz	= 0.;
		getTranslation (dx, dy, dz);
		
		emit (transformationChanged ( ));
	}
	catch (const Exception& exc)
	{
		UTF8String	errorMsg (charset);
		errorMsg << "Valeur invalide : " << exc.getFullMessage ( );
		QMessageBox::critical (this, UTF8TOQSTRING (_appTitle), UTF8TOQSTRING (errorMsg), QMessageBox::Ok, QMessageBox::NoButton);
	}
	catch (const IN_STD exception& stdExc)	// IN_STD : pour OSF
	{
		UTF8String	errorMsg (charset);
		errorMsg << "Valeur invalide : " << stdExc.what ( );
		QMessageBox::critical (this, UTF8TOQSTRING (_appTitle),  UTF8TOQSTRING (errorMsg), QMessageBox::Ok, QMessageBox::NoButton);
	}
	catch (...)
	{
		QMessageBox::critical (this, UTF8TOQSTRING (_appTitle), "Saisie invalide", QMessageBox::Ok, QMessageBox::NoButton);
	}
}	// QtVtkTransformationPanel::transformationModifiedCallback


