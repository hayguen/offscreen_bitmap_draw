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
#include "response_image.hpp"


namespace OffScreenBitmapDraw
{

template <class BitmapImageType = bitmap_image_rgb<>, class Float = double>
inline BitmapImageType & sobel_operator(
    const BitmapImageType& src_image,
    BitmapImageType& dst_image,
    const Float threshold = 0,
    const Float back_scale_gray = 1
    )
{
    using T = Float;
    response_image<T> im0(src_image.width(), src_image.height(), T(0));
    response_image<T> im1(src_image.width(), src_image.height(), T(0));

    T* temp_response_A = im0.row(0);
    src_image.export_gray_scale_response_image(temp_response_A);

    for (std::size_t y = 1; y < im0.height() - 1; ++y)
    {
        const T* itr0 = im0.row(y - 1);
        const T* itr1 = im0.row(y    );
        const T* itr2 = im0.row(y + 1);
        T* out  = im1.row(y) + 1;

        for (std::size_t x = 1; x < im0.width() - 1; ++x)
        {
            const T c0 = itr0[x - 1];   const T c1 = itr0[x];   const T c2 = itr0[x + 1];
            const T c3 = itr1[x - 1]; /*const T c4 = itr1[x];*/ const T c5 = itr1[x + 1];
            const T c6 = itr2[x - 1];   const T c7 = itr2[x];   const T c8 = itr2[x + 1];
            const T gx = (T(2) * (c5 - c3)) + (c2 - c0) + (c8 - c6);
            const T gy = (T(2) * (c1 - c7)) + (c0 - c6) + (c2 - c8);
            *(out++) = std::sqrt((gx * gx) + (gy * gy));
        }
    }

    if ( threshold > T(0) )
    {
        const T* end = im1.row(0) + (im1.width() * im1.height());
        for (T* itr = im1.row(0); itr != end; ++itr)
        {
            T& v = *itr;
            if ( v <= threshold )
                v = T(0);
        }
    }

    dst_image.setwidth_height( static_cast<unsigned int>(im1.width()), static_cast<unsigned int>(im1.height()) );
    const T* temp_response_B = im1.row(0);
    dst_image.import_gray_scale_clamped(temp_response_B, back_scale_gray);
    return dst_image;
}

template <class BitmapImageType = bitmap_image_rgb<>, class Float = double>
inline BitmapImageType & no_sobel_operator(
    const BitmapImageType& src_image,
    BitmapImageType& dst_image,
    const Float threshold = 0,
    const Float back_scale_gray = 1
    )
{
    using T = Float;
    response_image<T> im0(src_image.width(), src_image.height(), T(0));
    response_image<T> im1(src_image.width(), src_image.height(), T(0));

    T* temp_response_A = im0.row(0);
    src_image.export_gray_scale_response_image(temp_response_A);

    for (std::size_t y = 1; y < im0.height() - 1; ++y)
    {
        const T* itr0 = im0.row(y - 1);
        const T* itr1 = im0.row(y    );
        const T* itr2 = im0.row(y + 1);
        T* out  = im1.row(y) + 1;
        for (std::size_t x = 1; x < im0.width() - 1; ++x)
        {
            const T c0 = itr0[x - 1];   const T c1 = itr0[x];   const T c2 = itr0[x + 1];
            const T c3 = itr1[x - 1]; /*const T c4 = itr1[x];*/ const T c5 = itr1[x + 1];
            const T c6 = itr2[x - 1];   const T c7 = itr2[x];   const T c8 = itr2[x + 1];
            const T gx = (T(2) * (c5 - c3)) + (c2 - c0) + (c8 - c6);
            const T gy = (T(2) * (c1 - c7)) + (c0 - c6) + (c2 - c8);
            *(out++) = std::sqrt((gx * gx) + (gy * gy));
        }
    }

    if ( threshold > T(0) )
    {
        const T* end = im1.row(0) + (im1.width() * im1.height());
        for (T* itr = im1.row(0); itr != end; ++itr)
        {
            T& v = *itr;
            if ( v <= threshold )
                v = T(0);
        }
    }

    dst_image.setwidth_height( static_cast<unsigned int>(im1.width()), static_cast<unsigned int>(im1.height()) );
    const T* temp_response_B = im1.row(0);
    dst_image.import_gray_scale_clamped(temp_response_B, back_scale_gray);
    return dst_image;
}

}
