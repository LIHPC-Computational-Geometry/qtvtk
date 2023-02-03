#ifndef QT_VTK__H
#define QT_VTK__H

#include <TkUtil/Version.h>

#include <vtkHardwareSelector.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>


/*!
 * \mainpage    Page principale de la bibliothèque QtVtk.
 *
 * \section presentation    Présentation
 * La bibliothèque <I>QtVtk</I> propose un service d'impression d'une fenêtre
 * <I>VTK</I> dans une IHM <I>Qt</I>, et un éditeur de fontes 2D en environnement <I>VTK</I>
 * \section depend      Dépendances
 * Cette bibliothèque utilise les bibliothèques TkUtil et QtUtil.
 * Les directives de compilation et d'édition des liens de ces bibliothèques s'appliquent également à celle-ci.
 *
 */


/**
 * <P>Classe d'initialisation des services.
 * </P>
 */
class QtVtk
{
	public :

	/**
	 * Initialisation des services..
	 */
	static void initialize ( );

	/**
	 * Arrêts des services.
	 * @see			releaseOffScreenWindow
	 */
	static void finalize ( );

	/**
	 * @return		La version de cette bibliothèque.
	 */
	static IN_UTIL Version getVersion ( );

	/**
	 * @return		Un sélecteur <I>Open GL VTK</I>.
	 * @see			getHardwareSelectionWindow
	 */
	static vtkHardwareSelector& getHardwareSelector ( );

	/**
	 * @return		Une fenêtre de sélection destinée à fonctionner avec le sélecteur <I>Open GL VTK</I>.
	 * @see			getHardwareSelector
	 * @see			getHardwareSelectionRenderer
	 */
	static vtkRenderWindow& getHardwareSelectionWindow ( );

	/**
	 * @return		Le gestionnaire de rendu de la fenêtre de sélection destinée à fonctionner avec le sélecteur <I>Open GL VTK</I>.
	 * @see			getHardwareSelectionWindow
	 */
	static vtkRenderer& getHardwareSelectionRenderer ( );


	private :

	/**
	 * Constructeurs et destructeur : interdits.
	 */
	QtVtk ( );
	QtVtk (const QtVtk&);
	QtVtk& operator = (const QtVtk&);
	~QtVtk ( );
};	// QtVtk

#endif	// QT_VTK__H
