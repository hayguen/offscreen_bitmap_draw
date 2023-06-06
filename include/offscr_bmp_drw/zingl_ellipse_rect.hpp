/*
 *****************************************************************************
 *                                                                           *
 *                          Platform Independent                             *
 *                    Bitmap Image Reader Writer Library                     *
 *                                                                           *
 * Author: Arash Partow - 2002                                               *
 * URL: http://partow.net/programming/bitmap/index.html                      *
 *                                                                           *
 * Note: This library only supports 24-bits per pixel bitmap format files.   *
 *                                                                           *
 * Copyright notice:                                                         *
 * Free use of the Platform Independent Bitmap Image Reader Writer Library   *
 * is permitted under the guidelines and in accordance with the most current *
 * version of the MIT License.                                               *
 * http://www.opensource.org/licenses/MIT                                    *
 *                                                                           *
 *****************************************************************************
*/

#pragma once

#include "zingl_image_drawer.hpp"


namespace OffScreenBitmapDraw
{

template <class BitmapImageType>
template <class Setter>
void zingl_image_drawer<BitmapImageType>::plotEllipseRect(
    int x0, int y0, int x1, int y1, const pixel_t color)
{                              /* rectangular parameter enclosing the ellipse */
    long a = std::abs(x1-x0), b = std::abs(y1-y0), b1 = b&1;       /* diameter */
    double dx = 4*(1.0-a)*b*b, dy = 4*(b1+1)*a*a;           /* error increment */
    double err = dx+dy+b1*a*a, e2;                          /* error of 1.step */

    if (x0 > x1) { x0 = x1; x1 += a; }        /* if called with swapped points */
    if (y0 > y1) y0 = y1;                                  /* .. exchange them */
    y0 += (b+1)/2; y1 = y0-b1;                               /* starting pixel */
    a = 8*a*a; b1 = 8*b*b;

    const int row_inc = int(image_.row_increment());
    pixel_t * row0 = image_.row(y0);
    pixel_t * row1 = image_.row(y1);
    Setter setPixel{image_};

    do {
        setPixel(x1, y0, &row0[x1], color);                    /*   I. Quadrant */
        setPixel(x0, y0, &row0[x0], color);                    /*  II. Quadrant */
        setPixel(x0, y1, &row1[x0], color);                    /* III. Quadrant */
        setPixel(x1, y1, &row1[x1], color);                    /*  IV. Quadrant */
        e2 = 2*err;
        if (e2 <= dy) {                                               /* y step */
             y0++; y1--; err += dy += a;
             row0 += row_inc;
             row1 -= row_inc;
        }
        if (e2 >= dx || 2*err > dy) { x0++; x1--; err += dx += b1; }  /* x step */
    } while (x0 <= x1);

    while (y0-y1 <= b) {                /* too early stop of flat ellipses a=1 */
        /* -> finish tip of ellipse */
        setPixel(x0-1, y0, &row0[x0-1], color);
        setPixel(x1+1, y0, &row0[x1+1], color);
        setPixel(x0-1, y1, &row1[x0-1], color);
        setPixel(x1+1, y1, &row1[x1+1], color);
        y0++; row0 += row_inc;
        y1--; row1 -= row_inc;
    }
}

template <class BitmapImageType>
template <class Setter>
void zingl_image_drawer<BitmapImageType>::plotEllipseRect(
    const Point pt0, const Point pt1, const pixel_t color)
{
    plotEllipseRect<Setter>(pt0.first, pt0.second, pt1.first, pt1.second, color);
}

}
