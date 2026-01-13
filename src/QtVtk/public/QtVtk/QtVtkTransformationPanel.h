/**
 * \file		QtVtkTransformationPanel.h
 * \author		Charles PIGNEROL, CEA/DAM/DCLC
 * \date		13/01/2026
 */
#ifndef QT_VTK_TRANSFORMATION_PANEL_H
#define QT_VTK_TRANSFORMATION_PANEL_H

#include <TkUtil/util_config.h>
#include <TkUtil/UTF8String.h>
#include <QtUtil/Qt3DDataPanel.h>
#include <QtUtil/QtTextField.h>
#include <QCheckBox>
#include <QLabel>

#include <vtkTransform.h>

#include <string>


/**
 * <P>
 * Panneau Qt permettant la saisie des paramètres d'une transformation linéaire VTK extrinsèque, c'est à dire appliquée
 * dans le repère global. Les axes du repère ne suivent pas les transformations.
 * </P>
 * <P>Cette transformation est caractérisée par un ensemble de rotations dans les 3 dimensions et d'une translation qui
 * peut être avantou après les rotations.
 * </P>
 * <P>Dans la version actuelle, les rotations sont effectuées dans l'ordre suivant : </P>
 * <OL>
 * 	<LI>Rotation autour de l'axe Oz (plan xOy),
 * 	<LI>Rotation autour de l'axe Ox (plan yOz),
 * 	<LI>Rotation autour de l'axe Oy (plan xOz),
 * </OL>
 * </P> 
  * @since	8.9.0
 */
class QtVtkExtrinsicTransformationPanel : public QWidget 
{
	Q_OBJECT

	public :


	/** Constructeur.
	 * @param		widget parent. Ne doit pas être nul.
	 * @param		titre de l'application (pour les messages d'erreur).
	 * @param		Angle de rotation dans le plan xOy
	 * @param		Angle de rotation dans le plan xOz
	 * @param		Angle de rotation dans le plan yOz
	 * @param		<I>true</I> si la translation précède les rotations, <I>false</I> si elle suit les rotations.
	 * @param		translation pré ou post-rotations
	 */
	QtVtkExtrinsicTransformationPanel (QWidget* parent, const IN_UTIL UTF8String& appTitle, double xoy = 0., double xoz = 0., double yoz = 0., bool translationFirst = true, double dx = 0., double dy = 0., double dz = 0.);


	/** Destructeur. */
	virtual ~QtVtkExtrinsicTransformationPanel ( );

	/**
	 * @param			l'angle de rotation dans le plan xOy, en degrés.
	 */
	virtual void setXOYAngle (double angle);

	/**
	 * @return			l'angle de rotation dans le plan xOy, en degrés.
	 */
	virtual double getXOYAngle ( ) const;

	/**
	 * @param			l'angle de rotation dans le plan xOz, en degrés.
	 */
	virtual void setXOZAngle (double angle);

	/**
	 * @return			l'angle de rotation dans le plan xOz, en degrés.
	 */
	virtual double getXOZAngle ( ) const;

	/**
	 * @param			l'angle de rotation dans le plan yOz, en degrés.
	 */
	virtual void setYOZAngle (double angle);

	/**
	 * @return			l'angle de rotation dans le plan yOz, en degrés.
	 */
	virtual double getYOZAngle ( ) const;

	/** 
	 * @param	<I>true</I> si la translation précède les rotations, <I>false</I> si elle suit les rotations.
	 */
	virtual void setTranslationFirst (bool first);
	
	/** 
	 * @return	<I>true</I> si la translation précède les rotations, <I>false</I> si elle suit les rotations.
	 */
	virtual bool isTranslationFirst ( ) const;

	/**
	 * @param			la translation à effectuer.
	 */
	virtual void setTranslation (double dx, double dy, double dz);

	/**
	 * @param			la translation à effectuer.
	 */
	virtual void getTranslation (double& dx, double& dy, double& dz) const;

	/**
	 * @return			une transformation correspondant au paramétrage en cours. A détruire par l'appelant.
	 */
	virtual vtkTransform* getTransformation ( ) const;
	
	/**
	 * @return			une description textuelle de la transformation.
	 */
	virtual IN_UTIL UTF8String getTransformationDescription ( ) const;


	signals :

	/**
	 * Signal émis lorsque l'utilisateur change le paramétrage de la transformation.
	 */
	 void transformationChanged ( );



	protected slots :


	/** Appelé lorsque l'utilisateur modifie un des champs de paramétrage de la transformation.
	 * Emet le signal <I>transformationChanged</I>.
	 */
	virtual void transformationModifiedCallback ( );


	private :

	/** Constructeur de copie. Interdit. */
	QtVtkExtrinsicTransformationPanel (const QtVtkExtrinsicTransformationPanel&);

	/** Opérateur de copie. Interdit. */
	QtVtkExtrinsicTransformationPanel& operator = (const QtVtkExtrinsicTransformationPanel&);

	/** Le titre de l'application, pour les messages d'erreur. */
	IN_UTIL UTF8String					_appTitle;

	/** La translation précède t'elle les rotations. */
	QCheckBox*							_translationFirstCheckBox;
	
	/** L'angle de rotation dans le plan xOy, en degrés. */
	QtTextField*						_xOyAngleTextField;

	/** L'angle de rotation dans le plan xOz, en degrés. */
	QtTextField*						_xOzAngleTextField;

	/** L'angle de rotation dans le plan yOz, en degrés. */
	QtTextField*						_yOzAngleTextField;

	/** La translation rotations. */
	Qt3DDataPanel*						_translationPanel;
};	// class QtVtkExtrinsicTransformationPanel




/**
 * <P>
 * Panneau Qt permettant la saisie des paramètres d'une transformation linéaire VTK intrinsèque, c'est à dire appliquée
 * dans le repère local à l'objet considéré. Les axes du repère suivent les transformations.
 * </P>
 * <P>Cette transformation est caractérisée par un centre, défini dans le repère global, et par un ensemble de rotations définies dans le repère local.
 * </P>
 * <P>Dans la version actuelle, les rotations sont effectuées dans l'ordre suivant : </P>
 * <OL>
 * 	<LI>Rotation d'angle phi autour de l'axe Oy,
 * 	<LI>Rotation d'angle thêta autour de l'axe Oz,
 * 	<LI>Rotation d'angle oméga autour de l'axe Ox,
  * </OL>
 * </UL>
 * @since	8.9.0
 */
class QtVtkIntrinsicTransformationPanel : public QWidget 
{
	Q_OBJECT

	public :

	/** Constructeur.
	 * @param		widget parent. Ne doit pas être nul.
	 * @param		titre de l'application (pour les messages d'erreur).
	 * @param		coordonnées du centre du repère local
	 * @param		Angle phi, exprimé en degrés
	 * @param		Angle thêta, exprimé en degrés
	 * @param		Angle oméga, exprimé en degrés
	 */
	QtVtkIntrinsicTransformationPanel (QWidget* parent, const IN_UTIL UTF8String& appTitle, double x = 0., double y = 0., double z = 0., double phi = 0., double theta = 0., double omega = 0.);


	/** Destructeur. */
	virtual ~QtVtkIntrinsicTransformationPanel ( );

	/**
	 * @param			Les coordonnées du centre du repère local.
	 */
	virtual void setCenter (double dx, double dy, double dz);

	/**
	 * @param			Les coordonnées du centre du repère local.
	 */
	virtual void getCenter (double& dx, double& dy, double& dz) const;

	/**
	 * @param			l'angle de rotation phi, en degrés.
	 */
	virtual void setPhiAngle (double angle);

	/**
	 * @return			l'angle de rotation phi, en degrés.
	 */
	virtual double getPhiAngle ( ) const;

	/**
	 * @param			l'angle de rotation thêta, en degrés.
	 */
	virtual void setThetaAngle (double angle);

	/**
	 * @return			l'angle de rotation thêta, en degrés.
	 */
	virtual double getThetaAngle ( ) const;

	/**
	 * @param			l'angle de rotation oméga, en degrés.
	 */
	virtual void setOmegaAngle (double angle);

	/**
	 * @return			l'angle de rotation oméga, en degrés.
	 */
	virtual double getOmegaAngle ( ) const;

	/**
	 * @return			une transformation correspondant au paramétrage en cours. A détruire par l'appelant.
	 */
	virtual vtkTransform* getTransformation ( ) const;
	
	/**
	 * @return			une description textuelle de la transformation.
	 */
	virtual IN_UTIL UTF8String getTransformationDescription ( ) const;

	signals :

	/**
	 * Signal émis lorsque l'utilisateur change le paramétrage de la transformation.
	 */
	 void transformationChanged ( );



	protected slots :


	/** Appelé lorsque l'utilisateur modifie un des champs de paramétrage de la transformation.
	 * Emet le signal <I>transformationChanged</I>.
	 */
	virtual void transformationModifiedCallback ( );


	private :

	/** Constructeur de copie. Interdit. */
	QtVtkIntrinsicTransformationPanel (const QtVtkIntrinsicTransformationPanel&);

	/** Opérateur de copie. Interdit. */
	QtVtkIntrinsicTransformationPanel& operator = (const QtVtkIntrinsicTransformationPanel&);

	/** Le titre de l'application, pour les messages d'erreur. */
	IN_UTIL UTF8String					_appTitle;

	/** Le centre du repère local. */
	Qt3DDataPanel*						_centerPanel;

	/** L'angle de rotation phi, en degrés. */
	QtTextField*						_phiAngleTextField;

	/** L'angle de rotation théta, en degrés. */
	QtTextField*						_thetaAngleTextField;

	/** L'angle de rotation oméga, en degrés. */
	QtTextField*						_omegaAngleTextField;
};	// class QtVtkIntrinsicTransformationPanel







/**
 * <P>
 * Panneau Qt permettant la saisie des paramètres d'une transformation linéaire VTK intrinsèque ou extrinsèque.
 * </P>
 * @since	8.9.0
 */
class QtVtkTransformationPanel : public QWidget 
{
	Q_OBJECT

	public :


	/** Constructeur avec initialisation par une translation extrinsèque.
	 * @param		widget parent. Ne doit pas être nul.
	 * @param		titre de l'application (pour les messages d'erreur).
	 * @param		Angle de rotation dans le plan xOy
	 * @param		Angle de rotation dans le plan xOz
	 * @param		Angle de rotation dans le plan yOz
	 * @param		<I>true</I> si la translation précède les rotations, <I>false</I> si elle suit les rotations.
	 * @param		translation pré ou post-rotations
	 */
	QtVtkTransformationPanel (QWidget* parent, const IN_UTIL UTF8String& appTitle, double xoy = 0., double xoz = 0., double yoz = 0., bool translationFirst = true, double dx = 0., double dy = 0., double dz = 0.);

	/** Constructeur avec initialisation par une translation intrinsèque.
	 * @param		widget parent. Ne doit pas être nul.
	 * @param		titre de l'application (pour les messages d'erreur).
	 * @param		coordonnées du centre du repère local
	 * @param		Angle phi, exprimé en degrés
	 * @param		Angle thêta, exprimé en degrés
	 * @param		Angle oméga, exprimé en degrés
	 */
	QtVtkTransformationPanel (QWidget* parent, const IN_UTIL UTF8String& appTitle, double x = 0., double y = 0., double z = 0., double phi = 0., double theta = 0., double omega = 0.);

	/** Destructeur. */
	virtual ~QtVtkTransformationPanel ( );

	/** 
	 * @param	<I>true</I> pour afficher le paramétrage d'une transformation extrinsèque, <I>false</I> pour afficher le paramétrage d'une transformation intrinsèque.
	 */
	virtual void setExtrinsic (bool extrinsic);
	
	/** 
	 * @return	<I>true</I> pour afficher le paramétrage d'une transformation extrinsèque, <I>false</I> pour afficher le paramétrage d'une transformation intrinsèque.
	 */
	virtual bool isExtrinsic ( ) const;

	/**
	 * @return			une transformation correspondant au paramétrage en cours. A détruire par l'appelant.
	 */
	virtual vtkTransform* getTransformation ( ) const;
	
	/**
	 * @return			une description textuelle de la transformation.
	 */
	virtual IN_UTIL UTF8String getTransformationDescription ( ) const;
	
	/**
	 * Le panneau de paramétrage d'une transformation extrinsèque, en consultation.
	 */
	virtual const QtVtkExtrinsicTransformationPanel& getExtrinsicPanel ( ) const;
	
	/**
	 * Le panneau de paramétrage d'une transformation intrinsèque, en consultation.
	 */
	virtual const QtVtkIntrinsicTransformationPanel& getIntrinsicPanel ( ) const;


	signals :

	/**
	 * Signal émis lorsque l'utilisateur change le paramétrage de la transformation en cours.
	 */
	 void transformationChanged ( );



	protected :
	
	/**
	 * La création des éléments d'IHM.
	 */
	virtual void createGui ( );


	protected slots :

	/**
	 * Appelé lorsque l'utilisateur change de type de transformation en cours.
	 * Emet le signal <I>transformationChanged</I>.
	 */
	 void transformationTypeModifiedCallback ( );
	 
	/**
	 * Appelé lorsque l'utilisateur modifie un des champs de paramétrage de la transformation.
	 * Emet le signal <I>transformationChanged</I>.
	 */
	virtual void transformationModifiedCallback ( );


	private :

	/** Constructeur de copie. Interdit. */
	QtVtkTransformationPanel (const QtVtkTransformationPanel&);

	/** Opérateur de copie. Interdit. */
	QtVtkTransformationPanel& operator = (const QtVtkTransformationPanel&);

	/** Le titre de l'application, pour les messages d'erreur. */
	IN_UTIL UTF8String					_appTitle;

	/** Type de transformation en cours */
	QCheckBox*							_extrinsicCheckBox;

	/** Aide contextuelle et sa représentation. */
	QLabel*								_contextualHelpLabel;
	QLabel*								_imageLabel;

	QtVtkExtrinsicTransformationPanel*	_extrinsicPanel;
	QtVtkIntrinsicTransformationPanel*	_intrinsicPanel;
};	// class QtVtkTransformationPanel



#endif	// QT_VTK_TRANSFORMATION_PANEL_H
