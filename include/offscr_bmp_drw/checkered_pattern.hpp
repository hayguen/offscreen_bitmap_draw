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

inline void checkered_pattern(const unsigned int x_width,
                              const unsigned int y_width,
                              const unsigned char value,
                              const bitmap_image::color_plane color,
                              bitmap_image& image)
{
   if (
        (x_width >= image.width ()) ||
        (y_width >= image.height())
      )
   {
      return;
   }

   bool setter_x = false;
   bool setter_y = true;

   const unsigned int color_plane_offset = image.offset(color);
   const unsigned int height = image.height();
   const unsigned int width  = image.width();

   for (unsigned int y = 0; y < height; ++y)
   {
      if (0 == (y % y_width))
      {
         setter_y = !setter_y;
      }

      unsigned char* row = image.row(y) + color_plane_offset;

      for (unsigned int x = 0; x < width; ++x, row += image.bytes_per_pixel())
      {
         if (0 == (x % x_width))
         {
            setter_x = !setter_x;
         }

         if (setter_x ^ setter_y)
         {
            *row = value;
         }
      }
   }
}

inline void checkered_pattern(const unsigned int x_width,
                              const unsigned int y_width,
                              const unsigned char red,
                              const unsigned char green,
                              const unsigned char blue,
                              bitmap_image& image)
{
   if (
        (x_width >= image.width ()) ||
        (y_width >= image.height())
      )
   {
      return;
   }

   bool setter_x = false;
   bool setter_y = true;

   const unsigned int height = image.height();
   const unsigned int width  = image.width();

   for (unsigned int y = 0; y < height; ++y)
   {
      if (0 == (y % y_width))
      {
         setter_y = !setter_y;
      }

      unsigned char* row = image.row(y);

      for (unsigned int x = 0; x < width; ++x, row += image.bytes_per_pixel())
      {
         if (0 == (x % x_width))
         {
            setter_x = !setter_x;
         }

         if (setter_x ^ setter_y)
         {
            *(row + 0) = blue;
            *(row + 1) = green;
            *(row + 2) = red;
         }
      }
   }
}

}
