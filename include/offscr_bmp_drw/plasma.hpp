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

template <class BitmapImageType = bitmap_image_rgb<>, class Float = double>
inline void plasma(
    BitmapImageType& image,
    const Float& x,     const Float& y,
    const Float& width, const Float& height,
    const Float& c1,    const Float& c2,
    const Float& c3,    const Float& c4,
    const Float& roughness  = Float(3),
    const rgb_t colormap[] = 0
    )
{
    // Note: c1,c2,c3,c4 -> [0.0,1.0]
    const Float half_width  = ( width / Float(2));
    const Float half_height = (height / Float(2));

    if ( width >= Float(1) || height >= Float(1) )
    {
        const Float corner1 = (c1 + c2) / Float(2);
        const Float corner2 = (c2 + c3) / Float(2);
        const Float corner3 = (c3 + c4) / Float(2);
        const Float corner4 = (c4 + c1) / Float(2);
        Float center  = (c1 + c2 + c3 + c4) / Float(4)
                       + (( Float(1) * ::rand() / (Float(1) * RAND_MAX))  - Float(0.5))   // should use a better rng
                             * ( (Float(1) * half_width + half_height) / (image.width() + image.height()) * roughness );
        center = std::min<Float>(std::max<Float>(Float(0),center), Float(1));
        plasma(image, x,                            y, half_width, half_height,      c1, corner1,  center, corner4,roughness,colormap);
        plasma(image, x + half_width,               y, half_width, half_height, corner1,      c2, corner2,  center,roughness,colormap);
        plasma(image, x + half_width, y + half_height, half_width, half_height,  center, corner2,      c3, corner3,roughness,colormap);
        plasma(image, x,              y + half_height, half_width, half_height, corner4,  center, corner3,      c4,roughness,colormap);
    }
    else
    {
        rgb_t color = colormap[static_cast<unsigned int>(Float(1000) * ((c1 + c2 + c3 + c4) / Float(4))) % 1000];
        image.set_pixel(static_cast<unsigned int>(x), static_cast<unsigned int>(y), color);
    }
}

template <class BitmapImageType = bitmap_image_rgb<>, class Float = double>
inline void plasma(
    BitmapImageType& image,
    const Float& c1, const Float& c2,
    const Float& c3, const Float& c4,
    const Float& roughness  = Float(3),
    const rgb_t colormap[] = 0
    )
{
    plasma(
        image, 0, 0, image.width(), image.height(),
        c1, c2, c3, c4,
        roughness, colormap
        );
}

}
