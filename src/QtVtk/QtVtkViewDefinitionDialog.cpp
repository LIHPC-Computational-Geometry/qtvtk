#include "QtVtk/QtVtkViewDefinitionDialog.h"
#include <QtUtil/QtGroupBox.h>
#include <QtUtil/QtConfiguration.h>
#include <QtUtil/QtUnicodeHelper.h>

#include <TkUtil/Exception.h>

#include <assert.h>

#include <QLayout>
#include <QLabel>


USING_STD
USING_UTIL

static const	Charset	charset ("àéèùô");
USE_ENCODING_AUTODETECTION

/**
 * Aide pour positionner un widget au centre de "son parent".
 * Utile depuis le passage à Qt 5 : une boite de dialogue modale ayant un parent est positionnée au centre du parent, ce qui est très bien, mais si on la
 * déplace le parent suit le mouvement ... Donc pas moyen de voir ce qu'il y a dessous la boite de dialogue.
 * => créer la boite de dialogue sans parent, et utiliser cette macro juste au moment de l'afficher.
 */
#ifndef QTVTK_CENTER_DIALOG
#define LEM_CENTER_DIALOG(dlg,parent)                                         \
if ((0 != dlg) && (0 != parent))                                              \
{                                                                             \
        QRect   pfg     = parent->frameGeometry ( );                                  \
        QSize   sz      = dlg->size ( );                                              \
        dlg->move (pfg.x ( ) + (pfg.width ( ) - sz.width ( )) / 2,                \
                   pfg.y ( ) + (pfg.height ( ) - sz.height ( )) / 2);             \
}
#endif  // LEM_CENTER_DIALOG


QtVtkViewDefinitionDialog::QtVtkViewDefinitionDialog (
			QWidget* parent, const string& title, bool modal, const UTF8String& name, const UTF8String& comment,
			double position [3], double focal [3], double viewUp [3], double roll,
			vtkRenderer* renderer, const string& helpURL, const string& helpTag)
	: QDialog (0, true == modal ? (Qt::WindowFlags)(QtConfiguration::modalDialogWFlags | Qt::WindowStaysOnTopHint) : (Qt::WindowFlags)(QtConfiguration::amodalDialogWFlags | Qt::WindowStaysOnTopHint)), _viewDefinitionPanel (0), _closurePanel (0)
{	
	setModal (modal);
	setWindowTitle (QSTR (title));

	// Creation de l'ihm :
	QVBoxLayout*	layout	= new QVBoxLayout (this);
#ifdef QT_5
	layout->setMargin (QtConfiguration::margin);
#else	// QT_5
	layout->setContentsMargins (QtConfiguration::margin, QtConfiguration::margin, QtConfiguration::margin, QtConfiguration::margin);
#endif	// QT_5
	layout->setSizeConstraint (QLayout::SetMinimumSize);
	QtGroupBox*		frame		=  new QtGroupBox (QSTR ("Paramètres de la vue"), this, "frame");
	QVBoxLayout*	frameLayout	= new QVBoxLayout (frame);
	layout->addWidget (frame);
	frame->setLayout (frameLayout);
	frame->setMargin (QtConfiguration::margin);
	frame->setSpacing (QtConfiguration::spacing);
	_viewDefinitionPanel	= new QtVtkViewDefinitionPanel (frame, title, name, comment, position, focal, viewUp, roll, renderer);
	_viewDefinitionPanel->adjustSize ( );
	frameLayout->addWidget (_viewDefinitionPanel);

	layout->addWidget (new QLabel (" ", this));
	_closurePanel	= new QtDlgClosurePanel (this, true, "Appliquer", "Fermer", "Annuler",helpURL, helpTag);
	layout->addWidget (_closurePanel);
	_closurePanel->setMinimumSize (_closurePanel->sizeHint ( ));

	// Par defaut le bouton OK est artificellement clique par QDialog quand l'utilisateur fait return dans un champ de texte => on inhibe ce comportement par defaut :
	_closurePanel->getApplyButton ( )->setAutoDefault (false);
	_closurePanel->getApplyButton ( )->setDefault (false);
	_closurePanel->getCloseButton ( )->setAutoDefault (false);
	_closurePanel->getCloseButton ( )->setDefault (false);
	_closurePanel->getCancelButton ( )->setAutoDefault (false);
	_closurePanel->getCancelButton ( )->setDefault (false);
	connect (_closurePanel->getApplyButton ( ), SIGNAL(clicked ( )), this, SLOT(apply ( )));
	connect (_closurePanel->getCloseButton ( ), SIGNAL(clicked ( )), this, SLOT(close ( )));
	connect (_closurePanel->getCancelButton ( ), SIGNAL(clicked ( )), this, SLOT(reject ( )));

	layout->activate ( );
	setMinimumSize (layout->sizeHint ( ));
	LEM_CENTER_DIALOG (this, parent)
}	// QtVtkViewDefinitionDialog::QtVtkViewDefinitionDialog


QtVtkViewDefinitionDialog::QtVtkViewDefinitionDialog (const QtVtkViewDefinitionDialog&)
{
	assert (0 && "QtVtkViewDefinitionDialog copy constructor is forbidden.");
}	// QtVtkViewDefinitionDialog::QtVtkViewDefinitionDialog (const QtVtkViewDefinitionDialog&)


QtVtkViewDefinitionDialog& QtVtkViewDefinitionDialog::operator = (const QtVtkViewDefinitionDialog&)
{
	assert (0 && "QtVtkViewDefinitionDialog assignment operator is forbidden.");
	return *this;
}	// QtVtkViewDefinitionDialog::operator =


QtVtkViewDefinitionDialog::~QtVtkViewDefinitionDialog ( )
{
}	// QtVtkViewDefinitionDialog::~QtVtkViewDefinitionDialog


UTF8String QtVtkViewDefinitionDialog::getName ( ) const
{
	assert ((0 != _viewDefinitionPanel) && "QtVtkViewDefinitionDialog::getName : null view definition panel.");
	return _viewDefinitionPanel->getName ( );
}	// QtVtkViewDefinitionDialog::getName


UTF8String QtVtkViewDefinitionDialog::getComment ( ) const
{
	assert ((0 != _viewDefinitionPanel) && "QtVtkViewDefinitionDialog::getComment : null view definition panel.");
	return _viewDefinitionPanel->getComment ( );
}	// QtVtkViewDefinitionDialog::getComment

 
void QtVtkViewDefinitionDialog::getPosition (double coords [3]) const
{
	assert ((0 != _viewDefinitionPanel) && "QtVtkViewDefinitionDialog::getPosition : null view definition panel.");
	_viewDefinitionPanel->getPosition (coords);
}	// QtVtkViewDefinitionDialog::getPosition


void QtVtkViewDefinitionDialog::getFocalPoint (double coords [3]) const
{
	assert ((0 != _viewDefinitionPanel) && "QtVtkViewDefinitionDialog::getFocalPoint : null view definition panel.");
	_viewDefinitionPanel->getFocalPoint (coords);
}	// QtVtkViewDefinitionDialog::getFocalPoint


void QtVtkViewDefinitionDialog::getViewUp (double direction [3]) const
{
	assert ((0 != _viewDefinitionPanel) && "QtVtkViewDefinitionDialog::getViewUp : null view definition panel.");
	_viewDefinitionPanel->getViewUp (direction);
}	// QtVtkViewDefinitionDialog::getViewUp


double QtVtkViewDefinitionDialog::getRoll ( ) const
{
	assert ((0 != _viewDefinitionPanel) && "QtVtkViewDefinitionDialog::getRoll : null view definition panel.");
	return _viewDefinitionPanel->getRoll ( );
}	// QtVtkViewDefinitionDialog::getRoll


QPushButton* QtVtkViewDefinitionDialog::getApplyButton ( ) const
{
	assert ((0 != _closurePanel) && "QtVtkViewDefinitionDialog::getApplyButton : null closure panel.");
	return _closurePanel->getApplyButton ( );
}	// QtVtkViewDefinitionDialog::getApplyButton


QPushButton* QtVtkViewDefinitionDialog::getCancelButton ( ) const
{
	assert ((0 != _closurePanel) && "QtVtkViewDefinitionDialog::getCancelButton : null closure panel.");
	return _closurePanel->getCancelButton ( );
}	// QtVtkViewDefinitionDialog::getCancelButton


void QtVtkViewDefinitionDialog::apply ( )
{
	assert ((0 != _viewDefinitionPanel) && "QtVtkViewDefinitionDialog::apply : null view definition panel.");
	_viewDefinitionPanel->apply ( );
}	// QtVtkViewDefinitionDialog::apply


void QtVtkViewDefinitionDialog::close ( )
{
	QDialog::accept ( );
	emit applied (this);
}	// QtVtkViewDefinitionDialog::close


void QtVtkViewDefinitionDialog::reject ( )
{
	assert ((0 != _viewDefinitionPanel) && "QtVtkViewDefinitionDialog::reject : null view definition panel.");
	_viewDefinitionPanel->reset ( );
	QDialog::reject ( );
	emit canceled (this);
}	// QtVtkViewDefinitionDialog::reject


