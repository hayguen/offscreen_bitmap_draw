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

#include "bitmap_image.hpp"


namespace OffScreenBitmapDraw
{

inline void plasma(bitmap_image& image,
                   const double& x,     const double& y,
                   const double& width, const double& height,
                   const double& c1,    const double& c2,
                   const double& c3,    const double& c4,
                   const double& roughness  = 3.0,
                   const rgb_t   colormap[] = 0)
{
   // Note: c1,c2,c3,c4 -> [0.0,1.0]

   const double half_width  = ( width / 2.0);
   const double half_height = (height / 2.0);

   if ((width >= 1.0) || (height >= 1.0))
   {
      const double corner1 = (c1 + c2) / 2.0;
      const double corner2 = (c2 + c3) / 2.0;
      const double corner3 = (c3 + c4) / 2.0;
      const double corner4 = (c4 + c1) / 2.0;
            double center  = (c1 + c2 + c3 + c4) / 4.0 +
                             ((1.0 * ::rand() /(1.0 * RAND_MAX))  - 0.5) * // should use a better rng
                             ((1.0 * half_width + half_height) / (image.width() + image.height()) * roughness);

      center = std::min<double>(std::max<double>(0.0,center),1.0);

      plasma(image, x,                            y, half_width, half_height,      c1, corner1,  center, corner4,roughness,colormap);
      plasma(image, x + half_width,               y, half_width, half_height, corner1,      c2, corner2,  center,roughness,colormap);
      plasma(image, x + half_width, y + half_height, half_width, half_height,  center, corner2,      c3, corner3,roughness,colormap);
      plasma(image, x,              y + half_height, half_width, half_height, corner4,  center, corner3,      c4,roughness,colormap);
   }
   else
   {
      rgb_t color = colormap[static_cast<unsigned int>(1000.0 * ((c1 + c2 + c3 + c4) / 4.0)) % 1000];

      image.set_pixel(static_cast<unsigned int>(x),static_cast<unsigned int>(y),color);
   }
}

inline void plasma(bitmap_image& image,
                   const double& c1, const double& c2,
                   const double& c3, const double& c4,
                   const double& roughness  = 3.0,
                   const rgb_t   colormap[] = 0)
{
   plasma
   (
     image, 0, 0, image.width(), image.height(),
     c1, c2, c3, c4,
     roughness, colormap
   );
}

}
