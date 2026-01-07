#include "QtUtil/Qt3DDataPanel.h"

#include "QtUtil/QtConfiguration.h"
#include <TkUtil/Exception.h>

#include <assert.h>

#include <QValidator>
#include <QLayout>
#include <QLabel>


USING_STD
USING_UTIL

int Qt3DDataPanel::numberOfCharacters	= 18;


Qt3DDataPanel::Qt3DDataPanel (QWidget* parent, const string& title,
                              bool editable, const string& xTitle,
                              const string& yTitle, const string& zTitle,
                              double x, double xMin, double xMax,
                              double y, double yMin, double yMax,
                              double z, double zMin, double zMax,
                              bool verticalLayout)
	: QtGroupBox (title.c_str ( ), parent), 
	  _initialX (x), _initialY (y), _initialZ (z), 
	  _xTextField (0), _yTextField (0), _zTextField (0), _title (title)
{
	QVBoxLayout*	vlayout	= new QVBoxLayout (this);
	vlayout->setContentsMargins (0, 0, 0, 0);
	vlayout->setSpacing (0);
	setLayout (vlayout);

	QHBoxLayout*	layout	= new QHBoxLayout ( );
	vlayout->addLayout (layout);
	setMargin (QtConfiguration::margin);
	setSpacing (QtConfiguration::spacing);
	QLabel*			label	= new QLabel (xTitle.c_str ( ), this);
	label->setMinimumSize (label->sizeHint ( ));
	layout->addWidget (label);
	_xTextField				= new QtDoubleTextField (this);
	_xTextField->setVisibleColumns (numberOfCharacters);
//	_xTextField->setMaxLength (numberOfCharacters);
	_xTextField->setMinimumSize (_xTextField->sizeHint ( ));
	layout->addWidget (_xTextField);
	if (true == verticalLayout)
	{
		layout->addStretch (2.);
		layout	= new QHBoxLayout ( );
		vlayout->addLayout (layout);
	}	// if (true == verticalLayout)
	label					= new QLabel (yTitle.c_str ( ), this);
	label->setMinimumSize (label->sizeHint ( ));
	layout->addWidget (label);
	_yTextField				= new QtDoubleTextField (this);
	_yTextField->setVisibleColumns (numberOfCharacters);
//	_yTextField->setMaxLength (numberOfCharacters);
	_yTextField->setMinimumSize (_yTextField->sizeHint ( ));
	layout->addWidget (_yTextField);
	if (true == verticalLayout)
	{
		layout->addStretch (2.);
		layout	= new QHBoxLayout ( );
		vlayout->addLayout (layout);
	}	// if (true == verticalLayout)
	label					= new QLabel (zTitle.c_str ( ), this);
	label->setMinimumSize (label->sizeHint ( ));
	layout->addWidget (label);
	_zTextField				= new QtDoubleTextField (this);
	_zTextField->setVisibleColumns (numberOfCharacters);
//	_zTextField->setMaxLength (numberOfCharacters);
	_zTextField->setMinimumSize (_zTextField->sizeHint ( ));
	layout->addWidget (_zTextField);
	layout->addStretch (2.);

	_xTextField->setText (QString::number (x));
	_yTextField->setText (QString::number (y));
	_zTextField->setText (QString::number (z));

	if (false == editable)
	{
		_xTextField->setReadOnly (true);
		_yTextField->setReadOnly (true);
		_zTextField->setReadOnly (true);
	}	// if (false == editable)
	else
	{
		_xTextField->setRange (xMin, xMax);
		_yTextField->setRange (yMin, yMax);
		_zTextField->setRange (zMin, zMax);
	}	// else if (false == editable)

	adjustSize ( );
}	// Qt3DDataPanel::Qt3DDataPanel


Qt3DDataPanel::Qt3DDataPanel (const Qt3DDataPanel&)
	: QtGroupBox (0)
{
	assert (0 && "Qt3DDataPanel copy constructor is forbidden.");
}	// Qt3DDataPanel::Qt3DDataPanel (const Qt3DDataPanel&)


Qt3DDataPanel& Qt3DDataPanel::operator = (const Qt3DDataPanel&)
{
	assert (0 && "Qt3DDataPanel assignment operator is forbidden.");
	return *this;
}	// Qt3DDataPanel::operator =


Qt3DDataPanel::~Qt3DDataPanel ( )
{
}	// Qt3DDataPanel::~Qt3DDataPanel


bool Qt3DDataPanel::isModified ( ) const
{
	double	x,y, z;

	try
	{
		x	= getX ( );
		y	= getY ( );
		z	= getZ ( );
	}
	catch (...)
	{
		return true;
	}

	if ((x != _initialX) || (y != _initialY) || (z != _initialZ))
		return true;

	return false;
}	// Qt3DDataPanel::isModified


void Qt3DDataPanel::setX (double x)
{
	assert ((0 != _xTextField) && "Qt3DDataPanel::setX : null x field.");
	_xTextField->setValue (x);
}	// Qt3DDataPanel::setX


double Qt3DDataPanel::getX ( ) const
{
	assert ((0 != _xTextField) && "Qt3DDataPanel::getX : null x field.");
	return _xTextField->getValue ( );
}	// Qt3DDataPanel::getX


void Qt3DDataPanel::setY (double y)
{
	assert ((0 != _yTextField) && "Qt3DDataPanel::setY : null y field.");
	_yTextField->setValue (y);
}	// Qt3DDataPanel::setY


double Qt3DDataPanel::getY ( ) const
{
	assert ((0 != _yTextField) && "Qt3DDataPanel::getY : null y field.");
	return _yTextField->getValue ( );
}	// Qt3DDataPanel::getY


void Qt3DDataPanel::setZ (double z)
{
	assert ((0 != _zTextField) && "Qt3DDataPanel::setZ : null z field.");
	_zTextField->setValue (z);
}	// Qt3DDataPanel::setZ


double Qt3DDataPanel::getZ ( ) const
{
	assert ((0 != _zTextField) && "Qt3DDataPanel::getZ : null z field.");
	return _zTextField->getValue ( );
}	// Qt3DDataPanel::getZ



