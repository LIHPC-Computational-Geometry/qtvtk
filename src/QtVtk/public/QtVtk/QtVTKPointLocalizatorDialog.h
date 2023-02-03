#ifndef QT_VTK_POINT_LOCALIZATOR_DIALOG_H
#define QT_VTK_POINT_LOCALIZATOR_DIALOG_H

#include <TkUtil/util_config.h>
#include <QtVtk/QtVTKPointLocalizatorPanel.h>

#include <QDialog>



/**
 * Boite de dialogue Qt permettant de localiser un point, matérialisé par une
 * croix, dans une scène <I>VTK</I>.
 */
class QtVTKPointLocalizatorDialog : public QDialog 
{
	public :

	/** Constructeur
	 * @param		widget parent. Ne doit pas être nul.
	 * @param		titre du panneau
	 * @param		afficheur <I>VTK</I>
	 * @param		titre de la première composante.
	 * @param		titre de la seconde composante.
	 * @param		titre de la troisième composante.
	 * @param		valeur initiale de la première composante.
	 * @param		valeur initiale de la seconde composante.
	 * @param		valeur initiale de la troisième composante.
	 * @param		Longueur du demi-segment selon la première composante.
	 * @param		Longueur du demi-segment selon la seconde composante.
	 * @param		Longueur du demi-segment selon la troisième composante.
	 */
	QtVTKPointLocalizatorDialog (QWidget* parent, const IN_STD string& title, 
            vtkRenderer& renderer, const IN_STD string& xTitle,
			const IN_STD string& yTitle, const IN_STD string& zTitle, 
            double x, double y, double z, double dx, double dy, double dz);

	/** Destructeur. */
	virtual ~QtVTKPointLocalizatorDialog ( );

	/**
	 * @param			Nouvelle première composante de la donnée à représenter.
	 */
	virtual void setX (double x);

	/**
	 * @return			la première composante de la donnée représentée.
	 * @throw			Exception si la valeur est incorrecte. */
	virtual double getX ( ) const;

	/**
	 * @param			Nouvelle seconde composante de la donnée à représenter.
	 */
	virtual void setY (double y);

	/**
	* @return			la seconde composante de la donnée représentée.
	*/
	virtual double getY ( ) const;

	/**
	 * @param			Nouvelle troisième composante de la donnée à représenter.
	 */
	virtual void setZ (double z);

	/**
	 * @return			la troisième composante de la donnée représentée. 
	 */
	virtual double getZ ( ) const;

	/**
	 * @return			le panneau utilisé pour définir le point.
	 */
	virtual const QtVTKPointLocalizatorPanel& getLocalizatorPanel ( ) const;
	virtual QtVTKPointLocalizatorPanel& getLocalizatorPanel ( );


	protected :

	/**
	 * N'affiche plus le localisateur.
	 */
	virtual void hideEvent (QHideEvent* event);

	/**
	 * Affiche le localisateur.
	 */
	virtual void showEvent (QShowEvent* event);


	private :

	/** Constructeur de copie. Interdit. */
	QtVTKPointLocalizatorDialog (const QtVTKPointLocalizatorDialog&);

	/** Opérateur de copie. Interdit. */
	QtVTKPointLocalizatorDialog& operator = (const QtVTKPointLocalizatorDialog&);

	/**
	 * Le panneau utilisé pour définir le point.
	 */
	QtVTKPointLocalizatorPanel		*_localizatorPanel;
};	// class QtVTKPointLocalizatorDialog



#endif	// QT_VTK_POINT_LOCALIZATOR_DIALOG_H
