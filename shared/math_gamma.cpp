/* ******************************************************************** **
** @@ Gamma function
** @  Copyrt : 
** @  Author : 
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

/*************************************************************************
Cephes Math Library Release 2.8:  June, 2000
Copyright by Stephen L. Moshier

>>> SOURCE LICENSE >>>
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation (www.fsf.org); either version 2 of the 
License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

A copy of the GNU General Public License is available at
http://www.fsf.org/licensing/licenses

>>> END OF LICENSE >>>
*************************************************************************/

/* ******************************************************************** **
** uses pre-compiled headers
** ******************************************************************** */

#include <stdafx.h>

#include "math.h"

#include "math_gamma.h"

/*
Gamma function

Input parameters:
    X   -   argument

Domain:
    0 < X < 171.6
    -170 < X < 0, X is not an integer.

Relative error:
 arithmetic   domain     # trials      peak         rms
    IEEE    -170, -33     20000       2.3e-15     3.3e-16
    IEEE     -33,  33     20000       9.4e-16     2.2e-16
    IEEE      33, 171.6   20000       2.3e-15     3.2e-16

Cephes Math Library Release 2.8:  June, 2000
Original copyright 1984, 1987, 1989, 1992, 2000 by Stephen L. Moshier
*/

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/* ******************************************************************** **
** @@ internal defines
** ******************************************************************** */

const double   Pi = 3.141592653589793238462643383279502884197;

/* ******************************************************************** **
** @@ internal prototypes
** ******************************************************************** */

/* ******************************************************************** **
** @@ external global variables
** ******************************************************************** */

/* ******************************************************************** **
** @@ static global variables
** ******************************************************************** */
                  
/* ******************************************************************** **
** @@ real code
** ******************************************************************** */

/* ******************************************************************** **
** @@ inline IFloor()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

inline int IFloor(double x)
{
   return int(floor(x));
}

/* ******************************************************************** **
** @@ inline Round()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

inline int Round(double x)
{
   return int(floor(x + 0.5));
}

/* ******************************************************************** **
** @@ static Stirling()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

static double Stirling(double x)
{
   if (fabs(x) > 170.0)
   {
      // Error !
      // Out of range
      return 0.0;
   }

   double   w = 1 / x;

   double   stir =  7.87311395793093628397E-4;

   stir = -2.29549961613378126380E-4 + w * stir;
   stir = -2.68132617805781232825E-3 + w * stir;
   stir =  3.47222221605458667310E-3 + w * stir;
   stir =  8.33333333333482257126E-2 + w * stir;

   w = 1 + w * stir;

   double   y = exp(x);

   if (x > 143.01608)
   {
      double   v = pow(x,0.5 * x - 0.25);

      y = v * (v / y);
   }
   else
   {
      y = pow(x,x - 0.5) / y;
   }
   
   return 2.50662827463100050242 * y * w;
}

/* ******************************************************************** **
** @@ Gamma()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

double Gamma(double x)
{
   double   z = 0.0;

   double   sgngam   = 1.0;

   double   q = fabs(x);

   if (q > 33.0)
   {
      if (x < 0.0)
      {
         double   p = IFloor(q);

         int      i = Round(p);

         if (!(i % 2))
         {
            sgngam = -1;
         }

         z = q - p;

         if (z > 0.5)
         {
            p = p + 1;
            z = q - p;
         }

         z = q * sin(Pi * z);
         z = fabs(z);
         z = Pi / (z * Stirling(q));
      }
      else
      {
         z = Stirling(x);
      }
      
      return sgngam * z;
   }

   z = 1;

   while (x >= 3)
   {
      x = x - 1;
      z = z * x;
   }

   while (x < 0)
   {
      if (x > -0.000000001)
      {
         return z / ((1 + 0.5772156649015329 * x) * x);
      }

      z = z / x;
      x = x + 1;
   }

   while (x < 2)
   {
      if (x < 0.000000001)
      {
         return z / ((1 + 0.5772156649015329 * x) * x);
      }

      z = z / x;
      x = x + 1.0;
   }

   if (x == 2)
   {
      return z;
   }

   x = x - 2.0;

   double   pp = 1.60119522476751861407E-4;

   pp = 1.19135147006586384913E-3 + x * pp;
   pp = 1.04213797561761569935E-2 + x * pp;
   pp = 4.76367800457137231464E-2 + x * pp;
   pp = 2.07448227648435975150E-1 + x * pp;
   pp = 4.94214826801497100753E-1 + x * pp;
   pp = 9.99999999999999996796E-1 + x * pp;

   double   qq = -2.31581873324120129819E-5;

   qq = 5.39605580493303397842E-4  + x * qq;
   qq = -4.45641913851797240494E-3 + x * qq;
   qq = 1.18139785222060435552E-2  + x * qq;
   qq = 3.58236398605498653373E-2  + x * qq;
   qq = -2.34591795718243348568E-1 + x * qq;
   qq = 7.14304917030273074085E-2  + x * qq;
   qq = 1.00000000000000000320     + x * qq;

   return z * pp / qq;
}

/* ******************************************************************** **
** End of File
** ******************************************************************** */
