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

    struct PixelSetter
    {
        // an operator with additional x and y would allow clipping
        inline void operator()(pixel_t* pos, pixel_t value)
        {
            *pos = value;
        }
    };

    struct PixelAdder
    {
        inline void operator()(pixel_t* pos, pixel_t value)
        {
            *pos += value;
        }
    };


    zingl_image_drawer(BitmapImageType& image)
        : image_(image)
    {}

    template <class Setter = PixelSetter>
    void plotPoint(int x0, int y0, const pixel_t color)
    {
        pixel_t * pos = image_.row(y0) + x0;
        Setter setPixel{};
        setPixel(pos, color);
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
        Setter setPixel{};
        setPixel(pos - image_.row_increment(), color);
        setPixel(pos - 1, color);
        setPixel(pos, color);
        setPixel(pos + 1, color);
        setPixel(pos + image_.row_increment(), color);
    }

    template <class Setter = PixelSetter>
    inline void plotCross(Point pt, const pixel_t color)
    {
        plotCross<Setter>(pt.first, pt.second, color);
    }

    template <class Setter = PixelSetter>
    void plotLine(int x0, int y0, int x1, int y1, const pixel_t color)
    {
        const int dx =  std::abs(x1-x0), sx = x0<x1 ? 1 : -1;
        const int dy = -std::abs(y1-y0), sy = y0<y1 ? 1 : -1;
        const int dy_inc = sy * int(image_.row_increment());
        int err = dx+dy, e2;                                  /* error value e_xy */
        pixel_t * pos = image_.row(y0) + x0;
        Setter setPixel{};

        for (;;) {                                                        /* loop */
            setPixel(pos, color);
            e2 = 2*err;
            if (e2 >= dy) {                                       /* e_xy+e_x > 0 */
                if (x0 == x1) break;
                err += dy;
                x0 += sx;
                pos += sx;
            }
            if (e2 <= dx) {                                       /* e_xy+e_y < 0 */
                if (y0 == y1) break;
                err += dx;
                y0 += sy;
                pos += dy_inc;
            }
        }
    }

    template <class Setter = PixelSetter>
    inline void plotLine(Point ptA, Point ptB, const pixel_t color)
    {
        plotLine<Setter>(ptA.first, ptA.second, ptB.first, ptB.second, color);
    }

    template <class Setter = PixelSetter>
    void plotLineWidth(int x0, int y0, const int x1, const int y1, float wd, const pixel_t color)
    {                                    /* plot an anti-aliased line of width wd */
        int dx = std::abs(x1-x0);
        int dy = std::abs(y1-y0);
        const int sx = x0 < x1 ? 1 : -1;
        const int sy = y0 < y1 ? 1 : -1;
        int err = dx-dy, e2, x2, y2;                           /* error value e_xy */
        float ed = dx+dy == 0 ? 1 : std::sqrt((float)dx*dx+(float)dy*dy);
        const int dy_inc = sy * int(image_.row_increment());
        pixel_t * row0 = image_.row(y0);
        Setter setPixel{};

        for (wd = (wd+1)/2; ; ) {                                    /* pixel loop */
            float col_mulA = (std::abs(err-dx+dy)/ed-wd+1);  // * 255
            setPixel(&row0[x0], color * (1.0F -std::max(0.0F, col_mulA)));
            e2 = err; x2 = x0;
            if (2*e2 >= -dx) {                                            /* x step */
                pixel_t * row2 = row0;
                for (e2 += dy, y2 = y0; e2 < ed*wd && (y1 != y2 || dx > dy); e2 += dx)
                {
                    y2 += sy; row2 += dy_inc;
                    float col_mulB = (std::abs(e2)/ed-wd+1);  // * 255
                    setPixel(&row2[x0], color * (1.0F -std::max(0.0F, col_mulB)));
                }
                if (x0 == x1) break;
                e2 = err; err -= dy; x0 += sx;
            }
            if (2*e2 <= dy) {                                             /* y step */
                for (e2 = dx-e2; e2 < ed*wd && (x1 != x2 || dx < dy); e2 += dy)
                {
                    x2 += sx;
                    float col_mulB = (std::abs(e2)/ed-wd+1);  // * 255
                    setPixel(&row0[x2], color * (1.0F -std::max(0.0F, col_mulB)));
                }
                if (y0 == y1) break;
                err += dx; y0 += sy; row0 += dy_inc;
            }
        }
    }

    template <class Setter = PixelSetter>
    inline void plotLineWidth(const Point ptA, const Point ptB, float wd, const pixel_t color)
    {
        plotLineWidth<Setter>(ptA.first, ptA.second, ptB.first, ptB.second, wd, color);
    }

    template <class Setter = PixelSetter>
    void plotEllipse(const int xm, const int ym, const int rx, const int ry, const pixel_t color)
    {
        int x = -rx, y = 0;           /* II. quadrant from bottom left to top right */
        long e2 = (long)ry*ry, err = (long)x*(2*e2+x)+e2;         /* error of 1.step */
        const int row_inc = int(image_.row_increment());
        pixel_t * row_upper = image_.row(ym + y);   // ym + y
        pixel_t * row_lower = image_.row(ym - y);   // ym - y
        Setter setPixel{};

        do {
            setPixel(&row_upper[xm-x], color);                   /*   I. Quadrant */
            setPixel(&row_upper[xm+x], color);                   /*  II. Quadrant */
            setPixel(&row_lower[xm+x], color);                   /* III. Quadrant */
            setPixel(&row_lower[xm-x], color);                   /*  IV. Quadrant */
            e2 = 2*err;
            if (e2 >= (x*2+1)*(long)ry*ry)                        /* e_xy+e_x > 0 */
                err += (++x*2+1)*(long)ry*ry;
            if (e2 <= (y*2+1)*(long)rx*rx)                        /* e_xy+e_y < 0 */
            {
                err += (++y*2+1)*(long)rx*rx;
                row_upper += row_inc;   // image_.row(ym+y);
                row_lower -= row_inc;   // image_.row(ym-y);
            }
        } while (x <= 0);

        while (y++ < ry)                  /* too early stop of flat ellipses a=1, */
        {
            row_upper += row_inc;   // image_.row(ym+y)
            row_lower -= row_inc;   // image_.row(ym-y)
            setPixel(&row_upper[xm], color);           /* -> finish tip of ellipse */
            setPixel(&row_lower[xm], color);
        }
    }

    template <class Setter = PixelSetter>
    void plotEllipse(const Point ptCenter, const Point radius, const pixel_t color)
    {
        plotEllipse<Setter>(ptCenter.first, ptCenter.second, radius.first, radius.second, color);
    }

    template <class Setter = PixelSetter>
    void plotOptimizedEllipse(const int xm, const int ym, const int rx, const int ry, const pixel_t color)
    {
        long x = -rx, y = 0;          /* II. quadrant from bottom left to top right */
        long e2 = ry, dx = (1+2*x)*e2*e2;                       /* error increment  */
        long dy = x*x, err = dx+dy;                             /* error of 1.step */
        const int row_inc = int(image_.row_increment());
        pixel_t * row_upper = image_.row(ym + y);   // ym + y
        pixel_t * row_lower = image_.row(ym - y);   // ym - y
        Setter setPixel{};

        do {
            setPixel(&row_upper[xm-x], color);                    /*   I. Quadrant */
            setPixel(&row_upper[xm+x], color);                    /*  II. Quadrant */
            setPixel(&row_lower[xm+x], color);                    /* III. Quadrant */
            setPixel(&row_lower[xm-x], color);                    /*  IV. Quadrant */
            e2 = 2*err;
            if (e2 >= dx) { x++; err += dx += 2*(long)ry*ry; }           /* x step */
            if (e2 <= dy) { /* y step */
                y++;
                err += dy += 2*(long)rx*rx;
                row_upper += row_inc;   // image_.row(ym+y)
                row_lower -= row_inc;   // image_.row(ym-y)
            }
        } while (x <= 0);

        while (y++ < ry)                  /* too early stop of flat ellipses a=1, */
        {
            row_upper += row_inc;   // image_.row(ym+y)
            row_lower -= row_inc;   // image_.row(ym-y)
            setPixel(&row_upper[xm], color);           /* -> finish tip of ellipse */
            setPixel(&row_lower[xm], color);
        }
    }

    template <class Setter = PixelSetter>
    void plotOptimizedEllipse(const Point ptCenter, const Point radius, const pixel_t color)
    {
        plotOptimizedEllipse<Setter>(ptCenter.first, ptCenter.second, radius.first, radius.second, color);
    }

    template <class Setter = PixelSetter>
    void plotCircle(const int xm, const int ym, const int radius, const pixel_t color)
    {
        int r = radius;
        int x = -r, y = 0, err = 2-2*r;                /* bottom left to top right */
        const int row_inc = int(image_.row_increment());
        pixel_t * row_upper = image_.row(ym + y);
        pixel_t * row_lower = image_.row(ym - y);
        pixel_t * row_left  = image_.row(ym - x);
        pixel_t * row_right = image_.row(ym + x);
        Setter setPixel{};

        do {
            setPixel(&row_upper[xm-x], color);               /*   I. Quadrant +x +y */
            setPixel(&row_left [xm-y], color);               /*  II. Quadrant -x +y */
            setPixel(&row_lower[xm+x], color);               /* III. Quadrant -x -y */
            setPixel(&row_right[xm+y], color);               /*  IV. Quadrant +x -y */

            r = err;
            if (r <= y) {                                           /* e_xy+e_y < 0 */
                err += ++y*2+1;
                row_upper += row_inc;   // image_.row(ym+y)
                row_lower -= row_inc;   // image_.row(ym-y)
            }
            if (r > x || err > y) {                /* e_xy+e_x > 0 or no 2nd y-step */
                err += ++x*2+1;                                    /* -> x-step now */
                row_left  -= row_inc;   // image_.row(ym-x)
                row_right += row_inc;   // image_.row(ym+x)
            }
        } while (x < 0);
    }

    template <class Setter = PixelSetter>
    void plotCircle(const Point ptCenter, const int radius, const pixel_t color)
    {
        plotCircle<Setter>(ptCenter.first, ptCenter.second, radius, color);
    }

    template <class Setter = PixelSetter>
    void plotEllipseRect(int x0, int y0, int x1, int y1, const pixel_t color)
    {                              /* rectangular parameter enclosing the ellipse */
        long a = std::abs(x1-x0), b = std::abs(y1-y0), b1 = b&1;       /* diameter */
        double dx = 4*(1.0-a)*b*b, dy = 4*(b1+1)*a*a;           /* error increment */
        double err = dx+dy+b1*a*a, e2;                          /* error of 1.step */

        if (x0 > x1) { x0 = x1; x1 += a; }        /* if called with swapped points */
        if (y0 > y1) y0 = y1;                                  /* .. exchange them */
        y0 += (b+1)/2; y1 = y0-b1;                               /* starting pixel */
        a = 8*a*a; b1 = 8*b*b;

        const int row_inc = int(image_.row_increment());
        pixel_t * row0 = image_.row(y0);
        pixel_t * row1 = image_.row(y1);
        Setter setPixel{};

        do {
            setPixel(&row0[x1], color);                            /*   I. Quadrant */
            setPixel(&row0[x0], color);                            /*  II. Quadrant */
            setPixel(&row1[x0], color);                            /* III. Quadrant */
            setPixel(&row1[x1], color);                            /*  IV. Quadrant */
            e2 = 2*err;
            if (e2 <= dy) {                                               /* y step */
                 y0++; y1--; err += dy += a;
                 row0 += row_inc;
                 row1 -= row_inc;
            }
            if (e2 >= dx || 2*err > dy) { x0++; x1--; err += dx += b1; }  /* x step */
        } while (x0 <= x1);

        while (y0-y1 <= b) {                /* too early stop of flat ellipses a=1 */
            setPixel(&row0[x0-1], color);               /* -> finish tip of ellipse */
            setPixel(&row0[x1+1], color);
            y0++; row0 += row_inc;
            setPixel(&row1[x0-1], color);
            setPixel(&row1[x1+1], color);
            y1--; row1 -= row_inc;
        }
    }

    template <class Setter = PixelSetter>
    void plotEllipseRect(const Point pt0, const Point pt1, const pixel_t color)
    {
        plotEllipseRect<Setter>(pt0.first, pt0.second, pt1.first, pt1.second, color);
    }

private:
    zingl_image_drawer(const zingl_image_drawer& id);
    zingl_image_drawer& operator =(const zingl_image_drawer& id);

    BitmapImageType& image_;
};

}
