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

struct Slice {};

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
        , data_vec_       ()
        , data_           (nullptr)
        , data_end_       (nullptr)
    {}

    bitmap_image_generic(const Type & image)
        : width_          (image.width_)
        , height_         (image.height_)
        , row_increment_  (image.row_increment_)
        , data_vec_       (image.data_vec_)
        , data_           (data_vec_.data())
        , data_end_       (data_ + ((height_ -1) * row_increment_ + width_))
    {
    }

    bitmap_image_generic(
        const unsigned width,
        const unsigned height,
        const unsigned row_inc = 0
        )
        : bitmap_image_generic()
    {
        resize(width, height, row_inc);
    }

    bitmap_image_generic(
        const Slice,
        Type & image,
        const unsigned x0 = 0,
        const unsigned y0 = 0,
        const unsigned width  = 0,
        const unsigned height = 0
        )
        : bitmap_image_generic()
    {
        set_slice_region_from(image, x0, y0, width, height);
    }

    bool resize(
        const unsigned width,
        const unsigned height,
        const unsigned row_inc = 0
        )
    {
        assert(width > 0 && height > 0);
        assert( !row_inc || row_inc >= width );
        if ( !width || !height )
            return false;
        if ( row_inc && row_inc < width )
            return false;
        width_ = width;
        height_ = height;
        row_increment_ = row_inc ? row_inc : width_;
        data_vec_.resize((height_ -1) * row_increment_ + width_ +1);
        data_ = data_vec_.data();
        data_end_ = data_ + ((height_ -1) * row_increment_ + width_);
        return true;
    }

    bool set_slice_region_from(
        Type & image,
        const unsigned x0 = 0,
        const unsigned y0 = 0,
        const unsigned width = 0,
        const unsigned height = 0
        )
    {
        if (!image.is_valid_region(x0, y0, width, height, true))
            return false;
        width_  = (width ) ? width  : (image.width_  - x0);
        height_ = (height) ? height : (image.height_ - y0);
        row_increment_ = image.row_increment_;
        data_ = image.row(y0) + x0;
        data_end_ = data_ + ((height_ -1) * row_increment_ + width_);
        data_vec_.clear();
        return true;
    }

    bool is_valid_region(
        const unsigned x0 = 0,
        const unsigned y0 = 0,
        const unsigned width  = 0,
        const unsigned height = 0,
        bool assert_ = false
        ) const
    {
        const int w = (width ) ? width  : (width_  - x0);
        const int h = (height) ? height : (height_ - y0);
        if ( int(x0) + w > int(width_) )
        {
            assert(!assert_);
            return false;
        }
        if ( int(y0) + h > int(height_) )
        {
            assert(!assert_);
            return false;
        }
        return true;
    }

    bitmap_image_generic& operator=(const Type& image)
    {
        if (this != &image)
        {
            if ( image.data_vec_.size() )
            {
                width_           = image.width_;
                height_          = image.height_;
                row_increment_   = image.row_increment_;
                data_vec_ = image.data_vec_;
                data_ = data_vec_.data();
                data_end_ = data_ + ((height_ -1) * row_increment_ + width_);
            }
            else
            {
                width_           = image.width_;
                height_          = image.height_;
                row_increment_   = image.width_;
                data_vec_.resize((height_ -1) * row_increment_ + width_ +1);
                data_ = data_vec_.data();
                data_end_ = data_ + ((height_ -1) * row_increment_ + width_);
                bool success = copy_from(image, 0, 0);
                assert(success);
                (void)success;
            }
        }
        return *this;
    }

    inline bool operator!() const
    {
        return (width_         == 0) ||
               (height_        == 0) ||
               (row_increment_ == 0);
    }

    inline void clear(const pixel_t v)
    {
        std::fill(data_, data_end_, v);
    }

    inline const pixel_t * row(unsigned row_index) const
    {
        return &data_[row_index * row_increment_];
    }

    inline pixel_t * row(unsigned row_index)
    {
        return &data_[row_index * row_increment_];
    }

    const component_t * char_row(unsigned row_index) const
    {
        return begin(data_[row_index * row_increment_]);
    }

    component_t * char_row(unsigned row_index)
    {
        return begin(data_[row_index * row_increment_]);
    }

    const pixel_t & pixel(const unsigned x, const unsigned y) const
    {
        return data_[ (y * row_increment_) + x];
    }

    pixel_t & pixel(const unsigned x, const unsigned y)
    {
        return data_[ (y * row_increment_) + x];
    }

    // @todo: provide this function with an additional/extra (template parameter) class
    void set_pixel(const unsigned x, const unsigned y, const pixel_t color)
    {
        data_[ (y * row_increment_) + x] = color;
    }

    // copies source_image to this (to_x_offset|to_y_offset)
    bool copy_from(
        const Type& source_image,
        const unsigned to_x_offset = 0,
        const unsigned to_y_offset = 0
        )
    {
        if (!is_valid_region(to_x_offset, to_y_offset, source_image.width_, source_image.height_, true))
            return false;
        for (unsigned y = 0; y < source_image.height_; ++y)
        {
            pixel_t* dest_itr          = row(y + to_y_offset) + to_x_offset;
            const pixel_t* src_itr     = source_image.row(y);
            const pixel_t* src_itr_end = src_itr + source_image.width_;
            std::copy(src_itr, src_itr_end, dest_itr);
        }
        return true;
    }

    inline bool copy_region_to(
        const unsigned x0,
        const unsigned y0,
        const unsigned width,
        const unsigned height,
        Type& dest_image,
        const unsigned to_x_offset = 0,
        const unsigned to_y_offset = 0
        ) const
    {
        if ((x0 + width ) > width_ ) { return false; }
        if ((y0 + height) > height_) { return false; }

        if (
            (dest_image.width_  < to_x_offset + width_ ) ||
            (dest_image.height_ < to_y_offset + height_)
            )
        {
            dest_image.setwidth_height(width, height);
        }

        for (unsigned r = 0; r < height; ++r)
        {
            const pixel_t* itr1     = row(r + y0) + x0;
            const pixel_t* itr1_end = itr1 + width;
            pixel_t* dest_itr = dest_image.row(r + to_y_offset) + to_x_offset;
            std::copy(itr1, itr1_end, dest_itr);
        }

        return true;
    }

    inline bool roi_from_center_to(
        const unsigned cx,
        const unsigned cy,
        const unsigned width,
        const unsigned height,
        Type& dest_image,
        const unsigned to_x_offset = 0,
        const unsigned to_y_offset = 0
        ) const
    {
        const int from_x = cx - (width / 2);
        const int from_y = cy - (height / 2);
        assert( from_x >= 0 && from_y >= 0 );
        if ( from_x < 0 || from_y < 0 )
            return false;
        return copy_region_to(
            from_x, from_y,
            width, height,
            dest_image,
            to_x_offset, to_y_offset
            );
    }

    inline bool set_region(
        const unsigned  x0,
        const unsigned  y0,
        const unsigned  width,
        const unsigned  height,
        const pixel_t color
        )
    {
        if (!is_valid_region(x0, y0, width, height, true))
            return false;
        for (unsigned r = 0; r < height; ++r)
        {
            pixel_t* itr     = row(r + y0) + x0;
            pixel_t* itr_end = itr + width;
            std::fill(itr, itr_end, color);
        }
        return true;
    }

    bool reflective_image(Type& image, const bool include_diagnols = false)
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

        return true;
    }

    // non-padded width
    inline unsigned width() const
    {
        return width_;
    }

    inline unsigned height() const
    {
        return height_;
    }

    inline unsigned row_increment() const {
        return row_increment_;
    }

    inline unsigned pixel_count_unpadded() const
    {
        return width_ *  height_;
    }

    inline unsigned pixel_count_with_padding() const
    {
        return row_increment_ *  height_;
    }

    void reset()
    {
        width_ = height_ = row_increment_ = 0;
        data_vec_.clear();
        data_ = data_end_ = nullptr;
    }

    inline bool setwidth_height(
        const unsigned width,
        const unsigned height,
        const unsigned row_inc = 0,
        const bool clear = false)
    {
        assert( !row_inc || row_inc >= width );
        bool success = resize(width, height, row_inc);
        if (!success)
            return false;
        if (clear)
        {
            pixel_t v{};
            std::fill(data_, data_end_, v);
        }
        return true;
    }

    inline const pixel_t* cdata() const
    {
        return data_;
    }

    inline pixel_t* data()
    {
       return data_;
    }

    inline void horizontal_flip()
    {
        for (unsigned y = 0; y < height_; ++y)
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
        for (unsigned y = 0; y < (height_ / 2); ++y)
        {
            pixel_t* itr1 = row(y);
            pixel_t* itr2 = row(height_ - y - 1);
            for (std::size_t x = 0; x < width_; ++x)
            {
                std::swap(*(itr1 + x),*(itr2 + x));
            }
        }
    }

    inline const pixel_t* cend() const
    {
        return data_end_;
    }

    inline pixel_t* end()
    {
        return data_end_;
    }

    inline const pixel_t* clast() const
    {
        return data_end_ - 1;
    }

    inline pixel_t* last()
    {
        return data_end_ - 1;
    }

   static inline bool big_endian()
   {
      unsigned v = 0x01;
      return (1 != reinterpret_cast<char*>(&v)[0]);
   }

protected:

   inline unsigned short flip(const unsigned short& v) const
   {
      return ((v >> 8) | (v << 8));
   }

   inline unsigned flip(const unsigned v) const
   {
      return (
               ((v & 0xFF000000) >> 0x18) |
               ((v & 0x000000FF) << 0x18) |
               ((v & 0x00FF0000) >> 0x08) |
               ((v & 0x0000FF00) << 0x08)
             );
   }

   unsigned width_;
   unsigned height_;
   unsigned row_increment_;
   std::vector<PixelType> data_vec_;
   PixelType* data_;
   PixelType* data_end_;
};

}
