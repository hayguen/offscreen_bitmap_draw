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


namespace OffScreenBitmapDraw
{

template <class BitmapImageType = bitmap_image_rgb<>, class Float = double>
inline Float psnr_region(
    const unsigned int& width,
    const unsigned int& height,
    const BitmapImageType& image1,
    const unsigned int& x1,
    const unsigned int& y1,
    const BitmapImageType& image2,
    const unsigned int& x2,
    const unsigned int& y2
    )
{
    if ((x1 + width ) > image1.width() ) { return Float(0); }
    if ((y1 + height) > image1.height()) { return Float(0); }
    if ((x2 + width ) > image2.width() ) { return Float(0); }
    if ((y2 + height) > image2.height()) { return Float(0); }

    Float mse = 0;
    for (unsigned int r = 0; r < height; ++r)
    {
        const typename BitmapImageType::component_t * itr1     = cbegin(image1.row(r + y1)[x1]);
        const typename BitmapImageType::component_t * itr1_end = cend(image1.row(r + y1)[x1 +width -1]);
        const typename BitmapImageType::component_t * itr2     = cbegin(image2.row(r + y2)[x2]);
        while (itr1 != itr1_end)
        {
            Float v = (static_cast<Float>(*itr1) - static_cast<Float>(*itr2));
            mse += v * v;
            ++itr1;
            ++itr2;
        }
    }
    if (mse <= Float(0.0000001))
        return Float(1000000.0);
    else
    {
        mse /= (Float(3) * width * height);
        return Float(20) * std::log10(Float(255) / std::sqrt(mse));
    }
}

template <class BitmapImageType = bitmap_image_rgb<>, class Float = double>
inline Float psnr(
    const BitmapImageType& image1,
    const BitmapImageType& image2
    )
{
    return psnr_region<BitmapImageType, Float>(
        image1.width(), image1.height(),
        image1, 0, 0,
        image2, 0, 0
        );
}

template <class BitmapImageType = bitmap_image_rgb<>, class Float = double>
inline Float psnr(
    const unsigned int& x,
    const unsigned int& y,
    const BitmapImageType& image1,
    const BitmapImageType& image2
    )
{
    return psnr_region<BitmapImageType, Float>(
        x, y, image1.width(), image1.height(),
        image1, image2
        );
}


template <class BitmapImageType = bitmap_image_rgb<>, class Float = double>
inline void hierarchical_psnr_r(
    const Float& x,
    const Float& y,
    const Float& width,
    const Float& height,
    const BitmapImageType& image1,
    BitmapImageType& image2,
    const Float& threshold,
    const rgb_t colormap[]
    )
{
    if ( width <= Float(4) || height <= Float(4) )
    {
        const Float psnr = psnr_region(
            static_cast<unsigned int>(x),
            static_cast<unsigned int>(y),
            static_cast<unsigned int>(width),
            static_cast<unsigned int>(height),
            image1, image2
            );
        if (psnr < threshold)
        {
            rgb_t c = colormap[static_cast<unsigned int>(Float(1000) * (Float(1) - (psnr / threshold)))];
            image2.set_region(
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
        const Float half_width  =  width / Float(2);
        const Float half_height = height / Float(2);
        hierarchical_psnr_r(x             , y              , half_width, half_height, image1, image2, threshold, colormap);
        hierarchical_psnr_r(x + half_width, y              , half_width, half_height, image1, image2, threshold, colormap);
        hierarchical_psnr_r(x + half_width, y + half_height, half_width, half_height, image1, image2, threshold, colormap);
        hierarchical_psnr_r(x             , y + half_height, half_width, half_height, image1, image2, threshold, colormap);
    }
}

template <class BitmapImageType = bitmap_image_rgb<>, class Float = double>
inline void hierarchical_psnr(
    const BitmapImageType& image1,
    BitmapImageType& image2,
    const Float threshold,
    const rgb_t colormap[]
    )
{
    if ( image1.width()  != image2.width () || image1.height() != image2.height() )
        return;

    const Float psnr = psnr_region(
        0, 0, image1.width(), image1.height(),
        image1, image2
        );

    if (psnr < threshold)
    {
        hierarchical_psnr_r(
            0, 0, image1.width(), image1.height(),
            image1, image2,
            threshold,
            colormap
            );
    }
}

}
