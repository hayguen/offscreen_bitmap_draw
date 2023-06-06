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
void zingl_image_drawer<BitmapImageType>::fillEllipseRect(
    int x0, int y0, int x1, int y1, const pixel_t color)
{                              /* rectangular parameter enclosing the ellipse */
    long a = std::abs(x1-x0), b = std::abs(y1-y0), b1 = b&1;       /* diameter */
    double dx = 4*(1.0-a)*b*b, dy = 4*(b1+1)*a*a;           /* error increment */
    double err = dx+dy+b1*a*a, e2;                          /* error of 1.step */
    const int xm = (x0 + x1) / 2;

    if (x0 > x1) { x0 = x1; x1 += a; }        /* if called with swapped points */
    if (y0 > y1) y0 = y1;                                  /* .. exchange them */
    y0 += (b+1)/2; y1 = y0-b1;                               /* starting pixel */
    a = 8*a*a; b1 = 8*b*b;

    const int row_inc = int(image_.row_increment());
    pixel_t * row0 = image_.row(y0);
    pixel_t * row1 = image_.row(y1);
    Setter setPixel{image_};
    int xL = xm +1, xR = xm;
    int yL = -1;

    do {
        xL = std::min(xL, x0); xR = std::max(xR, x1);
        e2 = 2*err;
        if (e2 <= dy) {                                               /* y step */
            if (yL != y0)
            {
                yL = y0;
                setPixel.hLine(xL, xR, y0, &row0[xL], &row0[xR], color);
                if (y0 != y1)
                    setPixel.hLine(xL, xR, y1, &row1[xL], &row1[xR], color);
            }
            xL = xm +1; xR = xm;
            y0++; y1--; err += dy += a;
            row0 += row_inc;
            row1 -= row_inc;
        }
        if (e2 >= dx || 2*err > dy) { x0++; x1--; err += dx += b1; }  /* x step */
    } while (x0 <= x1);

    while (y0-y1 <= b) {                /* too early stop of flat ellipses a=1 */
        /* -> finish tip of ellipse */
        xL = std::min(xL, x0-1); xR = std::max(xR, x1+1);
        if (y0 != yL)
        {
            yL = y0;
            setPixel.hLine(xL, xR, y0, &row0[xL], &row0[xR], color);
            if (y0 != y1)
                setPixel.hLine(x0-1, x1+1, y1, &row1[x0-1], &row1[x1+1], color);
            xL = x0-1; xR = x1+1;
        }
        y0++; row0 += row_inc;
        y1--; row1 -= row_inc;
    }
}

template <class BitmapImageType>
template <class Setter>
void zingl_image_drawer<BitmapImageType>::fillEllipseRect(
    const Point pt0, const Point pt1, const pixel_t color)
{
    fillEllipseRect<Setter>(pt0.first, pt0.second, pt1.first, pt1.second, color);
}

}
