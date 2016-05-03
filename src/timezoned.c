#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <shapefil.h>

//http://redwarrior.org/blog/?p=16


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
            if(pnpoly(psShape->nVertices, psShape->padfX, psShape->padfY,
                        atof(argv[1]),atof(argv[2]))){
                printf ("FOUND!!! TZID for shape: %d is: %s\n", i,
                        DBFReadStringAttribute(hDBF, i, 0));
            }
        }
        SHPDestroyObject( psShape );
    }
}
