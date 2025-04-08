#include "QtVtk/QtVtkViewPointToolBar.h"
#include <QtUtil/QtAutoWaitingCursor.h>
#include <QtUtil/QtFileDialogUtilities.h>
#include <QtUtil/QtMessageBox.h>
#include <QtUtil/QtUnicodeHelper.h>
#include <PrefsCore/DoubleNamedValue.h>
#include <PrefsCore/DoubleTripletNamedValue.h>
#include <PrefsCore/PreferencesHelper.h>
#include <PrefsXerces/XMLLoader.h>
#include <TkUtil/Exception.h>
#include <TkUtil/Process.h>
#include <TkUtil/UTF8String.h>
#include <QFileDialog>
#include <QMenu>

#include <assert.h>



using namespace std;
using namespace TkUtil;
using namespace Preferences;

static const	Charset	charset ("àéèùô");
USE_ENCODING_AUTODETECTION




QtVtkViewPointToolBar::QtVtkViewPointToolBar (QWidget* parent, vtkCamera& camera, vtkRenderer* renderer)
	: QToolBar (parent), _viewPointButtons ( ), _nextButton (1), _camera (&camera), _renderer (renderer)
{
	_camera->Register (0);
	if (0 != _renderer)
		_renderer->Register (0);

	QAction*		action	= new QAction (QIcon(":/images/add_viewpoint.png"), "+", this);
	connect (action, SIGNAL (triggered ( )), this, SLOT (newViewPointCallback ( )));
	QToolButton*	button	= new QToolButton (this);
	button->setText ("+");
	button->setDefaultAction (action);
	addWidget (button);
	action	= new QAction (QIcon(":/images/remove_viewpoints.png"), "Réinitialiser", this);
	connect (action, SIGNAL (triggered ( )), this, SLOT (initializeCallback ( )));
	button	= new QToolButton (this);
	button->setDefaultAction (action);
	addWidget (button);
	action	= new QAction (QIcon(":/images/export_viewpoints.png"), "Exporter ...", this);
	connect (action, SIGNAL (triggered ( )), this, SLOT (exportViewPointsCallback ( )));
	button	= new QToolButton (this);
	button->setDefaultAction (action);
	addWidget (button);
	action	= new QAction (QIcon(":/images/import_viewpoints.png"), "Importer ...", this);
	connect (action, SIGNAL (triggered ( )), this, SLOT (importViewPointsCallback ( )));
	button	= new QToolButton (this);
	button->setDefaultAction (action);
	addWidget (button);
}	// QtVtkViewPointToolBar::QtVtkViewPointToolBar


QtVtkViewPointToolBar::QtVtkViewPointToolBar (const QtVtkViewPointToolBar& tb)
	: QToolBar ( ), _camera (0), _renderer (0)
{
	assert (0 && "QtVtkViewPointToolBar copy constructor is not allowed.");
}	// QtVtkViewPointToolBar::QtVtkViewPointToolBar


QtVtkViewPointToolBar& QtVtkViewPointToolBar::operator = (const QtVtkViewPointToolBar&)
{
	assert (0 && "QtVtkViewPointToolBar assignment operator is not allowed.");
	return *this;
}	// QtVtkViewPointToolBar::QtVtkViewPointToolBar


QtVtkViewPointToolBar::~QtVtkViewPointToolBar ( )
{
	if (0 != _camera)
		_camera->UnRegister (0);
	_camera	= 0;
	if (0 != _renderer)
		_renderer->UnRegister (0);
	_renderer	= 0;
}	// QtVtkViewPointToolBar::~QtVtkViewPointToolBar


void QtVtkViewPointToolBar::addViewPoint (const QtVtkViewPointToolButton::VtkViewPoint& viewPoint)
{
	assert (0 != _camera);
	UTF8String					name (charset);
	name << "Vue_" << _nextButton;
	UTF8String					icon (charset);
	icon << ":/images/viewpoint_" << TkUtil::setw (2) << _nextButton << ".png";
	QtVtkViewPointToolButton*	tb	= new QtVtkViewPointToolButton (this, icon.utf8 ( ), viewPoint, *_camera, _renderer);
	addWidget (tb);
	_viewPointButtons.push_back (tb);
	_nextButton++;
}	// QtVtkViewPointToolBar::addViewPoint


void QtVtkViewPointToolBar::removeViewPoint (QtVtkViewPointToolButton& viewPointButton)
{
	for (vector<QtVtkViewPointToolButton*>::iterator ittb = _viewPointButtons.begin ( ); _viewPointButtons.end ( ) != ittb; ittb++)
	{
		if (&viewPointButton == *ittb)
		{
//			_nextButton--;				// NON, tant qu'on ne supporte pas les trous dans la numérotation !
if (2 == _nextButton)	_nextButton--;	// => En attendant ;)
			_viewPointButtons.erase (ittb);
			viewPointButton.deleteLater ( );
			return;
		}	// if (&viewPointButton == *ittb)
	}	// for (vector<QtVtkViewPointToolButton*>::iterator ittb = _viewPointButtons.begin ( ); _viewPointButtons.end ( ) != ittb; ittb++)
	
	UTF8String	error (charset);
	error << "La barre d'outils \"Points de vue\" ne contient pas le point de vue \"" << viewPointButton.getViewPoint ( ).name << "\" : impossibilité de l'enlever.";
	throw Exception (error);
}	// QtVtkViewPointToolBar::removeViewPoint


void QtVtkViewPointToolBar::newViewPointCallback ( )
{
	assert (0 != _camera);
	UTF8String					name (charset);
	name << "Vue_" << _nextButton;
	UTF8String					icon (charset);
	icon << ":/images/viewpoint_" << TkUtil::setw (2) << _nextButton << ".png";
	QtVtkViewPointToolButton*	tb	= new QtVtkViewPointToolButton (this, icon.utf8 ( ), name.utf8 ( ), *_camera, _renderer);
	addWidget (tb);
	_viewPointButtons.push_back (tb);
	_nextButton++;
}	// QtVtkViewPointToolBar::newViewPointCallback


void QtVtkViewPointToolBar::initializeCallback ( )
{
	vector<QtVtkViewPointToolButton*>	buttons	= _viewPointButtons;
	_nextButton	= 1;
	_viewPointButtons.clear ( );
	for (vector<QtVtkViewPointToolButton*>::iterator ittb = buttons.begin ( ); buttons.end ( ) != ittb; ittb++)
		(*ittb)->deleteLater ( );
}	// QtVtkViewPointToolBar::initializeCallback


void QtVtkViewPointToolBar::exportViewPointsCallback ( )
{
	if (true == _viewPointButtons.empty ( ))
	{
		QtMessageBox::displayErrorMessage (this, "Exportation des points de vue", UTF8String ("Absence de point de vue à exporter.", charset));
		return;
	}	// if (true == _viewPointButtons.empty ( ))
	
	static QString	lastDir (Process::getCurrentDirectory ( ).c_str ( ));
	UTF8String	formats (charset);
	formats << "XML (*.xml);;";
	QFileDialog	dialog (this, "Exporter les points de vue", lastDir, UTF8TOQSTRING (formats));
	dialog.setOption (QFileDialog::DontUseNativeDialog);
	dialog.setFileMode (QFileDialog::AnyFile);
	dialog.setAcceptMode (QFileDialog::AcceptSave);
	
	if (QDialog::Accepted == dialog.exec ( ))
	{
		QtAutoWaitingCursor	cursor (true);

		QStringList	fileList	= dialog.selectedFiles ( );
		if (1 != fileList.size ( ))
			throw Exception ("Erreur, la liste de fichiers d'exportation de points de vue ne contient pas qu'un fichier.");

		string	fileName (QtFileDialogUtilities::completeFileName (fileList [0].toStdString ( ), dialog.selectedNameFilter ( ).toStdString ( )));
		File	file (fileName);

		if (false == file.isWritable ( ))
		{
			cursor.hide ( );
			UTF8String	message (charset);
			message << "Exportation des points de vue impossible dans le fichier \n" << file.getFullFileName ( ) << " :" << "\n"
			        << " absence de droits en écriture.";
			QtMessageBox::displayErrorMessage (this, "Exportation des points de vue", message);
			return;
		}	// if (false == file.isWritable ( ))
		
		lastDir				= file.getPath ( ).getFullFileName ( ).c_str ( );

		try
		{
			unique_ptr<Section>	mainSection (new Section (UTF8String ("QtVtk", charset), UTF8String ("Définitions de points de vue pour applications 3D", charset)));
			for (vector<QtVtkViewPointToolButton*>::const_iterator ittb = _viewPointButtons.begin ( ); _viewPointButtons.end ( ) != ittb; ittb++)
			{
				Section*	viewPointSection	= QtVtkViewPointToolButton::viewPointToSection ((*ittb)->getViewPoint ( ));
				mainSection->addSection (viewPointSection);
			}
			XMLLoader::save (*mainSection, file.getFullFileName ( ).c_str ( ));
		}
		catch (const Exception& exc)
		{
			UTF8String	message (charset);
			message << "Exportation des points de vue impossible dans le fichier \n" << file.getFullFileName ( ) << " :" << "\n" << exc.getFullMessage ( );
			QtMessageBox::displayErrorMessage (this, "Exportation de points de vue", message);
		}
		catch (...)
		{
			UTF8String	message (charset);
			message << "Exportation des points de vue impossible dans le fichier \n" << file.getFullFileName ( ) << " :" << " erreur non documentée.";
			QtMessageBox::displayErrorMessage (this, "Exportation de points de vue", message);
		}
	}	// if (QDialog::Accepted == dialog.exec ( ))

}	// QtVtkViewPointToolBar::exportViewPointsCallback


void QtVtkViewPointToolBar::importViewPointsCallback ( )
{
	static QString	lastDir (Process::getCurrentDirectory ( ).c_str ( ));
	UTF8String	formats (charset);
	formats << "XML (*.xml);;";
	QFileDialog	dialog (this, "Importer des points de vue", lastDir, UTF8TOQSTRING (formats));
	dialog.setOption (QFileDialog::DontUseNativeDialog);
	dialog.setFileMode (QFileDialog::ExistingFile);
	dialog.setAcceptMode (QFileDialog::AcceptOpen);
	
	if (QDialog::Accepted == dialog.exec ( ))
	{
		QtAutoWaitingCursor	cursor (true);

		QStringList	fileList	= dialog.selectedFiles ( );
		if (0 == fileList.size ( ))
			throw Exception ("Erreur, la liste de fichiers importés de point de vue ne contient pas qu'un fichier.");

		string	fileName (QtFileDialogUtilities::completeFileName (fileList [0].toStdString ( ), dialog.selectedNameFilter ( ).toStdString ( )));
		File	file (fileName);

		if (false == file.isReadable ( ))
		{
			cursor.hide ( );
			UTF8String	message (charset);
			message << "Import de points de vue impossible depuis le fichier \n" << file.getFullFileName ( ) << " :" << "\n"
			        << " absence de droits en lecture.";
			QtMessageBox::displayErrorMessage (this, "Import de points de vue", message);
			return;
		}	// if (false == file.isReadable ( ))
		
		lastDir				= file.getPath ( ).getFullFileName ( ).c_str ( );

		try
		{
			unique_ptr<Section>	mainSection (new Section (UTF8String ("QtVtk", charset), UTF8String ("Définitions de points de vue pour applications 3D", charset)));
			mainSection.reset (XMLLoader::load (file.getFullFileName ( ).c_str ( )));
			const vector<Section*>	viewPoints	= mainSection->getSections ( );
			for (vector<Section*>::const_iterator itvp = viewPoints.begin ( ); viewPoints.end ( ) != itvp; itvp++)
			{
				addViewPoint (QtVtkViewPointToolButton::sectionToViewPoint (**itvp));
			}	// for (vector<Section*>::const_iterator itvp = viewPoints.begin ( ); viewPoints.end ( ) != itvp; itvp++)
		}
		catch (const Exception& exc)
		{
			UTF8String	message (charset);
			message << "Importation de points de vue impossible depuis le fichier \n" << file.getFullFileName ( ) << " :" << "\n" << exc.getFullMessage ( );
			QtMessageBox::displayErrorMessage (this, "Importation de points de vue", message);
		}
		catch (...)
		{
			UTF8String	message (charset);
			message << "Importation de points de vue impossible depuis le fichier \n" << file.getFullFileName ( ) << " :" << " erreur non documentée.";
			QtMessageBox::displayErrorMessage (this, "Importation de points de vue", message);
		}
	}	// if (QDialog::Accepted == dialog.exec ( ))
}	// QtVtkViewPointToolBar::importViewPointsCallback
