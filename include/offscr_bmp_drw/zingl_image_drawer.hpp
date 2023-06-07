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

#include <utility>
#include <algorithm>
#include <cmath>


namespace OffScreenBitmapDraw
{

/**
 * Bresenham Curve Rasterizing Algorithms
 * @author alois zingl
 * @version V20.15 april 2020
 * @copyright MIT open-source license software
 * @url https://github.com/zingl/Bresenham
 * @author  Zingl Alois
 *
 * above is the origin. the code got adapted/refactored ..
*/

template <class BitmapImageType = bitmap_image_rgb<> >
class zingl_image_drawer
{
public:
    typedef typename BitmapImageType::pixel_t pixel_t;
    //typedef typename BitmapImageType::Point Point;  // x, y
    typedef std::pair<int, int> Point;  // x, y

    struct PixelSetterNoClipUsingPtr
    {
        PixelSetterNoClipUsingPtr(BitmapImageType &image) { (void)image; }
        // prevent copying
        PixelSetterNoClipUsingPtr() = delete;
        PixelSetterNoClipUsingPtr(const PixelSetterNoClipUsingPtr&) = delete;
        PixelSetterNoClipUsingPtr(PixelSetterNoClipUsingPtr&&) = delete;
        PixelSetterNoClipUsingPtr& operator=(const PixelSetterNoClipUsingPtr&) = delete;

        inline void operator()(int x, int y, pixel_t* pos, pixel_t value)
        {
            (void)x; (void)y;
            *pos = value;
        }

        inline void hLine(int x0, int x1, int y, pixel_t* pos0, pixel_t* pos1, pixel_t value)
        {
            (void)x0; (void)x1; (void)y;
            for (pixel_t *p = pos0; p <= pos1; ++p)
                *p = value;
        }

        inline void hLineCorners(int x0, int x1, int y, pixel_t* pos0, pixel_t* pos1, pixel_t value)
        {
            (void)x0; (void)x1; (void)y;
            *pos0 = value;
            if (pos1 != pos0)
                *pos1 = value;
        }

    };

    struct PixelAdderNoClipUsingPtr
    {
        PixelAdderNoClipUsingPtr(BitmapImageType &image) { (void)image; }
        // prevent copying
        PixelAdderNoClipUsingPtr() = delete;
        PixelAdderNoClipUsingPtr(const PixelAdderNoClipUsingPtr&) = delete;
        PixelAdderNoClipUsingPtr(PixelAdderNoClipUsingPtr&&) = delete;
        PixelAdderNoClipUsingPtr& operator=(const PixelAdderNoClipUsingPtr&) = delete;

        inline void operator()(int x, int y, pixel_t* pos, pixel_t value)
        {
            (void)x; (void)y;
            *pos += value;
        }

        inline void hLine(int x0, int x1, int y, pixel_t* pos0, pixel_t* pos1, pixel_t value)
        {
            (void)x0; (void)x1; (void)y;
            for (pixel_t *p = pos0; p <= pos1; ++p)
                *p += value;
        }

        inline void hLineCorners(int x0, int x1, int y, pixel_t* pos0, pixel_t* pos1, pixel_t value)
        {
            (void)x0; (void)x1; (void)y;
            *pos0 += value;
            if (pos1 != pos0)
                *pos1 += value;
        }

    };

    struct PixelSetterClippedUsingXY
    {
        PixelSetterClippedUsingXY(BitmapImageType &image)
            : w(image.width()), h(image.height()), image_(image) { }
        // prevent copying
        PixelSetterClippedUsingXY() = delete;
        PixelSetterClippedUsingXY(const PixelSetterClippedUsingXY&) = delete;
        PixelSetterClippedUsingXY(PixelSetterClippedUsingXY&&) = delete;
        PixelSetterClippedUsingXY& operator=(const PixelSetterClippedUsingXY&) = delete;

        inline void operator()(int x, int y, pixel_t* pos, pixel_t value)
        {
            (void)pos;
            if ( x >= 0 && x < w && y >= 0 && y < h )
                image_.pixel(x, y) = value;
        }

        inline void hLine(int x0, int x1, int y, pixel_t* pos0, pixel_t* pos1, pixel_t value)
        {
            (void)pos0; (void)pos1;
            if ( y >= 0 && y < h && ( x0 >= 0 || x1 < w ) )
            {
                if ( x0 < 0 ) x0 = 0;
                if ( x1 >= w ) x1 = w - 1;
                pixel_t *row = image_.row(y);
                for (int x = x0; x <= x1; ++x)
                    row[x] = value;
            }
        }

        inline void hLineCorners(int x0, int x1, int y, pixel_t* pos0, pixel_t* pos1, pixel_t value)
        {
            (void)pos0; (void)pos1;
            if ( y >= 0 && y < h && ( x0 >= 0 || x1 < w ) )
            {
                pixel_t *row = image_.row(y);
                if ( 0 <= x0 && x0 < w )
                    row[x0] = value;
                if ( x1 != x0 && 0 <= x1 && x1 < w )
                    row[x1] = value;
            }
        }

    private:
        const int w, h;
        BitmapImageType &image_;
    };

    struct PixelAdderClippedUsingXY
    {
        PixelAdderClippedUsingXY(BitmapImageType &image)
            : w(image.width()), h(image.height()), image_(image) { }
        // prevent copying
        PixelAdderClippedUsingXY() = delete;
        PixelAdderClippedUsingXY(const PixelAdderClippedUsingXY&) = delete;
        PixelAdderClippedUsingXY(PixelAdderClippedUsingXY&&) = delete;
        PixelAdderClippedUsingXY& operator=(const PixelAdderClippedUsingXY&) = delete;

        inline void operator()(int x, int y, pixel_t* pos, pixel_t value)
        {
            (void)pos;
            if ( x >= 0 && x < w && y >= 0 && y < h )
                image_.pixel(x, y) += value;
        }

        inline void hLine(int x0, int x1, int y, pixel_t* pos0, pixel_t* pos1, pixel_t value)
        {
            (void)pos0; (void)pos1;
            if ( y >= 0 && y < h && ( x0 >= 0 || x1 < w ) )
            {
                if ( x0 < 0 ) x0 = 0;
                if ( x1 >= w ) x1 = w - 1;
                pixel_t *row = image_.row(y);
                for (int x = x0; x <= x1; ++x)
                    row[x] += value;
            }
        }

        inline void hLineCorners(int x0, int x1, int y, pixel_t* pos0, pixel_t* pos1, pixel_t value)
        {
            (void)pos0; (void)pos1;
            if ( y >= 0 && y < h && ( x0 >= 0 || x1 < w ) )
            {
                pixel_t *row = image_.row(y);
                if ( 0 <= x0 && x0 < w )
                    row[x0] += value;
                if ( x1 != x0 && 0 <= x1 && x1 < w )
                    row[x1] += value;
            }
        }

    private:
        const int w, h;
        BitmapImageType &image_;
    };


    // using PixelSetter = PixelSetterNoClipUsingPtr;
    // using PixelAdder = PixelAdderNoClipUsingPtr;
    using PixelSetter = PixelSetterClippedUsingXY;
    using PixelAdder = PixelAdderClippedUsingXY;


    zingl_image_drawer(BitmapImageType& image)
        : image_(image)
    {}

    template <class Setter = PixelSetter>
    void plotPoint(int x0, int y0, const pixel_t color)
    {
        pixel_t * pos = image_.row(y0) + x0;
        Setter setPixel{image_};
        setPixel(x0, y0, pos, color);
    }

    template <class Setter = PixelSetter>
    inline void plotPoint(Point pt, const pixel_t color)
    {
        plotPoint<Setter>(pt.first, pt.second, color);
    }

    template <class Setter = PixelSetter>
    void plotCross(int x0, int y0, const pixel_t color)
    {
        pixel_t * pos = image_.row(y0) + x0;
        Setter setPixel{image_};
        setPixel(x0, y0-1, pos - image_.row_increment(), color);
        setPixel(x0-1, y0, pos - 1, color);
        setPixel(x0,   y0, pos, color);
        setPixel(x0+1, y0, pos + 1, color);
        setPixel(x0, y0+1, pos + image_.row_increment(), color);
    }

    template <class Setter = PixelSetter>
    inline void plotCross(Point pt, const pixel_t color)
    {
        plotCross<Setter>(pt.first, pt.second, color);
    }

    template <class Setter = PixelSetter>
    inline void plotHLine(int x0, int x1, int y, const pixel_t color)
    {
        Setter setPixel{image_};
        pixel_t * row = image_.row(y);
        setPixel.hLine(x0, x1, y, &row[x0], &row[x1], color);
    }

    template <class Setter = PixelSetter>
    inline void plotVLine(int x, int y0, int y1, const pixel_t color)
    {
        Setter setPixel{image_};
        pixel_t * row = image_.row(y0);
        const int row_inc = int(image_.row_increment());
        for ( int y = y0; y <= y1; ++y, row += row_inc )
            setPixel(x, y, &row[x], color);
    }

    template <class Setter = PixelSetter>
    inline void plotRect(int x0, int y0, int x1, int y1, const pixel_t color)
    {
        Setter setPixel{image_};
        pixel_t * row = image_.row(y0);
        const int row_inc = int(image_.row_increment());
        setPixel.hLine(x0, x1, y0, &row[x0], &row[x1], color);
        for (int y = y0+1; y < y1; ++y)
        {
            row += row_inc;
            setPixel(x0, y, &row[x0], color);
            setPixel(x1, y, &row[x1], color);
        }
        if (y1 != y0)
        {
            row += row_inc;
            setPixel.hLine(x0, x1, y1, &row[x0], &row[x1], color);
        }
    }

    template <class Setter = PixelSetter>
    inline void plotRect(Point ptA, Point ptB, const pixel_t color)
    {
        plotRect<Setter>(ptA.first, ptA.second, ptB.first, ptB.second, color);
    }

    template <class Setter = PixelSetter>
    inline void fillRect(int x0, int y0, int x1, int y1, const pixel_t color)
    {
        Setter setPixel{image_};
        pixel_t * row = image_.row(y0);
        const int row_inc = int(image_.row_increment());
        for (int y = y0; y <= y1; ++y, row += row_inc)
            setPixel.hLine(x0, x1, y, &row[x0], &row[x1], color);
    }

    template <class Setter = PixelSetter>
    inline void fillRect(Point ptA, Point ptB, const pixel_t color)
    {
        fillRect<Setter>(ptA.first, ptA.second, ptB.first, ptB.second, color);
    }

    template <class Setter = PixelSetter>
    inline void plotLine(int x0, int y0, int x1, int y1, const pixel_t color);

    template <class Setter = PixelSetter>
    inline void plotLine(Point ptA, Point ptB, const pixel_t color);

    /* plot an anti-aliased line of width wd */
    template <class Setter = PixelSetter>
    inline void plotLineWidth(int x0, int y0, const int x1, const int y1, float wd, const pixel_t color);

    template <class Setter = PixelSetter>
    inline void plotLineWidth(const Point ptA, const Point ptB, float wd, const pixel_t color);

    template <class Setter = PixelSetter>
    inline void plotEllipse(const int xm, const int ym, const int rx, const int ry, const pixel_t color);

    template <class Setter = PixelSetter>
    inline void plotEllipse(const Point ptCenter, const Point radius, const pixel_t color);

    template <class Setter = PixelSetter>
    inline void fillEllipse(const int xm, const int ym, const int rx, const int ry, const pixel_t color);

    template <class Setter = PixelSetter>
    inline void fillEllipse(const Point ptCenter, const Point radius, const pixel_t color);

    template <class Setter = PixelSetter>
    inline void plotOptimizedEllipse(const int xm, const int ym, const int rx, const int ry, const pixel_t color);

    template <class Setter = PixelSetter>
    inline void plotOptimizedEllipse(const Point ptCenter, const Point radius, const pixel_t color);

    template <class Setter = PixelSetter>
    inline void fillOptimizedEllipse(const int xm, const int ym, const int rx, const int ry, const pixel_t color);

    template <class Setter = PixelSetter>
    inline void fillOptimizedEllipse(const Point ptCenter, const Point radius, const pixel_t color);

    template <class Setter = PixelSetter>
    inline void plotEllipseRect(int x0, int y0, int x1, int y1, const pixel_t color);

    template <class Setter = PixelSetter>
    inline void plotEllipseRect(const Point pt0, const Point pt1, const pixel_t color);

    template <class Setter = PixelSetter>
    inline void fillEllipseRect(int x0, int y0, int x1, int y1, const pixel_t color);
    /* rectangular parameter enclosing the ellipse */

    template <class Setter = PixelSetter>
    inline void fillEllipseRect(const Point pt0, const Point pt1, const pixel_t color);

    template <class Setter = PixelSetter>
    inline void plotCircle(const int xm, const int ym, const int radius, const pixel_t color);

    template <class Setter = PixelSetter>
    inline void plotCircle(const Point ptCenter, const int radius, const pixel_t color);

    template <class Setter = PixelSetter>
    inline void fillCircle(const int xm, const int ym, const int radius, const pixel_t color);

    template <class Setter = PixelSetter>
    inline void fillCircle(const Point ptCenter, const int radius, const pixel_t color);

private:
    zingl_image_drawer(const zingl_image_drawer& id);
    zingl_image_drawer& operator =(const zingl_image_drawer& id);

    BitmapImageType& image_;
};

}

#include "zingl_ellipse.hpp"
#include "zingl_ellipse_fill.hpp"
#include "zingl_ellipse_optimized.hpp"
#include "zingl_ellipse_optimized_fill.hpp"
#include "zingl_ellipse_rect.hpp"
#include "zingl_ellipse_rect_fill.hpp"
#include "zingl_circle.hpp"
#include "zingl_circle_fill.hpp"
#include "zingl_line.hpp"
#include "zingl_line_width.hpp"
