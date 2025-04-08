#ifndef QT_VTK_VIEW_DEFINITION_PANEL_H
#define QT_VTK_VIEW_DEFINITION_PANEL_H

#include <TkUtil/util_config.h>
#include <QtUtil/Qt3DDataPanel.h>
#include <QtUtil/QtTextField.h>
#include <TkUtil/UTF8String.h>

#include <QWidget> 

#include <vtkRenderer.h>

#include <string>


/**
 * Panneau Qt permettant de définir interactivement la vue d'un  théâtre. Cette vue est définie par l'emplacement et la focale de la caméra.
 * Ce panneau peut piloter, optionnellement, une caméra VTK. La caméra est mise à jour avec les derniers paramètres saisis lorsque 
 * l'utilisateur effectue un retour chariot dans un champ numérique.
 */
class QtVtkViewDefinitionPanel : public QWidget 
{
	Q_OBJECT

	public :

	/** Constructeur.
	 * @param		widget parent. Ne doit pas être nul.
	 * @param		titre de l'application (pour les messages d'erreur).
	 * @param		Eventuel nom du point de vue
	 * @param		Eventuel commentaire associé au point de vue
	 * @param		position initiale (x, y, z) de la caméra.
	 * @param		focale initiale (x, y, z) de la caméra.
	 * @param		direction vers le haut (dx, dy, dz) de la caméra.
	 * @param		roulis (angle de la caméra par rapport à sa direction (axe position-focale)). En degrés.
	 * @param		Renderer utilisant la caméra à piloter si non nul (GetActiveCamera sur le renderer est utilisé).
	 */
	QtVtkViewDefinitionPanel (QWidget* parent, const IN_STD string& appTitle, const TkUtil::UTF8String& name, const TkUtil::UTF8String& comment,
	                          double position [3], double focal [3], double viewUp [3], double roll, vtkRenderer* renderer);

	/** Destructeur. */
	virtual ~QtVtkViewDefinitionPanel ( );

	/** @return			l'éventuel nom du point de vue. */
	virtual TkUtil::UTF8String getName ( ) const;
	
	/** @return			l'éventuel commentaire associé au point de vue. */
	virtual TkUtil::UTF8String getComment ( ) const;

	/** @param			les coordonnées de la caméra, en retour. */
	virtual void getPosition (double coords [3]) const;

	/** @param			la position de la focale la caméra, en retour. */
	virtual void getFocalPoint (double coords [3]) const;
	
	/** @param			direction vers le haut (dx, dy, dz) de la caméra, en retour. */
	virtual void getViewUp (double direction [3]) const;

	/** @return			le roulis de la caméra. */
	virtual double getRoll ( ) const;

	/** Applique les modification à l'éventuelle caméra. Affecte true à _updated. */
	virtual void apply ( );

	/** Restaure les paramètres initiaux, aussi bien au niveau du panneau qu'au niveau de la caméra éventuellement pilotée.
	 */
	virtual void reset ( );

	/** @return		true si les paramètres ont été modifiés depuis le constructeur, sinon retourne false. */
	virtual bool isModified ( ) const
	{ return _modified; }


	protected :


	/**
	 * @param			Nouvelle coordonnées de la caméra.
	 */
	virtual void setPosition (double coords [3]);

	/**
	 * @param			Nouvelle coordonnées de la focale de la caméra.
	 */
	virtual void setFocalPoint (double coords [3]);
	
	/**
	 * @param			Nouvelle direction haute de la caméra.
	 */
	virtual void setViewUp (double direction [3]);

	/**
	 * @param			Nouveau roulis de la caméra.
	 */
	virtual void setRoll (double roll);

	/**
	 * Affecte le caractère "modifié" (depuis sa construction) à cette instance.
	 */
	virtual void setModified (bool modified)
	{ _modified	= modified; }

	/** @return		true si les paramètres n'ont pas été modifiés depuis la dernière mise à jour de la caméra, sinon retourne false. */
	virtual bool isUpdated ( ) const
	{ return _updated; }

	/**
	 * @param		Mise à jour du caractère "caméra mis à jour". 
	 */
	virtual void setUpdated (bool updated)
	{ _updated	= updated; }

	/**
	 * Met à jour les paramètres au regard des valeurs dans le panneau.
	 * Appelle setModified (true) et setUpdated (false) si l'un d'entre-eux est modifié.
	 */
	virtual void updateData ( );


	protected slots :

	/** Appelé quand l'utilisateur modifie un champ dans le  panneau.
	 * @param		Nouveau texte.
	 * Affecte setUpdated (false).
	 */
	virtual void viewFieldModifiedCallback (const QString&);

	/** Appelé quand l'utilisateur valide la saisie d'un champ dans le panneau. Appelle apply.
	 * Affecte true à _modified si un paramètre est changé.
	 */
	virtual void viewFieldValidatedCallback ( );


	private :

	/** Constructeur de copie. Interdit. */
	QtVtkViewDefinitionPanel (const QtVtkViewDefinitionPanel&);

	/** Opérateur de copie. Interdit. */
	QtVtkViewDefinitionPanel& operator = (const QtVtkViewDefinitionPanel&);

	/** Le titre de l'application. */
	IN_STD string						_appTitle;

	/** Le champ de définition du nom. */
	QtTextField*						_nameTextField;

	/** Le champ de définition du commentaire. */
	QtTextField*						_commentTextField;

	/** La position de la caméra. */
	Qt3DDataPanel*						_positionPanel;

	/** La position de la focale de la caméra. */
	Qt3DDataPanel*						_focalPanel;
	
	/** La direction vers le haut de la caméra. */
	Qt3DDataPanel*						_viewUpPanel;

	/** Le champ de définition du roulis. */
	QtTextField*						_rollTextField;

	/** Le renderer utilisant la caméra pilotée. */
	vtkRenderer*						_renderer;

	/** La position initiale. */
	double								_initialPosition [3];

	/** La position initiale de la focale. */
	double 								_initialFocal [3];
	
	/** La direction vers le haut initiale. */
	double 								_initialViewUp [3];

	/** La valeur initiale du roulis. */
	double								_initialRoll;

	/** La dernière position. */
	double								_position [3];

	/** La dernière position de la focale. */
	double 								_focal [3];

	/** La direction vers le haut. */
	double 								_viewUp [3];

	/** La dernère valeur du roulis. */
	double								_roll;

	/** Les paramètres ont ils été modifiés par l'utilisateur ? */
	bool								_modified;

	/** La vue est-elle à jour par rapport aux paramètres ? */
	bool								_updated;
};	// class QtVtkViewDefinitionPanel



#endif	// QT_VTK_VIEW_DEFINITION_PANEL_H
