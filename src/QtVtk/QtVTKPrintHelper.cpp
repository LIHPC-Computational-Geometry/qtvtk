// Sources d'informations pour des evolutions ulterieures :
// src.304/qt-x11-free-3.0.4/src/kernel/qprinter_unix.cpp
// src.304/qt-x11-free-3.0.4/src/dialogs/qprintdialog.cpp
// voir fichier /etc/printcap : liste des imprimantes

#include <QtVtk/QtVTKPrintHelper.h>
#include <QtUtil/QtMessageBox.h>

#include <TkUtil/Exception.h>
#include <TkUtil/File.h>
#include <TkUtil/MachineData.h>
#include <TkUtil/PrintCommand.h>
#include <QtUtil/QtAutoWaitingCursor.h>
#include <QtUtil/QtUnicodeHelper.h>

#include <vtkWindowToImageFilter.h>
#include <vtkPostScriptWriter.h>
#include <vtkJPEGWriter.h>
#include <vtkBMPWriter.h>
#include <vtkPNGWriter.h>
#include <vtkPNMWriter.h>
#include <vtkTIFFWriter.h>
#include <vtkGL2PSExporter.h>
#include <vtkRenderWindow.h>
#include <vtkRendererCollection.h>

#include <QPrinter>
#include <QPrintDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QApplication>

#include <GL/gl.h>
#include <sstream>
#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <math.h>


USING_UTIL
USING_STD

static const Charset	charset ("àéèùô");
USE_ENCODING_AUTODETECTION


// ===========================================================================
//                            Structure PrinterStatus
// ===========================================================================

QtVTKPrintHelper::PrinterStatus::PrinterStatus ( )
	: completed (false), destination ( ), deviceName ( )
{
}	// PrinterStatus::PrinterStatus


QtVTKPrintHelper::PrinterStatus::PrinterStatus (const QtVTKPrintHelper::PrinterStatus& copied)
	: completed (copied.completed), destination (copied.destination), 
	  deviceName (copied.deviceName)
{
}	// PrinterStatus::PrinterStatus copy ctor


QtVTKPrintHelper::PrinterStatus& QtVTKPrintHelper::PrinterStatus::operator = (const QtVTKPrintHelper::PrinterStatus& copied)
{
	if (&copied != this)
	{
		completed	= copied.completed;
		destination	= copied.destination;
		deviceName	= copied.deviceName;
	}

	return *this;
}	// PrinterStatus::operator =


// ===========================================================================
//                            Classe QtVTKPrintHelper
// ===========================================================================


QtVTKPrintHelper::PrinterStatus QtVTKPrintHelper::print (vtkRenderWindow& window, QWidget* top)
{
	QPrinter		printer;
	PrinterStatus	status;
	static QString	lastOutputFileName;

	if (0 != lastOutputFileName.length ( ))
		printer.setOutputFileName (lastOutputFileName);

	QPrintDialog	dialog (&printer, top);	
	if (QDialog::Accepted == dialog.exec ( ))
	{
		// S'assurer que la boite de dialogue d'impression ait bien disparue.
		if (0 != top)
			top->raise ( );
		while (true == qApp->hasPendingEvents ( ))
			qApp->processEvents ( );
		QtAutoWaitingCursor	cursor (true);
		if (printer.outputFileName ( ).size ( ) != 0)
		{
			string	fileName (printer.outputFileName ( ).toStdString ( ));
			File	file (fileName);

			if (false == file.isWritable ( ))
			{
				cursor.hide ( );
				UTF8String	message (charset);
				message << "Impression impossible dans le fichier "
				        << "\n" << file.getFullFileName ( ) << " :" << "\n"
				        << " absence de droits en écriture.";
				QtMessageBox::displayErrorMessage (top, "Impression", message);
				return status;
			}	// if (false == file.isWritable ( ))
			printToFile (window, fileName, 0);
			status.completed	= true;
			status.destination	= PrinterStatus::FILE;
			status.deviceName	= fileName;
			lastOutputFileName	= fileName.c_str ( );
		}
		else
		{
			string	tmpFile (File::createTemporaryName ("prn", true));
			string	tmpPSFile (tmpFile + ".ps");
			PrintCommand::CommandOptions	options;
			options.eraseFile	= true;
			options.copiesNumber= (size_t)printer.numCopies ( );
			printToFile (window, tmpPSFile, 720);
			string		printerName	= printer.printerName ( ).toStdString ( );
			printToPrinter (tmpPSFile, printerName, options);
			status.completed	= true;
			status.destination	= PrinterStatus::PRINTER;
			status.deviceName	= printerName;
		}
	}	// if (true == printer.setup (0))

	return status;
}	// QtVTKPrintHelper::print


/**
 * @return	La première extension du filtre Qt reçu en argument (format <I>NOM (*.ext1 *.ext2 ... *.extn)</I>.
 */
static string getFirstExtension (const string& filter)	// v 7.4.0
{
	UTF8String	prepared (filter);
	prepared.replace (string ("("), string (" "), true);	// => nom *.ext1)
	prepared.replace (string ("*"), string (""), true);	// => nom .ext1)
	prepared.replace (string (")"), string (""), true);	// => nom *.ext1
	istringstream	stream (prepared.utf8 ( ));
	string	name, ext;
	stream >> name >> ext;


	if ((false == stream.fail ( )) && (false == stream.bad ( )))
		return ext;

	return string ( );
}	// getFirstExtension


/**
 * @param	Nom de fichier proposé
 * @param	Filtre Qt utilisé lors du choix du fichier
 * @return	Le nom de fichier avec extension. Si le nom proposé n'en n'a pas alors la première du filtre est ajoutée.
 */
static string completeFileName (const string& path, const string& filter)	// v 7.4.0
{
	File	file (path);
	if (0 != file.getExtension ( ).length ( ))
		return path;

	// Le fichier n'a pas d'extension, on rajoute la première du filtre 
	const string	ext	= getFirstExtension (filter);
	UTF8String	newPath (charset);
	newPath << path << ext;

	return newPath.utf8 ( );
}	// completeFileName


QtVTKPrintHelper::PrinterStatus QtVTKPrintHelper::printToFile (vtkRenderWindow& window, QWidget* top)
{
	QPrinter		printer;
	PrinterStatus	status;
	static QString	lastDir (Process::getCurrentDirectory ( ).c_str ( ));
	UTF8String	formats;
	formats << "JPG (*.jpg *.jpeg);; "
	        << "PNG (*.png);; "
	        << "TIF (*.tiff);; "
	        << "BMP (*.bmp);; "
	        << "PNM (*.pnm);; "
	        << "PS (*.ps);; "
	        << "EPS (*.eps);; "
	        << "PDF (*.pdf)";

	QFileDialog	dialog (top, "Imprimer dans un fichier", lastDir, UTF8TOQSTRING (formats));	
	dialog.setModal (true);
	dialog.setFileMode (QFileDialog::AnyFile);
	dialog.setAcceptMode (QFileDialog::AcceptSave);
	dialog.setOption (QFileDialog::DontUseNativeDialog);
	
	if (QDialog::Accepted == dialog.exec ( ))
	{
		// S'assurer que la boite de dialogue d'impression ait bien disparue.
		if (0 != top)
			top->raise ( );
		while (true == qApp->hasPendingEvents ( ))
			qApp->processEvents ( );
		QtAutoWaitingCursor	cursor (true);

		QStringList	fileList	= dialog.selectedFiles ( );
		if (1 != fileList.size ( ))
			throw Exception ("Erreur, la liste de fichiers d'impression ne contient pas qu'un fichier.");

//		string	fileName (fileList [0].toStdString ( ));
		string	fileName (completeFileName (fileList [0].toStdString ( ), dialog.selectedNameFilter ( ).toStdString ( )));	// v 7.4.0
		File	file (fileName);

		if (false == file.isWritable ( ))
		{
			cursor.hide ( );
			UTF8String	message (charset);
			message << "Impression impossible dans le fichier "
			        << "\n" << file.getFullFileName ( ) << " :" << "\n"
			        << " absence de droits en écriture.";
			QtMessageBox::displayErrorMessage (top, "Impression", message);
			return status;
		}	// if (false == file.isWritable ( ))
		printToFile (window, fileName, 0);
		status.completed	= true;
		status.destination	= PrinterStatus::FILE;
		status.deviceName	= fileName;
		lastDir				= file.getPath ( ).getFullFileName ( ).c_str ( );
	}	// if (QDialog::Accepted == dialog.exec ( ))

	return status;
}	// QtVTKPrintHelper::printToFile


QtVTKPrintHelper::PrinterStatus QtVTKPrintHelper::printTo4kFile (vtkRenderWindow& window, QWidget* top)
{
	QPrinter		printer;
	PrinterStatus	status;
	static QString	lastDir (Process::getCurrentDirectory ( ).c_str ( ));
	UTF8String	formats;
	formats << "JPG (*.jpg *.jpeg);; "
	        << "PNG (*.png);; "
	        << "TIF (*.tiff);; "
	        << "BMP (*.bmp);; "
	        << "PNM (*.pnm)";

	QFileDialog	dialog (top, "Imprimer dans un fichier 4k (3840x2160)", lastDir, UTF8TOQSTRING (formats));	
	dialog.setModal (true);
	dialog.setFileMode (QFileDialog::AnyFile);
	dialog.setAcceptMode (QFileDialog::AcceptSave);
	dialog.setOption (QFileDialog::DontUseNativeDialog);
	
	if (QDialog::Accepted == dialog.exec ( ))
	{
		// S'assurer que la boite de dialogue d'impression ait bien disparue.
		if (0 != top)
			top->raise ( );
		while (true == qApp->hasPendingEvents ( ))
			qApp->processEvents ( );
		QtAutoWaitingCursor	cursor (true);

		QStringList	fileList	= dialog.selectedFiles ( );
		if (1 != fileList.size ( ))
			throw Exception ("Erreur, la liste de fichiers d'impression ne contient pas qu'un fichier.");

		const string	fileName (completeFileName (fileList [0].toStdString ( ), dialog.selectedNameFilter ( ).toStdString ( )));
		File	file (fileName);
		if (false == file.isWritable ( ))
		{
			cursor.hide ( );
			UTF8String	message (charset);
			message << "Impression impossible dans le fichier "
			        << "\n" << file.getFullFileName ( ) << " :" << "\n"
			        << " absence de droits en écriture.";
			QtMessageBox::displayErrorMessage (top, "Impression", message);
			return status;
		}	// if (false == file.isWritable ( ))
		printToFile (window, fileName, 3840, 2160);
		status.completed	= true;
		status.destination	= PrinterStatus::FILE;
		status.deviceName	= fileName;
		lastDir			= file.getPath ( ).getFullFileName ( ).c_str ( );
	}	// if (QDialog::Accepted == dialog.exec ( ))

	return status;
}	// QtVTKPrintHelper::printTo4kFile


void QtVTKPrintHelper::printToFile (vtkRenderWindow& window, const string& fileName, size_t dpi)
{
	string::size_type	dotPos	= fileName.rfind ('.');
	if ((string::npos == dotPos) || (string::npos == fileName.length ( ) - 1))
	{
		UTF8String	error (charset);
		error << "Impossibilité de déterminer le format du fichier à "
		      << "partir de son extension.\n"
		      << "Extensions possibles : "
		      << ".ps .eps .jpg .jpeg .bmp .png .pnm .tiff .pdf";
		throw Exception (error);
	}	// if ((string::npos == dotPos) || ...
	string	ext	= fileName.substr (dotPos + 1);

	// Impression vectorielle : on passe par un exporter.
	// ATTENTION : pas de transparence.
	if ((0 == strcasecmp (ext.c_str ( ), "eps")) ||
	    (0 == strcasecmp (ext.c_str ( ), "pdf")))
	{
		vtkGL2PSExporter*	exporter	= vtkGL2PSExporter::New ( );
//		vtkGL2PSExporter*	exporter	= vtkOpenGLGL2PSExporter::New ( );	// OK mais mauvais pattern
		exporter->SetRenderWindow (&window);
		if (0 == strcasecmp (ext.c_str ( ), "pdf"))
			exporter->SetFileFormatToPDF ( );
// exporter->SetFileFormatToPS ( ) : l'extension du fichier devient .ps et non
// pas .eps ...
//else exporter->SetFileFormatToPS ( );
//exporter->CompressOff ( );
		string	filePrefix	= fileName.substr (0, dotPos);
		exporter->SetFilePrefix (filePrefix.c_str ( ));
//		exporter->SetGlobalLineWidthFactor (10);	// Sans effet
//		exporter->LandscapeOn ( );
//		exporter->Write3DPropsAsRasterImageOn ( );
		exporter->SetSortToBSP ( );
		exporter->Write ( );
		exporter->Delete ( );
		QApplication::beep ( );
		return;
	}	// EPS ou PDF
	
	// V 7.14.7 : on s'assure que la copie du buffer graphique pourra fonctionner, à savoir qu'on fonctionne sur une
	// machine aveccarte graphique, ou, à défaut, pas sur une machine virtuelle :
	static bool	first	= true;
	static bool	useHard	= true;
	if (true == first)
	{
		const string	glVendor ((const char*)glGetString (GL_VENDOR));		// NVIDIA, ATI, Intel, ...
		const string	glRenderer ((const char*)glGetString (GL_RENDERER));	// Quattro, Mesa DRI Intel, ...
		if ((true == glVendor.empty ( )) || (true == glRenderer.empty ( )))
			useHard	= false;
		if (NULL != strcasestr (glRenderer.c_str ( ), "llvmpipe"))
			useHard	= false;
		if (false == useHard)	// Pas de hard détecté, par sécurité on fait une impression offscreen :
			cout << "Absence de carte graphique détectée, les impressions dans un fichier seront effectuées en rendu logiciel par sécurité." << endl;
		else
		{
			if (true == MachineData::isVirtualMachine ( ))
			{
				useHard	= false;
				cout << "Machine virtuelle détectée, les impressions dans un fichier seront effectuées en rendu logiciel par sécurité." << endl;
			}	// if (true == MachineData::isVirtualMachine ( ))
		}	// else if (false == useHard)
		first	= false;
	}	// if (true == first)

	if (false == useHard)
	{
		printToFile (window, fileName, window.GetSize ( )[0], window.GetSize ( )[1]);
		return;
	}	// if (false == useHard)
	
	// Impression raster : on passe par un image.
	vtkWindowToImageFilter*	windowToImageFilter	= vtkWindowToImageFilter::New ( );
	vtkImageWriter*		writer			= createWriter (fileName);

	windowToImageFilter->SetInput (&window);

	// SetMagnification : augmente la taille de l'image du facteur transmis en 
	// argument. Effet desagreable : augmente la taille de la render window.
	// Objectif : avoir une resolution superieure ou egale a dpi en A4.
	// A4 : 297 * 210 mm <=> 11.7 * 8.26  en paysage
	// Autre bogue connu : perte des affichages 2D.
	if (0 != dpi)
	{
		int	screenDpi	= window.GetDPI ( ) < 1 ? 1 : window.GetDPI ( );
//		int	width			= (int)(11.7 * dpi);
//		int	height			= (int)(8.26 * dpi);
		int	magnification	= dpi / screenDpi;
		if (1 < magnification)
		{
#ifdef VTK_7
			// v 6.2.0 : en cas de multicouches (ex : trièdres XYZ) on écrête
			// en cas de grossissement > 5 car sinon lesdidts trièdres
			// apparaissent 4 fois à l'impression.
			if (window.GetNumberOfLayers ( ) > 1)
				magnification	= magnification < 5 ? magnification : 5;
			windowToImageFilter->SetMagnification (magnification);
#else	// => VTK 8 and newer
			windowToImageFilter->SetScale (magnification);
#endif	// VTK_7
		}	// if (1 < magnification)
	}	// if (0 != dpi)
	windowToImageFilter->Update ( );
#ifndef VTK_5
	writer->SetInputConnection (windowToImageFilter->GetOutputPort ( ));
#else	// VTK_5
	writer->SetInput (windowToImageFilter->GetOutput ( ));
#endif	// VTK_5
	writer->SetFileName (fileName.c_str ( ));
#ifdef VTK_5
// On force le raffraichissement total de la fenetre. En effet, si la fenetre
// VTK est masquee par une autre fenetre X11 la zone masquee n'est pas
// forcement raffraichie, et la zone correspondante de l'image est alors noire.
window.MappedOff ( );
window.MappedOn ( );
#endif	// VTK_5
	window.Render ( );
#ifdef VTK_5
	windowToImageFilter->Modified ( );
	windowToImageFilter->Update ( );
#endif	// VTK_5
	writer->Write ( );
/*
	windowToImageFilter->SetInput (&window);
	windowToImageFilter->Update ( );
	writer->SetInput (windowToImageFilter->GetOutput ( ));
	writer->SetFileName (fileName.c_str ( ));
	window.Render ( );
	writer->Write ( );
*/

	writer->Delete ( );
	windowToImageFilter->Delete ( );

	// On avertit d'un beep que l'impression est finie.
	QApplication::beep ( );
}	// QtVTKPrintHelper::printToFile


void QtVTKPrintHelper::printToFile (vtkRenderWindow& window, const string& fileName, size_t width, size_t height)	// v 7.4.0
{
	string::size_type	dotPos	= fileName.rfind ('.');
	if ((string::npos == dotPos) || (string::npos == fileName.length ( ) - 1))
	{
		UTF8String	error (charset);
		error << "Impossibilité de déterminer le format du fichier à "
		      << "partir de son extension.\nExtensions possibles : "
		      << ".jpg .jpeg .bmp .png .pnm .tiff";
		throw Exception (error);
	}	// if ((string::npos == dotPos) || ...
	string	ext	= fileName.substr (dotPos + 1);

	// Impression raster : on passe par un image.
	vtkRenderWindow*	rw			= vtkRenderWindow::New ( );
	rw->SetSize (width, height);
	rw->OffScreenRenderingOn ( );
	rw->SetNumberOfLayers (window.GetNumberOfLayers ( ));
	vtkRendererCollection*	rs	= window.GetRenderers ( );
	rs->InitTraversal ( );
	vector<vtkRenderer*>	renderers;
	if (0 != rs)
	{
		vtkRenderer*	renderer	= rs->GetFirstRenderer ( );
		while (0 != renderer)
		{
			renderers.push_back (renderer);
			renderer	= rs->GetNextItem ( );
		}	// while (0 != renderer)
	}	// if (0 != rs)
	for (vector<vtkRenderer*>::const_iterator itr = renderers.begin ( ); renderers.end ( ) != itr; itr++)
	{
		window.RemoveRenderer (*itr);
		rw->AddRenderer (*itr);
	}	// for (vector<vtkRenderer*>::const_iterator itr = renderers.begin ( ); ...
	vtkWindowToImageFilter*	windowToImageFilter	= vtkWindowToImageFilter::New ( );
	vtkImageWriter*		writer			= createWriter (fileName);

	windowToImageFilter->SetInput (rw);
	windowToImageFilter->Update ( );
#ifndef VTK_5
	writer->SetInputConnection (windowToImageFilter->GetOutputPort ( ));
#else	// VTK_5
	writer->SetInput (windowToImageFilter->GetOutput ( ));
#endif	// VTK_5
	writer->SetFileName (fileName.c_str ( ));
#ifdef VTK_5
// On force le raffraichissement total de la fenetre. En effet, si la fenetre
// VTK est masquee par une autre fenetre X11 la zone masquee n'est pas
// forcement raffraichie, et la zone correspondante de l'image est alors noire.
rw->MappedOff ( );
rw->MappedOn ( );
#endif	// VTK_5
	rw->Render ( );
#ifdef VTK_5
	windowToImageFilter->Modified ( );
	windowToImageFilter->Update ( );
#endif	// VTK_5
	writer->Write ( );
	writer->Delete ( );
	windowToImageFilter->Delete ( );

	for (vector<vtkRenderer*>::const_iterator itr = renderers.begin ( ); renderers.end ( ) != itr; itr++)
	{
		rw->RemoveRenderer (*itr);
		window.AddRenderer (*itr);
	}	// for (vector<vtkRenderer*>::const_iterator itr = renderers.begin ( ); ...
	rw->Delete ( );		rw	= 0;

	// On avertit d'un beep que l'impression est finie.
	QApplication::beep ( );
}	// QtVTKPrintHelper::printToFile


void QtVTKPrintHelper::printToPrinter (const string& fileName, const string& printerName,
                                       PrintCommand::CommandOptions& options)
{
	options.eraseFile		= true;
	PrintCommand*	command = new PrintCommand (printerName, fileName, options);
	command->execute ( );
}	// QtVTKPrintHelper::printToFile


vtkImageWriter* QtVTKPrintHelper::createWriter (const string& fileName)
{
	string::size_type	dotPos	= fileName.rfind ('.');
	if ((string::npos == dotPos) || (string::npos == fileName.length ( ) - 1))
	{
		UTF8String	error (charset);
		error << "Impossibilité de déterminer le format du fichier à "
		      << "partir de son extension.\n"
		      << "Extensions possibles : "
		      << ".ps .eps .jpg .jpeg .bmp .png .pnm .tiff";
		throw Exception (error);
	}	// if ((string::npos == dotPos) || ...
	string	ext	= fileName.substr (dotPos + 1);

	vtkImageWriter*		writer	= 0;
	if ((0 == strcasecmp (ext.c_str ( ), "ps")) ||
	    (0 == strcasecmp (ext.c_str ( ), "eps")))
		writer	= vtkPostScriptWriter::New ( );
	else if ((0 == strcasecmp (ext.c_str ( ), "jpg")) ||
	         (0 == strcasecmp (ext.c_str ( ), "jpeg")))
		writer	= vtkJPEGWriter::New ( );
	else if (0 == strcasecmp (ext.c_str ( ), "bmp"))
		writer	= vtkBMPWriter::New ( );
	else if (0 == strcasecmp (ext.c_str ( ), "png"))
		writer	= vtkPNGWriter::New ( );
	else if (0 == strcasecmp (ext.c_str ( ), "pnm"))
		writer	= vtkPNMWriter::New ( );
	else if (0 == strcasecmp (ext.c_str ( ), "tiff"))
		writer	= vtkTIFFWriter::New ( );

	if (0 == writer)
	{
		UTF8String	message (charset);
		message << "Impression impossible. Extension de fichier \"" << ext << "\"non reconnue "
		        << "pour le fichier " << fileName << "\n";
		message << "Extension possibles : .ps .eps .jpg .jpeg .bmp .png .pnm .tiff";
		throw Exception (message);
	}	// if (0 == writer)

	return writer;
}	// QtVTKPrintHelper::createWriter


Version QtVTKPrintHelper::getVersion ( )
{
	static const Version	version (QT_VTK_VERSION);

	return version;
}   // QtVTKPrintHelper::getVersion

