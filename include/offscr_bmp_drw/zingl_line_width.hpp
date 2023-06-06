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
void zingl_image_drawer<BitmapImageType>::plotLineWidth(
    int x0, int y0, const int x1, const int y1, float wd, const pixel_t color)
{
    int dx = std::abs(x1-x0);
    int dy = std::abs(y1-y0);
    const int sx = x0 < x1 ? 1 : -1;
    const int sy = y0 < y1 ? 1 : -1;
    int err = dx-dy, e2, x2, y2;                           /* error value e_xy */
    float ed = dx+dy == 0 ? 1 : std::sqrt((float)dx*dx+(float)dy*dy);
    const int dy_inc = sy * int(image_.row_increment());
    pixel_t * row0 = image_.row(y0);
    Setter setPixel{image_};

    for (wd = (wd+1)/2; ; ) {                                    /* pixel loop */
        float col_mulA = (std::abs(err-dx+dy)/ed-wd+1);  // * 255
        setPixel(x0, y0, &row0[x0], color * (1.0F -std::max(0.0F, col_mulA)));
        e2 = err; x2 = x0;
        if (2*e2 >= -dx) {                                            /* x step */
            pixel_t * row2 = row0;
            for (e2 += dy, y2 = y0; e2 < ed*wd && (y1 != y2 || dx > dy); e2 += dx)
            {
                y2 += sy; row2 += dy_inc;
                float col_mulB = (std::abs(e2)/ed-wd+1);  // * 255
                setPixel(x0, y2, &row2[x0], color * (1.0F -std::max(0.0F, col_mulB)));
            }
            if (x0 == x1) break;
            e2 = err; err -= dy; x0 += sx;
        }
        if (2*e2 <= dy) {                                             /* y step */
            for (e2 = dx-e2; e2 < ed*wd && (x1 != x2 || dx < dy); e2 += dy)
            {
                x2 += sx;
                float col_mulB = (std::abs(e2)/ed-wd+1);  // * 255
                setPixel(x2, y0, &row0[x2], color * (1.0F -std::max(0.0F, col_mulB)));
            }
            if (y0 == y1) break;
            err += dx; y0 += sy; row0 += dy_inc;
        }
    }
}

template <class BitmapImageType>
template <class Setter>
void zingl_image_drawer<BitmapImageType>::plotLineWidth(
    const Point ptA, const Point ptB, float wd, const pixel_t color)
{
    plotLineWidth<Setter>(ptA.first, ptA.second, ptB.first, ptB.second, wd, color);
}

}
