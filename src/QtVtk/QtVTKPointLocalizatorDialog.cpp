#include "QtVtk/QtVTKPointLocalizatorDialog.h"

#include <QtUtil/QtConfiguration.h>
#include <QtUtil/QtDlgClosurePanel.h>
#include <TkUtil/MemoryError.h>

#include <QBoxLayout>
#include <QFrame>

#include <assert.h>


USING_STD
USING_UTIL


QtVTKPointLocalizatorDialog::QtVTKPointLocalizatorDialog (
			QWidget* parent, const string& title, vtkRenderer& renderer,
			const string& xTitle, const string& yTitle, const string& zTitle,
            double x, double y, double z, double dx, double dy, double dz)
	: QDialog (parent), _localizatorPanel (0)
{
	setWindowTitle (title.c_str ( ));
	QVBoxLayout*	layout	= new QVBoxLayout (this);
//	layout->setContentsMargins (0, 0, 0, 0);
	setLayout (layout);

	_localizatorPanel	= new QtVTKPointLocalizatorPanel (
								this, title, renderer, xTitle, yTitle, zTitle,
								x, y, z, dx, dy, dz);
	layout->addWidget (_localizatorPanel);

	QFrame*	separator	= new QFrame (this);
	separator->setFrameStyle (QFrame::HLine | QFrame::Sunken);
	separator->setMidLineWidth (2);
	layout->addWidget (separator);

	QtDlgClosurePanel*	closurePanel	=
					 new QtDlgClosurePanel (this, false, "Fermer", "", "");
	layout->addWidget (closurePanel);
	closurePanel->getApplyButton ( )->setAutoDefault (false);
	closurePanel->getApplyButton ( )->setDefault (false);
	connect (closurePanel->getApplyButton ( ), SIGNAL(clicked ( )), this,
	         SLOT(accept ( )));

	adjustSize ( );
	setFixedSize (sizeHint ( ));
}	// QtVTKPointLocalizatorDialog::QtVTKPointLocalizatorDialog


QtVTKPointLocalizatorDialog::QtVTKPointLocalizatorDialog (
											const QtVTKPointLocalizatorDialog&)
	: QDialog (0), _localizatorPanel (0)
{
	assert (0 && "QtVTKPointLocalizatorDialog copy constructor is forbidden.");
}	// QtVTKPointLocalizatorDialog::QtVTKPointLocalizatorDialog (const QtVTKPointLocalizatorDialog&)


QtVTKPointLocalizatorDialog& QtVTKPointLocalizatorDialog::operator = (
											const QtVTKPointLocalizatorDialog&)
{
	assert (0 && "QtVTKPointLocalizatorDialog assignment operator is forbidden.");
	return *this;
}	// QtVTKPointLocalizatorDialog::operator =


QtVTKPointLocalizatorDialog::~QtVTKPointLocalizatorDialog ( )
{
}	// QtVTKPointLocalizatorDialog::~QtVTKPointLocalizatorDialog


void QtVTKPointLocalizatorDialog::setX (double x)
{
	CHECK_NULL_PTR_ERROR (_localizatorPanel)
	_localizatorPanel->setX (x);
}	// QtVTKPointLocalizatorDialog::setX


double QtVTKPointLocalizatorDialog::getX ( ) const
{
	CHECK_NULL_PTR_ERROR (_localizatorPanel)
	return _localizatorPanel->getX ( );
}	// QtVTKPointLocalizatorDialog::getX


void QtVTKPointLocalizatorDialog::setY (double y)
{
	CHECK_NULL_PTR_ERROR (_localizatorPanel)
	_localizatorPanel->setY (y);
}	// QtVTKPointLocalizatorDialog::setY


double QtVTKPointLocalizatorDialog::getY ( ) const
{
	CHECK_NULL_PTR_ERROR (_localizatorPanel)
	return _localizatorPanel->getY ( );
}	// QtVTKPointLocalizatorDialog::getY


void QtVTKPointLocalizatorDialog::setZ (double z)
{
	CHECK_NULL_PTR_ERROR (_localizatorPanel)
	_localizatorPanel->setZ (z);
}	// QtVTKPointLocalizatorDialog::setZ


double QtVTKPointLocalizatorDialog::getZ ( ) const
{
	CHECK_NULL_PTR_ERROR (_localizatorPanel)
	return _localizatorPanel->getZ ( );
}	// QtVTKPointLocalizatorDialog::getZ


const QtVTKPointLocalizatorPanel&
					QtVTKPointLocalizatorDialog::getLocalizatorPanel ( ) const
{
	CHECK_NULL_PTR_ERROR (_localizatorPanel)
	return *_localizatorPanel;
}	// QtVTKPointLocalizatorDialog::getLocalizatorPanel


QtVTKPointLocalizatorPanel& QtVTKPointLocalizatorDialog::getLocalizatorPanel ( )
{
	CHECK_NULL_PTR_ERROR (_localizatorPanel)
	return *_localizatorPanel;
}	// QtVTKPointLocalizatorDialog::getLocalizatorPanel


void QtVTKPointLocalizatorDialog::hideEvent (QHideEvent* event)
{
	getLocalizatorPanel ( ).setDisplayed (false);

	QDialog::hideEvent (event);
}	// QtVTKPointLocalizatorDialog::hideEvent


void QtVTKPointLocalizatorDialog::showEvent (QShowEvent* event)
{
	getLocalizatorPanel ( ).setDisplayed (true);

	QDialog::showEvent (event);
}	// QtVTKPointLocalizatorDialog::showEvent


