#ifndef QT_VTK_POINT_LOCALIZATOR_PANEL_H
#define QT_VTK_POINT_LOCALIZATOR_PANEL_H

#include <TkUtil/util_config.h>
#include <QtUtil/QtDoubleTextField.h>
#include <VtkContrib/vtkECMAxesActor.h>

#include <QCheckBox>
#include <QSlider>
#include <vtkRenderer.h>

#include <string>


/**
 * Panneau Qt permettant de localiser un point, matérialisé par une croix,
 * dans une scène <I>VTK</I>.
 */
class QtVTKPointLocalizatorPanel : public QWidget 
{
	Q_OBJECT

	public :

	/** Constructeur. Affiche le localisateur (croix) au point transmis en
	 * argument.
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
	 * @see			setDisplayed
	 */
	QtVTKPointLocalizatorPanel (QWidget* parent, const IN_STD string& title, 
            vtkRenderer& renderer, const IN_STD string& xTitle,
			const IN_STD string& yTitle, const IN_STD string& zTitle, 
            double x, double y, double z, double dx, double dy, double dz);

	/** Destructeur.
	 * @see			setDisplayed
	 */
	virtual ~QtVTKPointLocalizatorPanel ( );

	/**
	 * @param		<I>true</I> si le localisateur doit etre affiché,
	 * 				<I>false</I> dans le cas contraire.
	 */
	virtual void setDisplayed (bool display);

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
	 * @return			le widget utilisé pour la première composante.
	 */
	virtual const QtDoubleTextField& getXTextField ( ) const;
	virtual QtDoubleTextField& getXTextField ( );

	/**
	 * @return			le widget utilisé pour la seconde composante.
	 */
	virtual const QtDoubleTextField& getYTextField ( ) const;
	virtual QtDoubleTextField& getYTextField ( );

	/**
	 * @return			le widget utilisé pour la troisième composante.
	 */
	virtual const QtDoubleTextField& getZTextField ( ) const;
	virtual QtDoubleTextField& getZTextField ( );

	/**
	 * @return			<I>true</I> si la vue doit être automatiquement centrée
	 *					sur le point.
	 */
	virtual bool centerOnPoint ( ) const;

	/**
	 * @return			La représentation <I>VTK</I> du point.
	 */
	virtual vtkECMAxesActor& getPointActor ( );

	/**
	 * @return			L'afficheur <I>VTK</I> utilisé
	 */
	virtual vtkRenderer& getRenderer ( );


	/** Nombre de caractères par défaut des champs numériques. Cette
	 * valeur est initialement de 12. */
	static int			numberOfCharacters;

	/** <P>Service visant à déterminer au mieux un domaine de zoom géré par
	 * slider pour le localisateur ou tout autre objet 3D de ce type
	 * (ex : tièdre). L'objet est centré au milieu d'une boite englobante (ex :
	 * surface de maillage), et on souhaite initialement voir ses bras en sortir
	 * un peu.</P>
	 * <P>Les valeurs par défaut correspondent à un slider allant de 1 à 100,
	 * pour un zoom allant de 20% à 500%, en s'ajustant par rapport à la
	 * diagonale d'une boite englobante (zoomComul).</P>
	 * <P>Le zoomComul permet de s'adapter à des cas particuliers, celui
	 * proposé par défaut vaut sqrt (3), diagonale de la boite englobante.</P>
	 * @return		Valeur initiale proposée pour le slider.
	 * @param		Valeurs min et max du slider.
	 * @param		Valeurs min et max du zoom (domaine de zoom par rapport à 1
	 *				qui correspond à la taille de la diagonale de la boite
	 *				englobante).
	 * @see			zoomValue
	 */
	static int defaultSliderValue (
			int minValue = 1, int maxValue = 100,
			double zoomMin = 0.2, double zoomMax = 5., double comul = 1.732);

	/**
	 * La fonction de transfert slider/boite englobane -> zoom à appliquer à un
	 * objet graphique centré dans une boite englobante.
	 * Les boites englobantes sont renseignées comme suit : (xminm xmax, ymin,
	 * ymax, zmin, zmax).
	 * @param		Valeur du slider.
	 * @param		Boite englobante de l'objet graphique affiché (ex :
	 * 				localisateur, trièdre, ...).
	 * @param		Boite englobante de la zone d'affichage (ex : surface de
	 * 				maillage).
	 * @param		Valeurs min et max du slider.
	 * @param		Valeurs min et max du zoom (domaine de zoom par rapport à 1
	 *				qui correspond à la taille de la diagonale de la boite
	 *				englobante).
	 * @see			defaultSliderValue
	 */
	static double zoomValue (int sliderValue, double objectBounds [6],
				double targetBounds [6], int minValue = 1, int maxValue = 100,
				double zoomMin = 0.2, double zoomMax = 5.);

	/** La fonction de transfert min <-> max du zoom de la croix. Par défaut
	 * 20% à 500% de la diagonale de la boite donnée (définie par la longueur 
	 * des demi-axes).
	 */
	static double	zoomMin, zoomMax, zoomComul;



	protected slots :

	/**
	 * Appelé lorsqu'une coordonnée du point est modifiée avec demande
	 * d'actualisation (retour chariot). Actualise la représentation <I>VTK</I>.
	 */
	virtual void updatePointActorCallback ( );

	/**
	 * Appeé lorsque l'utilisateur clique sur la case à cocher "Centrer la vue
	 * sur le point". Actualise si nécessaire la représentation <I>VTK</I>.
	 */
	virtual void centerViewCallback ( );

	/**
	 * Ajuste la taille du localisateur (croix).
	 */
	virtual void crossZoomCallback ( );


	private :

	/** Constructeur de copie. Interdit. */
	QtVTKPointLocalizatorPanel (const QtVTKPointLocalizatorPanel&);

	/** Opérateur de copie. Interdit. */
	QtVTKPointLocalizatorPanel& operator = (const QtVTKPointLocalizatorPanel&);

	/**
	 * Les tailles d'une demi-segment de croix.
	 */
	double					_dx, _dy, _dz;

	/**
	 * Les champs de saisie des composantes de la donnée représentée.
	 */
	QtDoubleTextField		*_xTextField, *_yTextField, *_zTextField;

	/**
	 * Centrer la vue sur le point ?
	 */
	QCheckBox*				_centerViewCheckBox;

	/**
 	 * Zoom sur le localisateur.
 	 */
	QSlider*				_zoomSlider;

	/**
	 * L'afficheur VTK.
	 */
	vtkRenderer*			_renderer;

	/**
	 * La matérialisation du point dans la scène VTK.
	 * */
	vtkECMAxesActor*		_pointActor;
};	// class QtVTKPointLocalizatorPanel



#endif	// QT_VTK_POINT_LOCALIZATOR_PANEL_H
