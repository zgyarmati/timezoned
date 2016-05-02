#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <shapefil.h>

//http://redwarrior.org/blog/?p=16

//shamelessly taken from
// http://stackoverflow.com/a/2922778/1494352
int pnpoly(int nvert, double *vertx, double *verty, float testx, float testy)
{
  int i, j, c = 0;
  for (i = 0, j = nvert-1; i < nvert; j = i++) {
    if ( ((verty[i]>testy) != (verty[j]>testy)) &&
     (testx < (vertx[j]-vertx[i]) * (testy-verty[i]) / (verty[j]-verty[i]) + vertx[i]) )
       c = !c;
  }
  return c;
}

int main(int argc, char* argv[])
{

    int i, nEntities, quality;
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
//            printf ("TZID for shape: %d is: %s, number of parts: %d\n", i,
//                    DBFReadStringAttribute(hDBF, i, 0), psShape->nParts);

//int pnpoly(int nvert, float *vertx, float *verty, float testx, float testy)
            if(pnpoly(psShape->nVertices, psShape->padfX, psShape->padfY,
                        atof(argv[1]),atof(argv[2]))){
                    printf ("FOUND!!! TZID for shape: %d is: %s\n", i,
                    DBFReadStringAttribute(hDBF, i, 0));
                exit(0);
//            }
//            for(int j = 0; j < psShape->nVertices; j++){
//                double tx = psShape->padfX[j];
//                double ty = psShape->padfY[j];
//                printf("point X: %f; Y:%f\n", tx,ty);
            }
        }

        SHPDestroyObject( psShape );
    }




}
