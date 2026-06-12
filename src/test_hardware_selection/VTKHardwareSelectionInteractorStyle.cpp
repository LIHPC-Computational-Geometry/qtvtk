#include "VTKHardwareSelectionInteractorStyle.h"
#include <vtkActor.h>
#include <vtkAssemblyPath.h>
#include <vtkCell.h>
#include <vtkDataSet.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkIdTypeArray.h>
#include <vtkInformation.h>
#include <vtkMapper.h>
#include <vtkProp3DCollection.h>
#include <vtkSelection.h>
#include <vtkSelectionNode.h>

#include <TkUtil/MemoryError.h>

#include <assert.h>
#include <stdexcept>


using namespace std;

int		VTKHardwareSelectionInteractorStyle::VTKIS_RUBBER_BAND		= 2048;


VTKHardwareSelectionInteractorStyle::VTKHardwareSelectionInteractorStyle ( )
	: vtkInteractorStyleTrackballCamera ( ), 
	  RubberButtonDown (false), 
	  PixelArray (vtkSmartPointer<vtkUnsignedCharArray>::New ( )), TmpPixelArray (vtkSmartPointer<vtkUnsignedCharArray>::New ( )),
	  HardwareSelector(vtkSmartPointer<vtkHardwareSelector>::New ( ))
{
	ButtonPressPosition [0]	= ButtonPressPosition [1]	= 0;
	StartPosition [0]		= StartPosition [1]			= 0;
	EndPosition [0]			= EndPosition [1]			= 0;
	PixelArray->Initialize ( );
	TmpPixelArray->Initialize ( );
	PixelArray->SetNumberOfComponents (4);
	TmpPixelArray->SetNumberOfComponents (4);
}	// VTKHardwareSelectionInteractorStyle::VTKHardwareSelectionInteractorStyle


VTKHardwareSelectionInteractorStyle::VTKHardwareSelectionInteractorStyle (const VTKHardwareSelectionInteractorStyle&)
	: vtkInteractorStyleTrackballCamera ( ),
	  RubberButtonDown (false), 
	  PixelArray ( ), TmpPixelArray ( ), HardwareSelector ( )
{
	assert (0 && "VTKHardwareSelectionInteractorStyle copy constructor is not allowed.");
}	// VTKHardwareSelectionInteractorStyle copy constructor


VTKHardwareSelectionInteractorStyle& VTKHardwareSelectionInteractorStyle::operator = (const VTKHardwareSelectionInteractorStyle&)
{
	assert (0 && "VTKHardwareSelectionInteractorStyle::operator = is not allowed.");
	return *this;
}	// VTKHardwareSelectionInteractorStyle::operator =


VTKHardwareSelectionInteractorStyle::~VTKHardwareSelectionInteractorStyle ( )
{
}	// VTKHardwareSelectionInteractorStyle::~VTKHardwareSelectionInteractorStyle


VTKHardwareSelectionInteractorStyle* VTKHardwareSelectionInteractorStyle::New ( )
{
	vtkObject*	object  = vtkObjectFactory::CreateInstance ("VTKHardwareSelectionInteractorStyle");
	if (0 != object)
		return (VTKHardwareSelectionInteractorStyle*)object;

	return new VTKHardwareSelectionInteractorStyle ( );
}	// VTKHardwareSelectionInteractorStyle::New


void VTKHardwareSelectionInteractorStyle::OnMouseMove ( )
{
	if (VTKIS_RUBBER_BAND != GetState ( ))
		vtkInteractorStyleTrackballCamera::OnMouseMove ( );
	else
	{
		vtkRenderWindowInteractor*	rwi	= this->Interactor;
		if ((0 != rwi) && (0 != rwi->GetRenderWindow ( )))
		{
			EndPosition [0]	= rwi->GetEventPosition ( )[0];
			EndPosition [1]	= rwi->GetEventPosition ( )[1];
			int*	size	= rwi->GetRenderWindow ( )->GetSize ( );
			if (EndPosition [0] > (size [0] - 1))
				EndPosition [0]	= size [0] - 1;
			if (EndPosition [0] < 0)
				EndPosition [0] = 0;
			if (EndPosition [1] > (size [1] - 1))
				EndPosition [1] = size [1] - 1;
			if (EndPosition [1] < 0)
				EndPosition [1] = 0;
			RedrawRubberBand ( );
		}	// if ((0 != rwi) && (0 != rwi->GetRenderWindow ( )))
	}	// else if (VTKIS_RUBBER_BAND != GetState ( ))
}	// VTKHardwareSelectionInteractorStyle::OnMouseMove


void VTKHardwareSelectionInteractorStyle::OnLeftButtonDown ( )
{
cout << __FILE__ << ' ' << __LINE__ << " VTKHardwareSelectionInteractorStyle::OnLeftButtonDown" << endl;
	vtkRenderWindowInteractor*	rwi	= this->Interactor;
	vtkInteractorStyleTrackballCamera::OnLeftButtonDown ( );

cout << __FILE__ << ' ' << __LINE__ << " VTKHardwareSelectionInteractorStyle::OnLeftButtonDown" << endl;
	if ((0 == rwi) || (0 == rwi->GetRenderWindow ( )))
		return;

cout << __FILE__ << ' ' << __LINE__ << " VTKHardwareSelectionInteractorStyle::OnLeftButtonDown" << endl;
	StartState (VTKIS_RUBBER_BAND);
	RubberButtonDown	= true;
	StartPosition [0]	= EndPosition [0]	= rwi->GetEventPosition ( )[0];
	StartPosition [1]	= EndPosition [1]	= rwi->GetEventPosition ( )[1];
	int*	size		= rwi->GetRenderWindow ( )->GetSize ( );
	PixelArray->SetNumberOfTuples (size [0] * size [1]);
	TmpPixelArray->SetNumberOfTuples (size [0] * size [1]);
//	rwi->GetRenderWindow ( )->GetRGBACharPixelData (0, 0, size [0] - 1, size [1] - 1, 1, PixelArray);
// Si on est en double buffering on prend l'image dans le back buffer, sinon dans le front buffer.
	rwi->GetRenderWindow ( )->GetRGBACharPixelData (0, 0, size [0] - 1, size [1] - 1, !rwi->GetRenderWindow ( )->GetDoubleBuffer ( ), PixelArray);
	FindPokedRenderer (StartPosition [0], StartPosition [1]);
}	// VTKHardwareSelectionInteractorStyle::OnLeftButtonDown


void VTKHardwareSelectionInteractorStyle::OnLeftButtonUp ( )
{
	try
	{
		// Arrêter un éventuel déclenchement d'interaction :
cout << __FILE__ << ' ' << __LINE__ << " VTKHardwareSelectionInteractorStyle::OnLeftButtonUp" << endl;
		vtkInteractorStyleTrackballCamera::OnLeftButtonUp ( );

		vtkRenderWindowInteractor*	rwi	= this->Interactor;
		
		if (VTKIS_RUBBER_BAND == GetState ( ))
		{
			StopState ( );
			const int* const	size	= rwi->GetRenderWindow ( )->GetSize ( );
			// Si on est en double buffering on met l'image dans le back buffer, Frame ( )la rebasculera dans le front buffer, sinon on la met dans le front buffer.
			rwi->GetRenderWindow ( )->SetRGBACharPixelData (0, 0, size [0] - 1, size [1] - 1, PixelArray->GetPointer (0), !rwi->GetRenderWindow ( )->GetDoubleBuffer ( ));
			rwi->GetRenderWindow ( )->Frame ( );
			RubberButtonDown		= false;
			const int	xmin	= StartPosition [0] < EndPosition [0] ? StartPosition [0] : EndPosition [0];
			const int	xmax	= StartPosition [0] < EndPosition [0] ? EndPosition [0] : StartPosition [0];
			const int	ymin	= StartPosition [1] < EndPosition [1] ? StartPosition [1] : EndPosition [1];
			const int	ymax	= StartPosition [1] < EndPosition [1] ? EndPosition [1] : StartPosition [1];
cout << "XMIN=" << xmin << " XMAX=" << xmax << " YMIN=" << ymin << " YMAX=" << ymax << endl;
			vtkHardwareSelector&	selector	= GetHardwareSelector ( );
			selector.SetArea (xmin, ymin, xmax, ymax);
			selector.SetRenderer (CurrentRenderer);
			vtkSmartPointer<vtkSelection>	selection;
			selection.TakeReference (selector.Select ( ));
cout << __FILE__ << ' ' << __LINE__ << " SELECTION HAS " << selection->GetNumberOfNodes ( ) << " NODES" << endl;
			for (unsigned int n = 0; n < selection->GetNumberOfNodes ( ); n++)
			{
				vtkSelectionNode*	node	= selection->GetNode (n);
				if ((0 == node) || (0 == node->GetProperties ( )))
					continue;
						
				const int			propId		= node->GetProperties ( )->Get (vtkSelectionNode::PROP_ID ( ));
cout << __FILE__ << ' ' << __LINE__ << " ACTOR ADDR " << dynamic_cast<vtkActor*>(selector.GetPropFromID (propId)) << " SELECTED CELLS : ";
				// Mistral AI :
				vtkIdTypeArray*		ids			= vtkIdTypeArray::SafeDownCast (node->GetSelectionList ( ));	// index des mailles
				if (ids && ids->GetNumberOfTuples ( ) > 0)
				{
					vtkProp*	prop	= vtkProp::SafeDownCast (node->GetProperties ( )->Get (vtkSelectionNode::PROP ( )));
					vtkActor*	actor	= vtkActor::SafeDownCast (prop);
					if (actor)
					{
						vtkDataSet*	dataset	= actor->GetMapper ( )->GetInput ( );
						// ids contient les IDs des mailles sélectionnées dans dataset
						for (vtkIdType j = 0; j < ids->GetNumberOfTuples ( ); ++j)
						{
							vtkIdType	cellId	= ids->GetValue (j);
cout << cellId << ", ";
							// Exemple : récupérer la maille correspondante
							vtkCell*	cell	= dataset->GetCell (cellId);
							// Faites ce que vous voulez avec cell
						}	// for (vtkIdType j = 0; j < ids->GetNumberOfTuples ( ); ++j)
					}	// if (actor)
				}	// if (ids && ids->GetNumberOfTuples ( ) > 0)
cout << endl;
			}	// for (unsigned int n = 0; n < selection->GetNumberOfNodes ( ); n++)
			
			rwi->GetRenderWindow ( )->Render ( );
		}	// if (VTKIS_RUBBER_BAND == GetState ( ))
	}
	catch (...)
	{
		cout << __FILE__ << ' ' << __LINE__ << " VTKHardwareSelectionInteractorStyle::OnLeftButtonDown. EXC CAUGHT : " << endl;
	}
}	// VTKHardwareSelectionInteractorStyle::OnLeftButtonDown


vtkHardwareSelector& VTKHardwareSelectionInteractorStyle::GetHardwareSelector ( )
{
	assert ((0 != HardwareSelector.Get ( )) && "VTKHardwareSelectionInteractorStyle::GetHardwareSelector : null selector.");
	return *(HardwareSelector.Get ( ));
}	// VTKHardwareSelectionInteractorStyle::GetHardwareSelector 


void VTKHardwareSelectionInteractorStyle::RedrawRubberBand ( )
{
	vtkRenderWindowInteractor*	rwi	= this->Interactor;
	if ((0 == rwi) || (0 == rwi->GetRenderWindow ( )))
		return;

	TmpPixelArray->DeepCopy (PixelArray);
	unsigned char*		pixels	= TmpPixelArray->GetPointer (0);
	const int* const	size	= rwi->GetRenderWindow ( )->GetSize ( );

	int min [2], max [2];

	min [0] = StartPosition [0] <= EndPosition [0] ? StartPosition [0] : EndPosition [0];
	if (min [0] < 0)
		min [0] = 0;
	if (min [0] >= size [0])
		min [0] = size [0] - 1;

	min [1] = StartPosition [1] <= EndPosition [1] ? StartPosition [1] : EndPosition [1];
	if (min [1] < 0)
		min [1] = 0;
	if (min [1] >= size [1])
		min [1] = size [1] - 1;

	max [0] = EndPosition [0] > StartPosition [0] ?	EndPosition [0] : StartPosition [0];
	if (max [0] < 0)
		max [0] = 0;
	if (max [0] >= size [0])
		max [0] = size [0] - 1;

	max [1] = EndPosition [1] > StartPosition [1] ?	EndPosition [1] : StartPosition [1];
	if (max [1] < 0)
		max [1] = 0;
	if (max [1] >= size [1])
		max [1] = size [1] - 1;

	for (int i = min [0]; i <= max [0]; i++)
	{
		pixels [4 * (min [1]*size [0] + i)] = 255 ^ pixels [4 * (min [1]*size [0] + i)];
		pixels [4 * (min [1]*size [0] + i) + 1] = 255 ^ pixels [4 * (min [1]*size [0] + i) + 1];
		pixels [4 * (min [1]*size [0] + i) + 2] = 255 ^ pixels [4 * (min [1]*size [0] + i) + 2];
		pixels [4 * (max [1]*size [0] + i)] = 255 ^ pixels [4 * (max [1]*size [0] + i)];
		pixels [4 * (max [1]*size [0] + i) + 1] = 255 ^ pixels [4 * (max [1]*size [0] + i) + 1];
		pixels [4 * (max [1]*size [0] + i) + 2] = 255 ^ pixels [4 * (max [1]*size [0] + i) + 2];
	}	// for (i = min [0]; i <= max [0]; i++)
	for (int i = min [1] + 1; i < max [1]; i++)
	{
		pixels [4 * (i * size [0] + min [0])] = 255 ^ pixels [4 * (i * size [0] + min [0])];
		pixels [4 * (i * size [0] + min [0]) + 1] = 255 ^ pixels [4 * (i * size [0] + min [0]) + 1];
		pixels [4 * (i * size [0] + min [0]) + 2] = 255 ^ pixels [4 * (i * size [0] + min [0]) + 2];
		pixels [4 * (i * size [0] + max [0])] = 255 ^ pixels [4 * (i * size [0] + max [0])];
		pixels [4 * (i * size [0] + max [0]) + 1] = 255 ^ pixels [4 * (i * size [0] + max [0]) + 1];
		pixels [4 * (i * size [0] + max [0]) + 2] = 255 ^ pixels [4 * (i * size [0] + max [0]) + 2];
	}	// for (i = min [1] + 1; i < max [1]; i++)

	// Si on est en double buffering on met l'image dans le back buffer, Frame ( )la rebasculera dans le front buffer, sinon on la met dans le front buffer.
	rwi->GetRenderWindow ( )->SetRGBACharPixelData (0, 0, size [0] - 1, size [1] - 1, pixels, !rwi->GetRenderWindow ( )->GetDoubleBuffer ( ));
	rwi->GetRenderWindow ( )->Frame ( );
}	// VTKHardwareSelectionInteractorStyle::RedrawRubberBand

