/*
 * Unpaper.
 *
 * Copyright © 2005-2007 Jens Gulden
 * Copyright © 2011-2011 Diego Elio Pettenò
 *
 * Unpaper is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2 of the License.
 *
 * Unpaper is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __UNPAPER_H__
#define __UNPAPER_H__

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>

/* --- global declarations ------------------------------------------------ */


#if HAVE_STDBOOL_H
# include <stdbool.h>
#else
 typedef enum {
   FALSE,
   TRUE
 } BOOLEAN;

# define bool BOOLEAN
# define false FALSE
# define true TRUE
#endif

/* --- preprocessor macros ------------------------------------------------ */
              
#define max(a, b) ( (a >= b) ? (a) : (b) )
#define pluralS(i) ( (i > 1) ? "s" : "" )
#define pixelValue(r, g, b) ( (r)<<16 | (g)<<8 | (b) )
#define pixelGrayscaleValue(g) ( (g)<<16 | (g)<<8 | (g) )
#define pixelGrayscale(r, g, b) ( ( ( r == g ) && ( r == b ) ) ? r : ( ( r + g + b ) / 3 ) ) // average (optimized for already gray values)
#define pixelLightness(r, g, b) ( r < g ? ( r < b ? r : b ) : ( g < b ? g : b ) ) // minimum
#define pixelDarknessInverse(r, g, b) ( r > g ? ( r > b ? r : b ) : ( g > b ? g : b ) ) // maximum
#define red(pixel) ( (pixel >> 16) & 0xff )
#define green(pixel) ( (pixel >> 8) & 0xff )
#define blue(pixel) ( pixel & 0xff )


/* --- preprocessor constants ---------------------------------------------- */
              
#define MAX_MULTI_INDEX 10000 // maximum pixel count of virtual line to detect rotation with
#define MAX_ROTATION_SCAN_SIZE 10000 // maximum pixel count of virtual line to detect rotation with
#define MAX_MASKS 100
#define MAX_POINTS 100
#define MAX_FILES 100
#define MAX_PAGES 2
#define WHITE 255
#define GRAY 127
#define BLACK 0
#define BLANK_TEXT "<blank>"


/* --- typedefs ----------------------------------------------------------- */

typedef enum {
    VERBOSE_QUIET = -1,
    VERBOSE_NONE = 0,
    VERBOSE_NORMAL = 1,
    VERBOSE_MORE = 2,
    VERBOSE_DEBUG = 3,
    VERBOSE_DEBUG_SAVE = 4
} VERBOSE_LEVEL;

typedef enum {
	X,
	Y,
	COORDINATES_COUNT
} COORDINATES;

typedef enum {
	WIDTH,
	HEIGHT,
	DIMENSIONS_COUNT
} DIMENSIONS;

typedef enum {
	HORIZONTAL,
	VERTICAL,
	DIRECTIONS_COUNT
} DIRECTIONS;

typedef enum {
	LEFT,
	TOP,
	RIGHT,
	BOTTOM,
	EDGES_COUNT
} EDGES;

typedef enum {
    LAYOUT_NONE,
	LAYOUT_SINGLE,
	LAYOUT_DOUBLE,
	LAYOUTS_COUNT
} LAYOUTS;

typedef enum {
	BRIGHT,
	DARK,
	SHADINGS_COUNT
} SHADINGS;

typedef enum {
	RED,
	GREEN,
	BLUE,
	COLORCOMPONENTS_COUNT
} COLORCOMPONENTS;

typedef enum {
	PBM,
	PGM,
	PPM,
	FILETYPES_COUNT
} FILETYPES;

typedef enum {
	INTERP_NN,
	INTERP_LINEAR,
	INTERP_CUBIC,
	INTERP_FUNCTIONS_COUNT
} INTERP_FUNCTIONS;


/* --- struct ------------------------------------------------------------- */

struct IMAGE {
    uint8_t *buffer;
    uint8_t *bufferGrayscale;
    uint8_t *bufferLightness;
    uint8_t *bufferDarknessInverse;
    int width;
    int height;
    int bitdepth;
    int background;
    bool color;
};

void errOutput(const char *fmt, ...)
    __attribute__((format(printf, 1, 2)))
    __attribute__((noreturn));

/* --- global variable ---------------------------------------------------- */

extern VERBOSE_LEVEL verbose;
extern INTERP_FUNCTIONS interpolateType;


/* --- tool function for file handling ------------------------------------ */

void loadImage(FILE *f, struct IMAGE* image, int* type);

void saveImage(FILE *outputFile, struct IMAGE* image, int type, float blackThreshold);

void saveDebug(char* filename, struct IMAGE* image);


/****************************************************************************
 * image processing functions                                               *
 ****************************************************************************/


/* --- deskewing ---------------------------------------------------------- */

double detectRotation(int deskewScanEdges, int deskewScanRange, float deskewScanStep, int deskewScanSize, float deskewScanDepth, float deskewScanDeviation, int left, int top, int right, int bottom, struct IMAGE* image);

void rotate(double radians, struct IMAGE* source, struct IMAGE* target);

void convertToQPixels(struct IMAGE* image, struct IMAGE* qpixelImage);

void convertFromQPixels(struct IMAGE* qpixelImage, struct IMAGE* image);


/* --- stretching / resizing / shifting ------------------------------------ */


void stretch(int w, int h, struct IMAGE* image);

void resize(int w, int h, struct IMAGE* image);

void shift(int shiftX, int shiftY, struct IMAGE* image);


/* --- mask-detection ----------------------------------------------------- */


int detectMasks(int mask[MAX_MASKS][EDGES_COUNT], bool maskValid[MAX_MASKS], int point[MAX_POINTS][COORDINATES_COUNT], int pointCount, int maskScanDirections, int maskScanSize[DIRECTIONS_COUNT], int maskScanDepth[DIRECTIONS_COUNT], int maskScanStep[DIRECTIONS_COUNT], float maskScanThreshold[DIRECTIONS_COUNT], int maskScanMinimum[DIMENSIONS_COUNT], int maskScanMaximum[DIMENSIONS_COUNT],  struct IMAGE* image);

void applyMasks(int mask[MAX_MASKS][EDGES_COUNT], int maskCount, int maskColor, struct IMAGE* image);


/* --- wiping ------------------------------------------------------------- */


void applyWipes(int area[MAX_MASKS][EDGES_COUNT], int areaCount, int wipeColor, struct IMAGE* image);


/* --- mirroring ---------------------------------------------------------- */


void mirror(int directions, struct IMAGE* image);


/* --- flip-rotating ------------------------------------------------------ */


void flipRotate(int direction, struct IMAGE* image);


/* --- blackfilter -------------------------------------------------------- */


void blackfilter(int blackfilterScanDirections, int blackfilterScanSize[DIRECTIONS_COUNT], int blackfilterScanDepth[DIRECTIONS_COUNT], int blackfilterScanStep[DIRECTIONS_COUNT], float blackfilterScanThreshold, int blackfilterExclude[MAX_MASKS][EDGES_COUNT], int blackfilterExcludeCount, int blackfilterIntensity, float blackThreshold, struct IMAGE* image);


/* --- noisefilter -------------------------------------------------------- */


int noisefilter(int intensity, float whiteThreshold, struct IMAGE* image);


/* --- blurfilter --------------------------------------------------------- */

int blurfilter(int blurfilterScanSize[DIRECTIONS_COUNT], int blurfilterScanStep[DIRECTIONS_COUNT], float blurfilterIntensity, float whiteThreshold, struct IMAGE* image);


/* --- grayfilter --------------------------------------------------------- */


int grayfilter(int grayfilterScanSize[DIRECTIONS_COUNT], int grayfilterScanStep[DIRECTIONS_COUNT], float grayfilterThreshold, float blackThreshold, struct IMAGE* image);


/* --- border-detection --------------------------------------------------- */


void centerMask(int centerX, int centerY, int left, int top, int right, int bottom, struct IMAGE* image);

void alignMask(int mask[EDGES_COUNT], int outside[EDGES_COUNT], int direction, int margin[DIRECTIONS_COUNT], struct IMAGE* image);

void detectBorder(int border[EDGES_COUNT], int borderScanDirections, int borderScanSize[DIRECTIONS_COUNT], int borderScanStep[DIRECTIONS_COUNT], int borderScanThreshold[DIRECTIONS_COUNT], float blackThreshold, int outsideMask[EDGES_COUNT], struct IMAGE* image);

void borderToMask(int border[EDGES_COUNT], int mask[EDGES_COUNT], struct IMAGE* image);

void applyBorder(int border[EDGES_COUNT], int borderColor, struct IMAGE* image);


/* --- tool functions for parameter parsing and verbose output ------------ */


int parseDirections(char* s);

void printDirections(int d);

int parseEdges(char* s);

void printEdges(int d);

void parseInts(char* s, int i[2]);

void parseSize(char* s, int i[2], int dpi);

int parseColor(char* s);

void printInts(int i[2]);

void parseFloats(char* s, float f[2]);

void printFloats(float f[2]);

char* implode(char* buf, const char* s[], int cnt);

void parseMultiIndex(const char *optarg, int multiIndex[], int* multiIndexCount);

bool isInMultiIndex(int index, int multiIndex[MAX_MULTI_INDEX], int multiIndexCount);

bool isExcluded(int index, int multiIndex[MAX_MULTI_INDEX], int multiIndexCount, int excludeIndex[MAX_MULTI_INDEX], int excludeIndexCount);

void printMultiIndex(int multiIndex[MAX_MULTI_INDEX], int multiIndexCount);

bool masksOverlapAny(int m[EDGES_COUNT], int masks[MAX_MASKS][EDGES_COUNT], int masksCount);


/* --- arithmetic tool functions ------------------------------------------ */


double sqr(double d);

double degreesToRadians(double d);

double radiansToDegrees(double r);

void limit(int* i, int max);


/* --- tool functions for image handling ---------------------------------- */


void initImage(struct IMAGE* image, int width, int height, int bitdepth, bool color, int background);

void freeImage(struct IMAGE* image);

void replaceImage(struct IMAGE* image, struct IMAGE* newimage);

bool setPixel(int pixel, int x, int y, struct IMAGE* image);

int getPixel(int x, int y, struct IMAGE* image);

int getPixelGrayscale(int x, int y, struct IMAGE* image);

int getPixelDarknessInverse(int x, int y, struct IMAGE* image);

int clearRect(int left, int top, int right, int bottom, struct IMAGE* image, int blackwhite);

void copyImageArea(int x, int y, int width, int height, struct IMAGE* source, int toX, int toY, struct IMAGE* target);

void copyImage(struct IMAGE* source, int toX, int toY, struct IMAGE* target);

void centerImage(struct IMAGE* source, int toX, int toY, int ww, int hh, struct IMAGE* target);

int brightnessRect(int x1, int y1, int x2, int y2, struct IMAGE* image);

int lightnessRect(int x1, int y1, int x2, int y2, struct IMAGE* image);

int darknessInverseRect(int x1, int y1, int x2, int y2, struct IMAGE* image);

int countPixelsRect(int left, int top, int right, int bottom, int minColor, int maxBrightness, bool clear, struct IMAGE* image);

int countPixelNeighbors(int x, int y, int intensity, int whiteMin, struct IMAGE* image);

void clearPixelNeighbors(int x, int y, int whiteMin, struct IMAGE* image);

void floodFill(int x, int y, int color, int maskMin, int maskMax, int intensity, struct IMAGE* image);

#endif /* __UNPAPER_H__ */
