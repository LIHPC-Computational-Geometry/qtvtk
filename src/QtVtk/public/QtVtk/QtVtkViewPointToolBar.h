#ifndef QT_VTK_VIEWPOINT_TOOL_BAR_H
#define QT_VTK_VIEWPOINT_TOOL_BAR_H

#include <QToolBar>
#include <QtVtk/QtVtkViewPointToolButton.h>

#include <vector>


/**
 * @brief	Barre de boutons à menu permettant de gérer un point de vue dans une fenêtre VTK et d'assurer la persistance de ce point de vue.
 * @since	8.7.0
 */
class QtVtkViewPointToolBar : public QToolBar
{
	Q_OBJECT
	
	public :

	/**
	 * @param	Parent
	 * @param	Camera VTK associée aux points de vue
	 * @param	Eventuel gestionnaire de rendu (pour forcer les opération d'actualisation de la vue).
	 */
	QtVtkViewPointToolBar (QWidget* parent, vtkCamera& camera, vtkRenderer* renderer);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtVtkViewPointToolBar ( );
	
	/**
	 * Ajoute le point de vue transmis en argument à la barre de boutons.
	 */
	virtual void addViewPoint (const QtVtkViewPointToolButton::VtkViewPoint& viewPoint);
	
	/**
	 * Enlève le point de vue dont une référence sur le bouton est transmise en argument.
	 */
	virtual void removeViewPoint (QtVtkViewPointToolButton& viewPointButton);


	protected slots :

	/**
	 * Ajoute le point de vue aux points de vue existants.
	 */
	virtual void newViewPointCallback ( );
	
	/**
	 * Réinitialise (supprime tous les points de vue.
	 */
	virtual void initializeCallback ( );
	
	/**
	 * Exporte les points de vue dans un fichier.
	 */
	virtual void exportViewPointsCallback ( );
	
	/**
	 * Importe les points de vue depusi un fichier.
	 */
	virtual void importViewPointsCallback ( );


	private :

	QtVtkViewPointToolBar (const QtVtkViewPointToolBar&);
	QtVtkViewPointToolBar& operator = (const QtVtkViewPointToolBar&);

	/** Points de vue gérés. */
	std::vector<QtVtkViewPointToolButton*>		_viewPointButtons;
	
	/** Indice du prochain bouton créé. */
	size_t										_nextButton;
	
	/** Camera VTK associée. */
	vtkCamera*									_camera;
	
	/** Gestionnaire de rendu (pour forcer les opération d'actualisation de la vue). */
	vtkRenderer*								_renderer;
};	// class QtVtkViewPointToolBar

#endif	// QT_VTK_VIEWPOINT_TOOL_BAR_H
