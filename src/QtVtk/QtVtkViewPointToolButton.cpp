#include "QtVtk/QtVtkViewPointToolButton.h"
#include "QtVtk/QtVtkViewDefinitionDialog.h"
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
#include <vtkRenderWindow.h>

#include <assert.h>
#include <string.h>

using namespace TkUtil;
using namespace Preferences;
using namespace std;

static const	Charset	charset ("àéèùô");
USE_ENCODING_AUTODETECTION


// ======================================================================================================================================
//                                         LA STRUCTURE QtVtkViewPointToolButton::VtkViewPoint
// ======================================================================================================================================

QtVtkViewPointToolButton::VtkViewPoint::VtkViewPoint ( )
	: name ( ), comment ( ), roll (0.)
{
	memset (position, 0, 3 * sizeof (double));
	memset (focalPoint, 0, 3 * sizeof (double));
	memset (viewUp, 0, 3 * sizeof (double));
}	// VtkViewPoint::VtkViewPoint


QtVtkViewPointToolButton::VtkViewPoint::VtkViewPoint (const QtVtkViewPointToolButton::VtkViewPoint& vp)
	: name (vp.name), comment (vp.comment), roll (vp.roll)
{
	memcpy (position, vp.position, 3 * sizeof (double));
	memcpy (focalPoint, vp.focalPoint, 3 * sizeof (double));
	memcpy (viewUp, vp.viewUp, 3 * sizeof (double));
}	// VtkViewPoint::VtkViewPoint


QtVtkViewPointToolButton::VtkViewPoint& QtVtkViewPointToolButton::VtkViewPoint::operator = (const QtVtkViewPointToolButton::VtkViewPoint& vp)
{
	name		= vp.name;
	comment		= vp.comment;
	roll		= vp.roll;
	memcpy (position, vp.position, 3 * sizeof (double));
	memcpy (focalPoint, vp.focalPoint, 3 * sizeof (double));
	memcpy (viewUp, vp.viewUp, 3 * sizeof (double));

	return *this;
}	// VtkViewPoint::operator =


QtVtkViewPointToolButton::VtkViewPoint& QtVtkViewPointToolButton::VtkViewPoint::operator = (const vtkCamera& camera)
{
	roll		= ((vtkCamera&)camera).GetRoll ( );
	memcpy (position, ((vtkCamera&)camera).GetPosition ( ), 3 * sizeof (double));
	memcpy (focalPoint, ((vtkCamera&)camera).GetFocalPoint ( ), 3 * sizeof (double));
	memcpy (viewUp, ((vtkCamera&)camera).GetViewUp ( ), 3 * sizeof (double));

	return *this;
}	// VtkViewPoint::operator =


QtVtkViewPointToolButton::VtkViewPoint::~VtkViewPoint ( )
{
}	// VtkViewPoint::~VtkViewPoint


// ======================================================================================================================================
//                                                      LA CLASSE QtVtkViewPointToolButton
// ======================================================================================================================================

Section* QtVtkViewPointToolButton::viewPointToSection (const QtVtkViewPointToolButton::VtkViewPoint& vp)
{
	Section*					section		= new Section (vp.name, vp.comment);
	DoubleTripletNamedValue*	position	= new DoubleTripletNamedValue (UTF8String ("position", charset), vp.position [0], vp.position [1], vp.position [2]);
	DoubleTripletNamedValue*	focalPoint	= new DoubleTripletNamedValue (UTF8String ("focalPoint", charset), vp.focalPoint [0], vp.focalPoint [1], vp.focalPoint [2]);
	DoubleTripletNamedValue*	viewUp		= new DoubleTripletNamedValue (UTF8String ("viewUp", charset), vp.viewUp [0], vp.viewUp [1], vp.viewUp [2]);
	DoubleNamedValue*			roll		= new DoubleNamedValue (UTF8String ("roll", charset), vp.roll);
	section->addNamedValue (position);
	section->addNamedValue (focalPoint);
	section->addNamedValue (viewUp);
	section->addNamedValue (roll);
	
	return section;
}	// QtVtkViewPointToolButton::viewPointToSection


QtVtkViewPointToolButton::VtkViewPoint QtVtkViewPointToolButton::sectionToViewPoint (const Section& section)
{
	QtVtkViewPointToolButton::VtkViewPoint	vp;

	vp.name				= section.getName ( );
	vp.comment			= section.getComment ( );
	try
	{
		DoubleTripletNamedValue*	pos	= dynamic_cast<DoubleTripletNamedValue*>(&(section.getNamedValue (UTF8String ("position", charset))));
		if (0 != pos)
		{
			vp.position [0]		= pos->getX ( );
			vp.position [1]		= pos->getY ( );
			vp.position [2]		= pos->getZ ( );
		}	// if (0 != pos)
	}
	catch (...)
	{
	}
	
	try
	{
		DoubleTripletNamedValue*	fp	= dynamic_cast<DoubleTripletNamedValue*>(&(section.getNamedValue (UTF8String ("focalPoint", charset))));
		if (0 != fp)
		{
			vp.focalPoint [0]	= fp->getX ( );
			vp.focalPoint [1]	= fp->getY ( );
			vp.focalPoint [2]	= fp->getZ ( );
		}	// if (0 != fp)
	}
	catch (...)
	{
	}
	
	try
	{
		DoubleTripletNamedValue*	vu	= dynamic_cast<DoubleTripletNamedValue*>(&(section.getNamedValue (UTF8String ("viewUp", charset))));
		if (0 != vu)
		{
			vp.viewUp [0]	= vu->getX ( );
			vp.viewUp [1]	= vu->getY ( );
			vp.viewUp [2]	= vu->getZ ( );
		}	// if (0 != vu)
	}
	catch (...)
	{
	}
	
	try
	{
		DoubleNamedValue*	r	= dynamic_cast<DoubleNamedValue*>(&(section.getNamedValue (UTF8String ("roll", charset))));
		if (0 != r)
			vp.roll	= r->getValue ( );
	}
	catch (...)
	{
	}
	
	return vp;
}	// QtVtkViewPointToolButton::sectionToViewPoint
	
	
QtVtkViewPointToolButton::QtVtkViewPointToolButton (QWidget* parent, const std::string& icon, const TkUtil::UTF8String& name, vtkCamera& camera, vtkRenderer* renderer)
	: QToolButton (parent), _viewPoint ( ), _camera (&camera), _renderer (renderer)
{
	setText (UTF8TOQSTRING (name));
	setIcon (QIcon (icon.c_str ( )));
	_viewPoint.name	= name;
	_viewPoint		= *_camera;
	setToolButtonStyle (Qt::ToolButtonIconOnly);
	setPopupMode (QToolButton::MenuButtonPopup);
	_camera->Register (0);
	if (0 != _renderer)
		_renderer->Register (0);

	QMenu*	menu	= new QMenu (this);
	QAction*	action	= new QAction ("Appliquer", this);
	connect (action, SIGNAL (triggered ( )), this, SLOT (applyViewPointCallback ( )));
	menu->addAction (action);
	action	= new QAction ("Modifier ...", this);
	connect (action, SIGNAL (triggered ( )), this, SLOT (editViewPointCallback ( )));
	menu->addAction (action);
	action	= new QAction ("Supprimer ...", this);
	connect (action, SIGNAL (triggered ( )), this, SLOT (removeViewPointCallback ( )));
	menu->addAction (action);
	action	= new QAction ("Exporter ...", this);
	connect (action, SIGNAL (triggered ( )), this, SLOT (exportViewPointCallback ( )));
	menu->addAction (action);
	setMenu (menu);
}	// QtVtkViewPointToolButton::QtVtkViewPointToolButton


QtVtkViewPointToolButton::QtVtkViewPointToolButton (QWidget* parent, const string& icon, const QtVtkViewPointToolButton::VtkViewPoint& viewPoint, vtkCamera& camera, vtkRenderer* renderer)
	: QToolButton (parent), _viewPoint (viewPoint), _camera (&camera), _renderer (renderer)
{
	setText (UTF8TOQSTRING (_viewPoint.name));
	setToolTip (UTF8TOQSTRING (_viewPoint.comment));
	setIcon (QIcon (icon.c_str ( )));
	setToolButtonStyle (Qt::ToolButtonIconOnly);
	setPopupMode (QToolButton::MenuButtonPopup);
	_camera->Register (0);
	if (0 != _renderer)
		_renderer->Register (0);

	QMenu*	menu	= new QMenu (this);
	QAction*	action	= new QAction ("Appliquer", this);
	connect (action, SIGNAL (triggered ( )), this, SLOT (applyViewPointCallback ( )));
	menu->addAction (action);
	action	= new QAction ("Modifier ...", this);
	connect (action, SIGNAL (triggered ( )), this, SLOT (editViewPointCallback ( )));
	menu->addAction (action);
	action	= new QAction ("Supprimer ...", this);
	connect (action, SIGNAL (triggered ( )), this, SLOT (removeViewPointCallback ( )));
	menu->addAction (action);
	action	= new QAction ("Exporter ...", this);
	connect (action, SIGNAL (triggered ( )), this, SLOT (exportViewPointCallback ( )));
	menu->addAction (action);
	setMenu (menu);
}	// QtVtkViewPointToolButton::QtVtkViewPointToolButton


QtVtkViewPointToolButton::QtVtkViewPointToolButton (const QtVtkViewPointToolButton& tb)
	: QToolButton ( ), _viewPoint ( ), _camera (0), _renderer (0)
{
	assert (0 && "QtVtkViewPointToolButton copy constructor is not allowed.");
}	// QtVtkViewPointToolButton::QtVtkViewPointToolButton


QtVtkViewPointToolButton& QtVtkViewPointToolButton::operator = (const QtVtkViewPointToolButton&)
{
	assert (0 && "QtVtkViewPointToolButton assignment operator is not allowed.");
	return *this;
}	// QtVtkViewPointToolButton::QtVtkViewPointToolButton


QtVtkViewPointToolButton::~QtVtkViewPointToolButton ( )
{
	if (0 != _camera)
		_camera->UnRegister (0);
	_camera	= 0;
	if (0 != _renderer)
		_renderer->UnRegister (0);
	_renderer	= 0;
}	// QtVtkViewPointToolButton::~QtVtkViewPointToolButton


const QtVtkViewPointToolButton::VtkViewPoint& QtVtkViewPointToolButton::getViewPoint ( ) const
{
	return _viewPoint;
}	// QtVtkViewPointToolButton::getViewPoint


void QtVtkViewPointToolButton::setViewPoint (const QtVtkViewPointToolButton::VtkViewPoint& vp)
{
	_viewPoint	= vp;
	setText (UTF8TOQSTRING (_viewPoint.name));
	setToolTip (UTF8TOQSTRING (_viewPoint.comment));
	applyViewPointCallback ( );
}	// QtVtkViewPointToolButton::setViewPoint


void QtVtkViewPointToolButton::applyViewPointCallback ( )
{
	assert (0 != _camera && "QtVtkViewPointToolButton::applyViewPointCallback : null camera");
	_camera->SetPosition (_viewPoint.position);
	_camera->SetFocalPoint (_viewPoint.focalPoint);
	_camera->SetViewUp (_viewPoint.viewUp);
	_camera->SetRoll (_viewPoint.roll);

	if (0 != _renderer)
	{
		_renderer->ResetCameraClippingRange ( );
		if (0 != _renderer->GetRenderWindow ( ))
			_renderer->GetRenderWindow ( )->Render ( );
	}	// if (0 != _renderer)
}	// QtVtkViewPointToolButton::applyViewPointCallback


void QtVtkViewPointToolButton::editViewPointCallback ( )
{
	assert (0 != _camera && "QtVtkViewPointToolButton::editViewPointCallback : null camera");

	try
	{
		UTF8String	dlgTitle (charset);
		dlgTitle << "Paramètres de définition de la vue.";

		QtVtkViewPointToolButton::VtkViewPoint	vp	= getViewPoint ( );
		QtVtkViewDefinitionDialog				viewDialog (this, dlgTitle.utf8 ( ).c_str ( ), true, vp.name, vp.comment, vp.position, vp.focalPoint, vp.viewUp, vp.roll, _renderer);
		int	retVal	= viewDialog.exec ( );

		if (0 == retVal)
		{
			applyViewPointCallback ( );	// Annule d'éventuelles modifications effectuées par la boite de dialogue.
			return;
		}

		vp.name		= viewDialog.getName ( );
		vp.comment	= viewDialog.getComment ( );
		viewDialog.getPosition (vp.position);
		viewDialog.getFocalPoint (vp.focalPoint);
		viewDialog.getViewUp (vp.viewUp);
		vp.roll		= viewDialog.getRoll ( );
		
		UTF8String	message (charset);
		message << "Paramètres de la vues modifiés. "
				<< "Position de la caméra : " 
				<< vp.position [0] << ", " << vp.position [1] << ", " << vp.position [2]
				<< ". Position de la focale : "
				<< vp.focalPoint [0] << ", " << vp.focalPoint [1] << ", " << vp.focalPoint [2]
				<< ". Direction vers le haut : "
				<< vp.viewUp [0] << ", " << vp.viewUp [1] << ", " << vp.viewUp [2]
				<< " Roulis : " << vp.roll << " degrés.";
		cout << message << endl;
		setViewPoint (vp);
	}
	catch (const Exception& exc)
	{
		UTF8String	message (charset);
		message << "Modification du point de vue impossible :" << "\n" << exc.getFullMessage ( );
		QtMessageBox::displayErrorMessage (this, "Modification de point de vue", message);
	}
	catch (...)
	{
		UTF8String	message (charset);
		message << "Modification du point de vue impossible : erreur non renseignée.";
		QtMessageBox::displayErrorMessage (this, "Modification de point de vue", message);
	}
}	// QtVtkViewPointToolButton::editViewPointCallback
	

void QtVtkViewPointToolButton::removeViewPointCallback ( )
{
	try
	{
		QtVtkViewPointToolBar*	toolbar	= dynamic_cast<QtVtkViewPointToolBar*>(parentWidget ( ));
		if (0 != toolbar)
			toolbar->removeViewPoint (*this);
		else
			deleteLater ( );
	}
	catch (...)
	{
	}
}	// QtVtkViewPointToolButton::removeViewPointCallback


void QtVtkViewPointToolButton::exportViewPointCallback ( )
{
	static QString	lastDir (Process::getCurrentDirectory ( ).c_str ( ));
	UTF8String	formats (charset);
	formats << "XML (*.xml);;";
	QFileDialog	dialog (this, "Exporter le point de vue", lastDir, UTF8TOQSTRING (formats));
	dialog.setOption (QFileDialog::DontUseNativeDialog);
	dialog.setFileMode (QFileDialog::AnyFile);
	dialog.setAcceptMode (QFileDialog::AcceptSave);
	
	if (QDialog::Accepted == dialog.exec ( ))
	{
		QtAutoWaitingCursor	cursor (true);

		QStringList	fileList	= dialog.selectedFiles ( );
		if (1 != fileList.size ( ))
			throw Exception ("Erreur, la liste de fichiers d'export de point de vue ne contient pas qu'un fichier.");

		string	fileName (QtFileDialogUtilities::completeFileName (fileList [0].toStdString ( ), dialog.selectedNameFilter ( ).toStdString ( )));
		File	file (fileName);

		if (false == file.isWritable ( ))
		{
			cursor.hide ( );
			UTF8String	message (charset);
			message << "Exportation du point de vue impossible dans le fichier \n" << file.getFullFileName ( ) << " :" << "\n"
			        << " absence de droits en écriture.";
			QtMessageBox::displayErrorMessage (this, "Export de point de vue", message);
			return;
		}	// if (false == file.isWritable ( ))
		
		lastDir				= file.getPath ( ).getFullFileName ( ).c_str ( );

		try
		{
			unique_ptr<Section>	mainSection (new Section (UTF8String ("QtVtk", charset), UTF8String ("Définitions de points de vue pour applications 3D", charset)));
			Section*	viewPointSection	= QtVtkViewPointToolButton::viewPointToSection (getViewPoint ( ));
			mainSection->addSection (viewPointSection);
			XMLLoader::save (*mainSection, file.getFullFileName ( ).c_str ( ));
		}
		catch (const Exception& exc)
		{
			UTF8String	message (charset);
			message << "Exportation du point de vue impossible dans le fichier \n" << file.getFullFileName ( ) << " :" << "\n" << exc.getFullMessage ( );
			QtMessageBox::displayErrorMessage (this, "Exportation de point de vue", message);
		}
		catch (...)
		{
			UTF8String	message (charset);
			message << "Exportation du point de vue impossible dans le fichier \n" << file.getFullFileName ( ) << " :" << " erreur non documentée.";
			QtMessageBox::displayErrorMessage (this, "Exportation de point de vue", message);
		}
	}	// if (QDialog::Accepted == dialog.exec ( ))
}	// QtVtkViewPointToolButton::exportViewPointCallback


