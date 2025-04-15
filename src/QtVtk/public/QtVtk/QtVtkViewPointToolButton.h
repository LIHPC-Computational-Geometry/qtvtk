#ifndef QT_VTK_VIEWPOINT_TOOL_BUTTON_H
#define QT_VTK_VIEWPOINT_TOOL_BUTTON_H

#include <PrefsCore/Section.h>
#include <QToolButton>
#include <vtkCamera.h>
#include <vtkRenderer.h>
#include <string>


/**
 * @brief	Bouton à menu permettant de gérer un point de vue dans une fenêtre VTK et d'assurer la persistance de ce point de vue.
 * @since	8.7.0
 */
class QtVtkViewPointToolButton : public QToolButton
{
	Q_OBJECT

	public :

	/**
	 * La définition d'un point de vue.
	 */
	struct VtkViewPoint
	{
		VtkViewPoint ( );
		VtkViewPoint (const VtkViewPoint& vp);
		VtkViewPoint& operator = (const VtkViewPoint& vp);
		VtkViewPoint& operator = (const vtkCamera&);
		~VtkViewPoint ( );

		/** Nom de la vue et commentaire associé. */
		TkUtil::UTF8String		name, comment;

		/** Position de l'observateur. */
		double					position [3];
		
		/** Point focal. */
		double					focalPoint [3];
		
		/** Direction vers le haut. */
		double					viewUp [3];
		
		/** Roulis (rotation autour de l'axe de visée). */
		double					roll;
	};	// struct VtkViewPoint

	static Preferences::Section* viewPointToSection (const VtkViewPoint& viewpoint);
	static VtkViewPoint sectionToViewPoint (const Preferences::Section& section);

	
	/**
	 * @param	Parent
	 * @param	Icône du bouton
	 * @param	Nom du point de vue
	 * @param	Camera VTK associée au point de vue
	 * @param	Eventuel gestionnaire de rendu (pour forcer les opération d'actualisation de la vue).
	 */
	QtVtkViewPointToolButton (QWidget* parent, const std::string& icon, const TkUtil::UTF8String& name, vtkCamera& camera, vtkRenderer* renderer);

	/**
	 * @param	Parent
	 * @param	Icône du bouton
	 * @param	Définition du point de vue
	 * @param	Camera VTK associée au point de vue
	 * @param	Eventuel gestionnaire de rendu (pour forcer les opération d'actualisation de la vue).
	 */
	QtVtkViewPointToolButton (QWidget* parent, const std::string& icon, const VtkViewPoint& viewPoint, vtkCamera& camera, vtkRenderer* renderer);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtVtkViewPointToolButton ( );
	
	/**
	 * @return	La description du point de vue
	 */
	virtual const VtkViewPoint& getViewPoint ( ) const;
	
	/**
	 * @param	Nouveau point de vue. Modifie également la vue graphique.
	 */
	virtual void setViewPoint (const VtkViewPoint& vp);

	/**
	 * Réinitialise le point de vue à partir du point de vue transmis en argument.
	 */
	virtual void reinitializeViewPoint (vtkCamera& camera, vtkRenderer* renderer);
	

	protected slots :

	/**
	 * Modifie la vue conformément aux paramétrage de ce point de vue.
	 */
	virtual void applyViewPointCallback ( );
	
	/**
	 * Réinitialise le point de vue à partir de la vue courante.
	 */
	virtual void reinitializeViewPointCallback ( );

	/**
	 * Affiche une boite de dialogue de modification du point de vue.
	 */
	virtual void editViewPointCallback ( );
	
	/**
	 * Supprime la vue (et le bouton).
	 */
	virtual void removeViewPointCallback ( );
	
	/**
	 * Exporte le point de vue dans un fichier.
	 */
	virtual void exportViewPointCallback ( );


	private :

	QtVtkViewPointToolButton (const QtVtkViewPointToolButton&);
	QtVtkViewPointToolButton& operator = (const QtVtkViewPointToolButton&);

	/** Le point de vue. */
	VtkViewPoint	_viewPoint;
	
	/** Camera VTK associée. */
	vtkCamera*		_camera;
	
	/** Gestionnaire de rendu (pour forcer les opération d'actualisation de la vue). */
	vtkRenderer*	_renderer;
};	// class QtVtkViewPointToolButton

#endif	// QT_VTK_VIEWPOINT_TOOL_BUTTON_H
