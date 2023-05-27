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
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

namespace OffScreenBitmapDraw
{

// "generic" by means of NOT RGB
// PixelType could be rgb_t, bgr_t, rgba_t or abgr_t from colors.hpp
//   alternative: float
template <class PixelType, class ColorComponentType = unsigned char>
class bitmap_image_generic
{
public:
    using pixel_t = PixelType;
    using component_t = ColorComponentType;
    using Type = bitmap_image_generic<PixelType>;

    bitmap_image_generic()
        : width_          (0)
        , height_         (0)
        , row_increment_  (0)
    {}

    bitmap_image_generic(const unsigned int width, const unsigned int height, const unsigned row_inc = 0)
        : width_ (width )
        , height_(height)
        , row_increment_  (0)
    {
        assert( !row_inc || row_inc >= width );
        row_increment_ = row_inc ? row_inc : width_;
        data_.resize(height_ * row_increment_);
    }

    bitmap_image_generic(const Type& image)
        : width_    (image.width_    )
        , height_   (image.height_   )
        , row_increment_  (image.row_increment_ )
    {
        data_ = image.data_;
    }

    bitmap_image_generic& operator=(const Type& image)
    {
        if (this != &image)
        {
            width_           = image.width_;
            height_          = image.height_;
            row_increment_   = image.row_increment_;
            data_ = image.data_;
        }
        return *this;
    }

    inline bool operator!()
    {
        return (data_.size()   == 0) ||
               (width_         == 0) ||
               (height_        == 0) ||
               (row_increment_ == 0);
    }

    inline void clear(const pixel_t v)
    {
        std::fill(data_.begin(), data_.end(), v);
    }

    // inline unsigned char red_channel(const unsigned int x, const unsigned int y) const
    // inline unsigned char green_channel(const unsigned int x, const unsigned int y) const
    // inline unsigned char blue_channel (const unsigned int x, const unsigned int y) const
    // inline void red_channel(const unsigned int x, const unsigned int y, const unsigned char value)
    // inline void green_channel(const unsigned int x, const unsigned int y, const unsigned char value)
    // inline void blue_channel (const unsigned int x, const unsigned int y, const unsigned char value)

    inline const pixel_t * row(unsigned int row_index) const
    {
        return &data_[row_index * row_increment_];
    }

    inline pixel_t * row(unsigned int row_index)
    {
        return &data_[row_index * row_increment_];
    }

    const component_t * char_row(unsigned int row_index) const
    {
        return begin(data_[row_index * row_increment_]);
    }

    component_t * char_row(unsigned int row_index)
    {
        return begin(data_[row_index * row_increment_]);
    }

    const pixel_t & pixel(const unsigned int x, const unsigned int y) const
    {
        return data_[ (y * row_increment_) + x];
    }

    pixel_t & pixel(const unsigned int x, const unsigned int y)
    {
        return data_[ (y * row_increment_) + x];
    }

    // @todo: provide this function with an additional/extra (template parameter) class
    void set_pixel(const unsigned int x, const unsigned int y, const pixel_t color)
    {
        data_[ (y * row_increment_) + x] = color;
    }

    bool copy_from(const Type& image)
    {
        if ( image.height_ != height_ || image.width_  != width_ )
        {
            return false;
        }
        data_ = image.data_;
        return true;
    }

    // copies source_image to this (to_x_offset|to_y_offset)
    bool copy_from(
        const Type& source_image,
        const unsigned int to_x_offset,
        const unsigned int to_y_offset
        )
    {
        if ((to_x_offset + source_image.width_ ) > width_ ) { return false; }
        if ((to_y_offset + source_image.height_) > height_) { return false; }

        for (unsigned int y = 0; y < source_image.height_; ++y)
        {
            pixel_t* dest_itr          = row(y + to_y_offset) + to_x_offset;
            const pixel_t* src_itr     = source_image.row(y);
            const pixel_t* src_itr_end = src_itr + source_image.width_;
            std::copy(src_itr, src_itr_end, dest_itr);
        }
        return true;
    }

    inline bool region_to(
        const unsigned int x,
        const unsigned int y,
        const unsigned int width,
        const unsigned int height,
        Type& dest_image,
        const unsigned int to_x_offset = 0,
        const unsigned int to_y_offset = 0
        ) const
    {
        if ((x + width ) > width_ ) { return false; }
        if ((y + height) > height_) { return false; }

        if (
            (dest_image.width_  < to_x_offset + width_ ) ||
            (dest_image.height_ < to_y_offset + height_)
            )
        {
            dest_image.setwidth_height(width,height);
        }

        for (unsigned int r = 0; r < height; ++r)
        {
            const pixel_t* itr1     = row(r + y) + x;
            const pixel_t* itr1_end = itr1 + width;
            pixel_t* dest_itr = dest_image.row(r + to_y_offset) + to_x_offset;
            std::copy(itr1, itr1_end, dest_itr);
        }

        return true;
    }

    inline bool roi_from_center_to(
        const unsigned int cx,
        const unsigned int cy,
        const unsigned int width,
        const unsigned int height,
        Type& dest_image,
        const unsigned int to_x_offset = 0,
        const unsigned int to_y_offset = 0
        ) const
    {
        return region(cx - (width / 2), cy - (height / 2),
                      width, height,
                      dest_image,
                      to_x_offset, to_y_offset
                      );
    }

    inline bool set_region(
        const unsigned int&  x,
        const unsigned int&  y,
        const unsigned int&  width,
        const unsigned int&  height,
        const pixel_t color
        )
    {
        if ((x + width ) > width_ ) { return false; }
        if ((y + height) > height_) { return false; }
        for (unsigned int r = 0; r < height; ++r)
        {
            pixel_t* itr     = row(r + y) + x;
            pixel_t* itr_end = itr + width;
            std::fill(itr, itr_end, color);
        }
        return true;
    }

    // inline bool set_region(const unsigned int&  x     ,
    //                        const unsigned int&  y     ,
    //                        const unsigned int&  width ,
    //                        const unsigned int&  height,
    //                        const unsigned char& value )
    // inline bool set_region(const unsigned int&  x     ,
    //                        const unsigned int&  y     ,
    //                        const unsigned int&  width ,
    //                        const unsigned int&  height,
    //                        const color_plane    color ,
    //                        const unsigned char& value )
    // inline bool set_region(const unsigned int&  x     ,
    //                        const unsigned int&  y     ,
    //                        const unsigned int&  width ,
    //                        const unsigned int&  height,
    //                        const unsigned char& red   ,
    //                        const unsigned char& green ,
    //                        const unsigned char& blue  )

    void reflective_image(Type& image, const bool include_diagnols = false)
    {
        image.setwidth_height(3 * width_, 3 * height_, true);
        image.copy_from(*this, width_, height_);
        vertical_flip();

        image.copy_from(*this, width_,           0);
        image.copy_from(*this, width_, 2 * height_);
        vertical_flip();
        horizontal_flip();

        image.copy_from(*this,          0, height_);
        image.copy_from(*this, 2 * width_, height_);
        horizontal_flip();

        if (include_diagnols)
        {
            Type tile = *this;
            tile.vertical_flip();
            tile.horizontal_flip();
            image.copy_from(tile,          0,           0);
            image.copy_from(tile, 2 * width_,           0);
            image.copy_from(tile, 2 * width_, 2 * height_);
            image.copy_from(tile, 0         , 2 * height_);
        }
    }

    // non-padded width
    inline unsigned int width() const
    {
        return width_;
    }

    inline unsigned int height() const
    {
        return height_;
    }

    inline unsigned int row_increment() const {
        return row_increment_;
    }

    inline unsigned int pixel_count_unpadded() const
    {
        return width_ *  height_;
    }

    inline unsigned int pixel_count_with_padding() const
    {
        return row_increment_ *  height_;
    }

    inline void setwidth_height(
        const unsigned int width,
        const unsigned int height,
        const unsigned int row_inc = 0,
        const bool clear = false)
    {
        assert( !row_inc || row_inc >= width );
        width_  = width;
        height_ = height;
        row_increment_ = row_inc ? row_inc : width_;
        data_.resize(height_ * row_increment_);

        if (clear)
        {
            pixel_t v{};
            std::fill(data_.begin(), data_.end(), v);
        }
    }

    // inline void set_all_ith_bits_low(const unsigned int bitr_index)
    // inline void set_all_ith_bits_high(const unsigned int bitr_index)
    // inline void set_all_ith_channels(const unsigned int& channel, const unsigned char& value)
    // inline void set_channel(const color_plane color,const unsigned char& value)
    // inline void ror_channel(const color_plane color, const unsigned int& ror)
    // inline void set_all_channels(const unsigned char& value)
    // inline void set_all_channels(const unsigned char& r_value,
    //                              const unsigned char& g_value,
    //                              const unsigned char& b_value)
    // inline void invert_color_planes()
    // inline void add_to_color_plane(const color_plane color, const unsigned char& value)
    // inline void convert_to_grayscale()

    inline const pixel_t* cdata() const
    {
        return data_.data();
    }

    inline pixel_t* data()
    {
       return data_.data();
    }

    // inline void bgr_to_rgb()
    // inline void rgb_to_bgr()
    // inline void reverse()

    inline void horizontal_flip()
    {
        for (unsigned int y = 0; y < height_; ++y)
        {
            pixel_t* itr1 = row(y);
            pixel_t* itr2 = itr1 + ( width_ - 1 );
            while (itr1 < itr2)
            {
                std::swap(*itr1, *itr2);
                ++itr1;
                --itr2;
            }
        }
    }

    inline void vertical_flip()
    {
        for (unsigned int y = 0; y < (height_ / 2); ++y)
        {
            pixel_t* itr1 = row(y);
            pixel_t* itr2 = row(height_ - y - 1);
            for (std::size_t x = 0; x < width_; ++x)
            {
                std::swap(*(itr1 + x),*(itr2 + x));
            }
        }
    }

    // inline void export_color_plane(const color_plane color, unsigned char* image)
    // inline void export_color_plane(const color_plane color, bitmap_image& image)
    // inline void export_response_image(const color_plane color, double* response_image)
    // inline void export_gray_scale_response_image(double* response_image) const
    // inline void export_rgb(double* red, double* green, double* blue) const
    // inline void export_rgb(float* red, float* green, float* blue) const
    // inline void export_rgb(unsigned char* red, unsigned char* green, unsigned char* blue) const
    // inline void export_ycbcr(double* y, double* cb, double* cr) const
    // inline void export_rgb_normal(double* red, double* green, double* blue) const
    // inline void export_rgb_normal(float* red, float* green, float* blue) const
    // inline void import_rgb(double* red, double* green, double* blue)
    // inline void import_rgb(float* red, float* green, float* blue)
    // inline void import_rgb(unsigned char* red, unsigned char* green, unsigned char* blue)
    // inline void import_ycbcr(double* y, double* cb, double* cr)
    // inline void import_gray_scale_clamped(double* gray)
    // inline void import_rgb_clamped(double* red, double* green, double* blue)
    // inline void import_rgb_clamped(float* red, float* green, float* blue)
    // inline void import_rgb_normal(double* red, double* green, double* blue)
    // inline void import_rgb_normal(float* red, float* green, float* blue)

    // inline void subsample(bitmap_image& dest) const
    // inline void upsample(bitmap_image& dest) const

    // inline void alpha_blend(const double& alpha, const bitmap_image& image)

    // inline double psnr(const bitmap_image& image)
    // inline double psnr(const unsigned int& x, const unsigned int& y, const bitmap_image& image)

    // inline void histogram(const color_plane color, double hist[256]) const
    // inline void histogram_normalized(const color_plane color, double hist[256]) const

    // inline unsigned int offset(const color_plane color) cons t

    // inline void incremental( )

    // inline void reverse_channels()


    inline const pixel_t* cend() const
    {
        return data_.data() + data_.size();
    }

    inline pixel_t* end()
    {
        return data() + data_.size();
    }

    inline const pixel_t* clast() const
    {
        return data() + ( data_.size() - 1);
    }

    inline pixel_t* last()
    {
        return data() + ( data_.size() - 1);
    }

   static inline bool big_endian()
   {
      unsigned int v = 0x01;

      return (1 != reinterpret_cast<char*>(&v)[0]);
   }

protected:

   inline unsigned short flip(const unsigned short& v) const
   {
      return ((v >> 8) | (v << 8));
   }

   inline unsigned int flip(const unsigned int& v) const
   {
      return (
               ((v & 0xFF000000) >> 0x18) |
               ((v & 0x000000FF) << 0x18) |
               ((v & 0x00FF0000) >> 0x08) |
               ((v & 0x0000FF00) << 0x08)
             );
   }

   unsigned int width_;
   unsigned int height_;
   unsigned int row_increment_;
   std::vector<PixelType> data_;
};

}


#ifndef NDEBUG

#include "colors.hpp"

// PixelType could be rgb_t, bgr_t, rgba_t or abgr_t from colors.hpp

namespace OffScreenBitmapDraw
{

static bitmap_image_generic<rgb_t>    internal_test_rgb;
static bitmap_image_generic<bgr_t>    internal_test_bgr;
static bitmap_image_generic<rgba_t>   internal_test_rgba;
static bitmap_image_generic<abgr_t>   internal_test_abgr;
static bitmap_image_generic<float, float>   internal_test_float;

}

#endif
