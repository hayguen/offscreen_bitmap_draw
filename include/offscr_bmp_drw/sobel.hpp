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
#include "response_image.hpp"


namespace OffScreenBitmapDraw
{

inline void sobel_operator(const bitmap_image& src_image,
                                 bitmap_image& dst_image,
                           const double threshold = 0.0)
{
   typedef double T;

   response_image<T> im0(src_image.width(), src_image.height(), 0.0);
   response_image<T> im1(src_image.width(), src_image.height(), 0.0);

   src_image.export_gray_scale_response_image(&im0(0,0));

   for (std::size_t y = 1; y < im0.height() - 1; ++y)
   {
      const T* itr0 = im0.row(y - 1);
      const T* itr1 = im0.row(y    );
      const T* itr2 = im0.row(y + 1);
            T* out  = im1.row(y    ) + 1;

      for (std::size_t x = 1; x < im0.width() - 1; ++x)
      {
         const T c0 = *(itr0 + x - 1);   const T c1 = *(itr0 + x);   const T c2 = *(itr0 + x + 1);
         const T c3 = *(itr1 + x - 1); /*const T c4 = *(itr1 + x);*/ const T c5 = *(itr1 + x + 1);
         const T c6 = *(itr2 + x - 1);   const T c7 = *(itr2 + x);   const T c8 = *(itr2 + x + 1);

         const T gx = (2.0 * (c5 - c3)) + (c2 - c0) + (c8 - c6);
         const T gy = (2.0 * (c1 - c7)) + (c0 - c6) + (c2 - c8);

         *(out++) = std::sqrt((gx * gx) + (gy * gy));
      }
   }

   if (threshold > 0.0)
   {
      const T* end = im1.row(0) + (im1.width() * im1.height());

      for (T* itr = im1.row(0); itr != end; ++itr)
      {
         T& v = *itr;
         if (v <= threshold) v = 0;
      }
   }

   dst_image.setwidth_height
             (
               static_cast<unsigned int>(im1.width()),
               static_cast<unsigned int>(im1.height())
             );

   dst_image.import_gray_scale_clamped(&im1(0,0));
}

}
