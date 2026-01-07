#ifndef QT_3D_DATA_PANEL_H
#define QT_3D_DATA_PANEL_H

#include <TkUtil/util_config.h>
#include <QtUtil/QtGroupBox.h>
#include <QtUtil/QtDoubleTextField.h>

#include <string>


/**
 * Panneau Qt permettant la saisie ou l'affichage d'une donnée 3D 
 * (coordonnées, vecteur, ...).
 */
class Qt3DDataPanel : public QtGroupBox 
{
	public :

	/** Constructeur
	 * @param		widget parent. Ne doit pas être nul.
	 * @param		titre du panneau
	 * @param		true si le panneau est éditable, sinon false.
	 * @param		titre de la première composante.
	 * @param		titre de la seconde composante.
	 * @param		titre de la troisième composante.
	 * @param		valeur initiale de la première composante.
	 * @param		valeur minimale de la première composante.
	 * @param		valeur maximale de la première composante.
	 * @param		valeur initiale de la seconde composante.
	 * @param		valeur minimale de la seconde composante.
	 * @param		valeur maximale de la seconde composante.
	 * @param		valeur initiale de la troisième composante.
	 * @param		valeur minimale de la troisième composante.
	 * @param		valeur maximale de la troisième composante.
	 * @param		<I>true si la mise en forme doit être verticale,
	 * 				<I>false</I> si elle doit être horizontale.
	 */
	Qt3DDataPanel (QWidget* parent, const IN_STD string& title, bool editable,
	               const IN_STD string& xTitle, const IN_STD string& yTitle,
	               const IN_STD string& zTitle, 
	               double x, double xMin, double xMax,
	               double y, double yMin, double yMax,
	               double z, double zMin, double zMax,
	               bool verticalLayout);

	/** Destructeur. */
	virtual ~Qt3DDataPanel ( );

	/**
	 * @return			true si l'utilisateur a modifié une composante
	 *					de la donnée représentée.
	 */
	virtual bool isModified ( ) const;

	/**
	 * @param			Nouvelle première composante de la donnée à
	 *					représenter.
	 * @throw			Exception si la valeur est incorrecte. */
	virtual void setX (double x);

	/** @return			la première composante de la donnée 
	 *					représentée.
	 * @throw			Exception si la valeur est incorrecte. */
	virtual double getX ( ) const;

	/**
	 * @param			Nouvelle seconde composante de la donnée à
	 *					représenter.
	 * @throw			Exception si la valeur est incorrecte. */
	virtual void setY (double y);

	/** @return			la seconde composante de la donnée 
	 *					représentée.
	 * @throw			Exception si la valeur est incorrecte. */
	virtual double getY ( ) const;

	/**
	 * @param			Nouvelle troisième composante de la donnée à
	 *					représenter.
	 * @throw			Exception si la valeur est incorrecte. */
	virtual void setZ (double z);

	/** @return			la troisième composante de la donnée 
	 *					représentée. 
	 * @throw			Exception si la valeur est incorrecte. */
	virtual double getZ ( ) const;

	/**
	 * @return			le widget utilisé pour la première composante.
	 */
	QtDoubleTextField* getXTextField ( ) const
	{ return _xTextField; }

	/**
	 * @return			le widget utilisé pour la seconde composante.
	 */
	QtDoubleTextField* getYTextField ( ) const
	{ return _yTextField; }

	/**
	 * @return			le widget utilisé pour la troisième composante.
	 */
	QtDoubleTextField* getZTextField ( ) const
	{ return _zTextField; }


	/** Nombre de caractères par défaut des champs numériques. Cette
	 * valeur est initialement de 12. */
	static int			numberOfCharacters;


	protected :


	private :

	/** Constructeur de copie. Interdit. */
	Qt3DDataPanel (const Qt3DDataPanel&);

	/** Opérateur de copie. Interdit. */
	Qt3DDataPanel& operator = (const Qt3DDataPanel&);

	/** Les valeurs initiales des trois composanntes. */
	double			_initialX, _initialY, _initialZ;

	/** Les champs de saisie des composantes de la donnée représentée.
	 */
	QtDoubleTextField		*_xTextField, *_yTextField, *_zTextField;

	/** Le titre du panneau. */
	const IN_STD string	_title;
};	// class Qt3DDataPanel



#endif	// QT_3D_DATA_PANEL_H
