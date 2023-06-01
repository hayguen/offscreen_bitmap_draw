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

#include "bitmap_image_rgb.hpp"

#include <limits>
#include <algorithm>


namespace OffScreenBitmapDraw
{

template <typename ResponseImage, typename Palette, class BitmapImageType = bitmap_image_rgb<> >
inline std::size_t convert_rsp_to_image(const ResponseImage& resp_image, const Palette& palette, BitmapImageType& image)
{
    if ( resp_image.width() > image.width() || resp_image.height() > image.height() )
        return 0;

    for (std::size_t y = 0; y < resp_image.height(); ++y)
    {
        const typename ResponseImage::Float *src_row = resp_image.row(y);
        typename BitmapImageType::pixel_t *dest_row = image.row(y);
        for (std::size_t x = 0; x < resp_image.width(); ++x)
        {
            const typename ResponseImage::Float v = *src_row++; // resp_image(x,y);
            unsigned int index = static_cast<unsigned int>((v < 0) ? 0 : v > (palette.size()) ? (palette.size() - 1) : v);
            *dest_row++ = palette[index];   // image.set_pixel(x,y,palette[index]);
        }
    }
    return (resp_image.width() * resp_image.height());
}

template <typename Palette, class FloatImageType, class RGBImageType = bitmap_image_rgb<> >
inline bool convert_float_to_rgb(
    const FloatImageType& float_image,
    RGBImageType& rgb_image,
    const Palette* palette,
    const int palette_size,
    typename FloatImageType::pixel_t max_factor,
    bool abs_min = true
    )
{
    if ( float_image.width() > rgb_image.width() || float_image.height() > rgb_image.height() )
        return false;

    using Float = typename FloatImageType::pixel_t;
    using RGB = typename RGBImageType::pixel_t;

    Float fmin = 0;
    Float fmax = 1;
    if (max_factor > 0)
    {
        fmin = std::numeric_limits<Float>::max();
        fmax = std::numeric_limits<Float>::lowest();
        const Float *row = float_image.row(0);
        for (unsigned y = 0; y < float_image.height(); ++y)
        {
            // fmin = std::min( fmin, *std::min_element(row, row + float_image.width()) );
            // fmax = std::max( fmax, *std::max_element(row, row + float_image.width()) );
            auto minmax = std::minmax_element(row, row + float_image.width());
            fmin = std::min( fmin, *minmax.first );
            fmax = std::max( fmax, *minmax.second );
            row += float_image.row_increment();
        }
        fmax *= max_factor;
        if (!abs_min)
            fmin = Float(0);
    }
    const Float frange = fmax - fmin;
    if (frange <= 0)
        return false;
    const Float scale = (palette_size - Float(0.01)) / frange;
    const int index_max = palette_size - 1;

    for (unsigned y = 0; y < float_image.height(); ++y)
    {
        const Float *src_row = float_image.row(y);
        RGB *dest_row = rgb_image.row(y);
        for (unsigned x = 0; x < float_image.width(); ++x)
        {
            const Float v = *src_row++;
            int index = static_cast<int>( (v - fmin) * scale );
            // *dest_row++ = palette[std::clamp(index, 0, index_max)];  // requires c++17
            *dest_row++ = palette[ (index < 0) ? 0 : (index > index_max ) ? index_max : index ];
        }
    }
    return true;
}

}
