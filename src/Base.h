#ifndef BASE_H
#define BASE_H

/***************************************************************************
Base.h
Comment:  This file contains all basic definitions.
***************************************************************************/

// STL includes
#include <iostream>
#include <assert.h>
#include <math.h>
#include <vector>
#include <algorithm>
#include <float.h>
#include <queue>
#include <set>

// local includes
#include "ColorDef.h"
#include "Point2.h"
#include "Point3.h"


#define X_RES 640
#define Y_RES 480
//#define round(x) floor(x+0.5)


//-----------------------------------------------------------------------------
/** draw modes */
enum DrawMode {
  None,
  Wire,
  Smooth,
  Points,
  Hidden,
  Flat,
  FlatWire,
  Radar,
  Patch
};


//-----------------------------------------------------------------------------
/** color modes */
enum ColorMode {
  CMNone,
  PerVert,
  PerFace
};


//-----------------------------------------------------------------------------
/** Point2 with int coordinates. */
typedef Point2 < int >  Point2i;

//-----------------------------------------------------------------------------
/** Point2 with double coordinates. */
typedef Point2 < double >  Point2d;

//-----------------------------------------------------------------------------
/** Point3 with float coordinates. */
typedef Point3 < float >  Point3f;

//-----------------------------------------------------------------------------
/** Point3 with double coordinates. */
typedef Point3 < double > Point3d;


#endif
