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
void zingl_image_drawer<BitmapImageType>::plotLine(
    int x0, int y0, int x1, int y1, const pixel_t color)
{
    const int dx =  std::abs(x1-x0), sx = x0<x1 ? 1 : -1;
    const int dy = -std::abs(y1-y0), sy = y0<y1 ? 1 : -1;
    const int dy_inc = sy * int(image_.row_increment());
    int err = dx+dy, e2;                                  /* error value e_xy */
    pixel_t * pos = image_.row(y0) + x0;
    Setter setPixel{image_};

    for (;;) {                                                        /* loop */
        setPixel(x0, y0, pos, color);
        e2 = 2*err;
        if (e2 >= dy) {                                       /* e_xy+e_x > 0 */
            if (x0 == x1) break;
            err += dy;
            x0 += sx;
            pos += sx;
        }
        if (e2 <= dx) {                                       /* e_xy+e_y < 0 */
            if (y0 == y1) break;
            err += dx;
            y0 += sy;
            pos += dy_inc;
        }
    }
}

template <class BitmapImageType>
template <class Setter>
void zingl_image_drawer<BitmapImageType>::plotLine(
    Point ptA, Point ptB, const pixel_t color)
{
    plotLine<Setter>(ptA.first, ptA.second, ptB.first, ptB.second, color);
}

}
