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

template <typename ResponseImage, typename Palette>
inline std::size_t convert_rsp_to_image(const ResponseImage& resp_image, const Palette& palette, bitmap_image& image)
{
   if (
        (resp_image.width () > image.width ()) ||
        (resp_image.height() > image.height())
      )
      return 0;

   for (std::size_t y = 0; y < resp_image.height(); ++y)
   {
      for (std::size_t x = 0; x < resp_image.width(); ++x)
      {
         const double v = resp_image(x,y);

         unsigned int index = static_cast<unsigned int>((v < 0) ? 0 : v > (palette.size()) ? (palette.size() - 1) : v);

         image.set_pixel(x,y,palette[index]);
      }
   }

   return (resp_image.width() * resp_image.height());
}

}
