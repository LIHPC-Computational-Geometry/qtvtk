#include "QtVtk/vtkSimpleTriangleFilter.h"

#include "vtkCellArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkTriangle.h"

vtkStandardNewMacro(vtkSimpleTriangleFilter);


vtkSimpleTriangleFilter::vtkSimpleTriangleFilter ( )
	: vtkPolyDataAlgorithm ( )
{
	this->SetNumberOfInputPorts (1);
	this->SetNumberOfOutputPorts (1);
}   // vtkSimpleTriangleFilter::vtkSimpleTriangleFilter


vtkSimpleTriangleFilter::~vtkSimpleTriangleFilter ( )
{
}   // vtkSimpleTriangleFilter::~vtkSimpleTriangleFilter


void vtkSimpleTriangleFilter::PrintSelf (ostream &os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}   // vtkSimpleTriangleFilter::PrintSelf


int vtkSimpleTriangleFilter::RequestData (vtkInformation* vtkNotUsed (request), vtkInformationVector** inputVector, vtkInformationVector* outputVector)
{
	vtkInformation* inInfo  = inputVector[0]->GetInformationObject (0);
	vtkInformation* outInfo = outputVector->GetInformationObject (0);
	if ((0 == inInfo) || (0 == outInfo))
		return 0;
	vtkPolyData*    input   = vtkPolyData::SafeDownCast (inInfo->Get (vtkDataObject::DATA_OBJECT ( )));
	vtkPolyData*    output  = vtkPolyData::SafeDownCast (outInfo->Get (vtkDataObject::DATA_OBJECT ( )));
	if ((0 == input) || (0 == output))
		return 0;

	vtkCellArray*   cellArray   = input->GetPolys ( );
	vtkCellArray*   newCells    = vtkCellArray::New ( );
	assert (0 != cellArray && "vtkSimpleTriangleFilter::RequestData : null cell array");
	assert (0 != newCells && "vtkSimpleTriangleFilter::RequestData : null cell array");
	output->SetPoints (input->GetPoints ( ));
	const vtkIdType   polysNum   = input->GetNumberOfPolys ( );

	vtkCellArray*   newPolys    = vtkCellArray::New ( );
	newPolys->Allocate (polysNum * 2, polysNum);
	output->SetPolys (newPolys);
	vtkIdType   npts        = 0;
#if VTK_MAJOR_VERSION < 9  
	vtkIdType*  pts         = 0;
#else 	// VTK_MAJOR_VERSION < 9
	const vtkIdType*  pts         = 0;  
#endif	// VTK_MAJOR_VERSION < 9  
	for (cellArray->InitTraversal ( ); cellArray->GetNextCell (npts, pts); )
	{
		switch (npts)
		{
			case 0  :
				continue;
			case 3  :
				newPolys->InsertNextCell(3, pts);
				break;
			case 4  :
				newPolys->InsertNextCell(3, pts);
				newPolys->InsertNextCell(3, (pts + 1));
				break;
		}   // switch (npts)
	}   // for (cellArray->InitTraversal ( ); cellArray->GetNextCell (npts, pts); cellNum++)
	output->Squeeze ( );

	return 1;
}   // vtkSimpleTriangleFilter::RequestData


int vtkSimpleTriangleFilter::FillInputPortInformation (int port, vtkInformation* info)
{
	if (!this->Superclass::FillInputPortInformation (port, info))
		return 0;
	if (port == 0)
		info->Set (vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE ( ), "vtkPolyData");
	else if (port == 1)
	{
		info->Set (vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE ( ), "vtkPolyData");
		info->Set (vtkAlgorithm::INPUT_IS_OPTIONAL ( ), 0);
	}

	return 1;
}   // vtkSimpleTriangleFilter::FillInputPortInformation
