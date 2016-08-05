#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <shapefil.h>
#include "tz_finder_location.h"



char *
tz_get_name_by_coordinates(double longitude, double lattitude,
                           const char* shp_path, const char *dbf_path)
{

    int i, nEntities, quality;
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
