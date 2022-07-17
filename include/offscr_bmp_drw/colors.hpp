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

#include <cstdlib>
#include <cmath>
#include <limits>
#include <iterator>


namespace OffScreenBitmapDraw
{

struct rgb_t
{
   unsigned char   red;
   unsigned char green;
   unsigned char  blue;
};

inline bool operator==(const rgb_t& c0, const rgb_t& c1)
{
   return (c0.red   == c1  .red) &&
          (c0.green == c1.green) &&
          (c0.blue  == c1 .blue) ;
}

inline bool operator!=(const rgb_t& c0, const rgb_t& c1)
{
   return (c0.red   != c1  .red) ||
          (c0.green != c1.green) ||
          (c0.blue  != c1 .blue) ;
}

inline std::size_t hamming_distance(const rgb_t& c0, const rgb_t& c1)
{
   std::size_t result = 0;

   if (c0.red   != c1  .red) ++result;
   if (c0.green != c1.green) ++result;
   if (c0.blue  != c1 .blue) ++result;

   return result;
}

inline rgb_t make_colour(const unsigned int& red, const unsigned int& green, const unsigned int& blue)
{
   rgb_t result;

   result.red   = static_cast<unsigned char>(red  );
   result.green = static_cast<unsigned char>(green);
   result.blue  = static_cast<unsigned char>(blue );

   return result;
}

template <typename OutputIterator>
inline void generate_colours(const std::size_t& steps, const rgb_t c0, const rgb_t& c1, OutputIterator out)
{
   double dr = ((double)c1.red   -  (double)c0.red   ) / steps;
   double dg = ((double)c1.green -  (double)c0.green ) / steps;
   double db = ((double)c1.blue  -  (double)c0.blue  ) / steps;

   for (std::size_t i = 0; i < steps; ++i)
   {
      rgb_t c;

      c.red   = static_cast<unsigned char>(c0.red   + (i * dr));
      c.green = static_cast<unsigned char>(c0.green + (i * dg));
      c.blue  = static_cast<unsigned char>(c0.blue  + (i * db));

      *(out++) = c;
   }
}

inline void rgb_to_ycbcr(const unsigned int& length, double* red, double* green, double* blue,
                                                     double* y,   double* cb,    double* cr)
{
   unsigned int i = 0;

   while (i < length)
   {
      ( *y) =   16.0 + (  65.481 * (*red) +  128.553 * (*green) +  24.966 * (*blue));
      (*cb) =  128.0 + ( -37.797 * (*red) +  -74.203 * (*green) + 112.000 * (*blue));
      (*cr) =  128.0 + ( 112.000 * (*red) +  -93.786 * (*green) -  18.214 * (*blue));

      ++i;
      ++red; ++green; ++blue;
      ++y;   ++cb;    ++cr;
   }
}

inline void ycbcr_to_rgb(const unsigned int& length, double* y,   double* cb,    double* cr,
                                                     double* red, double* green, double* blue)
{
   unsigned int i = 0;

   while (i < length)
   {
      double y_  =  (*y) -  16.0;
      double cb_ = (*cb) - 128.0;
      double cr_ = (*cr) - 128.0;

        (*red) = 0.000456621 * y_                    + 0.00625893 * cr_;
      (*green) = 0.000456621 * y_ - 0.00153632 * cb_ - 0.00318811 * cr_;
       (*blue) = 0.000456621 * y_                    + 0.00791071 * cb_;

      ++i;
      ++red; ++green; ++blue;
      ++y;   ++cb;    ++cr;
   }
}


inline rgb_t convert_wave_length_nm_to_rgb(const double wave_length_nm)
{
   // Credits: Dan Bruton http://www.physics.sfasu.edu/astro/color.html
   double red   = 0.0;
   double green = 0.0;
   double blue  = 0.0;

   if ((380.0 <= wave_length_nm) && (wave_length_nm <= 439.0))
   {
      red   = -(wave_length_nm - 440.0) / (440.0 - 380.0);
      green = 0.0;
      blue  = 1.0;
   }
   else if ((440.0 <= wave_length_nm) && (wave_length_nm <= 489.0))
   {
      red   = 0.0;
      green = (wave_length_nm - 440.0) / (490.0 - 440.0);
      blue  = 1.0;
   }
   else if ((490.0 <= wave_length_nm) && (wave_length_nm <= 509.0))
   {
      red   = 0.0;
      green = 1.0;
      blue  = -(wave_length_nm - 510.0) / (510.0 - 490.0);
   }
   else if ((510.0 <= wave_length_nm) && (wave_length_nm <= 579.0))
   {
      red   = (wave_length_nm - 510.0) / (580.0 - 510.0);
      green = 1.0;
      blue  = 0.0;
   }
   else if ((580.0 <= wave_length_nm) && (wave_length_nm <= 644.0))
   {
      red   = 1.0;
      green = -(wave_length_nm - 645.0) / (645.0 - 580.0);
      blue  = 0.0;
   }
   else if ((645.0 <= wave_length_nm) && (wave_length_nm <= 780.0))
   {
      red   = 1.0;
      green = 0.0;
      blue  = 0.0;
   }

   double factor = 0.0;

   if ((380.0 <= wave_length_nm) && (wave_length_nm <= 419.0))
      factor = 0.3 + 0.7 * (wave_length_nm - 380.0) / (420.0 - 380.0);
   else if ((420.0 <= wave_length_nm) && (wave_length_nm <= 700.0))
      factor = 1.0;
   else if ((701.0 <= wave_length_nm) && (wave_length_nm <= 780.0))
      factor = 0.3 + 0.7 * (780.0 - wave_length_nm) / (780.0 - 700.0);
   else
      factor = 0.0;

   rgb_t result;

   const double gamma         =   0.8;
   const double intensity_max = 255.0;

   #define round(d) std::floor(d + 0.5)

   result.red   = static_cast<unsigned char>((red   == 0.0) ? red   : round(intensity_max * std::pow(red   * factor, gamma)));
   result.green = static_cast<unsigned char>((green == 0.0) ? green : round(intensity_max * std::pow(green * factor, gamma)));
   result.blue  = static_cast<unsigned char>((blue  == 0.0) ? blue  : round(intensity_max * std::pow(blue  * factor, gamma)));

   #undef round

   return result;
}

inline double weighted_distance(const unsigned char r0, const unsigned char g0, const unsigned char b0,
                                const unsigned char r1, const unsigned char g1, const unsigned char b1)
{
   const double diff_r = /*0.30 */ (r0 - r1);
   const double diff_g = /*0.59 */ (g0 - g1);
   const double diff_b = /*0.11 */ (b0 - b1);

   return std::sqrt((diff_r * diff_r) + (diff_g * diff_g) + (diff_b * diff_b));
}

inline double weighted_distance(const rgb_t c0, const rgb_t c1)
{
   return weighted_distance(c0.red, c0.green, c0.blue,
                            c1.red, c1.green, c1.blue);
}

template <typename Iterator>
inline rgb_t find_nearest_color(const rgb_t& c, const Iterator begin, const Iterator end)
{
   if (0 == std::distance(begin,end))
      return c;

   double min_d = std::numeric_limits<double>::max();
   rgb_t result = *begin;

   for (Iterator itr = begin; itr != end; ++itr)
   {
      if (c == (*itr))
      {
         return (*itr);
      }

      double curr_d = weighted_distance(c,*itr);

      if (curr_d < min_d)
      {
          min_d = curr_d;
         result = *itr;
      }
   }

   return result;
}

template <template <typename,typename> class Sequence,
          typename Allocator>
inline rgb_t find_nearest_color(const rgb_t& c, const Sequence<rgb_t,Allocator>& seq)
{
   return find_nearest_color(c, seq.begin(),seq.end());
}

template <std::size_t N>
inline rgb_t find_nearest_color(const rgb_t& c, const rgb_t (&colors)[N])
{
   return find_nearest_color(c, colors, colors + N);
}

inline double find_nearest_wave_length(const rgb_t& c, const double increment = 0.001)
{
   const double max_wave_length = 800.0; //800nm

   double min_wave_length = 0.0;
   double min_d           = std::numeric_limits<double>::max();

   for (double i = 0.0; i < max_wave_length; i += increment)
   {
      const rgb_t  curr_rgb = convert_wave_length_nm_to_rgb(i);

      if (c == curr_rgb)
      {
         return i;
      }

      const double curr_d = weighted_distance(c, curr_rgb);

      if (curr_d <= min_d)
      {
         min_wave_length = i;
         min_d = curr_d;
      }
   }

   return min_wave_length;
}


enum palette_name
{
   e_red,           e_scarlet,      e_vermilion,        e_tangelo,         e_orange,
   e_gamboge,       e_amber,        e_gold,             e_yellow,          e_apple_green,
   e_lime_green,    e_spring_bud,   e_chartreuse_green, e_pistachio,       e_harlequin,
   e_sap_green,     e_green,        e_emerald_green,    e_malachite_green, e_sea_green,
   e_spring_green,  e_aquamarine,   e_turquoise,        e_opal,            e_cyan,
   e_arctic_blue,   e_cerulean,     e_cornflower_blue,  e_azure,           e_cobalt_blue,
   e_sapphire_blue, e_phthalo_blue, e_blue,             e_persian_blue,    e_indigo,
   e_blue_violet,   e_violet,       e_purple,           e_mulberry,        e_heliotrope,
   e_magenta,       e_orchid,       e_fuchsia,          e_cerise,          e_rose,
   e_raspberry,     e_crimson,      e_amaranth,         e_white,           e_black
};

}
