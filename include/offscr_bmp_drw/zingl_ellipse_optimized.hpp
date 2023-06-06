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
void zingl_image_drawer<BitmapImageType>::plotOptimizedEllipse(
    const int xm, const int ym, const int rx, const int ry, const pixel_t color)
{
    long x = -rx, y = 0;          /* II. quadrant from bottom left to top right */
    long e2 = ry, dx = (1+2*x)*e2*e2;                       /* error increment  */
    long dy = x*x, err = dx+dy;                             /* error of 1.step */
    const int row_inc = int(image_.row_increment());
    pixel_t * row_upper = image_.row(ym + y);   // ym + y
    pixel_t * row_lower = image_.row(ym - y);   // ym - y
    Setter setPixel{image_};

    do {
        setPixel(xm-x, ym+y, &row_upper[xm-x], color);        /*   I. Quadrant */
        setPixel(xm+x, ym+y, &row_upper[xm+x], color);        /*  II. Quadrant */
        setPixel(xm+x, ym-y, &row_lower[xm+x], color);        /* III. Quadrant */
        setPixel(xm-x, ym-y, &row_lower[xm-x], color);        /*  IV. Quadrant */
        e2 = 2*err;
        if (e2 >= dx) { x++; err += dx += 2*(long)ry*ry; }           /* x step */
        if (e2 <= dy) { /* y step */
            y++;
            err += dy += 2*(long)rx*rx;
            row_upper += row_inc;
            row_lower -= row_inc;
        }
    } while (x <= 0);

    while (y++ < ry)                  /* too early stop of flat ellipses a=1, */
    {
        /* -> finish tip of ellipse */
        row_upper += row_inc;
        row_lower -= row_inc;
        setPixel(xm, ym+y, &row_upper[xm], color);
        setPixel(xm, ym-y, &row_lower[xm], color);
    }
}

template <class BitmapImageType>
template <class Setter>
void zingl_image_drawer<BitmapImageType>::plotOptimizedEllipse(
    const Point ptCenter, const Point radius, const pixel_t color)
{
    plotOptimizedEllipse<Setter>(ptCenter.first, ptCenter.second, radius.first, radius.second, color);
}

}
