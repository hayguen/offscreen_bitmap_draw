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

#include "bitmap_image_rgb.hpp"


namespace OffScreenBitmapDraw
{

template <class BitmapImageType = bitmap_image_rgb<> >
inline void checkered_pattern(const unsigned x_width,
                              const unsigned y_width,
                              const typename BitmapImageType::component_t value,
                              const color_plane color,
                              BitmapImageType& image)
{
    if ( x_width >= image.width () || y_width >= image.height() )
    {
        assert(0);
        return;
    }
    bool setter_x = false;
    bool setter_y = true;
    const unsigned color_plane_offset = image.offset(color);
    const unsigned height = image.height();
    const unsigned width  = image.width();
    for (unsigned y = 0; y < height; ++y)
    {
        typename BitmapImageType::component_t* row = image.char_row(y) + color_plane_offset;
        if (0 == (y % y_width))
            setter_y = !setter_y;
        for (unsigned x = 0; x < width; ++x, row += image.bytes_per_pixel())
        {
            if (0 == (x % x_width))
                setter_x = !setter_x;
            if (setter_x ^ setter_y)
                *row = value;
        }
    }
}

template <class BitmapImageType = bitmap_image_rgb<> >
inline void checkered_pattern(const unsigned x_width,
                              const unsigned y_width,
                              const typename BitmapImageType::pixel_t color,
                              BitmapImageType& image)
{
    if ( x_width >= image.width () || y_width >= image.height() )
    {
        assert(0);
        return;
    }
    bool setter_x = false;
    bool setter_y = true;
    const unsigned height = image.height();
    const unsigned width  = image.width();
    for (unsigned y = 0; y < height; ++y)
    {
        typename BitmapImageType::pixel_t* row = image.row(y);
        if (0 == (y % y_width))
            setter_y = !setter_y;
        for (unsigned x = 0; x < width; ++x, ++row)
        {
            if (0 == (x % x_width))
                setter_x = !setter_x;
            if (setter_x ^ setter_y)
                *row = color;
        }
    }
}

}
