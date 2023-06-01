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
#include <cstdint>
#include <cmath>
#include <limits>
#include <iterator>


namespace OffScreenBitmapDraw
{

enum channel_mode {
    rgb_mode = 0,
    bgr_mode = 1
};

enum color_plane {
    blue_plane  = 0,
    green_plane = 1,
    red_plane   = 2
};


#pragma pack(push, 1)

/// 24-bit Big Endian storage of 0xRRGGBB
struct alignas(unsigned char) rgb_t
{
    using component = unsigned char;

    rgb_t() = default;
    rgb_t(const rgb_t &) = default;

    rgb_t(component r, component g, component b)
        : red(r), green(g), blue(b) { }

    static rgb_t from(const uint32_t u)
    { rgb_t c; c.red = (u >> 16) & 0xFF; c.green = (u >> 8) & 0xFF; c.blue = u & 0xFF; return c; }

    static inline unsigned offset(const color_plane color)
    {
        switch (color)
        {
        case red_plane:   return 0;
        case green_plane: return 1;
        case blue_plane:  return 2;
        default:          return std::numeric_limits<unsigned int>::max();
        }
    }

    explicit operator uint32_t() const
    { return ( (uint32_t(red) << 16) | (uint32_t(green) << 8) | uint32_t(blue) ); }

    component   red;
    component green;
    component  blue;
};

template <class RGBColorType>
RGBColorType scale_rgb(RGBColorType c, float s) {
    RGBColorType ret = c;
    ret.red   = static_cast<typename RGBColorType::component>(s * c.red  );
    ret.green = static_cast<typename RGBColorType::component>(s * c.green);
    ret.blue  = static_cast<typename RGBColorType::component>(s * c.blue );
    return ret;
}


rgb_t::component * begin(rgb_t &c) { return &c.red; }
rgb_t::component * end(rgb_t &c)   { return begin(c) + 3; }
const rgb_t::component * begin(const rgb_t &c) { return &c.red; }
const rgb_t::component * end(const rgb_t &c)   { return begin(c) + 3; }
const rgb_t::component * cbegin(const rgb_t &c) { return &c.red; }
const rgb_t::component * cend(const rgb_t &c)   { return begin(c) + 3; }
rgb_t::component * red  (rgb_t &c) { return &c.red;   }
rgb_t::component * green(rgb_t &c) { return &c.green; }
rgb_t::component * blue (rgb_t &c) { return &c.blue;  }
const rgb_t::component * red  (const rgb_t &c) { return &c.red;   }
const rgb_t::component * green(const rgb_t &c) { return &c.green; }
const rgb_t::component * blue (const rgb_t &c) { return &c.blue;  }
rgb_t &set_black(rgb_t &c) { c.red = c.green = c.blue = 0x0;  return c; }
rgb_t &set_white(rgb_t &c) { c.red = c.green = c.blue = 0xFF; return c; }
rgb_t &set_gray(rgb_t &c, rgb_t::component g) { c.red = c.green = c.blue = g; return c; }
rgb_t &set_gray(rgb_t &c, float g) { c.red = c.green = c.blue = rgb_t::component(255.99F * g); return c; }
rgb_t &set_rgb(rgb_t &c, rgb_t::component r, rgb_t::component g, rgb_t::component b) { c.red = r; c.green = g; c.blue = b; return c; }
rgb_t operator*(rgb_t c, float s) { return scale_rgb<rgb_t>(c, s); }



/// 24-bit Little Endian storage of 0xRRGGBB
struct alignas(unsigned char) bgr_t
{
    using component = unsigned char;

    bgr_t() = default;
    bgr_t(const bgr_t &) = default;

    bgr_t(const rgb_t & c)
        : blue(c.blue), green(c.green), red(c.red) { }

    bgr_t(component r, component g, component b)
        : blue(b), green(g), red(r) { }

    static bgr_t from(const uint32_t u)
    { bgr_t c; c.red = (u >> 16) & 0xFF; c.green = (u >> 8) & 0xFF; c.blue = u & 0xFF; return c; }

    static inline unsigned offset(const color_plane color)
    {
        switch (color)
        {
        case red_plane:   return 2;
        case green_plane: return 1;
        case blue_plane:  return 0;
        default:          return std::numeric_limits<unsigned int>::max();
        }
    }

    explicit operator uint32_t() const
    { return ( (uint32_t(red) << 16) | (uint32_t(green) << 8) | uint32_t(blue) ); }

    component  blue;
    component green;
    component   red;
};

bgr_t::component * begin(bgr_t &c) { return &c.blue; }
bgr_t::component * end(bgr_t &c)   { return begin(c) + 3; }
const bgr_t::component * begin(const bgr_t &c) { return &c.blue; }
const bgr_t::component * end(const bgr_t &c)   { return begin(c) + 3; }
const bgr_t::component * cbegin(const bgr_t &c) { return &c.blue; }
const bgr_t::component * cend(const bgr_t &c)   { return begin(c) + 3; }
bgr_t::component * red  (bgr_t &c) { return &c.red;   }
bgr_t::component * green(bgr_t &c) { return &c.green; }
bgr_t::component * blue (bgr_t &c) { return &c.blue;  }
const bgr_t::component * red  (const bgr_t &c) { return &c.red;   }
const bgr_t::component * green(const bgr_t &c) { return &c.green; }
const bgr_t::component * blue (const bgr_t &c) { return &c.blue;  }
bgr_t &set_black(bgr_t &c) { c.red = c.green = c.blue = 0x0;  return c; }
bgr_t &set_white(bgr_t &c) { c.red = c.green = c.blue = 0xFF; return c; }
bgr_t &set_gray(bgr_t &c, bgr_t::component g) { c.red = c.green = c.blue = g; return c; }
bgr_t &set_gray(bgr_t &c, float g) { c.red = c.green = c.blue = bgr_t::component(255.99F * g); return c; }
bgr_t &set_rgb(bgr_t &c, bgr_t::component r, bgr_t::component g, bgr_t::component b) { c.red = r; c.green = g; c.blue = b; return c; }
bgr_t operator*(bgr_t c, float s) { return scale_rgb<bgr_t>(c, s); }



/// 32-bit Big Endian storage of 0xAARRGGBB
struct alignas(unsigned char) rgba_t
{
    using component = unsigned char;

    rgba_t() = default;
    rgba_t( const rgba_t & ) = default;

    rgba_t( const rgb_t & c )
        : red(c.red), green(c.green), blue(c.blue), alpha(0) { }

    rgba_t(component r, component g, component b)
        : red(r), green(g), blue(b), alpha(0) { }

    static rgba_t from(const uint32_t u)
    { rgba_t c; c.alpha = (u >> 24) & 0xFF; c.red = (u >> 16) & 0xFF; c.green = (u >> 8) & 0xFF; c.blue = u & 0xFF; return c; }

    static inline unsigned offset(const color_plane color)
    {
       switch (color)
       {
       case red_plane:   return 0;
       case green_plane: return 1;
       case blue_plane:  return 2;
       default:          return std::numeric_limits<unsigned int>::max();
       }
    }

    explicit operator uint32_t() const
    { return ( (uint32_t(alpha) << 24) | (uint32_t(red) << 16) | (uint32_t(green) << 8) | uint32_t(blue) ); }

    component   red;
    component green;
    component  blue;
    component alpha;
};

rgba_t::component * begin(rgba_t &c) { return &c.red; }
rgba_t::component * end(rgba_t &c)   { return begin(c) + 4; }
const rgba_t::component * begin(const rgba_t &c) { return &c.red; }
const rgba_t::component * end(const rgba_t &c)   { return begin(c) + 4; }
const rgba_t::component * cbegin(const rgba_t &c) { return &c.red; }
const rgba_t::component * cend(const rgba_t &c)   { return begin(c) + 4; }
rgba_t::component * red  (rgba_t &c) { return &c.red;   }
rgba_t::component * green(rgba_t &c) { return &c.green; }
rgba_t::component * blue (rgba_t &c) { return &c.blue;  }
const rgba_t::component * red  (const rgba_t &c) { return &c.red;   }
const rgba_t::component * green(const rgba_t &c) { return &c.green; }
const rgba_t::component * blue (const rgba_t &c) { return &c.blue;  }
rgba_t &set_black(rgba_t &c) { c.red = c.green = c.blue = 0x0;  c.alpha = 0x0; return c; }
rgba_t &set_white(rgba_t &c) { c.red = c.green = c.blue = 0xFF; c.alpha = 0x0; return c; }
rgba_t &set_gray(rgba_t &c, rgba_t::component g) { c.red = c.green = c.blue = g; c.alpha = 0x0; return c; }
rgba_t &set_gray(rgba_t &c, float g) { c.red = c.green = c.blue = rgba_t::component(255.99F * g); return c; }
rgba_t &set_rgb(rgba_t &c, rgba_t::component r, rgba_t::component g, rgba_t::component b) { c.red = r; c.green = g; c.blue = b; c.alpha = 0; return c; }
rgba_t operator*(rgba_t c, float s) { return scale_rgb<rgba_t>(c, s); }



/// 32-bit Little Endian storage of 0xAARRGGBB
struct alignas(unsigned char) abgr_t
{
    using component = unsigned char;

    abgr_t() = default;
    abgr_t( const abgr_t & ) = default;

    abgr_t( const rgb_t & c )
        : alpha(0), blue(c.blue), green(c.green), red(c.red) { }

    abgr_t(component r, component g, component b)
        : alpha(0), blue(b), green(g), red(r) { }

    static abgr_t from(const uint32_t u)
    { abgr_t c; c.alpha = (u >> 24) & 0xFF; c.red = (u >> 16) & 0xFF; c.green = (u >> 8) & 0xFF; c.blue = u & 0xFF; return c; }

    static inline unsigned offset(const color_plane color)
    {
       switch (color)
       {
       case red_plane:   return 3;
       case green_plane: return 2;
       case blue_plane:  return 1;
       default:          return std::numeric_limits<unsigned int>::max();
       }
    }

    explicit operator uint32_t() const
    { return ( (uint32_t(alpha) << 24) | (uint32_t(red) << 16) | (uint32_t(green) << 8) | uint32_t(blue) ); }

    component alpha;
    component  blue;
    component green;
    component   red;
};

abgr_t::component * begin(abgr_t &c) { return &c.alpha; }
abgr_t::component * end(abgr_t &c)   { return begin(c) + 4; }
const abgr_t::component * begin(const abgr_t &c) { return &c.alpha; }
const abgr_t::component * end(const abgr_t &c)   { return begin(c) + 4; }
const abgr_t::component * cbegin(const abgr_t &c) { return &c.alpha; }
const abgr_t::component * cend(const abgr_t &c)   { return begin(c) + 4; }
abgr_t::component * red  (abgr_t &c) { return &c.red;   }
abgr_t::component * green(abgr_t &c) { return &c.green; }
abgr_t::component * blue (abgr_t &c) { return &c.blue;  }
const abgr_t::component * red  (const abgr_t &c) { return &c.red;   }
const abgr_t::component * green(const abgr_t &c) { return &c.green; }
const abgr_t::component * blue (const abgr_t &c) { return &c.blue;  }
abgr_t &set_black(abgr_t &c) { c.red = c.green = c.blue = 0x0;  c.alpha = 0x0; return c; }
abgr_t &set_white(abgr_t &c) { c.red = c.green = c.blue = 0xFF; c.alpha = 0x0; return c; }
abgr_t &set_gray(abgr_t &c, abgr_t::component g) { c.red = c.green = c.blue = g; c.alpha = 0x0; return c; }
abgr_t &set_gray(abgr_t &c, float g) { c.red = c.green = c.blue = abgr_t::component(255.99F * g); return c; }
abgr_t &set_rgb(abgr_t &c, abgr_t::component r, abgr_t::component g, abgr_t::component b) { c.red = r; c.green = g; c.blue = b; c.alpha = 0; return c; }
abgr_t operator*(abgr_t c, float s) { return scale_rgb<abgr_t>(c, s); }



#pragma pack(pop)


// single component float as color
float * begin(float &c) { return &c; }
float * end(float &c)   { return begin(c) + 1; }
const float * begin(const float &c) { return &c; }
const float * end(const float &c)   { return begin(c) + 1; }
const float * cbegin(const float &c) { return &c; }
const float * cend(const float &c)   { return begin(c) + 1; }
float * red  (float &c) { return &c; }
float * green(float &c) { return &c; }
float * blue (float &c) { return &c; }
const float * red  (const float &c) { return &c; }
const float * green(const float &c) { return &c; }
const float * blue (const float &c) { return &c; }
float &set_black(float &c) { c = 0.0F; return c; }
float &set_white(float &c) { c = 1.0F; return c; }
float &set_gray(float &c, float g) { c = g; return c; }


// single component double as color
double * begin(double &c) { return &c; }
double * end(double &c)   { return begin(c) + 1; }
const double * begin(const double &c) { return &c; }
const double * end(const double &c)   { return begin(c) + 1; }
const double * cbegin(const double &c) { return &c; }
const double * cend(const double &c)   { return begin(c) + 1; }
double * red  (double &c) { return &c; }
double * green(double &c) { return &c; }
double * blue (double &c) { return &c; }
const double * red  (const double &c) { return &c; }
const double * green(const double &c) { return &c; }
const double * blue (const double &c) { return &c; }
double &set_black(double &c) { c = 0.0; return c; }
double &set_white(double &c) { c = 1.0; return c; }
double &set_gray(double &c, double g) { c = g; return c; }



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

template <class RGBColorType = rgb_t>
inline std::size_t hamming_distance(const RGBColorType& c0, const RGBColorType& c1)
{
   int diff_r = int(red(c0)) - int(red(c1));
   int diff_g = int(green(c0)) - int(green(c1));
   int diff_b = int(blue(c0)) - int(blue(c1));
   int diff_sum = std::abs(diff_r) + std::abs(diff_g) + std::abs(diff_b);
   return diff_sum;
}


template <class RGBColorType = rgb_t>
inline RGBColorType make_colour(const unsigned int& red, const unsigned int& green, const unsigned int& blue)
{
   RGBColorType result;
   return set_rgb(result,
        static_cast<typename RGBColorType::component>(red),
        static_cast<typename RGBColorType::component>(green),
        static_cast<typename RGBColorType::component>(blue) );
}


template <typename OutputIterator, class RGBColorType = rgb_t, class Float = float>
inline void generate_colours(const std::size_t& steps, const RGBColorType c0, const RGBColorType c1, OutputIterator out)
{
    const Float r0 = Float(  *red(c0));
    const Float g0 = Float(*green(c0));
    const Float b0 = Float( *blue(c0));
    const Float dr = ( Float(  *red(c1)) -  r0 ) / steps;
    const Float dg = ( Float(*green(c1)) -  g0 ) / steps;
    const Float db = ( Float( *blue(c1)) -  b0 ) / steps;

    for (std::size_t i = 0; i < steps; ++i)
    {
        RGBColorType c;
        set_rgb( c,
                static_cast<typename RGBColorType::component>(r0 + (i * dr)),
                static_cast<typename RGBColorType::component>(g0 + (i * dg)),
                static_cast<typename RGBColorType::component>(b0 + (i * db)) );
      *(out++) = c;
    }
}


template <class Float = double>
inline void rgb_to_ycbcr(
    const unsigned int& length,
    Float* red, Float* green, Float* blue,
    Float* y,   Float* cb,    Float* cr
    )
{
    unsigned int i = 0;

    while (i < length)
    {
        ( *y) =  Float( 16) + ( Float( 65.481) * (*red) +  Float(128.553) * (*green) + Float( 24.966) * (*blue));
        (*cb) =  Float(128) + ( Float(-37.797) * (*red) +  Float(-74.203) * (*green) + Float(112.000) * (*blue));
        (*cr) =  Float(128) + ( Float(112.000) * (*red) +  Float(-93.786) * (*green) - Float( 18.214) * (*blue));

        ++i;
        ++red; ++green; ++blue;
        ++y;   ++cb;    ++cr;
    }
}

template <class Float = double>
inline void ycbcr_to_rgb(
    const unsigned int& length,
    Float* y,   Float* cb,    Float* cr,
    Float* red, Float* green, Float* blue)
{
   unsigned int i = 0;
   while (i < length)
   {
      const Float y_  =  (*y) - Float( 16);
      const Float cb_ = (*cb) - Float(128);
      const Float cr_ = (*cr) - Float(128);
        (*red) = Float(0.000456621) * y_                           + Float(0.00625893) * cr_;
      (*green) = Float(0.000456621) * y_ - Float(0.00153632) * cb_ - Float(0.00318811) * cr_;
       (*blue) = Float(0.000456621) * y_                           + Float(0.00791071) * cb_;
      ++i;
      ++red; ++green; ++blue;
      ++y;   ++cb;    ++cr;
   }
}


template <class RGBColorType = rgb_t, class Float = float>
inline RGBColorType convert_wave_length_nm_to_rgb(const Float wave_length_nm)
{
    // Credits: Dan Bruton http://www.physics.sfasu.edu/astro/color.html
    Float red   = 0;
    Float green = 0;
    Float blue  = 0;

    if ( 380 <= wave_length_nm && wave_length_nm <= 439 )
    {
       red   = -(wave_length_nm - 440) / Float(440 - 380);
       green = Float(0);
       blue  = Float(1);
    }
    else if ( 440 <= wave_length_nm && wave_length_nm <= 489 )
    {
       red   = Float(0);
       green = (wave_length_nm - 440) / Float(490 - 440);
       blue  = Float(1);
    }
    else if ( 490 <= wave_length_nm && wave_length_nm <= 509 )
    {
       red   = Float(0);
       green = Float(1);
       blue  = -(wave_length_nm - 510) / Float(510 - 490);
    }
    else if ( 510 <= wave_length_nm && wave_length_nm <= 579 )
    {
       red   = (wave_length_nm - 510) / Float(580 - 510);
       green = Float(1);
       blue  = Float(0);
    }
    else if ( 580 <= wave_length_nm && wave_length_nm <= 644 )
    {
       red   = Float(1);
       green = -(wave_length_nm - 645) / Float(645 - 580);
       blue  = Float(0);
    }
    else if ( 645 <= wave_length_nm && wave_length_nm <= 780 )
    {
       red   = Float(1);
       green = Float(0);
       blue  = Float(0);
    }

    Float factor = 0;

    if ( 380 <= wave_length_nm && wave_length_nm <= 419 )
        factor = Float(0.3) + Float(0.7) * (wave_length_nm - 380) / Float(420 - 380);
    else if ( 420 <= wave_length_nm && wave_length_nm <= 700 )
        factor = Float(1);
    else if ( 701 <= wave_length_nm && wave_length_nm <= 780 )
        factor = Float(0.3) + Float(0.7) * (780 - wave_length_nm) / Float(780 - 700);
    else
        factor = Float(0);

   RGBColorType result;

   const Float gamma         = Float(0.8);
   const Float intensity_max = Float(255);

   #define round(d) std::floor(d + 0.5)

   set_rgb( result,
           static_cast<typename RGBColorType::component>((red   == Float(0)) ? red   : round(intensity_max * std::pow(red   * factor, gamma))),
           static_cast<typename RGBColorType::component>((green == Float(0)) ? green : round(intensity_max * std::pow(green * factor, gamma))),
           static_cast<typename RGBColorType::component>((blue  == Float(0)) ? blue  : round(intensity_max * std::pow(blue  * factor, gamma))) );

   #undef round

   return result;
}


template <class Float = float>
inline Float weighted_rgb_distance(
    const unsigned char r0, const unsigned char g0, const unsigned char b0,
    const unsigned char r1, const unsigned char g1, const unsigned char b1
    )
{
    const Float diff_r = /*0.30 */ (r0 - r1);
    const Float diff_g = /*0.59 */ (g0 - g1);
    const Float diff_b = /*0.11 */ (b0 - b1);
    return std::sqrt((diff_r * diff_r) + (diff_g * diff_g) + (diff_b * diff_b));
}

template <class RGBColorType = rgb_t, class Float = float>
inline double weighted_distance(const RGBColorType c0, const RGBColorType c1)
{
    return weighted_rgb_distance<Float>(
        *red(c0), *green(c0), *blue(c0),
        *red(c1), *green(c1), *blue(c1)
        );
}

template <typename Iterator, class RGBColorType = rgb_t, class Float = float>
inline RGBColorType find_nearest_color(const RGBColorType& c, const Iterator begin, const Iterator end)
{
    if (0 == std::distance(begin,end))
        return c;
    Float min_d = std::numeric_limits<Float>::max();
    RGBColorType result = *begin;
    for (Iterator itr = begin; itr != end; ++itr)
    {
        if (c == (*itr))
            return (*itr);
        Float curr_d = weighted_distance<RGBColorType, Float>(c, *itr);
        if (curr_d < min_d)
        {
            min_d = curr_d;
            result = *itr;
        }
    }
    return result;
}

// template <template <typename,typename> class Sequence,
//           typename Allocator>
// inline rgb_t find_nearest_color(const rgb_t& c, const Sequence<rgb_t,Allocator>& seq)
// {
//    return find_nearest_color(c, seq.begin(),seq.end());
// }

template <std::size_t N, class RGBColorType = rgb_t, class Float = float>
inline RGBColorType find_nearest_color(const RGBColorType& c, const RGBColorType (&colors)[N])
{
    return find_nearest_color<const RGBColorType*, RGBColorType, Float>(c, colors, colors + N);
}

template <class RGBColorType = rgb_t, class Float = float>
inline Float find_nearest_wave_length(const RGBColorType& c, const Float increment = Float(0.001))
{
    const Float max_wave_length = 800; //800nm
    Float min_wave_length = 0;
    Float min_d           = std::numeric_limits<Float>::max();
    for (Float i = 0; i < max_wave_length; i += increment)
    {
        const RGBColorType  curr_rgb = convert_wave_length_nm_to_rgb<RGBColorType, Float>(i);
        if (c == curr_rgb)
            return i;
        const Float curr_d = weighted_distance<RGBColorType, Float>(c, curr_rgb);
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
