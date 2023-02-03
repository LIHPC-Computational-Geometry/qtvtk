/**
 * .NAME vtkSimpleTriangleFilter - Triangularise les quads (sans se poser de question), transmet les triangles.
 *
 * Tout reste à faire en dehors de la triangularisation qui est basique au possible. L'idée est ici de la faire
 * en toutes circonstances, même si le triangle est dégénéré ou presque.
 */
#ifndef VTK_SIMPLE_TRIANGLE_FILTER_H
#define VTK_SIMPLE_TRIANGLE_FILTER_H

#include "vtkPolyDataAlgorithm.h"


class vtkSimpleTriangleFilter :	public vtkPolyDataAlgorithm
{
	public:

	static vtkSimpleTriangleFilter* New ( );
	vtkTypeMacro (vtkSimpleTriangleFilter, vtkPolyDataAlgorithm);
	virtual void PrintSelf (ostream &os, vtkIndent indent);

	protected :

	vtkSimpleTriangleFilter ( );
	~vtkSimpleTriangleFilter ( );

	int RequestData (vtkInformation*, vtkInformationVector**, vtkInformationVector*);
	int FillInputPortInformation (int, vtkInformation*);


	private:

	vtkSimpleTriangleFilter (const vtkSimpleTriangleFilter&);
	void operator = (const vtkSimpleTriangleFilter&);
};	// vtkSimpleTriangleFilter


#endif  // VTK_SIMPLE_TRIANGLE_FILTER_H
