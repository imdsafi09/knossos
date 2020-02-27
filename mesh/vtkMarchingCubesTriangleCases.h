/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkMarchingCubesTriangleCases.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#pragma once
//
// marching cubes case table for generating isosurfaces
//
//#include "vtkCommonDataModelModule.h" // For export macro
//#include "vtkSystemIncludes.h"

typedef int EDGE_LIST;
struct /*VTKCOMMONDATAMODEL_EXPORT*/ vtkMarchingCubesTriangleCases
{
  EDGE_LIST edges[16];
  static vtkMarchingCubesTriangleCases* GetCases();
};
#include "vtkMarchingCubesTriangleCases.cxx" // avoid linking an object file
// VTK-HeaderTest-Exclude: vtkMarchingCubesTriangleCases.h
