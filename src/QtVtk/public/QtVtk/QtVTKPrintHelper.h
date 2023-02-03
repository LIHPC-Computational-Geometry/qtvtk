#ifndef QT_VTK_PRINT_HELPER
#define QT_VTK_PRINT_HELPER

#include <TkUtil/Version.h>
#include <TkUtil/PrintCommand.h>

#include <string>

#include <vtkRenderWindow.h>
#include <vtkImageWriter.h>
#include <QWidget>


/**
 * <P>Classe Qt permettant les impressions (imprimante/fichier) d'une 
 * fenêtre VTK. </P>
 * <P> Formats d'impressions supportés : ps, eps, jpg, bmp, pnm, png, tiff,
 * pdf.</P>
 *
 * @warning		La boite de dialogue sera a refaire afin d'imposer des extensions de fichiers et de ne proposer que les options utilisables.
 * @warning		Les impressions vectorielles (eps, pdf) ne sont pas parfaites (les bras ne sont pas toujours bien affichés), et ne 
 *				peuvent pas gèrer pas la transparence.
 */
class QtVTKPrintHelper
{
	public :

	/** Décrit le résultat de l'impression.
	 */
	struct PrinterStatus
	{
		/** Constructeurs et operateur = : RAS. */
		PrinterStatus ( );
		PrinterStatus (const PrinterStatus& copied);
		PrinterStatus& operator = (const PrinterStatus& copied);

		/** L'impression a-t-elle été réalisée (true) ou annulée
		 * (false). */
		bool			completed;

		/** La destination de l'impression. */
		enum DESTINATION {PRINTER, FILE};
		DESTINATION		destination;

		/** Le nom du fichier ou de l'imprimante. */
		IN_STD string	deviceName;
	};	// struct PrinterStatus


	/**
	 * Demande à l'utilisateur les paramètres d'impression (nom du
	 * fichier ou de l'imprimante, nombre de copies, ...), et effectue
	 * l'impression.
	 * En cas d'une impression dirigée vers une imprimante la résolution
	 * d'impression est portée à 720 dpi. Si l'impression est dirigée
	 * dans un fichier la résolution utilisée est celle de l'écran.
	 * @param		fenêtre VTK dont le contenu est à imprimer.
	 * @param		fenêtre Qt de plus haut niveau à réafficher avant
	 *				impression.
	 * @return		Le résultat de l'impression.
	 * @exception	Une exception de type Util::Exception est levée en cas 
	 *				d'erreur lors de l'impression.
	 * @warning		En cas d'impression vers une imprimante, perte possible des
	 *				affichages 2D.
	 */
	static PrinterStatus print (vtkRenderWindow& window, QWidget* top);

	/**
	 * Demande à l'utilisateur les paramètres d'impression dans un fichier
	 * fichier et effectue l'impression.
	 * La résolution utilisée est celle de l'écran.
	 * @param		fenêtre VTK dont le contenu est à imprimer.
	 * @param		fenêtre Qt de plus haut niveau à réafficher avant impression.
	 * @return		Le résultat de l'impression.
	 * @exception	Une exception de type Util::Exception est levée en cas d'erreur lors de l'impression.
	 */
	static PrinterStatus printToFile (vtkRenderWindow& window, QWidget* top);

	/**
	 * Demande à l'utilisateur les paramètres d'impression 4k (3840x2160) dans un fichier
	 * fichier et effectue l'impression.
	 * @param		fenêtre VTK dont le contenu est à imprimer.
	 * @param		fenêtre Qt de plus haut niveau à réafficher avant impression.
	 * @return		Le résultat de l'impression.
	 * @exception	Une exception de type Util::Exception est levée en cas d'erreur lors de l'impression.
	 */
	static PrinterStatus printTo4kFile (vtkRenderWindow& window, QWidget* top);	// v 7.4.0

	/**
	 * @return		La version de cette bibliothèque.
	 */
	static IN_UTIL Version getVersion ( );


	private :

	/** Imprime dans le fichier dont le nom est donné en argument. Tente de détecter si la machine courrante peut faire l'impression
	 * par un moyen hardware, ou si l'impression doit être software dans une vtkRenderWindow offscreen (cas d'une machine virtuelle).
	 * @param		fenêtre VTK dont le contenu est à imprimer.
	 * @param		nom du fichier d'impression.
	 * @param		résolution d'impression demandée, en dpi. Si nulle, la résolution de la fenêtre VTK est conservée.
	 * @warning		En cas de résolution non nulle, perte possible des affichages 2D.
	 */
	static void printToFile (vtkRenderWindow& window,  const IN_STD string& fileName, size_t dpi);

	/** Imprime dans le fichier RAW (non vectoriel) dont le nom est donné en argument.
	 * @param		fenêtre VTK dont le contenu est à imprimer.
	 * @param		nom du fichier d'impression.
	 * @param		largeur et hauteur de l'image imprimée.
	 * @warning		En cas de résolution non nulle, perte possible des affichages 2D.
	 */
	static void printToFile (vtkRenderWindow& window,  const IN_STD string& fileName, size_t width, size_t height);	// v 7.4.0

	/** Dirige vers l'imprimante le fichier postscript dont le nom est donné
	 * en argument.
	 * @param		nom du fichier d'impression.
	 * @param		nom de l'imprimante.
	 * @param		options d'impression.
	 */
	static void printToPrinter (const IN_STD string& fileName,  const IN_STD string& printerName, IN_UTIL PrintCommand::CommandOptions& options);

	/**
	 * Créé un ImageWriter au format spécifié par l'extension du
	 * fichier dont le nom est donné en argument.
	 * @param		nom du fichier dont on souhaite créé un ImageWriter.
	 * @exception	Si l'extension du nom du fichier ne correspond à aucun
	 *				format connu.
	 */
	static vtkImageWriter* createWriter (const IN_STD string& fileName);
};	// QtVTKPrintHelper

#endif	// QT_VTK_PRINT_HELPER
