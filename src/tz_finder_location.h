#ifndef TZ_FINDER_LOCATION_H
#define TZ_FINDER_LOCATION_H


/* looks up the timezone correspondig to the given
 * geographical coordinates
 */
char *tz_get_name_by_coordinates(double longitude, double lattitude,
                                const char* shp_path, const char *dbf_path);


/* accepts an array of a polygon's vertices' coordinates, the
 * coordinates of a point, and checks whether the point is within the
 * polygon. Returns 1 if the point is in the polygon, and 0 if not
 */
int tz_pnpoly(int nvert, double *vertx, double *verty, float testx, float testy);
#endif
