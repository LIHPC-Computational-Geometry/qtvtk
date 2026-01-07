/**
 * \file		QtVtkTransformationPanel.h
 * \author		Charles PIGNEROL, CEA/DAM/DCLC
 * \date		06/01/2026
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
 * Panneau Qt permettant la saisie des paramètres d'une transformation linéaire VTK exprimée en coordonnées cartésiennes ou en coordonnées polaires.
 * </P>
 * <P>Cette transformation est caractérisée par un ensemble de rotations dans les 3 dimensions suivies d'une translation.
 * </P>
 * <P>Dans la version actuelle, les transformations sont effectuées dans l'ordre suivant : </P>
 * <UL>
 * <LI>Transformation en repère cartésien :
 * <OL>
 * 	<LI>Rotation autour de l'axe Oz (plan xOy),
 * 	<LI>Rotation autour de l'axe Ox (plan yOz),
 * 	<LI>Rotation autour de l'axe Oy (plan xOz),
 *	<LI>Translation
 * </OL>
 * <LI>Transformation en repère sphérique :
 * <OL>
 * 	<LI>Rotation d'angle phi (plan xOz),
 * 	<LI>Rotation d'angle thêta (avec l'axe Oy),
 * 	<LI>Rotation d'angle oméga (autour de l'axe Ox),
 *	<LI>Translation
 * </OL>
 * </UL>
 * @since	8.9.0
 */
class QtVtkTransformationPanel : public QWidget 
{
	Q_OBJECT

	public :


	/** Constructeur.
	 * Un appel à  enable3DWidget permettra, à terme, d'utiliser un widget type "vtkTorusWidget" de VTK.
	 * @param		widget parent. Ne doit pas être nul.
	 * @param		titre de l'application (pour les messages d'erreur).
	 * @param		<I>true</I> si les angles de rotation sont exprimés en repère cartésien, <I>false</I> s'ils sont exprimés en repère sphérique.
	 * @param		Angle de rotation dans le plan xOy (ou angle phi)
	 * @param		Angle de rotation dans le plan xOz (ou angle thêta)
	 * @param		Angle de rotation dans le plan yOz (ou angle oméga)
	 * @param		translation post-rotations
	 */
	QtVtkTransformationPanel (QWidget* parent, const IN_UTIL UTF8String& appTitle, bool cartesian = true, double xoy = 0., double xoz = 0., double yoz = 0., double dx = 0., double dy = 0., double dz = 0.);


	/** Destructeur. */
	virtual ~QtVtkTransformationPanel ( );

	/** 
	 * @param	<I>true</I> si les angles de rotation sont exprimés en repère cartésien, <I>false</I> s'ils sont exprimés en repère sphérique.
	 */
	virtual void setCartesianCoordinateSystem (bool cartesian);
	
	/** 
	 * @return	<I>true</I> si les angles de rotation sont exprimés en repère cartésien, <I>false</I> s'ils sont exprimés en repère sphérique.
	 */
	virtual bool isCartesianCoordinateSystem ( ) const;

	/**
	 * @param			l'angle de rotation dans le plan xOy, en degrés. Cas d'une transformation en repère cartésien.
	 */
	virtual void setXOYAngle (double angle);

	/**
	 * @return			l'angle de rotation dans le plan xOy, en degrés. Cas d'une transformation en repère cartésien.
	 */
	virtual double getXOYAngle ( ) const;

	/**
	 * @param			l'angle de rotation dans le plan xOz, en degrés. Cas d'une transformation en repère cartésien.
	 */
	virtual void setXOZAngle (double angle);

	/**
	 * @return			l'angle de rotation dans le plan xOz, en degrés. Cas d'une transformation en repère cartésien.
	 */
	virtual double getXOZAngle ( ) const;

	/**
	 * @param			l'angle de rotation dans le plan yOz, en degrés. Cas d'une transformation en repère cartésien.
	 */
	virtual void setYOZAngle (double angle);

	/**
	 * @return			l'angle de rotation dans le plan yOz, en degrés. Cas d'une transformation en repère cartésien.
	 */
	virtual double getYOZAngle ( ) const;

	/**
	 * @param			l'angle de rotation phi, en degrés. Cas d'une transformation en repère sphérique.
	 */
	virtual void setPhiAngle (double angle);

	/**
	 * @return			l'angle de rotation phi, en degrés. Cas d'une transformation en repère sphérique.
	 */
	virtual double getPhiAngle ( ) const;

	/**
	 * @param			l'angle de rotation thêta, en degrés. Cas d'une transformation en repère sphérique.
	 */
	virtual void setThetaAngle (double angle);

	/**
	 * @return			l'angle de rotation thêta, en degrés. Cas d'une transformation en repère sphérique.
	 */
	virtual double getThetaAngle ( ) const;

	/**
	 * @param			l'angle de rotation oméga, en degrés. Cas d'une transformation en repère sphérique.
	 */
	virtual void setOmegaAngle (double angle);

	/**
	 * @return			l'angle de rotation oméga, en degrés. Cas d'une transformation en repère sphérique.
	 */
	virtual double getOmegaAngle ( ) const;

	/**
	 * @param			la translation à effectuer après les rotations.
	 */
	virtual void setTranslation (double dx, double dy, double dz);

	/**
	 * @param			la translation à effectuer après les rotations.
	 */
	virtual void getTranslation (double& dx, double& dy, double& dz) const;

	/**
	 * @return			une transformation correspondant au paramétrage en cours. A détruire par l'appelant.
	 */
	virtual vtkTransform* getTransformation ( ) const;


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
	QtVtkTransformationPanel (const QtVtkTransformationPanel&);

	/** Opérateur de copie. Interdit. */
	QtVtkTransformationPanel& operator = (const QtVtkTransformationPanel&);

	/** Le titre de l'application, pour les messages d'erreur. */
	IN_UTIL UTF8String					_appTitle;

	/** Expression ou non des angles de rotation en repère sphérique. */
	QCheckBox*							_sphericalCheckBox;

	/** Aide contextuelle. */
	QLabel*								_contextualHelpLabel;

	/** Libellés des angles de rotation. */
	QLabel*								_xOyAngleLabel;
	QLabel*								_xOzAngleLabel;
	QLabel*								_yOzAngleLabel;
	
	/** L'angle de rotation dans le plan xOy (coord. cartésiennes) ou phi (coord. polaires), en degrés. */
	QtTextField*						_xOyAngleTextField;

	/** L'angle de rotation dans le plan xOz (coord. cartésiennes) ou théta (coord. polaires), en degrés. */
	QtTextField*						_xOzAngleTextField;

	/** L'angle de rotation dans le plan yOz (coord. cartésiennes) ou oméga (coord. polaires), en degrés. */
	QtTextField*						_yOzAngleTextField;

	/** La translation post-rotations. */
	Qt3DDataPanel*						_translationPanel;
};	// class QtVtkTransformationPanel



#endif	// QT_VTK_TRANSFORMATION_PANEL_H
