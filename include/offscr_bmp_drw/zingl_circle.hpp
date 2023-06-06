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
void zingl_image_drawer<BitmapImageType>::plotCircle(
    const int xm, const int ym, const int radius, const pixel_t color)
{
    int r = radius;
    int x = -r, y = 0, err = 2-2*r;                /* bottom left to top right */
    const int row_inc = int(image_.row_increment());
    pixel_t * row_upper = image_.row(ym + y);
    pixel_t * row_lower = image_.row(ym - y);
    pixel_t * row_left  = image_.row(ym - x);
    pixel_t * row_right = image_.row(ym + x);
    Setter setPixel{image_};

    do {
        setPixel(xm-x, ym+y, &row_upper[xm-x], color);   /*   I. Quadrant +x +y */
        setPixel(xm-y, ym-x, &row_left [xm-y], color);   /*  II. Quadrant -x +y */
        setPixel(xm+x, ym-y, &row_lower[xm+x], color);   /* III. Quadrant -x -y */
        setPixel(xm+y, ym+x, &row_right[xm+y], color);   /*  IV. Quadrant +x -y */

        r = err;
        if (r <= y) {                                           /* e_xy+e_y < 0 */
             err += ++y*2+1;
             row_upper += row_inc;
             row_lower -= row_inc;
        }
        if (r > x || err > y) {                /* e_xy+e_x > 0 or no 2nd y-step */
             err += ++x*2+1;                                   /* -> x-step now */
             row_left  -= row_inc;
             row_right += row_inc;
        }
    } while (x < 0);
}

template <class BitmapImageType>
template <class Setter>
void zingl_image_drawer<BitmapImageType>::plotCircle(
    const Point ptCenter, const int radius, const pixel_t color)
{
    plotCircle<Setter>(ptCenter.first, ptCenter.second, radius, color);
}

}
