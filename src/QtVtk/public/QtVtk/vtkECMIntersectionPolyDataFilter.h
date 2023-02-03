/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkECMIntersectionPolyDataFilter.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkECMIntersectionPolyDataFilter
// .SECTION Description
//
// vtkECMIntersectionPolyDataFilter computes the intersection between two
// vtkPolyData objects. The first output is a set of lines that marks
// the intersection of the input vtkPolyData objects. The second and
// third outputs are the first and second input vtkPolyData,
// respectively. Optionally, the two output vtkPolyData can be split
// along the intersection lines.
//
// This code was contributed in the Insight Journal paper:
// "Boolean Operations on Surfaces in VTK Without External Libraries"
// by Cory Quammen, Chris Weigle C., Russ Taylor
// http://hdl.handle.net/10380/3262
// http://www.insight-journal.org/browse/publication/797

#ifndef __vtkECMIntersectionPolyDataFilter_h
#define __vtkECMIntersectionPolyDataFilter_h

#include "vtkPolyDataAlgorithm.h"
// CP : new code
#include <vtkImplicitFunction.h>
#include <set>
// !CP : new code

/**
 * NOTES CP :
 * ATTENTION : cette classe ne fonctionne pas convenablement si une dimension de
 * la boite englobante de la seconde surface (probablement aussi de la première)
 * est supérieure à 500.
 * Contournement : homothétie sur le maillage.
 * Raison : non complétement élucidée, mais les calculs intermédiaires donnent
 * des valeurs très grandes, les epsilons utilisés, en absolus, sont très
 * faibles, ...
 * Par ailleurs cette classe repose sur la classe  vtkOBBTree, qui a dans ses
 * commentaires :
 *     Since this algorithms works from a list of cells, the OBB tree will only
 *     bound the "geometry" attached to the cells if the convex hull of the
 *     cells bounds the geometry.
 *     Long, skinny cells (i.e., cells with poor aspect ratio) may cause
 *     unsatisfactory results. This is due to the fact that this is a top-down
 *     implementation of the OBB tree, requiring that one or more complete
 *     cells are contained in each OBB. This requirement makes it hard to find
 *     good split planes during the recursion process. A bottom-up
 *     implementation would go a long way to correcting this problem.
 * Les mailles du faisceau sont très longues (plusieurs mètres) et de largeur
 * de l'ordre du micron. Mais même raccourcies fortement le problème demeure.
 * Jouer sur les différentes tolérances semble sans effet.
 * Changer de locateur (vtkOBBTree -> ???) n'a pas été testé car
 * l'implémentation repose sur des spécificités de cette classe.
 * Contournement :
 * La simple réduction par homothétie du maillage suffit à rendre ce filtre
 * opérant ...
 */
#ifndef VTK_5
class vtkECMIntersectionPolyDataFilter :
#else	// VTK_5
class VTK_GRAPHICS_EXPORT vtkECMIntersectionPolyDataFilter :
#endif	// VTK_5
													public vtkPolyDataAlgorithm
{
public:

	// CP : new code. Valeurs code VTK original : 1e-6 et 1e-9
	static double	tolerance;
	static double	fineTolerance;
	// !CP new code

  static vtkECMIntersectionPolyDataFilter *New();
  vtkTypeMacro(vtkECMIntersectionPolyDataFilter, vtkPolyDataAlgorithm);
  virtual void PrintSelf(ostream &os, vtkIndent indent);

  // Description:
  // If on, the second output will be the first input mesh split by the
  // intersection with the second input mesh. Defaults to on.
  vtkGetMacro(SplitFirstOutput, int);
  vtkSetMacro(SplitFirstOutput, int);
  vtkBooleanMacro(SplitFirstOutput, int);

  // Description:
  // If on, the third output will be the second input mesh split by the
  // intersection with the first input mesh. Defaults to on.
  vtkGetMacro(SplitSecondOutput, int);
  vtkSetMacro(SplitSecondOutput, int);
  vtkBooleanMacro(SplitSecondOutput, int);

  // Description:
  // Given two triangles defined by points (p1, q1, r1) and (p2, q2,
  // r2), returns whether the two triangles intersect. If they do,
  // the endpoints of the line forming the intersection are returned
  // in pt1 and pt2. The parameter coplanar is set to 1 if the
  // triangles are coplanar and 0 otherwise.
  static int TriangleTriangleIntersection(double p1[3], double q1[3], double r1[3],
                                          double p2[3], double q2[3], double r2[3],
                                          int &coplanar, double pt1[3], double pt2[3]);

	// CP : new code
	// Description : fonction d'assistance pour déterminer si une maille est à
	// l'intérieur ou à l'extérieur de la découpe, et tolérance appliquée lors
	// de l'évaluation.
	void SetIntersectionImplicitFunction (
						vtkImplicitFunction* function, double tolerance);

	// Description : extrémités d'un segment de droite permettant de définir
	// l'intérieur de la découpe. Ce segment est supposé "perpendiculaire" à la
	// découpe, la traverser en son milieu.
	// Dans le cas d'une surface traversée à plusieurs reprises (ex : peau d'un
	// volume), la surface intérieure est celle la plus proche du point "top".
	void DefineInner (double top[3], double base [3]);

	//
	// Description : retourne la surface intérieure issue de la seconde entrée.
	// Si recompute vaut true le spot est recalculé.
	vtkPolyData* GetInner2 (bool recompute);
	// !CP new code

protected:
  vtkECMIntersectionPolyDataFilter();
  ~vtkECMIntersectionPolyDataFilter();

  int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*);
  int FillInputPortInformation(int, vtkInformation*);

private:
  vtkECMIntersectionPolyDataFilter(const vtkECMIntersectionPolyDataFilter&); // Not implemented
  void operator=(const vtkECMIntersectionPolyDataFilter&); // Not implemented

  int SplitFirstOutput;
  int SplitSecondOutput;

  class Impl;

	// CP : new code
	double							_top [3], _base [3];
	double							_innerTolerance;
	vtkImplicitFunction*			_intersectionFunction;
	vtkPolyData*					_inner2;
	vtkPolyData*					_output2;
	std::set<vtkIdType>				_innerCellsIds;
	std::set<vtkIdType>				_innerPointsSet;
	// !CP new code
};	// vtkECMIntersectionPolyDataFilter


#endif // __vtkECMIntersectionPolyDataFilter_h
