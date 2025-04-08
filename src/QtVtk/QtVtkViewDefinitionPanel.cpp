#include "QtVtk/QtVtkViewDefinitionPanel.h"

#include <QtUtil/QtConfiguration.h>
#include <QtUtil/QtGroupBox.h>
#include <QtUtil/QtAutoWaitingCursor.h>
#include <QtUtil/QtUnicodeHelper.h>
#include <TkUtil/InternalError.h>

#include <assert.h>
#include <float.h>		// FLT_MAX
#include <values.h>		// FLT_MAX sous Linux ...

#include <QLayout>
#include <QGridLayout>
#include <QLabel>
#include <QValidator>
#include <QMessageBox> 

#include <vtkCamera.h>
#include <vtkRenderWindow.h>


USING_STD
USING_UTIL

static const	Charset	charset ("àéèùô");
USE_ENCODING_AUTODETECTION


// ===========================================================================
//                         QtVtkViewDefinitionPanel
// ===========================================================================



QtVtkViewDefinitionPanel::QtVtkViewDefinitionPanel (QWidget* parent, const string& appTitle, const UTF8String& name, const UTF8String& comment,
                                                    double position [3], double focal [3], double viewUp [3],double roll, vtkRenderer* renderer)
	: QWidget (parent), _appTitle (appTitle), _positionPanel (0), _focalPanel (0), _rollTextField (0), _renderer (renderer), _modified (false), _updated (true)
{
	for (int i = 0; i < 3; i++)
	{
		_initialPosition [i]	= _position [i]	= position [i];
		_initialFocal [i]		= _focal [i]	= focal [i];
		_initialViewUp [i]		= _viewUp [i]	= viewUp [i];
	}	// for (int i = 0; i < 3; i++)
	_initialRoll	= _roll	= roll;

	// Creation de l'ihm :
	QVBoxLayout*	layout	= new QVBoxLayout (this);
	layout->setSpacing (QtConfiguration::spacing);
	layout->setMargin (QtConfiguration::margin);
	layout->setSizeConstraint (QLayout::SetMinimumSize);

	// Nom / Commentaire :
	QtGroupBox*		nameGroupBox	= new QtGroupBox ("", this);
	QGridLayout*	nameLayout		= new QGridLayout (nameGroupBox);
	nameGroupBox->setLayout (nameLayout);
	nameGroupBox->setSpacing (QtConfiguration::spacing);
	nameGroupBox->setMargin (QtConfiguration::margin);
	nameLayout->setSizeConstraint (QLayout::SetMinimumSize);
	QLabel*		label	= new QLabel ("Nom :", nameGroupBox);
	nameLayout->addWidget (label, 0, 0);
	_nameTextField		= new QtTextField (nameGroupBox);
	_nameTextField->setText (UTF8TOQSTRING (name));	
	connect (_nameTextField, SIGNAL (returnPressed ( )), this, SLOT (viewFieldValidatedCallback ( )));
	connect (_nameTextField, SIGNAL (textChanged (const QString&)), this, SLOT (viewFieldModifiedCallback (const QString&)));
	nameLayout->addWidget (_nameTextField, 0, 1);
	label	= new QLabel ("Commentaire :", nameGroupBox);
	nameLayout->addWidget (label, 1, 0);
	_commentTextField		= new QtTextField (nameGroupBox);
	_commentTextField->setText (UTF8TOQSTRING (comment));	
	connect (_commentTextField, SIGNAL (returnPressed ( )), this, SLOT (viewFieldValidatedCallback ( )));
	connect (_commentTextField, SIGNAL (textChanged (const QString&)), this, SLOT (viewFieldModifiedCallback (const QString&)));
	nameLayout->addWidget (_commentTextField, 1, 1);
	nameGroupBox->adjustSize ( );
	layout->addWidget (nameGroupBox);
	
	// Position :
	_positionPanel	= new Qt3DDataPanel (this, "Position", true, "x : ", "y : ", "z : ", position [0], -DBL_MAX, DBL_MAX, position [1], -DBL_MAX, DBL_MAX, position [2], -DBL_MAX, DBL_MAX, false);
	connect (_positionPanel->getXTextField ( ), SIGNAL (returnPressed ( )), this, SLOT (viewFieldValidatedCallback ( )));
	connect (_positionPanel->getYTextField ( ), SIGNAL (returnPressed ( )), this, SLOT (viewFieldValidatedCallback ( )));
	connect (_positionPanel->getZTextField ( ), SIGNAL (returnPressed ( )), this, SLOT (viewFieldValidatedCallback ( )));
	connect (_positionPanel->getXTextField ( ), SIGNAL (textChanged (const QString&)), this, SLOT (viewFieldModifiedCallback (const QString&)));
	connect (_positionPanel->getYTextField ( ),  SIGNAL (textChanged (const QString&)), this, SLOT (viewFieldModifiedCallback (const QString&)));
	connect (_positionPanel->getZTextField ( ), SIGNAL (textChanged (const QString&)), this, SLOT (viewFieldModifiedCallback (const QString&)));
	layout->addWidget (_positionPanel);

	// Focale :
	_focalPanel	= new Qt3DDataPanel (this, "Focale", true, "x : ", "y : ", "z : ", focal [0], -DBL_MAX, DBL_MAX, focal [1], -DBL_MAX, DBL_MAX, focal [2], -DBL_MAX, DBL_MAX, false);
	connect (_focalPanel->getXTextField ( ), SIGNAL (returnPressed ( )), this, SLOT (viewFieldValidatedCallback ( )));
	connect (_focalPanel->getYTextField ( ), SIGNAL (returnPressed ( )), this, SLOT (viewFieldValidatedCallback ( )));
	connect (_focalPanel->getZTextField ( ), SIGNAL (returnPressed ( )), this, SLOT (viewFieldValidatedCallback ( )));
	connect (_focalPanel->getXTextField ( ), SIGNAL (textChanged (const QString&)), this, SLOT (viewFieldModifiedCallback (const QString&)));
	connect (_focalPanel->getYTextField ( ), SIGNAL (textChanged (const QString&)), this, SLOT (viewFieldModifiedCallback (const QString&)));
	connect (_focalPanel->getZTextField ( ), SIGNAL (textChanged (const QString&)), this, SLOT (viewFieldModifiedCallback (const QString&)));
	layout->addWidget (_focalPanel);
	
	// Haut :
	_viewUpPanel	= new Qt3DDataPanel (this, "Direction vers le haut", true, "dx : ", "dy : ", "dz : ", viewUp [0], -DBL_MAX, DBL_MAX, viewUp [1], -DBL_MAX, DBL_MAX, viewUp [2], -DBL_MAX, DBL_MAX, false);
	connect (_viewUpPanel->getXTextField ( ), SIGNAL (returnPressed ( )), this, SLOT (viewFieldValidatedCallback ( )));
	connect (_viewUpPanel->getYTextField ( ), SIGNAL (returnPressed ( )), this, SLOT (viewFieldValidatedCallback ( )));
	connect (_viewUpPanel->getZTextField ( ), SIGNAL (returnPressed ( )), this, SLOT (viewFieldValidatedCallback ( )));
	connect (_viewUpPanel->getXTextField ( ), SIGNAL (textChanged (const QString&)), this, SLOT (viewFieldModifiedCallback (const QString&)));
	connect (_viewUpPanel->getYTextField ( ), SIGNAL (textChanged (const QString&)), this, SLOT (viewFieldModifiedCallback (const QString&)));
	connect (_viewUpPanel->getZTextField ( ), SIGNAL (textChanged (const QString&)), this, SLOT (viewFieldModifiedCallback (const QString&)));
	layout->addWidget (_viewUpPanel);

	// Roulis :
	QtGroupBox*		rollGroupBox	= new QtGroupBox ("Roulis", this);
	QHBoxLayout*	rollLayout		= new QHBoxLayout (rollGroupBox);
	rollGroupBox->setLayout (rollLayout);
	rollGroupBox->setSpacing (QtConfiguration::spacing);
	rollGroupBox->setMargin (QtConfiguration::margin);
	rollLayout->setSizeConstraint (QLayout::SetMinimumSize);
	label	= new QLabel ("Angle :", rollGroupBox);
	label->setFixedSize (label->sizeHint ( ));
	rollLayout->addWidget (label);
	_rollTextField		= new QtTextField (rollGroupBox);
	_rollTextField->setVisibleColumns (4);
	_rollTextField->setMaxLength (4);
	_rollTextField->setFixedSize (_rollTextField->sizeHint ( ));
	_rollTextField->setMaximumSize (_rollTextField->sizeHint ( ));
	_rollTextField->setText (QString::number (roll));	
	_rollTextField->setValidator (new QDoubleValidator (-360., 360., 4, _rollTextField));
	connect (_rollTextField, SIGNAL (returnPressed ( )), this, SLOT (viewFieldValidatedCallback ( )));
	connect (_rollTextField, SIGNAL (textChanged (const QString&)), this, SLOT (viewFieldModifiedCallback (const QString&)));
	rollLayout->addWidget (_rollTextField);
	label	= new QLabel (QSTR ("(Domaine : -360 à 360 degrés)"), rollGroupBox);
	label->setMinimumSize (label->sizeHint ( ));
	rollLayout->addWidget (label);
	rollGroupBox->adjustSize ( );
	layout->addWidget (rollGroupBox);

	layout->activate ( );
	setMinimumSize (layout->sizeHint ( ));
}	// QtVtkViewDefinitionPanel::QtVtkViewDefinitionPanel


QtVtkViewDefinitionPanel::QtVtkViewDefinitionPanel (const QtVtkViewDefinitionPanel&)
{
	assert (0 && "QtVtkViewDefinitionPanel copy constructor is forbidden.");
}	// QtVtkViewDefinitionPanel::QtVtkViewDefinitionPanel (const QtVtkViewDefinitionPanel&)


QtVtkViewDefinitionPanel& QtVtkViewDefinitionPanel::operator = (const QtVtkViewDefinitionPanel&)
{
	assert (0 && "QtVtkViewDefinitionPanel assignment operator is forbidden.");
	return *this;
}	// QtVtkViewDefinitionPanel::operator =


QtVtkViewDefinitionPanel::~QtVtkViewDefinitionPanel ( )
{
}	// QtVtkViewDefinitionPanel::~QtVtkViewDefinitionPanel


void QtVtkViewDefinitionPanel::setPosition (double coords [3])
{
	assert ((0 != _positionPanel) && "QtVtkViewDefinitionPanel::setPosition : null position panel.");
	_positionPanel->setX (coords [0]);
	_positionPanel->setY (coords [1]);
	_positionPanel->setZ (coords [2]);
	_updated	= false;
}	// QtVtkViewDefinitionPanel::setPosition


void QtVtkViewDefinitionPanel::setFocalPoint (double coords [3])
{
	assert ((0 != _focalPanel) && "QtVtkViewDefinitionPanel::setFocalPoint : null focal panel.");
	_focalPanel->setX (coords [0]);
	_focalPanel->setY (coords [1]);
	_focalPanel->setZ (coords [2]);
	_updated	= false;
}	// QtVtkViewDefinitionPanel::setFocalPoint


void QtVtkViewDefinitionPanel::setViewUp (double coords [3])
{
	assert ((0 != _focalPanel) && "QtVtkViewDefinitionPanel::setViewUp : null view up panel.");
	_viewUpPanel->setX (coords [0]);
	_viewUpPanel->setY (coords [1]);
	_viewUpPanel->setZ (coords [2]);
	_updated	= false;
}	// QtVtkViewDefinitionPanel::setViewUp


void QtVtkViewDefinitionPanel::setRoll (double roll)
{
	assert ((0 != _rollTextField) && "QtVtkViewDefinitionPanel::setRoll : null roll text field.");
	_rollTextField->setText (QString::number (roll));
	_updated	= false;
}	// QtVtkViewDefinitionPanel::setRoll


void QtVtkViewDefinitionPanel::updateData ( )
{
	bool	modified	= false;
	int		i			= 0;

	double	param3D [3];
	getPosition (param3D);
	for (i = 0; i < 3; i++)
	{
		if (param3D [i] != _position [i])
		{
			_position [i]	= param3D [i];
			modified	= true;
		}	// if (param3D [i] != _position [i])
	}	// for (i = 0; i < 3; i++)

	getFocalPoint (param3D);
	for (i = 0; i < 3; i++)
	{
		if (param3D [i] != _focal [i])
		{
			_focal [i]	= param3D [i];
			modified	= true;
		}	// if (param3D [i] != _focal [i])
	}	// for (i = 0; i < 3; i++)

	getViewUp (param3D);
	for (i = 0; i < 3; i++)
	{
		if (param3D [i] != _viewUp [i])
		{
			_viewUp [i]	= param3D [i];
			modified	= true;
		}	// if (param3D [i] != _focal [i])
	}	// for (i = 0; i < 3; i++)

	if (_roll != getRoll ( ))
	{
		_roll		= getRoll ( );
		modified	= true;
	}	// if (_roll != getRoll ( ))	

	if (false == modified)
		return;

	setModified (true);
	setUpdated (false);
}	// QtVtkViewDefinitionPanel::updateData


UTF8String QtVtkViewDefinitionPanel::getName ( ) const
{
	assert ((0 != _nameTextField) && "QtVtkViewDefinitionPanel::getName : null name textfield.");
	return QtUnicodeHelper::qstringToUTF8String (_nameTextField->text ( ));
}	// QtVtkViewDefinitionPanel::getName


UTF8String QtVtkViewDefinitionPanel::getComment ( ) const
{
	assert ((0 != _commentTextField) && "QtVtkViewDefinitionPanel::getComment : null comment textfield.");
	return QtUnicodeHelper::qstringToUTF8String (_commentTextField->text ( ));
}	// QtVtkViewDefinitionPanel::getComment


void QtVtkViewDefinitionPanel::getPosition (double coords [3]) const
{
	assert ((0 != _positionPanel) && "QtVtkViewDefinitionPanel::getPosition : null position panel.");
	coords [0]	= _positionPanel->getX ( );
	coords [1]	= _positionPanel->getY ( );
	coords [2]	= _positionPanel->getZ ( );
}	// QtVtkViewDefinitionPanel::getPosition


void QtVtkViewDefinitionPanel::getFocalPoint (double coords [3]) const
{
	assert ((0 != _focalPanel) && "QtVtkViewDefinitionPanel::getFocalPoint : null focal panel.");
	coords [0]	= _focalPanel->getX ( );
	coords [1]	= _focalPanel->getY ( );
	coords [2]	= _focalPanel->getZ ( );
}	// QtVtkViewDefinitionPanel::getFocalPoint


void QtVtkViewDefinitionPanel::getViewUp (double direction [3]) const
{
	assert ((0 != _viewUpPanel) && "QtVtkViewDefinitionPanel::getViewUp : null view up panel.");
	direction [0]	= _viewUpPanel->getX ( );
	direction [1]	= _viewUpPanel->getY ( );
	direction [2]	= _viewUpPanel->getZ ( );
}	// QtVtkViewDefinitionPanel::getViewUp


double QtVtkViewDefinitionPanel::getRoll ( ) const
{
	assert ((0 != _rollTextField) && "QtVtkViewDefinitionPanel::getRoll : null roll text field.");
	bool	ok	= true;
	return _rollTextField->text ( ).toDouble (&ok);
}	// QtVtkViewDefinitionPanel::getRoll


void QtVtkViewDefinitionPanel::apply ( )
{
	if (true == isUpdated ( ))
		return;
	QtAutoWaitingCursor	waitingCursor (true);
	updateData ( );

	vtkCamera*		camera		= 0 == _renderer ? 0 : _renderer->GetActiveCamera ( );
	if ((0 == _renderer) || (0 == camera))
		return;

	double	position [3]	= { 0., 0., 0. };
	double	focal [3]		= { 0., 0., 0. };
	double	viewUp [3]		= { 0., 0., 0. };
	double	roll	= getRoll ( );
	getPosition (position);
	getFocalPoint (focal);
	getViewUp (viewUp);
	camera->SetPosition (position);
	camera->SetFocalPoint (focal);
	camera->SetViewUp (viewUp);
	camera->SetRoll (roll);
	// ResetCameraClippingRange : en son absence il y a de bonnes chances
	// que certains objets ne soient plus affiches bien qu'ils devraient etre
	// visibles.
	_renderer->ResetCameraClippingRange ( );
	if (0 != _renderer->GetRenderWindow ( ))
		_renderer->GetRenderWindow ( )->Render ( );

	setUpdated (true);
}	// QtVtkViewDefinitionPanel::apply


void QtVtkViewDefinitionPanel::reset ( )
{
	if (false == isModified ( ))
		return;

	setPosition (_initialPosition);
	setFocalPoint (_initialFocal);
	setViewUp (_initialViewUp);
	setRoll (_initialRoll);
	apply ( );
	setModified (false);
}	// QtVtkViewDefinitionPanel::reset


void QtVtkViewDefinitionPanel::viewFieldModifiedCallback (const QString&)
{
//	setModified (true);
	setUpdated (false);
}	// QtVtkViewDefinitionPanel::viewFieldModifiedCallback


void QtVtkViewDefinitionPanel::viewFieldValidatedCallback ( )
{
	updateData ( );

	if (false == isUpdated ( ))
		apply ( );
}	// QtVtkViewDefinitionPanel::viewFieldModifiedCallback


