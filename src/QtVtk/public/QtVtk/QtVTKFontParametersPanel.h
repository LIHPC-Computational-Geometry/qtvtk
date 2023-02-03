#ifndef QT_VTK_FONT_PARAMETERS_PANEL_H
#define QT_VTK_FONT_PARAMETERS_PANEL_H

#include <string>

#include <TkUtil/util_config.h>
#include <TkUtil/ThreeStates.h> 
#include <TkUtil/PaintAttributes.h> 
#include <QtUtil/QtGroupBox.h> 
#include <QtUtil/QtTextField.h> 

#include <QCheckBox> 
#include <QComboBox> 
#include <QLabel> 


/**
 * Panneau Qt permettant la saisie des paramètres définissant une fonte
 * pour VTK :
 * - Famille de la fonte,
 * - Taille de la fonte,
 * - Caractères gras/italiques,
 * - Couleur.
 */
class QtVTKFontParametersPanel : public QtGroupBox 
{
	Q_OBJECT

	public :

	/** Constructeur
	 * @param		widget parent. Ne doit pas être nul.
	 * @param		titre de l'application (pour les boites de dialogue).
	 * @param		famille de police de caractères par défaut 
	 *				VTK_ARIAL, VTK_COURIER ou VTK_TIMES).
	 * @param		taille de la police de caractères par d§efaut.
	 * @param		couleur de la police de caractères par défaut.
	 * @param		caractères gras.
	 * @param		caractères italiques.
	 */
	QtVTKFontParametersPanel (QWidget* parent, const IN_STD string& appTitle,
					int family, int size, const IN_UTIL Color& color,
					IN_UTIL THREESTATES bold, IN_UTIL THREESTATES italic);


	/** Destructeur. */
	virtual ~QtVTKFontParametersPanel ( );

	/**
	 * @return			la famille de police de caractères (VTK_ARIAL,
	 *					VTK_COURIER ou VTK_TIMES) ou -1 si indéterminée.
	 */
	virtual int getFontFamily ( ) const;

	/**
	 * @param			nouvelle famille de police de caractères
	 *					(-1, VTK_ARIAL, VTK_COURIER ou VTK_TIMES).
	 */
	virtual void setFontFamily (int family);

	/**
	 * @return			la taille de la police de caractères, ou -1 si elle
	 *					est indéterminée.
	 */
	virtual int getFontSize ( ) const;

	/**
	 * @param			nouvelle taille de police de caractères.
	 */
	virtual void setFontSize (int size);

	/**
	 * @return			le caractère "gras" de la police de caractères.
	 */
	virtual IN_UTIL THREESTATES isBold ( ) const;

	/**
	 * @param			le caractère "gras" de la police de caractères.
	 */
	virtual void setBold (IN_UTIL THREESTATES bold);

	/**
	 * @return			le caractère "italiques" de la police de 
	 *					caractères.
	 */
	virtual IN_UTIL THREESTATES isItalic ( ) const;

	/**
	 * @param			le caractère "italiques" de la police de 
	 *					caractères.
	 */
	virtual void setItalic (IN_UTIL THREESTATES italic);

	/**
	 * @return			la couleur de la police de caractères.
	 */
	virtual IN_UTIL Color getColor ( ) const;

	/**
	 * @param			la couleur de la police de caractères.
	 */
	virtual void setColor (const IN_UTIL Color& color);

	/**
	 * @return			true si la couleur été modifiée., sinon false. 
	 */
	bool isColorModified ( ) const
	{ return _colorModified; }


	protected slots :

	/** Appelé quand l'utilisateur clique sur le bouton "Couleur ...".
	 */
	virtual void setColorCallback ( );

	/** Appelé quand l'utilisateur change un caractère de la famille de 
	 * fonte.
	 * Invoque parametersChanged.
	 * @param		Non utilisé.
	 */
	virtual void familyChangedCallback (int);

	/** Appelé quand l'utilisateur change un caractère de la famille de  
	 * la fonte.
	 * Invoque parametersChanged.
	 */
	virtual void fontChangedCallback ( );


	protected :

	/** Les famille de polices de caractères. */
	static const QString		_arialFamily, _courierFamily, _timesFamily;


	protected :

	/** Met à jour le texte en exemple et les paramètres suite à
	 * une modification de l'utilisateur.
	 */
	virtual void parametersChanged ( );


	private :

	/** Constructeur de copie. Interdit. */
	QtVTKFontParametersPanel (const QtVTKFontParametersPanel&);

	/** Opérateur de copie. Interdit. */
	QtVTKFontParametersPanel& operator = (const QtVTKFontParametersPanel&);

	/** Le label affichant la fonte telle qu'elle est définie. */
	QLabel*				_fontLabel;

	/** La boite à défilement affichant les familles possibles. */
	QComboBox*			_familiesComboBox;

	/** Le champs de text de saisie de la taille. */
	QtTextField*		_sizeTextField;

	/** Les cases à cocher. */
	QCheckBox			*_boldCheckBox, *_italicCheckBox;

	/** Le titre de l'application. */
	IN_STD string		_appTitle;

	/** La couleur a-t-elle été modifiée. */
	bool				_colorModified;
};	// class QtVTKFontParametersPanel



#endif	// QT_VTK_FONT_PARAMETERS_PANEL_H
