/*! @file tz_finder_location.c
 *  author: Zoltan Gyarmati <mr.zoltan.gyarmati@gmail.com>
 *
 *
 *                   The MIT License (MIT)
 *
 * Copyright (c) 2016 Zoltan Gyarmati <mr.zoltan.gyarmati@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of 
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so, 
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */


#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <shapefil.h>
#include "tz_finder_location.h"



char *
tz_get_name_by_coordinates(double longitude, double lattitude,
                           const char* shp_path, const char *dbf_path)
{

    int i, nEntities;
    char *retval = NULL;
    SHPHandle   hSHP;
    DBFHandle   hDBF;
    hSHP = SHPOpen(shp_path, "rb");
    if(hSHP == NULL){
        fprintf(stderr, "Failed to open .shp file: %s\n", shp_path);
        return retval;
    }
    SHPGetInfo(hSHP, &nEntities, NULL, NULL, NULL);
    hDBF = DBFOpen( dbf_path, "rb" );
    if( hDBF == NULL ){
        fprintf(stderr, "Failed to open .shp file: %s\n", shp_path);
        SHPClose(hSHP);
        return retval;
    }

    for(i = 0; i < nEntities; i++) {
        SHPObject *psShape;
        psShape = SHPReadObject( hSHP, i );
        if(psShape->nSHPType != SHPT_POLYGON) {
            continue;
        }
        if(tz_pnpoly(psShape->nVertices, psShape->padfX,
                     psShape->padfY, longitude, lattitude)){
            retval = strdup(DBFReadStringAttribute(hDBF, i, 0));
        }
        SHPDestroyObject( psShape );
    }

    SHPClose(hSHP);
    DBFClose(hDBF);
    return retval;
}


// shamelessly taken from
// http://stackoverflow.com/a/2922778/1494352
int tz_pnpoly(int nvert, double *vertx, double *verty, float testx, float testy)
{
  int i, j, c = 0;
  for (i = 0, j = nvert-1; i < nvert; j = i++) {
    if ( ((verty[i]>testy) != (verty[j]>testy)) &&
     (testx < (vertx[j]-vertx[i]) * (testy-verty[i]) / (verty[j]-verty[i]) + vertx[i]) )
       c = !c;
  }
  return c;
}
