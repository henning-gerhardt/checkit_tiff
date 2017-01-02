/* rule based checks if given TIFF is a specific baseline TIFF
 * 
 * author: Andreas Romeyke, 2015
 * licensed under conditions of libtiff 
 * (see file LICENSE)
 *
 */

#include "check.h"
#include "check_helper.h"
#include <assert.h>
#include <string.h>

/** check if date / time values are within correct ranges 
 * @param year year
 * @param month month
 * @param day day
 * @param hour hour
 * @param min min
 * @param sec sec
 * @return 0 if success, otherwise -1
 */
int test_plausibility (int * year, int * month, int * day, int * hour, int * min, int * sec) {
#ifdef DEBUG
  printf ("found: y=%d m=%d d=%d h=%d m=%d s=%d\n", *year, *month, *day, *hour, *min, *sec);
#endif
  if (
      1500 < *year && 
      2100 > *year &&
      0 < *month &&
      13 > *month &&
      0 < *day &&
      32 > *day &&
      0 <= *hour &&
      24 > *hour &&
      0 <= *min &&
      60 > *min &&
      0 <= *sec &&
      60 > *sec
     ) {
    return 0;
  } else {
    return -1;
  }
}


/** loads a tiff, fix it if needed, stores tiff
 * @param filename filename which should be processed, repaired
 */
ret_t check_datetime(ctiff_t * ctif ) {
  tifp_check( ctif);
  //printf("check if tag %u (%s) is correct\n", TIFFTAG_DATETIME, TIFFTagName(tif, TIFFTAG_DATETIME));
  tif_rules_tag(TIFFTAG_DATETIME, "is correct");
  /* find date-tag and fix it */
  int count=0;
  char *datetime=NULL;
  count = TIFFGetFieldASCII(ctif, TIFFTAG_DATETIME, &datetime);

  // printf("DATETIME='%s'\n", datetime);
    int day=0;
    int month=0;
    int year=0;
    int hour=0;
    int min=0;
    int sec=0;
    int r = 0;
    for (int i=0; i<count; i++) {
        if ((datetime[i] == '\0') && (i != 19)) { /* \0 at 20th byte  allowed */
          r = i+1;
          break;
        }
    }
#ifdef DEBUG
    printf(" count=%u\n\n", count);
#endif
    if (0 == r) {
      if (6 == sscanf(datetime, "%04d:%02d:%02d%02d:%02d:%02d", &year, &month, &day, &hour, &min, &sec)) {
        if (0 == test_plausibility(&year, &month, &day, &hour, &min, &sec)) {
          ret_t res;
          res.returnmsg=NULL;
          res.returncode=0;
          return res;

        } else {
          char array[TIFFAILSTRLEN];
          snprintf(array, sizeof(array), "of datetime not plausible, was \"%s\"", datetime);
          return tif_fails_tag( TIFFTAG_DATETIME, "should be  \"yyyy:MM:DD hh:mm:ss\"", array);
          //tif_fails("tag %u (%s) value of datetime not plausible, should be  \"yyyy:MM:DD hh:mm:ss\", but was \"%s\"\n", TIFFTAG_DATETIME, TIFFTagName(tif, TIFFTAG_DATETIME), datetime);
        }
      } else {
        char array[TIFFAILSTRLEN];
        snprintf(array, sizeof(array), "of datetime was \"%s\"", datetime);
        return tif_fails_tag( TIFFTAG_DATETIME, "should be  \"yyyy:MM:DD hh:mm:ss\"", array);
        //tif_fails("tag %u (%s) value of datetime should be \"yyyy:MM:DD hh:mm:ss\", but was \"%s\"\n", TIFFTAG_DATETIME, TIFFTagName(tif, TIFFTAG_DATETIME), datetime);
      }
    } else {
       char array[TIFFAILSTRLEN];
       snprintf(array, sizeof(array), "of datetime was \"%s\" and contains a \\0 at %i (count=%u)", datetime, r, count);
       return tif_fails_tag( TIFFTAG_DATETIME, "should be  \"yyyy:MM:DD hh:mm:ss\"", array);
       //tif_fails("tag %u (%s) value of datetime should be \"yyyy:MM:DD hh:mm:ss\", but was \"%s\" and contains a \\0 at %i (count=%u)\n", TIFFTAG_DATETIME, TIFFTagName(tif, TIFFTAG_DATETIME), datetime, r, count);
    }
  ret_t res;
  res.returnmsg=NULL;
  res.returncode=0;
  return res;
}

/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 smarttab expandtab :*/
