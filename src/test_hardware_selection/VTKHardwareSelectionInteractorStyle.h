#ifndef VTK_HARDWARE_SELECTION_INTERACTOR_STYLE_H
#define VTK_HARDWARE_SELECTION_INTERACTOR_STYLE_H


#include <vtkHardwareSelector.h>
#include <vtkSmartPointer.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkUnsignedCharArray.h>



/** Interacteur gérant la sélection de données au premierplan.
 */
class VTKHardwareSelectionInteractorStyle : public vtkInteractorStyleTrackballCamera
{
	public :

	/**
	 * Interactions type "motion" définies par cette classe dans le prolongement des <I>vtkInteractorStyle::VTKIS_*</I>.
	 */
	//@{
	
	/** Sélection par rectangle élastique. */
	static int						VTKIS_RUBBER_BAND;

	static VTKHardwareSelectionInteractorStyle* New ( );

	virtual ~VTKHardwareSelectionInteractorStyle ( );

	/**
	 * En mode <I>capture au rectangle élastique</I> ajuste le rectangle au nouvel emplacement de la souris.
	 */
	 virtual void OnMouseMove ( );

	/**
	 * Mémorise la position du curseur de la souris en cas de picking possible
	 * avec le bouton gauche de la souris.
	 */
	virtual void OnLeftButtonDown ( );

	/**
	 * Effectue une opération de picking si le picking sur le bouton gauche de
	 * la souris est actif et que le curseur de la souris n'a pas bougé depuis
	 * que le bouton a été enfoncé.
	 * @see		Pick
	 */
	virtual void OnLeftButtonUp ( );


	protected :

	VTKHardwareSelectionInteractorStyle ( );

	/**
	 * \return	Une référence sur le sélecteur hardware.
	 */
	virtual vtkHardwareSelector& GetHardwareSelector ( );
	
	/**
	 * Dessine un <I>rectangle élastique de sélection</I>.
	 */
	virtual void RedrawRubberBand ( );


	private :

	VTKHardwareSelectionInteractorStyle (const VTKHardwareSelectionInteractorStyle&);
	VTKHardwareSelectionInteractorStyle& operator = (const VTKHardwareSelectionInteractorStyle&);
	
	/** Le picking effectué est-il au premier plan (true) ou en profondeur (et approximatif) ? Vaut false par défaut.
	 */
	bool											ForegroundSelection;

	/** Vaut <I>true</I> si le bouton de <I>tracé élastique</I> est enfoncé. */
	bool											RubberButtonDown;
	
	/** Position du curseur de la souris au moment où le bouton est pressé. */
	int												ButtonPressPosition [2];

	/** Images pour le rendu avectracé élastique. */
	vtkSmartPointer<vtkUnsignedCharArray>			PixelArray, TmpPixelArray;
	
	/** Position du curseur de la souris en mode <I>rectangle élastique</I>. */
	int												StartPosition [2], EndPosition [2];

	/** Un sélecteur pour le picking précis de premier plan. */
	vtkSmartPointer<vtkHardwareSelector>			HardwareSelector;
};	// class VTKHardwareSelectionInteractorStyle




#endif	// VTK_HARDWARE_SELECTION_INTERACTOR_STYLE_H
