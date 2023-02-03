/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkECMIntersectionPolyDataFilter.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "QtVtk/vtkECMIntersectionPolyDataFilter.h"

#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkDelaunay2D.h"
#include "vtkDoubleArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkLine.h"
#include "vtkObjectFactory.h"
#include "vtkOBBTree.h"
#include "vtkPlane.h"
#include "vtkPointData.h"
#include "vtkPointLocator.h"
#include "vtkSmartPointer.h"
#include "vtkSortDataArray.h"
#include "vtkTransform.h"
#include "vtkTriangle.h"

#include "vtkActor.h"
#include "vtkCamera.h"
#include "vtkCellPicker.h"
#include "vtkPolyDataMapper.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"

#include <map>
#include <queue>
// CP : new code
#include <vtkIdTypeArray.h>
#include <set>
#include <QtVtk/vtkPatchedDelaunay2D.h>
// !CP new code


// CP : new code.	Valeur code VTK original : 1e-6
double	vtkECMIntersectionPolyDataFilter::tolerance		= 1E-6;
double	vtkECMIntersectionPolyDataFilter::fineTolerance	= 1E-9;


static double distance2 (double c1 [3], double c2 [3])
{
	return (c1 [0] - c2 [0]) * (c1 [0] - c2 [0]) +
			(c1 [1] - c2 [1]) * (c1 [1] - c2 [1]) +
			(c1 [2] - c2 [2]) * (c1 [2] - c2 [2]);
}	// distance2


//
// Etend la surface innerCells de proche en proche en partant des mailles
// dont les ids sont portés par seeds et définies dans input.
// frontier contient les ids des noeuds définissant la frontière de la surface.
// innerCellsSet contient les ids des mailles déjà évaluées, innerNodesSet les
// ids des noeuds déjà évalués.
// Cette fonction actualise tous les sets, y compris seeds, et est donc à
// appeler tant que seeds n'est pas vide.
//
void extendSurface (
	vtkCellArray& innerCells, std::set<vtkIdType>& seeds, vtkPolyData& input,
	std::set<vtkIdType>& frontier, std::set<vtkIdType>& innerCellsSet,
	std::set<vtkIdType>& innerNodesSet,
	vtkImplicitFunction* function, double innerTolerance)
{
	std::set<vtkIdType>			futurSeeds;
	vtkSmartPointer<vtkIdList>	cellsIds	=
									vtkSmartPointer<vtkIdList>::New ( );
	for (std::set<vtkIdType>::iterator its = seeds.begin ( );
	     seeds.end ( ) != its; its++)
	{
		vtkCell*	cell	= input.GetCell (*its);
		innerCells.InsertNextCell (cell);
		innerCellsSet.insert (*its);

		const vtkIdType	nodeNum	= cell->GetNumberOfPoints ( );
		for (vtkIdType n = 0; n < nodeNum; n++)
		{
			const vtkIdType	nodeId	= cell->GetPointId (n);
			if ((frontier.end ( ) == frontier.find (nodeId)) &&
			    (innerNodesSet.end ( ) == innerNodesSet.find (nodeId)))
			{
				innerNodesSet.insert (nodeId);
				input.GetPointCells (nodeId, cellsIds);
				for (vtkIdType c = 0; c < cellsIds->GetNumberOfIds ( ); c++)
				{
					const vtkIdType	cellId	= cellsIds->GetId (c);
					if (innerCellsSet.end ( ) == innerCellsSet.find (cellId))
					{
						vtkCell*	futurCell		= input.GetCell(cellId);
						vtkIdType	futurNodeNum	=
											futurCell->GetNumberOfPoints ( );
						bool	inner	= true;
						for (vtkIdType fn = 0; fn < futurNodeNum; fn++)
						{
							vtkIdType	id	= futurCell->GetPointId (fn);
							if (function->FunctionValue (input.GetPoints (
											)->GetPoint (id)) > innerTolerance)
							{
								inner	= false;
								break;
							}
						}
						if (true == inner)
							futurSeeds.insert (cellId);
					}	// if (innerCellsSet.end ( ) == ...
				}	// for (vtkIdType c = 0; c < ...
			}	// if (frontier.end ( ) == frontier.find (nodeId))
		}	// for (vtkIdType n = 0; n < nodeNum; n++)
	}	// for (std::set<vtkIdType>::iterator its = seeds.begin ( ); ...

	seeds	= futurSeeds;
}	// extendSurface


// !CP new code


//----------------------------------------------------------------------------
// Helper typedefs and data structure.
typedef std::multimap< vtkIdType, vtkIdType >    IntersectionMapType;
typedef IntersectionMapType::iterator            IntersectionMapIteratorType;

//typedef std::pair< vtkIdType, vtkIdType >            CellEdgePairType;
typedef struct _CellEdgeLine {
  vtkIdType CellId;
  vtkIdType EdgeId;
  vtkIdType LineId;
} CellEdgeLineType;

typedef std::multimap< vtkIdType, CellEdgeLineType > PointEdgeMapType;
typedef PointEdgeMapType::iterator                   PointEdgeMapIteratorType;


//----------------------------------------------------------------------------
// Private implementation to hide STL.
//----------------------------------------------------------------------------
class vtkECMIntersectionPolyDataFilter::Impl
{
public:
  Impl();
  virtual ~Impl();

  static int FindTriangleIntersections(vtkOBBNode *node0, vtkOBBNode *node1,
                                       vtkMatrix4x4 *transform, void *arg);

  int SplitMesh(int inputIndex, vtkPolyData *output,
                vtkPolyData *intersectionLines);


protected:

  vtkCellArray* SplitCell(vtkPolyData *input, vtkIdType cellId,
                          vtkIdType *cellPts, IntersectionMapType *map,
                          vtkPolyData *interLines);

  void AddToPointEdgeMap(int index, vtkIdType ptId, double x[3],
                         vtkPolyData *mesh, vtkIdType cellId,
                         vtkIdType edgeId, vtkIdType lineId,
                         vtkIdType triPts[3]);

  void SplitIntersectionLines(int inputIndex, vtkPolyData *sourceMesh,
                              vtkPolyData *splitLines);

public:
  vtkPolyData         *Mesh[2];
  vtkOBBTree          *OBBTree1;

  // Stores the intersection lines.
  vtkCellArray        *IntersectionLines;

  // Cell data that indicates in which cell each intersection
  // lies. One array for each output surface.
  vtkIdTypeArray      *CellIds[2];

  // Map from points to the cells that contain them. Used for point
  // data interpolation. For points on the edge between two cells, it
  // does not matter which cell is recorded bcause the interpolation
  // will be the same.  One array for each output surface.
  vtkIdTypeArray      *PointCellIds[2];

  // Merging filter used to convert intersection lines from "line
  // soup" to connected polylines.
  vtkPointLocator     *PointMerger;

  // Map from cell ID to intersection line.
  IntersectionMapType *IntersectionMap[2];

  // Map from point to an edge on which it resides, the ID of the
  // cell, and the ID of the line.
  PointEdgeMapType    *PointEdgeMap[2];

// CP : new code
	vtkImplicitFunction*	_intersectionFunction;
	double					_evaluateTolerance;
	std::set<vtkIdType>		_innerCellsIds;
	std::set<vtkIdType>		_innerPointsSet;
// !CP new code

protected:
  Impl(const Impl&); // purposely not implemented
  void operator=(const Impl&); // purposely not implemented
};

//----------------------------------------------------------------------------
vtkECMIntersectionPolyDataFilter::Impl::Impl() :
  OBBTree1(0), IntersectionLines(0), PointMerger(0),
	_evaluateTolerance (0.), _intersectionFunction (0)	// CP
{
  for (int i = 0; i < 2; i++)
    {
    this->Mesh[i]            = NULL;
    this->CellIds[i]         = NULL;
    this->PointCellIds [i]   = NULL;					// CP
    this->IntersectionMap[i] = new IntersectionMapType();
    this->PointEdgeMap[i]    = new PointEdgeMapType();
    }
}

//----------------------------------------------------------------------------
vtkECMIntersectionPolyDataFilter::Impl::~Impl()
{
	_intersectionFunction	= 0;	// CP, not registered
  for (int i = 0; i < 2; i++)
    {
    delete this->IntersectionMap[i];
    delete this->PointEdgeMap[i];
    }
}


//----------------------------------------------------------------------------
int vtkECMIntersectionPolyDataFilter::Impl
::FindTriangleIntersections(vtkOBBNode *node0, vtkOBBNode *node1,
                            vtkMatrix4x4 *transform, void *arg)
{
  vtkECMIntersectionPolyDataFilter::Impl *info =
    reinterpret_cast<vtkECMIntersectionPolyDataFilter::Impl*>(arg);

  vtkPolyData     *mesh0                = info->Mesh[0];
  vtkPolyData     *mesh1                = info->Mesh[1];
  vtkOBBTree      *obbTree1             = info->OBBTree1;
  vtkCellArray    *intersectionLines    = info->IntersectionLines;
  vtkIdTypeArray  *intersectionCellIds0 = info->CellIds[0];
  vtkIdTypeArray  *intersectionCellIds1 = info->CellIds[1];
  vtkPointLocator *pointMerger          = info->PointMerger;

  int numCells0 = node0->Cells->GetNumberOfIds();
  int retval = 0;

  for (vtkIdType id0 = 0; id0 < numCells0; id0++)
    {
    vtkIdType cellId0 = node0->Cells->GetId(id0);
    int type0 = mesh0->GetCellType(cellId0);

    if (type0 == VTK_TRIANGLE)
      {
#if VTK_MAJOR_VERSION < 9
      vtkIdType npts0, *triPtIds0;
#else	// VTK_MAJOR_VERSION < 9
		vtkIdType npts0	= 0;
		const vtkIdType*	triPtIds0;
#endif	// VTK_MAJOR_VERSION < 9
      mesh0->GetCellPoints(cellId0, npts0, triPtIds0);
      double triPts0[3][3];
      for (vtkIdType id = 0; id < npts0; id++)
        {
        mesh0->GetPoint(triPtIds0[id], triPts0[id]);
        }

      if (obbTree1->TriangleIntersectsNode
          (node1, triPts0[0], triPts0[1], triPts0[2], transform))
        {
        int numCells1 = node1->Cells->GetNumberOfIds();
        for (vtkIdType id1 = 0; id1 < numCells1; id1++)
          {
          vtkIdType cellId1 = node1->Cells->GetId(id1);
          int type1 = mesh1->GetCellType(cellId1);
          if (type1 == VTK_TRIANGLE)
            {
            // See if the two cells actually intersect. If they do,
            // add an entry into the intersection maps and add an
            // intersection line.
#if VTK_MAJOR_VERSION < 9
            vtkIdType npts1, *triPtIds1;
#else	// VTK_MAJOR_VERSION < 9
			vtkIdType npts1	= 0;
			const vtkIdType*	triPtIds1;
#endif	// VTK_MAJOR_VERSION < 9
            mesh1->GetCellPoints(cellId1, npts1, triPtIds1);

            double triPts1[3][3];
            for (vtkIdType id = 0; id < npts1; id++)
              {
              mesh1->GetPoint(triPtIds1[id], triPts1[id]);
              }

            int coplanar = 0;
            double outpt0[3], outpt1[3];
            int intersects =
              vtkECMIntersectionPolyDataFilter::TriangleTriangleIntersection
              (triPts0[0], triPts0[1], triPts0[2],
               triPts1[0], triPts1[1], triPts1[2],
               coplanar, outpt0, outpt1);

            if ( coplanar )
              {
              // Coplanar triangle intersection is not handled.
              // This intersection will not be included in the output.
              intersects = 0;
              continue;
              }

            if ( intersects &&
                 ( outpt0[0] != outpt1[0] ||
                   outpt0[1] != outpt1[1] ||
                   outpt0[2] != outpt1[2] ) )
              {
              vtkIdType lineId = intersectionLines->GetNumberOfCells();
              intersectionLines->InsertNextCell(2);

              vtkIdType ptId0, ptId1;
              pointMerger->InsertUniquePoint(outpt0, ptId0);
              pointMerger->InsertUniquePoint(outpt1, ptId1);
              intersectionLines->InsertCellPoint(ptId0);
              intersectionLines->InsertCellPoint(ptId1);

              intersectionCellIds0->InsertNextValue(cellId0);
              intersectionCellIds1->InsertNextValue(cellId1);

              info->PointCellIds[0]->InsertValue( ptId0, cellId0 );
              info->PointCellIds[0]->InsertValue( ptId1, cellId0 );
              info->PointCellIds[1]->InsertValue( ptId0, cellId1 );
              info->PointCellIds[1]->InsertValue( ptId1, cellId1 );

              info->IntersectionMap[0]->insert(std::make_pair(cellId0, lineId));
              info->IntersectionMap[1]->insert(std::make_pair(cellId1, lineId));

              // Check which edges of cellId0 and cellId1 outpt0 and
              // outpt1 are on, if any.
              for (vtkIdType edgeId = 0; edgeId < 3; edgeId++)
                {
                info->AddToPointEdgeMap(0, ptId0, outpt0, mesh0, cellId0,
                                        edgeId, lineId, (vtkIdType*)triPtIds0);
                info->AddToPointEdgeMap(0, ptId1, outpt1, mesh0, cellId0,
                                        edgeId, lineId, (vtkIdType*)triPtIds0);
                info->AddToPointEdgeMap(1, ptId0, outpt0, mesh1, cellId1,
                                        edgeId, lineId, (vtkIdType*)triPtIds1);
                info->AddToPointEdgeMap(1, ptId1, outpt1, mesh1, cellId1,
                                        edgeId, lineId, (vtkIdType*)triPtIds1);
                }
              retval++;
              }
            }
          }
        }
      }
    }

  return retval;
}


//----------------------------------------------------------------------------
int vtkECMIntersectionPolyDataFilter::Impl
::SplitMesh(int inputIndex, vtkPolyData *output, vtkPolyData *intersectionLines)
{
// CP : new code
	double	center [3] = { 0., 0., 0. };
	intersectionLines->GetCenter (center);
	std::set<vtkIdType>	innerCellsSet;
	_innerPointsSet.clear ( );
// !CP new code
  vtkPolyData *input = this->Mesh[inputIndex];
  IntersectionMapType *intersectionMap = this->IntersectionMap[inputIndex];
  vtkCellData *inCD  = input->GetCellData();
  vtkCellData *outCD = output->GetCellData();
  vtkIdType numCells = input->GetNumberOfCells();
  vtkIdType newId;
  vtkIdType cellIdX = 0;

  //
  // Process points
  //
  vtkIdType inputNumPoints = input->GetPoints()->GetNumberOfPoints();
  vtkSmartPointer< vtkPoints > points = vtkSmartPointer< vtkPoints >::New();
  points->Allocate(100);
  output->SetPoints(points);

  //
  // Split intersection lines. The lines structure is constructed
  // using a vtkPointLocator. However, some lines may have an endpoint
  // on a cell edge that has no neighbor. We need to duplicate a line
  // point in such a case and update the point ID in the line cell.
  //
  vtkSmartPointer< vtkPolyData > splitLines =
    vtkSmartPointer <vtkPolyData >::New();
  splitLines->DeepCopy(intersectionLines);
  this->SplitIntersectionLines( inputIndex, input, splitLines );

  vtkPointData *inPD  = input->GetPointData();
  vtkPointData *outPD = output->GetPointData();
  outPD->CopyAllocate( inPD, input->GetNumberOfPoints() );

  // Copy over the point data from the input
  for (vtkIdType ptId = 0; ptId < inputNumPoints; ptId++)
    {
    double pt[3];
    input->GetPoints()->GetPoint(ptId, pt);
    output->GetPoints()->InsertNextPoint(pt);
    outPD->CopyData(inPD, ptId, ptId);
    }

  // Copy the points from splitLines to the output, interpolating the
  // data as we go.
  for (vtkIdType id = 0; id < splitLines->GetNumberOfPoints(); id++)
    {
    double pt[3];
    splitLines->GetPoint(id, pt);
    vtkIdType newPtId = output->GetPoints()->InsertNextPoint(pt);

    // Retrieve the cell ID from splitLines
    vtkIdType cellId = this->PointCellIds[inputIndex]->GetValue( id );

    double closestPt[3], pcoords[3], dist2, weights[3];
    int subId;
    vtkCell *cell = input->GetCell(cellId);
    cell->EvaluatePosition(pt, closestPt, subId, pcoords, dist2, weights);
    outPD->InterpolatePoint(input->GetPointData(), newPtId, cell->PointIds,
                            weights);
    }

  //
  // Process cells
  //
  outCD->CopyAllocate(inCD, numCells);
  if ( input->GetPolys()->GetNumberOfCells() > 0 )
    {
    vtkCellArray *cells = input->GetPolys();
    newId = output->GetNumberOfCells();

    vtkSmartPointer< vtkCellArray > newPolys =
      vtkSmartPointer< vtkCellArray >::New();

    newPolys->EstimateSize(cells->GetNumberOfCells(),3);
    output->SetPolys(newPolys);

    vtkSmartPointer< vtkIdList > edgeNeighbors =
      vtkSmartPointer< vtkIdList >::New();
    vtkIdType nptsX = 0;
#if VTK_MAJOR_VERSION < 9
    vtkIdType *pts = 0;
#else 	// VTK_MAJOR_VERSION < 9
	const vtkIdType *pts = 0;
#endif	// VTK_MAJOR_VERSION < 9
    vtkSmartPointer< vtkIdList > cellsToCheck =
      vtkSmartPointer< vtkIdList >::New();
    for (cells->InitTraversal(); cells->GetNextCell(nptsX, pts); cellIdX++)
      {
      if ( nptsX != 3 )
        {
        vtkGenericWarningMacro( << "vtkECMIntersectionPolyDataFilter only works with "
                                << "triangle meshes." );
        continue;
        }

      cellsToCheck->Reset();
      cellsToCheck->Allocate(nptsX+1);
      cellsToCheck->InsertNextId(cellIdX);

      // Collect the cells relevant for splitting this cell.  If the
      // cell is in the intersection map, split. If not, one of its
      // edges may be split by an intersection line that splits a
      // neighbor cell. Mark the cell as needing a split if this is
      // the case.
      bool needsSplit = intersectionMap->find( cellIdX ) != intersectionMap->end();
      for (vtkIdType ptId = 0; ptId < nptsX; ptId++)
        {
        vtkIdType pt0Id = pts[ptId];
        vtkIdType pt1Id = pts[(ptId+1) % nptsX];
        edgeNeighbors->Reset();
        input->GetCellEdgeNeighbors(cellIdX, pt0Id, pt1Id, edgeNeighbors);
        for (vtkIdType nbr = 0; nbr < edgeNeighbors->GetNumberOfIds(); nbr++)
          {
          vtkIdType nbrId = edgeNeighbors->GetId(nbr);
          cellsToCheck->InsertNextId(nbrId);

          if ( intersectionMap->find( nbrId ) != intersectionMap->end() )
            {
            needsSplit = true;
            }
          } // for (vtkIdType nbr = 0; ...
        } // for (vtkIdType pt = 0; ...

      // Splitting occurs here
      if ( !needsSplit )
        {
        // Just insert the cell and copy the cell data
        newId = newPolys->InsertNextCell(3, pts);
        outCD->CopyData(inCD, cellIdX, newId);
        }
      else
        {
		// CP : new code
		std::set<vtkIdType>		cutCellIds;
		for (int cci = 0; cci < 3; cci++)
			cutCellIds.insert (pts [cci]);
		// !CP new code
        vtkCellArray *splitCells = this->SplitCell
          (input, cellIdX, (vtkIdType*)pts, intersectionMap, splitLines);

        double pt0[3], pt1[3], pt2[3], normal[3];
        points->GetPoint(pts[0], pt0);
        points->GetPoint(pts[1], pt1);
        points->GetPoint(pts[2], pt2);
        vtkTriangle::ComputeNormal(pt0, pt1, pt2, normal);
        vtkMath::Normalize(normal);

#if VTK_MAJOR_VERSION < 9
        vtkIdType npts, *ptIds, subCellId = 0;
#else 	// #if VTK_MAJOR_VERSION < 9
		vtkIdType npts	= 0, subCellId = 0;
		const vtkIdType* ptIds;
#endif	// #if VTK_MAJOR_VERSION < 9
        for (splitCells->InitTraversal(); splitCells->GetNextCell(npts, ptIds); subCellId++)
          {
			// CP : new code
			// On recense les points de la frontière (ligne de découpe) :
			for (int c = 0; c < 3; c++)
				if (cutCellIds.find (ptIds [c]) == cutCellIds.end ( ))
					_innerPointsSet.insert (ptIds [c]);
			// !CP new code
          // Check for reversed cells. I'm not sure why, but in some
          // cases, cells are reversed.
          double subCellNormal[3];
          points->GetPoint(ptIds[0], pt0);
          points->GetPoint(ptIds[1], pt1);
          points->GetPoint(ptIds[2], pt2);
          vtkTriangle::ComputeNormal(pt0, pt1, pt2, subCellNormal);
          vtkMath::Normalize(subCellNormal);

          if ( vtkMath::Dot(normal, subCellNormal) > 0 )
            {
            newId = newPolys->InsertNextCell(npts, ptIds);
            }
          else
            {
            newId = newPolys->InsertNextCell(npts);
            for ( int i = 0; i < npts; i++)
              {
              newPolys->InsertCellPoint( ptIds[ npts-i-1 ] );
              }
            }

          outCD->CopyData(inCD, cellIdX, newId); // Duplicate cell data

			// CP : new code
			if ((_intersectionFunction->FunctionValue (pt0)
													<= _evaluateTolerance) &&
			    (_intersectionFunction->FunctionValue (pt1)
													<= _evaluateTolerance) &&
			    (_intersectionFunction->FunctionValue (pt2)
													<= _evaluateTolerance))
				_innerCellsIds.insert (newId);
			// !CP : new code
          }	// for (splitCells->InitTraversal(); 
        splitCells->Delete();
        }
      } // for (cells->InitTraversal(); ...
    }

  return 1;
}	// vtkECMIntersectionPolyDataFilter::Impl::SplitMesh


//----------------------------------------------------------------------------
vtkCellArray* vtkECMIntersectionPolyDataFilter::Impl
::SplitCell(vtkPolyData *input, vtkIdType cellId, vtkIdType *cellPts,
            IntersectionMapType *map, vtkPolyData *interLines)
{
  // Gather points from the cell
  vtkSmartPointer< vtkPoints > points = vtkSmartPointer< vtkPoints >::New();
  vtkSmartPointer< vtkPointLocator > merger =
    vtkSmartPointer< vtkPointLocator >::New();
//  merger->SetTolerance( 1e-6 );	// CP
	merger->SetTolerance (tolerance);	// CP
  merger->InitPointInsertion( points, input->GetBounds() );

  double xyz[3];
  for ( int i = 0; i < 3; i++)
    {
    if ( cellPts[i] >= input->GetNumberOfPoints() )
      {
      vtkGenericWarningMacro(<< "invalid point read 1");
      }
    input->GetPoint(cellPts[i], xyz);
    merger->InsertNextPoint( xyz );
    }

  // Set up line cells.
  vtkSmartPointer< vtkCellArray > lines =
    vtkSmartPointer< vtkCellArray >::New();

  double p0[3], p1[3], p2[3];
  input->GetPoint(cellPts[0], p0);
  input->GetPoint(cellPts[1], p1);
  input->GetPoint(cellPts[2], p2);

  // This maps the point IDs for the vtkPolyData passed to
  // vtkDelaunay2D back to the original IDs in interLines. NOTE: The
  // point IDs from the cell are not stored here.
  std::map< vtkIdType, vtkIdType > ptIdMap;

  IntersectionMapIteratorType iterLower = map->lower_bound( cellId );
  IntersectionMapIteratorType iterUpper = map->upper_bound( cellId );
  while ( iterLower != iterUpper )
    {
    vtkIdType lineId = iterLower->second;
#if VTK_MAJOR_VERSION < 9
    vtkIdType nLinePts, *linePtIds;
#else 	// #if VTK_MAJOR_VERSION < 9
	vtkIdType nLinePts	= 0;
	const vtkIdType*	linePtIds;
#endif	// #if VTK_MAJOR_VERSION < 9
    interLines->GetLines()->GetCell( 3*lineId, nLinePts, linePtIds );
    lines->InsertNextCell(2);
    for (vtkIdType i = 0; i < nLinePts; i++)
      {
      std::map< vtkIdType, vtkIdType >::iterator location =
        ptIdMap.find( linePtIds[i] );
      if ( location == ptIdMap.end() )
        {
        interLines->GetPoint( linePtIds[i], xyz );
        if ( linePtIds[i] >= interLines->GetNumberOfPoints() )
          {
          vtkGenericWarningMacro(<< "invalid point read 2");
          }
        merger->InsertUniquePoint( xyz, ptIdMap[ linePtIds[i] ] );
        lines->InsertCellPoint( ptIdMap[ linePtIds[i] ] );
        }
      else
        {
        lines->InsertCellPoint( location->second );
        }
      }
    ++iterLower;
    }

  // Now check the neighbors of the cell
  vtkSmartPointer< vtkIdList > nbrCellIds =
    vtkSmartPointer< vtkIdList >::New();
  for (vtkIdType i = 0; i < 3; i++)
    {
    vtkIdType edgePtId0 = cellPts[i];
    vtkIdType edgePtId1 = cellPts[(i+1) % 3];

    double edgePt0[3], edgePt1[3];
    if ( edgePtId0 >= input->GetNumberOfPoints() )
      {
      vtkGenericWarningMacro( << "invalid point read 3");
      }
    if ( edgePtId1 >= input->GetNumberOfPoints() )
      {
      vtkGenericWarningMacro( << "invalid point read 4");
      }
    input->GetPoint(edgePtId0, edgePt0);
    input->GetPoint(edgePtId1, edgePt1);

    nbrCellIds->Reset();
    input->GetCellEdgeNeighbors(cellId, edgePtId0, edgePtId1, nbrCellIds);
    for (vtkIdType j = 0; j < nbrCellIds->GetNumberOfIds(); j++)
      {
      vtkIdType nbrCellId = nbrCellIds->GetId( j );
      iterLower = map->lower_bound( nbrCellId );
      iterUpper = map->upper_bound( nbrCellId );
      while ( iterLower != iterUpper )
        {
        vtkIdType lineId = iterLower->second;
#if VTK_MAJOR_VERSION < 9        
        vtkIdType nLinePts, *linePtIds;
#else 	// #if VTK_MAJOR_VERSION < 9
		vtkIdType nLinePts	= 0;
		const vtkIdType*	linePtIds;
#endif	// #if VTK_MAJOR_VERSION < 9
        interLines->GetLines()->GetCell( 3*lineId, nLinePts, linePtIds );
        for (vtkIdType k = 0; k < nLinePts; k++)
          {
          double t, closestPt[3];

          if ( linePtIds[k] >= interLines->GetNumberOfPoints() )
            {
            vtkGenericWarningMacro( << "invalid point read 5");
            }
          interLines->GetPoint( linePtIds[k], xyz );
          double dist = vtkLine::DistanceToLine(xyz, edgePt0, edgePt1, t,
                                                closestPt);

//          if ( fabs(dist) < 1e-6 && t >= 0.0 && t <= 1.0 )	// CP
          if ( fabs(dist) < tolerance && t >= 0.0 && t <= 1.0 )	// CP
            {
            // Point is on edge. See if it is in the point ID map. If
            // not, add it as a point.
            if ( ptIdMap.find( linePtIds[k] ) == ptIdMap.end() )
              {
              merger->InsertUniquePoint( xyz, ptIdMap[ linePtIds[k] ] );
              }
            }
          }

        ++iterLower;
        }
      }
    }

  // Set up reverse ID map
  std::map< vtkIdType, vtkIdType > reverseIdMap;
  std::map< vtkIdType, vtkIdType >::iterator iter = ptIdMap.begin();
  while ( iter != ptIdMap.end() )
    {
    // If we have more than one point mapping back to the same point
    // in the input mesh, just use the first one. This will give a
    // preference for using cell points when an intersection line shares
    // a point with a a cell and prevent introducing accidental holes
    // in the mesh.
    if ( reverseIdMap.find( iter->second ) == reverseIdMap.end() )
      {
      reverseIdMap[ iter->second ] = iter->first + input->GetNumberOfPoints();
      }
    ++iter;
    }

  // Angle sorting procedure for splitting cell boundary lines

  double v0[3], v1[3], n[3], c[3];
  vtkTriangle::TriangleCenter( p0, p1, p2, c );
  vtkTriangle::ComputeNormal( p0, p1, p2, n );
  vtkMath::Perpendiculars( n, v0, v1, 0.0 );

  // For each point on an edge, compute it's relative angle about n.
  vtkSmartPointer< vtkIdTypeArray > edgePtIdList =
    vtkSmartPointer< vtkIdTypeArray >::New();
  edgePtIdList->Allocate( points->GetNumberOfPoints() );
  vtkSmartPointer< vtkDoubleArray > angleList =
    vtkSmartPointer< vtkDoubleArray >::New();
  angleList->Allocate( points->GetNumberOfPoints() );

  for ( vtkIdType ptId = 0; ptId < points->GetNumberOfPoints(); ptId++ )
    {
    double x[3], closestPt[3], t0, t1, t2;
    points->GetPoint( ptId, x );
    double d0 = vtkLine::DistanceToLine(x, p0, p1, t0, closestPt);
    double d1 = vtkLine::DistanceToLine(x, p1, p2, t1, closestPt);
    double d2 = vtkLine::DistanceToLine(x, p2, p0, t2, closestPt);

//    double epsilon = 1e-6;	// CP
    double epsilon = tolerance;	// CP
    if ( (ptId < 3) || // Cell points
         (d0 < epsilon && t0 >= 0.0 && t0 <= 1.0) ||
         (d1 < epsilon && t1 >= 0.0 && t1 <= 1.0) ||
         (d2 < epsilon && t2 >= 0.0 && t2 <= 1.0) )
      {
      // Point is on line. Add its id to id list and add its angle to
      // angle list.
      edgePtIdList->InsertNextValue( ptId );
      double d[3];
      vtkMath::Subtract( x, c, d );
      angleList->InsertNextValue( atan2( vtkMath::Dot(d, v0),
                                         vtkMath::Dot(d, v1) ) );
      }
    }

  // Sort the edgePtIdList according to the angle list. The starting
  // point doesn't matter. We just need to generate boundary lines in
  // a consistent order.
  vtkSortDataArray::Sort( angleList, edgePtIdList );

  // Now add lines from the sorted result.
  for ( vtkIdType id = 0; id < edgePtIdList->GetNumberOfTuples()-1; id++ )
    {
    lines->InsertNextCell(2);
    lines->InsertCellPoint( edgePtIdList->GetValue( id ) );
    lines->InsertCellPoint( edgePtIdList->GetValue( id + 1 ) );
    }
  lines->InsertNextCell(2);
  lines->InsertCellPoint
    ( edgePtIdList->GetValue( edgePtIdList->GetNumberOfTuples()-1 ) );
  lines->InsertCellPoint( edgePtIdList->GetValue( 0 ) );

  //
  // Set up vtkPolyData to feed to vtkDelaunay2D
  //
  vtkSmartPointer< vtkPolyData > pd = vtkSmartPointer< vtkPolyData >::New();
  pd->SetPoints( points );
  pd->SetLines( lines );

  // Set up a transform that will rotate the points to the
  // XY-plane (normal aligned with z-axis).
  vtkSmartPointer< vtkTransform > transform =
    vtkSmartPointer< vtkTransform >::New();
  double zaxis[3] = {0, 0, 1};
  double rotationAxis[3], normal[3], center[3], rotationAngle;

  double pt0[3], pt1[3], pt2[3];
  points->GetPoint( 0, pt0 );
  points->GetPoint( 1, pt1 );
  points->GetPoint( 2, pt2 );
  vtkTriangle::ComputeNormal(pt0, pt1, pt2, normal);

  double dotZAxis = vtkMath::Dot( normal, zaxis );
//  if ( fabs(1.0 - dotZAxis) < 1e-6 )	// CP
  if ( fabs(1.0 - dotZAxis) < tolerance )	// CP
    {
    // Aligned with z-axis
    rotationAxis[0] = 1.0;
    rotationAxis[1] = 0.0;
    rotationAxis[2] = 0.0;
    rotationAngle = 0.0;
    }
//  else if ( fabs( 1.0 + dotZAxis ) < 1e-6 )	// CP
  else if ( fabs( 1.0 + dotZAxis ) < tolerance )	// CP
    {
    // Co-linear with z-axis, but reversed sense.
    // Aligned with z-axis
    rotationAxis[0] = 1.0;
    rotationAxis[1] = 0.0;
    rotationAxis[2] = 0.0;
    rotationAngle = 180.0;
    }
  else
    {
    // The general case
    vtkMath::Cross(normal, zaxis, rotationAxis);
    vtkMath::Normalize(rotationAxis);
    rotationAngle =
      vtkMath::DegreesFromRadians(acos(vtkMath::Dot(zaxis, normal)));
    }

  transform->PreMultiply();
  transform->Identity();

  transform->RotateWXYZ(rotationAngle,
                        rotationAxis[0],
                        rotationAxis[1],
                        rotationAxis[2]);

  vtkTriangle::TriangleCenter(pt0, pt1, pt2, center);
  transform->Translate(-center[0], -center[1], -center[2]);

// CP : new code
//  vtkSmartPointer< vtkDelaunay2D > del2D =
//    vtkSmartPointer< vtkDelaunay2D >::New();
  vtkSmartPointer< vtkPatchedDelaunay2D > del2D =
    vtkSmartPointer< vtkPatchedDelaunay2D >::New();
// !CP : new code
#ifndef VTK_5
  del2D->SetInputData (pd);
#else	// VTK_5
  del2D->SetInput(pd);
#endif	// VTK_5
  del2D->SetSource(pd);
  del2D->SetTolerance(0.0);
  del2D->SetAlpha(0.0);
  del2D->SetOffset(10);
  del2D->SetProjectionPlaneMode(VTK_SET_TRANSFORM_PLANE);
  del2D->SetTransform(transform);
  del2D->BoundingTriangulationOff();
  del2D->Update();

  vtkCellArray *polys = del2D->GetOutput()->GetPolys();
  vtkCellArray *splitCells = vtkCellArray::New();
  splitCells->Allocate( 3*polys->GetNumberOfCells() );

  // Renumber the point IDs.
#if VTK_MAJOR_VERSION < 9   
  vtkIdType npts, *ptIds;
#else 	// #if VTK_MAJOR_VERSION < 9   
	vtkIdType npts	= 0;
	const vtkIdType*	ptIds;
#endif	// #if VTK_MAJOR_VERSION < 9   
  for (polys->InitTraversal(); polys->GetNextCell(npts, ptIds); )
    {
    if ( ptIds[0] >= points->GetNumberOfPoints() ||
         ptIds[1] >= points->GetNumberOfPoints() ||
         ptIds[2] >= points->GetNumberOfPoints() )
      {
      vtkGenericWarningMacro( << "Invalid point ID!!!");
      }

    splitCells->InsertNextCell( npts );
    for (int i = 0; i < npts; i++)
      {
      vtkIdType remappedPtId;
      if ( ptIds[i] < 3) // Point from the cell
        {
        remappedPtId = cellPts[ ptIds[i] ];
        }
      else
        {
        remappedPtId = reverseIdMap[ ptIds[i] ];
        }
      splitCells->InsertCellPoint( remappedPtId );
      }
    }

  return splitCells;
}

//----------------------------------------------------------------------------
void vtkECMIntersectionPolyDataFilter::Impl
::AddToPointEdgeMap(int index, vtkIdType ptId, double x[3], vtkPolyData *mesh,
                    vtkIdType cellId, vtkIdType edgeId, vtkIdType lineId,
                    vtkIdType triPtIds[3])
{
  vtkIdType edgePtId0 = triPtIds[edgeId];
  vtkIdType edgePtId1 = triPtIds[(edgeId+1) % 3];
  double pt0[3], pt1[3];

  mesh->GetPoint(edgePtId0, pt0);
  mesh->GetPoint(edgePtId1, pt1);

  // Check to see if this point-cell combo is already in the list
  PointEdgeMapIteratorType iterLower =
    this->PointEdgeMap[index]->lower_bound( ptId );
  PointEdgeMapIteratorType iterUpper =
    this->PointEdgeMap[index]->upper_bound( ptId );

  while (iterLower != iterUpper)
    {
    if ( iterLower->second.CellId == cellId )
      {
      return;
      }
    ++iterLower;
    }

  double t, dist, closestPt[3];
  dist = vtkLine::DistanceToLine(x, pt0, pt1, t, closestPt);
//  if (fabs(dist) < 1e-9 && t >= 0.0 && t <= 1.0)	// CP
  if (fabs(dist) < fineTolerance && t >= 0.0 && t <= 1.0)	// CP
    {
    CellEdgeLineType cellEdgeLine;
    cellEdgeLine.CellId = cellId;
    cellEdgeLine.EdgeId = edgeId;
    cellEdgeLine.LineId = lineId;
    this->PointEdgeMap[index]->insert(std::make_pair(ptId, cellEdgeLine));
    }
}

//----------------------------------------------------------------------------
void vtkECMIntersectionPolyDataFilter::Impl
::SplitIntersectionLines(int inputIndex, vtkPolyData *sourceMesh,
                         vtkPolyData *splitLines)
{
  vtkIdType numOrigPts = splitLines->GetNumberOfPoints();

  // This maps the points to a cell that contains them. It will be
  // used later for interpolating point data.
  for (vtkIdType ptId = 0; ptId < numOrigPts; ptId++)
    {
    double pt[3];
    splitLines->GetPoint(ptId, pt);

    // Iterate over all edges to which this point belongs.
    PointEdgeMapIteratorType iterLower =
      this->PointEdgeMap[inputIndex]->lower_bound( ptId );
    PointEdgeMapIteratorType iterUpper =
      this->PointEdgeMap[inputIndex]->upper_bound( ptId );

    bool firstSplit = true;
    while (iterLower != iterUpper)
      {
      CellEdgeLineType cellEdgeLine = iterLower->second;
      vtkIdType cellId = cellEdgeLine.CellId;
      vtkIdType edgeId = cellEdgeLine.EdgeId;
      vtkIdType lineId = cellEdgeLine.LineId;

#if VTK_MAJOR_VERSION < 9
      vtkIdType nCellPts, *cellPtIds, edgePtIds[2];
#else 	// #if VTK_MAJOR_VERSION < 9
		vtkIdType nCellPts	= 0, edgePtIds[2];
		const vtkIdType*	cellPtIds;
#endif 	// #if VTK_MAJOR_VERSION < 9
      sourceMesh->GetCellPoints( cellId, nCellPts, cellPtIds );
      edgePtIds[0] = cellPtIds[edgeId];
      edgePtIds[1] = cellPtIds[(edgeId+1) % nCellPts];

#if VTK_MAJOR_VERSION < 9
      vtkIdType nLinePts, *linePtIds, newLinePtIds[2];
#else 	// #if VTK_MAJOR_VERSION < 9
		vtkIdType nLinePts, newLinePtIds[2];
		const vtkIdType*	linePtIds;
#endif	// #if VTK_MAJOR_VERSION < 9   
      splitLines->GetLines()->GetCell( 3*lineId, nLinePts, linePtIds );
      newLinePtIds[0] = linePtIds[0];
      newLinePtIds[1] = linePtIds[1];

      // Insert a copy of the point only if it is not the first time
      // we've seen it and renumber the line points.
      vtkIdType newPtId = ptId;
      if ( !firstSplit )
        {
        // Insert a copy of the point
        newPtId = splitLines->GetPoints()->InsertNextPoint(pt);
        if ( linePtIds[0] == ptId )
          {
          newLinePtIds[0] = newPtId;
          }
        if ( linePtIds[1] == ptId )
          {
          newLinePtIds[1] = newPtId;
          }
        splitLines->GetLines()->ReplaceCell( 3*lineId, nLinePts, newLinePtIds );
        }
      firstSplit = false;

      // Set the cell with which the point is associated for later
      // interpolation of point data.
      this->PointCellIds[inputIndex]->InsertValue( newPtId, cellId );

      // Find all cells that share this edge. We need just one point
      // for all connected edges.
      vtkSmartPointer< vtkIdList > nbrCellIds =
        vtkSmartPointer< vtkIdList >::New();
      sourceMesh->GetCellEdgeNeighbors( cellId, edgePtIds[0], edgePtIds[1],
                                        nbrCellIds );

      // Remove these cell IDs from the point-edge map.
      PointEdgeMapIteratorType removalIterLower =
        this->PointEdgeMap[inputIndex]->lower_bound( ptId );
      PointEdgeMapIteratorType removalIterUpper =
        this->PointEdgeMap[inputIndex]->upper_bound( ptId );
      while ( removalIterLower != removalIterUpper )
        {
        vtkIdType candidateCellId = removalIterLower->second.CellId;
        if ( nbrCellIds->IsId( candidateCellId ) >= 0 )
          {
          vtkIdType nbrLineId = removalIterLower->second.LineId;
          splitLines->GetLines()->GetCell( 3*nbrLineId, nLinePts, linePtIds );
          newLinePtIds[0] = linePtIds[0];
          newLinePtIds[1] = linePtIds[1];

          if ( linePtIds[0] == ptId )
            {
            newLinePtIds[0] = newPtId;
            }
          if ( linePtIds[1] == ptId )
            {
            newLinePtIds[1] = newPtId;
            }

          splitLines->GetLines()->ReplaceCell( 3*nbrLineId, nLinePts, newLinePtIds );

          this->PointEdgeMap[inputIndex]->erase( removalIterLower++ );
          }
        else
          {
          ++removalIterLower;
          }
        }


      // Remove this entry from the map.
      this->PointEdgeMap[inputIndex]->erase( iterLower++ );
      }
    }
}

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkECMIntersectionPolyDataFilter);

//----------------------------------------------------------------------------
vtkECMIntersectionPolyDataFilter::vtkECMIntersectionPolyDataFilter()
  : SplitFirstOutput(1), SplitSecondOutput(1),
    _intersectionFunction (0), _innerTolerance (0.),	// CP
     _inner2 (0), _output2 (0), _innerCellsIds ( ), _innerPointsSet ( )	// CP
{
  this->SetNumberOfInputPorts(2);
  this->SetNumberOfOutputPorts(3);
	// CP : new code
	for (int i = 0; i < 3; i++)
		_top [i]	= _base [i]	= 0.;
	// !CP new code
}

//----------------------------------------------------------------------------
vtkECMIntersectionPolyDataFilter::~vtkECMIntersectionPolyDataFilter()
{
	// CP : new code
	if (0 != _intersectionFunction)
		_intersectionFunction->UnRegister (this);
	_intersectionFunction	= 0;
	if (0 != _inner2)
		_inner2->UnRegister (0);
	_inner2	= 0;
//	if (0 != _output2)
//		_output2->UnRegister (0);	// CP V 2.58.1
	_output2	= 0;
	// !CP new code
}

//----------------------------------------------------------------------------
void vtkECMIntersectionPolyDataFilter::PrintSelf(ostream &os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "SplitFirstOutput: " << this->SplitFirstOutput << "\n";
  os << indent << "SplitSecondOutput: " << this->SplitSecondOutput << "\n";
}

//----------------------------------------------------------------------------
// CP : new code
void vtkECMIntersectionPolyDataFilter::SetIntersectionImplicitFunction (
							vtkImplicitFunction* function, double tolerance)
{
	_innerTolerance	= tolerance;
	if (function != _intersectionFunction)
	{
		if (0 != _intersectionFunction)
			_intersectionFunction->Delete ( );
		_intersectionFunction	= function;
		if (0 != _intersectionFunction)
			_intersectionFunction->Register (this);
	}	// if (function != _intersectionFunction)
}	// vtkECMIntersectionPolyDataFilter::SetIntersectionImplicitFunction
// !CP new code

//----------------------------------------------------------------------------
// CP : new code
void vtkECMIntersectionPolyDataFilter::DefineInner (
												double top [3], double base [3])
{
	for (int i = 0; i < 3; i++)
	{
		_top [i]	= top [i];
		_base [i]	= base [i];
	}	// for (int i = 0; i < 3; i++)
}	// vtkECMIntersectionPolyDataFilter::DefineInner
// !CP new code

//----------------------------------------------------------------------------
// CP : new code
vtkPolyData* vtkECMIntersectionPolyDataFilter::GetInner2 (bool recompute)
{
	if (true == recompute)
	{
		if (0 != _inner2)
			_inner2->Delete ( );
		_inner2	= 0;
	}	// if (true == recompute)

	if (0 != _inner2)
		return _inner2;

	if (0 == _output2)
	{
		cerr << __FILE__ << ' ' << __LINE__ << " vtkECMIntersectionPolyDataFilter::GetInner2 : NULL input 2." << endl;
		return 0;
	}	// if (0 == _output2)
	_inner2	= vtkPolyData::New ( );
	_inner2->Initialize ( );
	_inner2->Allocate ( );
	vtkSmartPointer<vtkCellArray>	innerCells	=
										vtkSmartPointer<vtkCellArray>::New ( );
	innerCells->Initialize ( );
	_inner2->SetPoints (_output2->GetPoints ( ));

	if (0 != _innerCellsIds.size ( ))
	{
		std::set<vtkIdType>	newInnerCells	= _innerCellsIds;
		std::set<vtkIdType>	innerCellsSet, innerNodesSet;

		// Les points de la ligne de découpe :
		vtkPolyData*	line	= GetOutput ( );
		line->BuildCells ( );

		while (false == newInnerCells.empty ( ))
			extendSurface (
					*innerCells, newInnerCells, *_output2, _innerPointsSet,
					innerCellsSet, innerNodesSet,
					_intersectionFunction, _innerTolerance);
	}	// if (0 != _innerCellsIds.size ( ))
	_inner2->SetPolys (innerCells);
	_inner2->BuildCells ( );

	return _inner2;
}	// vtkECMIntersectionPolyDataFilter::GetInner2
// !CP new code

//----------------------------------------------------------------------------
int vtkECMIntersectionPolyDataFilter
::TriangleTriangleIntersection(double p1[3], double q1[3], double r1[3],
                               double p2[3], double q2[3], double r2[3],
                               int &coplanar, double pt1[3], double pt2[3])
{
  double n1[3], n2[3];

  // Compute supporting plane normals.
  vtkTriangle::ComputeNormal(p1, q1, r1, n1);
  vtkTriangle::ComputeNormal(p2, q2, r2, n2);
  double s1 = -vtkMath::Dot(n1, p1);
  double s2 = -vtkMath::Dot(n2, p2);

  // Compute signed distances of points p1, q1, r1 from supporting
  // plane of second triangle.
  double dist1[3];
  dist1[0] = vtkMath::Dot(n2, p1) + s2;
  dist1[1] = vtkMath::Dot(n2, q1) + s2;
  dist1[2] = vtkMath::Dot(n2, r1) + s2;

  // If signs of all points are the same, all the points lie on the
  // same side of the supporting plane, and we can exit early.
  if ((dist1[0]*dist1[1] > 0.0) && (dist1[0]*dist1[2] > 0.0))
    {
    return 0;
    }
  // Do the same for p2, q2, r2 and supporting plane of first
  // triangle.
  double dist2[3];
  dist2[0] = vtkMath::Dot(n1, p2) + s1;
  dist2[1] = vtkMath::Dot(n1, q2) + s1;
  dist2[2] = vtkMath::Dot(n1, r2) + s1;

  // If signs of all points are the same, all the points lie on the
  // same side of the supporting plane, and we can exit early.
  if ((dist2[0]*dist2[1] > 0.0) && (dist2[0]*dist2[2] > 0.0))
    {
    return 0;
    }
  // Check for coplanarity of the supporting planes.
//  if ( fabs( n1[0] - n2[0] ) < 1e-9 &&	// CP
//       fabs( n1[1] - n2[1] ) < 1e-9 &&	// CP
//       fabs( n1[2] - n2[2] ) < 1e-9 &&	// CP
//       fabs( s1 - s2 ) < 1e-9 )	// CP
  if ( fabs( n1[0] - n2[0] ) < fineTolerance &&	// CP
       fabs( n1[1] - n2[1] ) < fineTolerance &&	// CP
       fabs( n1[2] - n2[2] ) < fineTolerance &&	// CP
       fabs( s1 - s2 ) < fineTolerance )	// CP
    {
    coplanar = 1;
    return 0;
    }

  coplanar = 0;

  // There are more efficient ways to find the intersection line (if
  // it exists), but this is clear enough.
  double *pts1[3] = {p1, q1, r1}, *pts2[3] = {p2, q2, r2};

  // Find line of intersection (L = p + t*v) between two planes.
  double n1n2 = vtkMath::Dot(n1, n2);
  double a = (s1 - s2*n1n2) / (n1n2*n1n2 - 1.0);
  double b = (s2 - s1*n1n2) / (n1n2*n1n2 - 1.0);
  double p[3], v[3];
  p[0] = a*n1[0] + b*n2[0];
  p[1] = a*n1[1] + b*n2[1];
  p[2] = a*n1[2] + b*n2[2];
  vtkMath::Cross(n1, n2, v);
  vtkMath::Normalize( v );

  int index1 = 0, index2 = 0;
  double t1[2], t2[2];
  for (int i = 0; i < 3; i++)
    {
    double t, x[3];
    int id1 = i, id2 = (i+1) % 3;

    // Find t coordinate on line of intersection between two planes.
    if (vtkPlane::IntersectWithLine( pts1[id1], pts1[id2], n2, p2, t, x ))
      {
      t1[index1++] = vtkMath::Dot(x, v) - vtkMath::Dot(p, v);
      }

    if (vtkPlane::IntersectWithLine( pts2[id1], pts2[id2], n1, p1, t, x ))
      {
      t2[index2++] = vtkMath::Dot(x, v) - vtkMath::Dot(p, v);
      }
    }

  // Check if only one edge or all edges intersect the supporting
  // planes intersection.
  if ( index1 != 2 || index2 != 2 )
    {
    return 0;
    }

  // Check for NaNs
  if ( vtkMath::IsNan( t1[0] ) || vtkMath::IsNan( t1[1] ) ||
       vtkMath::IsNan( t2[0] ) || vtkMath::IsNan( t2[1] ) )
    {
    return 0;
    }

  if ( t1[0] > t1[1] )
    {
    std::swap( t1[0], t1[1] );
    }
  if ( t2[0] > t2[1] )
    {
    std::swap( t2[0], t2[1] );
    }
  // Handle the different interval configuration cases.
  double tt1, tt2;
  if ( t1[1] < t2[0] || t2[1] < t1[0] )
    {
    return 0; // No overlap
    }
  else if ( t1[0] < t2[0] )
    {
    if ( t1[1] < t2[1] )
      {
      tt1 = t2[0];
      tt2 = t1[1];
      }
    else
      {
      tt1 = t2[0];
      tt2 = t2[1];
      }
    }
  else // t1[0] >= t2[0]
    {
    if ( t1[1] < t2[1] )
      {
      tt1 = t1[0];
      tt2 = t1[1];
      }
    else
      {
      tt1 = t1[0];
      tt2 = t2[1];
      }
    }

  // Create actual intersection points.
  pt1[0] = p[0] + tt1*v[0];
  pt1[1] = p[1] + tt1*v[1];
  pt1[2] = p[2] + tt1*v[2];

  pt2[0] = p[0] + tt2*v[0];
  pt2[1] = p[1] + tt2*v[1];
  pt2[2] = p[2] + tt2*v[2];

  return 1;
}

//----------------------------------------------------------------------------
int vtkECMIntersectionPolyDataFilter::RequestData(vtkInformation*        vtkNotUsed(request),
                                               vtkInformationVector** inputVector,
                                               vtkInformationVector*  outputVector)
{
// CP : new code
//	if (0 != _output2)
//		_output2->UnRegister (0);	// CP V 2.58.1
	_output2	= 0;
	if (0 != _inner2)
		_inner2->UnRegister (0);
	_inner2	= 0;
// !CP : new code
  vtkInformation* inInfo0 = inputVector[0]->GetInformationObject(0);
  vtkInformation* inInfo1 = inputVector[1]->GetInformationObject(0);
  vtkInformation* outIntersectionInfo = outputVector->GetInformationObject(0);
  vtkInformation* outPolyDataInfo0 = outputVector->GetInformationObject(1);
  vtkInformation* outPolyDataInfo1 = outputVector->GetInformationObject(2);

  vtkPolyData *input0 = vtkPolyData::SafeDownCast(
    inInfo0->Get(vtkDataObject::DATA_OBJECT()));

  vtkPolyData *input1 = vtkPolyData::SafeDownCast(
    inInfo1->Get(vtkDataObject::DATA_OBJECT()));

  vtkPolyData *outputIntersection = vtkPolyData::SafeDownCast(
    outIntersectionInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkSmartPointer< vtkPoints > outputIntersectionPoints =
    vtkSmartPointer< vtkPoints >::New();
  outputIntersection->SetPoints(outputIntersectionPoints);

  vtkPolyData *outputPolyData0 = vtkPolyData::SafeDownCast(
    outPolyDataInfo0->Get(vtkDataObject::DATA_OBJECT()));

  vtkPolyData *outputPolyData1 = vtkPolyData::SafeDownCast(
    outPolyDataInfo1->Get(vtkDataObject::DATA_OBJECT()));

  // Set up new poly data for the inputs to build cells and links.
  vtkSmartPointer< vtkPolyData > mesh0 = vtkSmartPointer< vtkPolyData >::New();
  mesh0->DeepCopy(input0);
#ifndef VTK_5
#else	// VTK_5
  mesh0->SetSource(NULL);
#endif	// VTK_5

  vtkSmartPointer< vtkPolyData > mesh1 = vtkSmartPointer< vtkPolyData >::New();
  mesh1->DeepCopy(input1);
#ifndef VTK_5
#else	// VTK_5
  mesh1->SetSource(NULL);
#endif	// VTK_5

  // Find the triangle-triangle intersections between mesh0 and mesh1
  vtkSmartPointer< vtkOBBTree > obbTree0 = vtkSmartPointer< vtkOBBTree >::New();
  obbTree0->SetDataSet(mesh0);
  obbTree0->SetNumberOfCellsPerNode(10);
  obbTree0->SetMaxLevel(1000000);
//  obbTree0->SetTolerance(1e-6);	// CP
  obbTree0->SetTolerance(tolerance);	// CP
  obbTree0->AutomaticOn();
  obbTree0->BuildLocator();

  vtkSmartPointer< vtkOBBTree > obbTree1 = vtkSmartPointer< vtkOBBTree >::New();
  obbTree1->SetDataSet(mesh1);
  obbTree1->SetNumberOfCellsPerNode(10);
  obbTree1->SetMaxLevel(1000000);
//  obbTree1->SetTolerance(1e-6);	// CP
  obbTree1->SetTolerance(tolerance);	// CP
  obbTree1->AutomaticOn();
  obbTree1->BuildLocator();

// CP : new code
	if ((0 == obbTree0->GetBuildTime ( )) || (0 == obbTree1->GetBuildTime ( )))
	{	// Echec BuildLocator => plantage si on continue.
		cerr << "Echec dans vtkECMIntersectionPolyDataFilter::RequestData. Impossibilité de construire un vtkOBBTree." << endl;
		return 0;
	}	// if ((0 == obbTree0->GetBuildTime ( )) || ...
// ! CP new code

  // Set up the structure for determining exact triangle-triangle
  // intersections.
  vtkECMIntersectionPolyDataFilter::Impl *impl = new vtkECMIntersectionPolyDataFilter::Impl();
  impl->Mesh[0]  = mesh0;
  impl->Mesh[1]  = mesh1;
  impl->OBBTree1 = obbTree1;

  vtkSmartPointer< vtkCellArray > lines = vtkSmartPointer< vtkCellArray >::New();
  outputIntersection->SetLines(lines);
  impl->IntersectionLines = lines;

  // Add cell data arrays that map the intersection line to the cells
  // it splits.
  impl->CellIds[0] = vtkIdTypeArray::New();
  impl->CellIds[0]->SetName("Input0CellID");
  outputIntersection->GetCellData()->AddArray(impl->CellIds[0]);
  impl->CellIds[0]->Delete();
  impl->CellIds[1] = vtkIdTypeArray::New();
  impl->CellIds[1]->SetName("Input1CellID");
  outputIntersection->GetCellData()->AddArray(impl->CellIds[1]);
  impl->CellIds[1]->Delete();

  impl->PointCellIds[0] = vtkIdTypeArray::New();
  impl->PointCellIds[0]->SetName("PointCellsIDs");
  impl->PointCellIds[1] = vtkIdTypeArray::New();
  impl->PointCellIds[1]->SetName("PointCellsIDs");

  double bounds0[6], bounds1[6];
  mesh0->GetBounds(bounds0);
  mesh1->GetBounds(bounds1);
  for (int i = 0; i < 3; i++)
    {
    int minIdx = 2*i;
    int maxIdx = 2*i+1;
    if (bounds1[minIdx] < bounds0[minIdx])
      {
      bounds0[minIdx] = bounds1[minIdx];
      }
    if (bounds1[maxIdx] > bounds0[maxIdx])
      {
      bounds0[maxIdx] = bounds1[maxIdx];
      }
    }

  vtkSmartPointer< vtkPointLocator > pointMerger =
    vtkSmartPointer< vtkPointLocator >::New();
//  pointMerger->SetTolerance(1e-6);	// CP
  pointMerger->SetTolerance(tolerance);	// CP
  pointMerger->InitPointInsertion(outputIntersection->GetPoints(), bounds0);
  impl->PointMerger = pointMerger;

  // This performs the triangle intersection search
  obbTree0->IntersectWithOBBTree
    (obbTree1, 0, vtkECMIntersectionPolyDataFilter::Impl::FindTriangleIntersections,
     impl);

  // Split the first output if so desired
  if ( this->SplitFirstOutput )
    {
    mesh0->BuildLinks();
    impl->SplitMesh(0, outputPolyData0, outputIntersection);
    }
  else
    {
    outputPolyData0->ShallowCopy( mesh0 );
    }

  // Split the second output if desired
	// CP : new code
	for (int i = 0; i < 3; i++)
	{
		impl->_intersectionFunction	= _intersectionFunction;
		impl->_evaluateTolerance	= _innerTolerance;
	}
	// !CP new code
  if ( this->SplitSecondOutput )
    {
    mesh1->BuildLinks();
    impl->SplitMesh(1, outputPolyData1, outputIntersection);
	// CP : new code
	_output2	= outputPolyData1;
//	_output2->Register (0);	// CP V 2.58.1
	_innerCellsIds	= impl->_innerCellsIds;
	_innerPointsSet	= impl->_innerPointsSet;
	// !CP new code
    }
  else
    {
    outputPolyData1->ShallowCopy( mesh1 );
    }

  impl->PointCellIds[0]->Delete();
  impl->PointCellIds[1]->Delete();
  delete impl;

//	Modified ( );	// CP v 4.10.0

  return 1;
}

//----------------------------------------------------------------------------
int vtkECMIntersectionPolyDataFilter::FillInputPortInformation(int port,
                                                            vtkInformation *info)
{
  if (!this->Superclass::FillInputPortInformation(port, info))
    {
    return 0;
    }
  if (port == 0)
    {
    info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData");
    }
  else if (port == 1)
    {
    info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData");
    info->Set(vtkAlgorithm::INPUT_IS_OPTIONAL(), 0);
    }
  return 1;
}
