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

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

#include "colors.hpp"
#include "bitmap_image_generic.hpp"

namespace OffScreenBitmapDraw
{

template <class PixelType = rgb_t, class ColorComponentType = unsigned char, class Float = double>
class bitmap_image_rgb
  : public bitmap_image_generic<PixelType>
{
private:
    using bitmap_image_generic<PixelType>::width_;
    using bitmap_image_generic<PixelType>::height_;
    using bitmap_image_generic<PixelType>::row_increment_;
    using bitmap_image_generic<PixelType>::data_;
    using bitmap_image_generic<PixelType>::data_end_;
    using bitmap_image_generic<PixelType>::data_vec_;

public:
    static constexpr double r_to_gray = 0.299;
    static constexpr double g_to_gray = 0.587;
    static constexpr double b_to_gray = 0.114;
    //                          carry = 1 22
    //                            sum = 1.000

    static constexpr unsigned bytes_per_pixel_ = sizeof(PixelType);
    using pixel_t = PixelType;
    using component_t = ColorComponentType;
    using Type = bitmap_image_rgb<PixelType, ColorComponentType, Float>;
    using BaseType = bitmap_image_generic<PixelType>;

    using bitmap_image_generic<PixelType>::pixel;
    using bitmap_image_generic<PixelType>::row;
    using bitmap_image_generic<PixelType>::setwidth_height;
    using bitmap_image_generic<PixelType>::width;
    using bitmap_image_generic<PixelType>::height;
    using bitmap_image_generic<PixelType>::row_increment;
    using bitmap_image_generic<PixelType>::clear;
    using bitmap_image_generic<PixelType>::set_pixel;
    using bitmap_image_generic<PixelType>::cdata;
    using bitmap_image_generic<PixelType>::data;
    using bitmap_image_generic<PixelType>::clast;
    using bitmap_image_generic<PixelType>::last;

    bitmap_image_rgb()
    : bitmap_image_generic<PixelType>()
    {}

    bitmap_image_rgb(const unsigned width, const unsigned height, const unsigned row_inc = 0)
    : bitmap_image_generic<PixelType>(width, height, row_inc)
    {
    }

    bitmap_image_rgb(const Type & image)
        : bitmap_image_rgb()
    {
        *this = image;
    }

    // generates a "slice" - without own data!
    bitmap_image_rgb(
        const Slice slice,
        Type& image,
        const int x,
        const int y,
        const int w = 0,
        const int h = 0
        )
        : bitmap_image_generic<PixelType>(slice, image, x, y, w, h)
    {
    }

    Type & operator=(const Type & image)
    {
        if (this != &image)
            static_cast<BaseType&>(*this) = static_cast<const BaseType&>(image);
        return *this;
    }

    inline void clear()
    {
        pixel_t color;
        set_black(color);
        clear(color);
    }

    const pixel_t * pixel_cbegin() const
    {
        return data_;
    }

    const pixel_t * pixel_cend() const
    {
        return data_end_;
    }

    const pixel_t * pixel_clast() const
    {
        return data_end_ - 1;
    }

    pixel_t * pixel_begin()
    {
        return data_;
    }

    pixel_t * pixel_end()
    {
        return data_end_;
    }

    pixel_t * pixel_last()
    {
        return data_end_ - 1;
    }

    const component_t * char_cbegin() const
    {
        return cbegin(*pixel_cbegin());
    }

    const component_t * char_cend() const
    {
        return cend(*pixel_clast());
    }

    component_t * char_begin()
    {
        return begin(*pixel_begin());
    }

    component_t * char_end()
    {
        return end(*pixel_last());
    }

    inline const pixel_t & get_pixel(const unsigned x, const unsigned y) const
    {
        return pixel(x, y);
    }

    inline void get_pixel(
        const unsigned x, const unsigned y,
        component_t& red,
        component_t& green,
        component_t& blue
        ) const
    {
        const pixel_t color = get_pixel(x, y);
        blue  = color.blue;
        green = color.green;
        red   = color.red;
    }

    inline void set_pixel(
        const unsigned x, const unsigned y,
        const component_t red,
        const component_t green,
        const component_t blue
        )
    {
        pixel_t &color = pixel(x, y);
        color.blue = blue;
        color.green = green;
        color.red = red;
    }

    inline bool set_region(
        const unsigned x,
        const unsigned y,
        const unsigned width,
        const unsigned height,
        const color_plane color,
        const component_t value
        )
    {
        if ((x + width ) > width_ ) { return false; }
        if ((y + height) > height_) { return false; }

        const unsigned color_plane_offset = offset(color);

        for (unsigned r = 0; r < height; ++r)
        {
            component_t* itr     = begin(pixel(x, r + y)) + color_plane_offset;
            component_t* itr_end = itr + (width * bytes_per_pixel_);
            while (itr < itr_end)
            {
                *itr  = value;
                itr += bytes_per_pixel_;
            }
        }
        return true;
    }

    inline bool set_region(
        const unsigned x,
        const unsigned y,
        const unsigned width,
        const unsigned height,
        const pixel_t color
        )
    {
        if ((x + width ) > width_ ) { return false; }
        if ((y + height) > height_) { return false; }

        for (unsigned r = 0; r < height; ++r)
        {
            pixel_t * itr     = &pixel(x, r + y);
            pixel_t * itr_end = itr + width;
            while (itr != itr_end)
            {
                *itr = color;
                ++itr;
            }
        }
        return true;
    }

    static inline unsigned bytes_per_pixel()
    {
        return bytes_per_pixel_;
    }

    inline Type & set_all_ith_bits_low(const unsigned bitr_index)
    {
        const component_t mask = static_cast<component_t>(~(1 << bitr_index));
        for (component_t* itr = char_begin(); itr != char_end(); ++itr)
        {
            *itr &= mask;
        }
        return *this;
    }

    inline Type & set_all_ith_bits_high(const unsigned bitr_index)
    {
        const component_t mask = static_cast<component_t>(1 << bitr_index);
        for (component_t* itr = char_begin(); itr != char_end(); ++itr)
        {
            *itr |= mask;
        }
        return *this;
    }

    inline Type & set_all_ith_channels(const unsigned channel, const component_t value)
    {
        for (component_t* itr = char_begin() + channel; itr < char_end(); itr += bytes_per_pixel_ )
        {
            *itr = value;
        }
        return *this;
    }

    inline Type & set_channel(const color_plane color, const component_t value)
    {
        for (component_t* itr = char_begin() + offset(color); itr < char_end(); itr += bytes_per_pixel_ )
        {
            *itr = value;
        }
        return *this;
    }

    inline Type & ror_channel(const color_plane color, const unsigned ror)
    {
        for (component_t* itr = char_begin() + offset(color); itr < char_end(); itr += bytes_per_pixel_ )
        {
            *itr = static_cast<component_t>(((*itr) >> ror) | ((*itr) << (8 - ror)));
        }
        return *this;
    }

    inline Type & set_all_channels(const component_t value)
    {
        for (component_t* itr = char_begin(); itr != char_end(); ++itr)
        {
            *itr = value;
        }
        return *this;
    }

    inline Type & invert_color_planes()
    {
        for (component_t* itr = char_begin(); itr != char_end(); ++itr)
            *itr = ~(*itr);
        return *this;
    }

    inline Type & add_to_color_plane(const color_plane color, const component_t value)
    {
        for (component_t* itr = char_begin() + offset(color); itr < char_end(); itr += bytes_per_pixel_ )
        {
            (*itr) += value;
        }
        return *this;
    }

    inline Type & convert_to_grayscale()
    {
        constexpr Float r_scaler = Float(0.299);
        constexpr Float g_scaler = Float(0.587);
        constexpr Float b_scaler = Float(0.114);
        for (pixel_t * itr = pixel_begin(); itr != pixel_end(); ++itr )
        {
            component_t gray_value = static_cast<component_t>(
                (r_scaler * itr->red) + (g_scaler * itr->green) + (b_scaler * itr->blue) );
            itr->blue  = gray_value;
            itr->green = gray_value;
            itr->red   = gray_value;
        }
        return *this;
    }

    inline Type & horizontal_flip()
    {
        for (unsigned y = 0; y < height_; ++y)
        {
            pixel_t* itr1 = row(y);
            pixel_t* itr2 = itr1 + (width() -1);
            while (itr1 < itr2)
            {
                std::swap(*itr1,*itr2);
                ++itr1;
                --itr2;
            }
        }
        return *this;
    }

    inline Type & vertical_flip()
    {
        for (unsigned y = 0; y < (height_ / 2); ++y)
        {
            pixel_t* itr1 = row(y);
            pixel_t* itr2 = row(height_ - y - 1);
            for (std::size_t x = 0; x < width(); ++x)
            {
                std::swap(*(itr1 + x),*(itr2 + x));
            }
        }
        return *this;
    }

    inline void export_color_plane(const color_plane color, component_t* image) const
    {
        assert( width() == row_increment() );
        for (const component_t* itr = char_cbegin() + offset(color); itr < char_cend(); ++image, itr += bytes_per_pixel_ )
        {
            (*image) = (*itr);
        }
    }

    inline Type & export_color_plane(const color_plane color, Type& image) const
    {
        if ( width_  != image.width_ || height_ != image.height_ )
        {
            image.setwidth_height(width_, height_);
        }
        image.clear();
        const component_t* itr1 = char_cbegin() + offset(color);
        const component_t* itr1_end = char_cend();
        component_t* itr2     = image.char_begin() + offset(color);
        while (itr1 < itr1_end)
        {
            (*itr2) = (*itr1);
            itr1 += bytes_per_pixel_;
            itr2 += bytes_per_pixel_;
        }
        return image;
    }

    inline void export_response_image(const color_plane color, Float* response_image) const
    {
        Float* resp_itr = response_image;
        assert( width() == row_increment() );
        for (const component_t* itr = char_begin() + offset(color); itr < char_end(); itr += bytes_per_pixel_ )
        {
            *(resp_itr++) = (Float(1.0) * (*itr)) / Float(256.0);
        }
    }

    inline void export_gray_scale_response_image(double* response_image) const
    {
        using T = double;
        T* resp_itr = response_image;
        constexpr T r_scaler = T(r_to_gray);
        constexpr T g_scaler = T(g_to_gray);
        constexpr T b_scaler = T(b_to_gray);
        assert( width() == row_increment() );
        constexpr T scale = T(1) / T(256);
        for (const pixel_t * itr = pixel_cbegin(); itr != pixel_cend(); ++itr )
        {
            T gray_value = (r_scaler * itr->red) + (g_scaler * itr->green) + (b_scaler * itr->blue);
            *(resp_itr++) = gray_value * scale;
        }
    }

    inline void export_gray_scale_response_image(float* response_image) const
    {
        using T = float;
        T* resp_itr = response_image;
        constexpr T r_scaler = T(r_to_gray);
        constexpr T g_scaler = T(g_to_gray);
        constexpr T b_scaler = T(b_to_gray);
        constexpr T scale = T(1) / T(256);
        assert( width() == row_increment() );
        for (const pixel_t * itr = pixel_cbegin(); itr != pixel_cend(); ++itr )
        {
            T gray_value = (r_scaler * itr->red) + (g_scaler * itr->green) + (b_scaler * itr->blue);
            *(resp_itr++) = gray_value * scale;
        }
    }

    inline void export_rgb(double* red, double* green, double* blue) const
    {
        assert( width() == row_increment() );
        for (const pixel_t * itr = data_.begin(); itr != data_.end(); ++itr )
        {
            *blue ++ = (1.0 * itr->blue)  / 256.0;
            *green++ = (1.0 * itr->green) / 256.0;
            *red  ++ = (1.0 * itr->red)   / 256.0;
        }
    }

    inline void export_rgb(float* red, float* green, float* blue) const
    {
        assert( width() == row_increment() );
        for (const pixel_t * itr = data_.begin(); itr != data_.end(); ++itr )
        {
            *blue ++ = (1.0F * itr->blue)  / 256.0F;
            *green++ = (1.0F * itr->green) / 256.0F;
            *red  ++ = (1.0F * itr->red)   / 256.0F;
        }
    }

    inline void export_rgb(unsigned char* red, unsigned char* green, unsigned char* blue) const
    {
        assert( width() == row_increment() );
        for (const pixel_t * itr = data_.begin(); itr != data_.end(); ++itr )
        {
            *blue ++ = itr->blue;
            *green++ = itr->green;
            *red  ++ = itr->red;
        }
    }

    inline void export_ycbcr(double* y, double* cb, double* cr) const
    {
        using Flt = double;
        assert( width() == row_increment() );
        for (const pixel_t * itr = pixel_cbegin(); itr != pixel_cend(); ++itr )
        {
            const Flt blue  = Flt(1) * itr->blue;
            const Flt green = Flt(1) * itr->green;
            const Flt red   = Flt(1) * itr->red;
            *y ++ = clamp<Flt>( Flt( 16) + Flt(1)/256 * ( Flt( 65.738) * red + Flt(129.057) * green + Flt( 25.064) * blue), Flt(1), Flt(254) );
            *cb++ = clamp<Flt>( Flt(128) + Flt(1)/256 * ( Flt(-37.945) * red + Flt(-74.494) * green + Flt(112.439) * blue), Flt(1), Flt(254) );
            *cr++ = clamp<Flt>( Flt(128) + Flt(1)/256 * ( Flt(112.439) * red + Flt(-94.154) * green + Flt(-18.285) * blue), Flt(1), Flt(254) );
        }
    }

    inline void export_ycbcr(float* y, float* cb, float* cr) const
    {
        using Flt = float;
        assert( width() == row_increment() );
        for (const pixel_t * itr = pixel_cbegin(); itr != pixel_cend(); ++itr )
        {
            const Flt blue  = Flt(1) * itr->blue;
            const Flt green = Flt(1) * itr->green;
            const Flt red   = Flt(1) * itr->red;
            *y ++ = clamp<Flt>( Flt( 16) + Flt(1)/256 * ( Flt( 65.738) * red + Flt(129.057) * green + Flt( 25.064) * blue), Flt(1), Flt(254) );
            *cb++ = clamp<Flt>( Flt(128) + Flt(1)/256 * ( Flt(-37.945) * red + Flt(-74.494) * green + Flt(112.439) * blue), Flt(1), Flt(254) );
            *cr++ = clamp<Flt>( Flt(128) + Flt(1)/256 * ( Flt(112.439) * red + Flt(-94.154) * green + Flt(-18.285) * blue), Flt(1), Flt(254) );
        }
    }

    inline void export_rgb_normal(double* red, double* green, double* blue) const
    {
        assert( width() == row_increment() );
        for (const pixel_t * itr = data_.begin(); itr != data_.end(); ++itr )
        {
            *blue ++ = 1.0 * itr->blue;
            *green++ = 1.0 * itr->green;
            *red  ++ = 1.0 * itr->red;
        }
    }

    inline void export_rgb_normal(float* red, float* green, float* blue) const
    {
        assert( width() == row_increment() );
        for (const pixel_t * itr = data_.begin(); itr != data_.end(); ++itr )
        {
            *blue ++ = 1.0f * itr->blue;
            *green++ = 1.0f * itr->green;
            *red  ++ = 1.0f * itr->red;
        }
    }

    inline Type & import_rgb(double* red, double* green, double* blue)
    {
        assert( width() == row_increment() );
        for (pixel_t * itr = data_.begin(); itr != data_.end(); ++itr )
        {
            itr->blue  = static_cast<component_t>(256.0 * (*blue ++));
            itr->green = static_cast<component_t>(256.0 * (*green++));
            itr->red   = static_cast<component_t>(256.0 * (*red  ++));
        }
        return *this;
    }

    inline Type & import_rgb(float* red, float* green, float* blue)
    {
        assert( width() == row_increment() );
        for (pixel_t * itr = data_.begin(); itr != data_.end(); ++itr )
        {
            itr->blue  = static_cast<component_t>(256.0f * (*blue ++));
            itr->green = static_cast<component_t>(256.0f * (*green++));
            itr->red   = static_cast<component_t>(256.0f * (*red  ++));
        }
        return *this;
    }

    inline Type & import_rgb(unsigned char* red, unsigned char* green, unsigned char* blue)
    {
        assert( width() == row_increment() );
        for (pixel_t * itr = data_.begin(); itr != data_.end(); ++itr )
        {
            itr->blue  = (*blue ++);
            itr->green = (*green++);
            itr->red   = (*red  ++);
        }
        return *this;
    }

    inline Type & import_ycbcr(double* y, double* cb, double* cr)
    {
        using Flt = double;
        assert( width() == row_increment() );
        for (pixel_t * itr = pixel_begin(); itr != pixel_end(); ++itr )
        {
            Flt y_  =  (*y++);
            Flt cb_ = (*cb++);
            Flt cr_ = (*cr++);
            itr->blue  = static_cast<component_t>(clamp( (Flt(298.082) * y_ + Flt(516.412) * cb_                      ) / Flt(256) - Flt(276.836), Flt(0), Flt(255) ));
            itr->green = static_cast<component_t>(clamp( (Flt(298.082) * y_ - Flt(100.291) * cb_ - Flt(208.120) * cr_ ) / Flt(256) + Flt(135.576), Flt(0), Flt(255) ));
            itr->red   = static_cast<component_t>(clamp( (Flt(298.082) * y_                      + Flt(408.583) * cr_ ) / Flt(256) - Flt(222.921), Flt(0), Flt(255) ));
        }
        return *this;
    }

    inline Type & import_ycbcr(float* y, float* cb, float* cr)
    {
        using Flt = float;
        assert( width() == row_increment() );
        for (pixel_t * itr = pixel_begin(); itr != pixel_end(); ++itr )
        {
            Flt y_  =  (*y++);
            Flt cb_ = (*cb++);
            Flt cr_ = (*cr++);
            itr->blue  = static_cast<component_t>(clamp( (Flt(298.082) * y_ + Flt(516.412) * cb_                      ) / Flt(256) - Flt(276.836), Flt(0), Flt(255) ));
            itr->green = static_cast<component_t>(clamp( (Flt(298.082) * y_ - Flt(100.291) * cb_ - Flt(208.120) * cr_ ) / Flt(256) + Flt(135.576), Flt(0), Flt(255) ));
            itr->red   = static_cast<component_t>(clamp( (Flt(298.082) * y_                      + Flt(408.583) * cr_ ) / Flt(256) - Flt(222.921), Flt(0), Flt(255) ));
        }
        return *this;
    }

    inline Type & import_gray_scale_clamped(const double* gray, const double scale = 1.0)
    {
        using T = double;
        const T comp_scale = T(256 * scale);
        assert( width() == row_increment() );
        for (pixel_t * itr = pixel_begin(); itr != pixel_end(); ++itr )
        {
            component_t c = static_cast<component_t>( clamp<T>(comp_scale * (*gray++), T(0), T(255)) );
            itr->blue  = c;
            itr->green = c;
            itr->red   = c;
        }
        return *this;
    }

    inline Type & import_gray_scale_clamped(const float* gray, const float scale = 1.0F)
    {
        using T = double;
        const T comp_scale = T(256 * scale);
        assert( width() == row_increment() );
        for (pixel_t * itr = pixel_begin(); itr != pixel_end(); ++itr )
        {
            component_t c = static_cast<component_t>( clamp<T>(comp_scale * (*gray++), T(0), T(255)) );
            itr->blue  = c;
            itr->green = c;
            itr->red   = c;
        }
        return *this;
    }

    inline Type & import_rgb_clamped(double* red, double* green, double* blue)
    {
        using T = double;
        assert( width() == row_increment() );
        for (pixel_t * itr = data_.begin(); itr != data_.end(); ++itr )
        {
            itr->blue  = static_cast<component_t>(clamp<T>(256.0 * (*blue ++), 0.0, 255.0));
            itr->green = static_cast<component_t>(clamp<T>(256.0 * (*green++), 0.0, 255.0));
            itr->red   = static_cast<component_t>(clamp<T>(256.0 * (*red  ++), 0.0, 255.0));
        }
        return *this;
    }

    inline Type & import_rgb_clamped(float* red, float* green, float* blue)
    {
        using T = float;
        assert( width() == row_increment() );
        for (pixel_t * itr = data_.begin(); itr != data_.end(); ++itr )
        {
            itr->blue  = static_cast<component_t>(clamp<T>(256.0F * (*blue ++), 0.0F, 255.0F));
            itr->green = static_cast<component_t>(clamp<T>(256.0F * (*green++), 0.0F, 255.0F));
            itr->red   = static_cast<component_t>(clamp<T>(256.0F * (*red  ++), 0.0F, 255.0F));
        }
        return *this;
    }

    inline Type & import_rgb_normal(double* red, double* green, double* blue)
    {
        assert( width() == row_increment() );
        for (pixel_t * itr = data_.begin(); itr != data_.end(); ++itr )
        {
            itr->blue  = static_cast<component_t>(*blue ++);
            itr->green = static_cast<component_t>(*green++);
            itr->red   = static_cast<component_t>(*red  ++);
        }
        return *this;
    }

    inline Type & import_rgb_normal(float* red, float* green, float* blue)
    {
        assert( width() == row_increment() );
        for (pixel_t * itr = data_.begin(); itr != data_.end(); ++itr )
        {
            itr->blue  = static_cast<component_t>(*blue ++);
            itr->green = static_cast<component_t>(*green++);
            itr->red   = static_cast<component_t>(*red  ++);
        }
        return *this;
    }

    inline Type & subsample_to(Type& dest) const
    {
        /* Half sub-sample of original image */
        unsigned w = 0;
        unsigned h = 0;

        bool odd_width = false;
        bool odd_height = false;

        if (0 == (width_ % 2))
            w = width_ / 2;
        else
        {
            w = 1 + (width_ / 2);
            odd_width = true;
        }

        if (0 == (height_ % 2))
            h = height_ / 2;
        else
        {
            h = 1 + (height_ / 2);
            odd_height = true;
        }

        unsigned horizontal_upper = (odd_width)  ? (w - 1) : w;
        unsigned vertical_upper   = (odd_height) ? (h - 1) : h;

        dest.setwidth_height(w, h);
        dest.clear();

        pixel_t * dest_row = dest.pixel_begin();
        const pixel_t * src_row = pixel_cbegin();

              component_t*  itrD;
        const component_t*  itrA, *itrB;
        unsigned total = 0;

        for (unsigned j = 0; j < h; ++j)
        {
            itrD = begin(*dest_row);
            itrA = cbegin(*src_row);
            itrB = cbegin(src_row[( odd_height && j == vertical_upper ) ? 0 : row_increment_]);
            for (unsigned i = 0; i < horizontal_upper; ++i)
            {
                for (unsigned k = 0; k < bytes_per_pixel_; ++k)
                {
                    total = unsigned(itrA[k]) + itrA[k+bytes_per_pixel_]
                          + unsigned(itrB[k]) + itrB[k+bytes_per_pixel_];
                    itrD[k] = static_cast<component_t>(total / 4);
                }
                itrA += 2 * bytes_per_pixel_;
                itrB += 2 * bytes_per_pixel_;
                itrD += bytes_per_pixel_;
            }
            if (odd_width)
            {
                for (unsigned k = 0; k < bytes_per_pixel_; ++k)
                {
                    total = unsigned(itrA[k]) + itrB[k];
                    itrD[k] = static_cast<component_t>(total / 2);
                }
            }
            src_row += 2 * row_increment_;
            dest_row += dest.row_increment();
        }
        return dest;
    }

    inline Type & upsample_to(Type& dest) const
    {
        /* 2x up-sample of original image. */

        dest.setwidth_height(2 * width_ ,2 * height_);
        dest.clear();

        pixel_t * dest_row = dest.pixel_begin();
        const pixel_t * src_row = pixel_cbegin();
        component_t*  itrA, *itrB;
        const component_t*  itrS;

        for (unsigned j = 0; j < height_; ++j)
        {
            itrS = cbegin(*src_row);
            itrA = begin(*dest_row);
            itrB = begin(dest_row[dest.row_increment()]);
            for (unsigned i = 0; i < width_; ++i)
            {
                for (unsigned k = 0; k < bytes_per_pixel_; ++k)
                {
                    itrA[k] = itrA[k+bytes_per_pixel_] = itrS[k];
                    itrB[k] = itrB[k+bytes_per_pixel_] = itrS[k];
                }
                itrA += 2 * bytes_per_pixel_;
                itrB += 2 * bytes_per_pixel_;
                itrS += bytes_per_pixel_;
            }
            src_row += row_increment_;
            dest_row += 2 * dest.row_increment();
        }
        return dest;
    }

    // each pixel's color := alpha * color of argument image + (1-alpha) * this_pixel
    inline Type & alpha_blend(const double& alpha, const Type& image)
    {
        if ( image.width_  != width_ || image.height_ != height_ )
            return *this;

        if ( alpha < 0.0 || alpha > 1.0 )
            return *this;

        double alpha_complement = 1.0 - alpha;
        const pixel_t * src = image.pixel_cbegin();
        for (pixel_t * dest = pixel_begin(); dest != pixel_end(); ++dest )
        {
            dest->blue  = static_cast<component_t>((alpha * src->blue ) + (alpha_complement * dest->blue ));
            dest->green = static_cast<component_t>((alpha * src->green) + (alpha_complement * dest->green));
            dest->red   = static_cast<component_t>((alpha * src->red  ) + (alpha_complement * dest->red  ));
            ++src;
        }
        return *this;
    }

    inline void histogram(const color_plane color, double hist[256]) const
    {
        std::fill(hist, hist + 256, 0.0);
        for (const unsigned char* itr = char_begin() + offset(color); itr < char_end(); itr += bytes_per_pixel_)
            ++hist[(*itr)];
    }

    inline void histogram_normalized(const color_plane color, double hist[256]) const
    {
        histogram(color, hist);

        double* h_itr = hist;
        const double* h_end = hist + 256;
        const double pixel_count = static_cast<double>(width_ * height_);

        while (h_end != h_itr)
            *(h_itr++) /= pixel_count;
    }

    static inline unsigned offset(const color_plane color)
    {
        return pixel_t::offset(color);
    }

    inline void generate_incremental()
    {
        component_t current_color = 0;
        for (pixel_t * itr = data_.begin(); itr != data_.end(); ++itr )
        {
            itr->blue  = current_color;
            itr->green = current_color;
            itr->red   = current_color;
            ++current_color;
        }
    }

    // change bgr <-> rgb
    //inline void reverse_channels()
    //{
    //   if (3 != bytes_per_pixel_)
    //      return;

    //   for (unsigned char* itr = data(); itr < end(); itr += bytes_per_pixel_)
    //   {
    //      std::swap(*(itr + 0),*(itr + 2));
    //   }
    //}

    static inline bool big_endian()
    {
        unsigned v = 0x01;
        return (1 != reinterpret_cast<char*>(&v)[0]);
    }

    static inline unsigned short flip(const unsigned short& v)
    {
        return ((v >> 8) | (v << 8));
    }

    static inline unsigned flip(const unsigned v)
    {
        return (
            ((v & 0xFF000000) >> 0x18) |
            ((v & 0x000000FF) << 0x18) |
            ((v & 0x00FF0000) >> 0x08) |
            ((v & 0x0000FF00) << 0x08)
            );
    }

protected:

    template <typename T>
    static inline T clamp(const T& v, const T& lower_range, const T& upper_range)
    {
        if (v < lower_range)
            return lower_range;
        else if (v >  upper_range)
            return upper_range;
        else
            return v;
    }

    //channel_mode channel_mode_;
};

}
