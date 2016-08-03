#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <shapefil.h>
#include "tz_finder_location.h"



char *
tz_get_name_by_coordinates(double longitude, double lattitude)
{

    int i, nEntities, quality;
    char *retval = NULL;
    SHPHandle   hSHP;
    DBFHandle   hDBF;
    hSHP = SHPOpen("../sandbox/world/tz_world.shp", "rb");
    SHPGetInfo(hSHP, &nEntities, NULL, NULL, NULL);
    printf("Number of entities: %d\n", nEntities);
    hDBF = DBFOpen( "../sandbox/world/tz_world.dbf", "rb" );
    if( hDBF == NULL ){
        printf( "DBFOpen(%s,\"r\") failed.\n","../world/tz_world.dbf" );
        exit( 2 );
    }

    for( i = 0; i < nEntities; i++ )
    {
        SHPObject *psShape;
        psShape = SHPReadObject( hSHP, i );
        if(psShape->nSHPType == SHPT_POLYGON)
        {
            if(tz_pnpoly(psShape->nVertices, psShape->padfX, psShape->padfY,
                        longitude,lattitude)){
                printf ("FOUND!!! TZID for shape: %d is: %s\n", i,
                        DBFReadStringAttribute(hDBF, i, 0));
                retval = strdup(DBFReadStringAttribute(hDBF, i, 0));
            }
        }
        SHPDestroyObject( psShape );
    }

    return retval;
}


//shamelessly taken from
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
