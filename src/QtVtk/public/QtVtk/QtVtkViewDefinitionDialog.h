#ifndef QT_VTK_VIEW_DEFINITION_DIALOG_H
#define QT_VTK_VIEW_DEFINITION_DIALOG_H

#include "QtVtk/QtVtkViewDefinitionPanel.h"
#include <QtUtil/QtDlgClosurePanel.h>

#include <QDialog> 

#include <string>


/**
 * Boite de dialogue Qt permettant la saisie des paramètres de définition d'un point de vue.
 * Cette boite de dialogue peut piloter, optionnellement, une caméra VTK. La caméra est mise à jour avec les derniers paramètres saisis lorsque 
 * l'utilisateur effectue un retour chariot dans un champ numérique.
 */
class QtVtkViewDefinitionDialog : public QDialog 
{
	Q_OBJECT

	public :

	/** Constructeur.
	 * @param		widget parent. Ne doit pas être nul.
	 * @param		titre de la boite de dialogue.
	 * @param		caractère modal de la boite de dialogue.
	 * @param		Eventuel nom du point de vue
	 * @param		Eventuel commentaire associé au point de vue
	 * @param		position initiale (x, y, z) de la caméra.
	 * @param		focale initiale (x, y, z) de la caméra.
	 * @param		direction vers le haut de la caméra
	 * @param		roulis (angle de la caméra par rapport à sa direction (axe position-focale)). En degrés.
	 * @param		Renderer utilisant la caméra à piloter si non nul (GetActiveCamera sur le renderer est utilisé).
	 * @param		URL de l'aide
	 * @param		Balise de l'aide
	 */
	QtVtkViewDefinitionDialog (QWidget* parent, const IN_STD string& title, bool modal, const TkUtil::UTF8String& name, const TkUtil::UTF8String& comment, 
	                           double position [3], double focal [3], double viewUp [3], double roll, vtkRenderer* renderer, 
	                           const IN_STD string& helpURL ="", const IN_STD string& helpTag = "");

	/** Destructeur. */
	virtual ~QtVtkViewDefinitionDialog ( );

	/** @return			l'éventuel nom du point de vue. */
	virtual TkUtil::UTF8String getName ( ) const;
	
	/** @return			l'éventuel commentaire associé au point de vue. */
	virtual TkUtil::UTF8String getComment ( ) const;

	/** @param			les coordonnées de la caméra, en retour. */
	virtual void getPosition (double coords [3]) const;

	/** @param			la position de la focale la caméra, en retour. */
	virtual void getFocalPoint (double coords [3]) const;
	
	/** @param			la direction vers le haut de la caméra, en retour. */
	virtual void getViewUp (double direction [3]) const;

	/** @return			le roulis de la caméra. */
	virtual double getRoll ( ) const;

	/** @return			le bouton "Appliquer" de la boite de dialogue. */
	virtual QPushButton* getApplyButton ( ) const;

	/** @return			le bouton "Annuler" de la boite de dialogue. */
	virtual QPushButton* getCancelButton ( ) const;


	public slots :

	/** Appelé quand l'utilisateur appuie sur le bouton "Appliquer".
	 */
	virtual void apply ( );

	/** Appelé quand l'utilisateur appuie sur le bouton "Fermer".
	 * Invoque Dialog::accept.
	 * Envoie le signal "applied" avec en argument cette instance. 
	 */
	virtual void close ( );

	/** Appelé quand l'utilisateur appuie sur le bouton "Annuler".
	 * Invoque Dialog::reject.
	 * Envoie le signal "canceled" avec en argument cette instance. 
	 */
	virtual void reject ( );


	signals :

	/** Envoyé lorsque le bouton "Appliquer" est cliqué.
	 * @param		l'instance siège de l'évènement.
	 */
	void applied (QtVtkViewDefinitionDialog* dialog);

	/** Envoyé lorsque le bouton "Annuler" est cliqué.
	 * @param		l'instance siège de l'évènement.
	 */
	void canceled (QtVtkViewDefinitionDialog* dialog);


	protected :


	private :

	/** Constructeur de copie. Interdit. */
	QtVtkViewDefinitionDialog (const QtVtkViewDefinitionDialog&);

	/** Opérateur de copie. Interdit. */
	QtVtkViewDefinitionDialog& operator = (const QtVtkViewDefinitionDialog&);

	/** Le panneau de paramétrage de l'opération d'extraction. */
	QtVtkViewDefinitionPanel*			_viewDefinitionPanel;

	/** Le panneau proposant "Appliquer" et "Annuler". */
	QtDlgClosurePanel*					_closurePanel;
};	// class QtVtkViewDefinitionDialog



#endif	// QT_VTK_VIEW_DEFINITION_DIALOG_H
