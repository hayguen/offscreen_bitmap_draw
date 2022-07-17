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


inline void subsample(const unsigned int& width,
                      const unsigned int& height,
                      const double* source,
                      unsigned int& w,
                      unsigned int& h,
                      double*& dest)
{
   /*  Single channel.  */

   w = 0;
   h = 0;

   bool odd_width = false;
   bool odd_height = false;

   if (0 == (width % 2))
      w = width / 2;
   else
   {
      w = 1 + (width / 2);
      odd_width = true;
   }

   if (0 == (height % 2))
      h = height / 2;
   else
   {
      h = 1 + (height / 2);
      odd_height = true;
   }

   unsigned int horizontal_upper = (odd_width)  ? w - 1 : w;
   unsigned int vertical_upper   = (odd_height) ? h - 1 : h;

   dest = new double[w * h];

         double* s_itr = dest;
   const double* itr1  = source;
   const double* itr2  = source + width;

   for (unsigned int j = 0; j < vertical_upper; ++j)
   {
      for (unsigned int i = 0; i < horizontal_upper; ++i, ++s_itr)
      {
          (*s_itr)  = *(itr1++);
          (*s_itr) += *(itr1++);
          (*s_itr) += *(itr2++);
          (*s_itr) += *(itr2++);
          (*s_itr) /=  4.0;
      }

      if (odd_width)
      {
         (*(s_itr++)) = ((*itr1++) + (*itr2++)) / 2.0;
      }

      itr1 += width;

      if (j != (vertical_upper -1))
      {
         itr2 += width;
      }
   }

   if (odd_height)
   {
      for (unsigned int i = 0; i < horizontal_upper; ++i, ++s_itr)
      {
         (*s_itr) += (*(itr1++));
         (*s_itr) += (*(itr1++));
         (*s_itr) /= 2.0;
      }

      if (odd_width)
      {
         (*(s_itr++)) = (*itr1);
      }
   }
}

inline void upsample(const unsigned int& width,
                     const unsigned int& height,
                     const double* source,
                     unsigned int& w,
                     unsigned int& h,
                     double*& dest)
{
   /* Single channel. */

   w = 2 * width;
   h = 2 * height;

   dest = new double[w * h];

   const double* s_itr = source;
         double* itr1  = dest;
         double* itr2  = dest + w;

   for (unsigned int j = 0; j < height; ++j)
   {
      for (unsigned int i = 0; i < width; ++i, ++s_itr)
      {
          *(itr1++) = (*s_itr);
          *(itr1++) = (*s_itr);
          *(itr2++) = (*s_itr);
          *(itr2++) = (*s_itr);
      }

      itr1 += w;
      itr2 += w;
   }
}

inline double psnr_region(const unsigned int& x,      const unsigned int& y,
                          const unsigned int& width,  const unsigned int& height,
                          const bitmap_image& image1, const bitmap_image& image2)
{
   if (
        (image1.width()  != image2.width ()) ||
        (image1.height() != image2.height())
      )
   {
      return 0.0;
   }

   if ((x + width ) > image1.width() ) { return 0.0; }
   if ((y + height) > image1.height()) { return 0.0; }

   double mse = 0.0;

   for (unsigned int r = 0; r < height; ++r)
   {
      const unsigned char* itr1     = image1.row(r + y) + x * image1.bytes_per_pixel();
      const unsigned char* itr1_end = itr1 + (width * image1.bytes_per_pixel());
      const unsigned char* itr2     = image2.row(r + y) + x * image2.bytes_per_pixel();

      while (itr1 != itr1_end)
      {
         double v = (static_cast<double>(*itr1) - static_cast<double>(*itr2));
         mse += v * v;
         ++itr1;
         ++itr2;
      }
   }

   if (mse <= 0.0000001)
   {
      return 1000000.0;
   }
   else
   {
      mse /= (3.0 * width * height);
      return 20.0 * std::log10(255.0 / std::sqrt(mse));
   }
}

inline void hierarchical_psnr_r(const double& x,     const double& y,
                                const double& width, const double& height,
                                const bitmap_image& image1,
                                      bitmap_image& image2,
                                const double& threshold,
                                const rgb_t colormap[])
{
   if ((width <= 4.0) || (height <= 4.0))
   {
      const double psnr = psnr_region
                          (
                            static_cast<unsigned int>(x),
                            static_cast<unsigned int>(y),
                            static_cast<unsigned int>(width),
                            static_cast<unsigned int>(height),
                            image1, image2
                          );

      if (psnr < threshold)
      {
         rgb_t c = colormap[static_cast<unsigned int>(1000.0 * (1.0 - (psnr / threshold)))];

         image2.set_region
                (
                  static_cast<unsigned int>(x),
                  static_cast<unsigned int>(y),
                  static_cast<unsigned int>(width + 1),
                  static_cast<unsigned int>(height + 1),
                  c.red, c.green, c.blue
                );
      }
   }
   else
   {
      const double half_width  = ( width / 2.0);
      const double half_height = (height / 2.0);

      hierarchical_psnr_r(x             , y              , half_width, half_height, image1, image2, threshold, colormap);
      hierarchical_psnr_r(x + half_width, y              , half_width, half_height, image1, image2, threshold, colormap);
      hierarchical_psnr_r(x + half_width, y + half_height, half_width, half_height, image1, image2, threshold, colormap);
      hierarchical_psnr_r(x             , y + half_height, half_width, half_height, image1, image2, threshold, colormap);
   }
}

inline void hierarchical_psnr(bitmap_image& image1, bitmap_image& image2, const double threshold, const rgb_t colormap[])
{
   if (
        (image1.width()  != image2.width ()) ||
        (image1.height() != image2.height())
      )
   {
      return;
   }

   const double psnr = psnr_region
                       (
                         0, 0, image1.width(), image1.height(),
                         image1, image2
                       );

   if (psnr < threshold)
   {
      hierarchical_psnr_r
      (
        0, 0, image1.width(), image1.height(),
        image1, image2,
        threshold,
        colormap
      );
   }
}

}
