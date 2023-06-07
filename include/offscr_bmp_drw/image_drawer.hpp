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

template <class BitmapImageType = bitmap_image_rgb<> >
class image_drawer
{
public:

    typedef typename BitmapImageType::pixel_t pixel_t;


    image_drawer(BitmapImageType& image)
        : image_(image), pen_width_(1), pen_color_{}
    {}

    void rectangle(int x1, int y1, int x2, int y2)
    {
        line_segment(x1, y1, x2, y1);
        line_segment(x2, y1, x2, y2);
        line_segment(x2, y2, x1, y2);
        line_segment(x1, y2, x1, y1);
    }

    void triangle(int x1, int y1, int x2, int y2,int x3, int y3)
    {
        line_segment(x1, y1, x2, y2);
        line_segment(x2, y2, x3, y3);
        line_segment(x3, y3, x1, y1);
    }

    void quadix(int x1, int y1, int x2, int y2,int x3, int y3, int x4, int y4)
    {
        line_segment(x1, y1, x2, y2);
        line_segment(x2, y2, x3, y3);
        line_segment(x3, y3, x4, y4);
        line_segment(x4, y4, x1, y1);
    }

    void line_segment(int x1, int y1, int x2, int y2)
    {
        int steep = 0;
        int sx    = ((x2 - x1) > 0) ? 1 : -1;
        int sy    = ((y2 - y1) > 0) ? 1 : -1;
        int dx    = abs(x2 - x1);
        int dy    = abs(y2 - y1);
        if (dy > dx)
        {
            std::swap(x1,y1);
            std::swap(dx,dy);
            std::swap(sx,sy);
            steep = 1;
        }

        int e = 2 * dy - dx;

        for (int i = 0; i < dx; ++i)
        {
            if (steep)
                plot_pen_pixel(y1,x1);
            else
                plot_pen_pixel(x1,y1);

            while (e >= 0)
            {
                y1 += sy;
                e -= (dx << 1);
            }
            x1 += sx;
            e  += (dy << 1);
        }
        plot_pen_pixel(x2,y2);
    }

    void horiztonal_line_segment(int x1, int x2, int y)
    {
        if (x1 > x2)
        {
            std::swap(x1,x2);
        }

        for (int i = 0; i <= (x2 - x1); ++i)    // include x2
        {
            plot_pen_pixel(x1 +  i,y);
        }
    }

    void vertical_line_segment(int x, int y1, int y2)   // API changed: now: pass x first
    {
        if (y1 > y2)
        {
            std::swap(y1,y2);
        }

        for (int i = 0; i <= (y2 - y1); ++i)    // include y2
        {
            plot_pen_pixel(x, y1 +  i);
        }
    }

    void ellipse(int center_x, int center_y, int a, int b)
    {
        int t1 = a * a;
        int t2 = t1 << 1;
        int t3 = t2 << 1;
        int t4 = b * b;
        int t5 = t4 << 1;
        int t6 = t5 << 1;
        int t7 = a * t5;
        int t8 = t7 << 1;
        int t9 = 0;

        int d1 = t2 - t7 + (t4 >> 1);
        int d2 = (t1 >> 1) - t8 + t5;
        int x  = a;
        int y  = 0;

        int negative_tx = center_x - x;
        int positive_tx = center_x + x;
        int negative_ty = center_y - y;
        int positive_ty = center_y + y;

        while (d2 < 0)
        {
            plot_pen_pixel(positive_tx, positive_ty);
            plot_pen_pixel(positive_tx, negative_ty);
            plot_pen_pixel(negative_tx, positive_ty);
            plot_pen_pixel(negative_tx, negative_ty);
            ++y;
            t9 = t9 + t3;
            if (d1 < 0)
            {
                d1 = d1 + t9 + t2;
                d2 = d2 + t9;
            }
            else
            {
                x--;
                t8 = t8 - t6;
                d1 = d1 + (t9 + t2 - t8);
                d2 = d2 + (t9 + t5 - t8);
                negative_tx = center_x - x;
                positive_tx = center_x + x;
            }
            negative_ty = center_y - y;
            positive_ty = center_y + y;
        }

        do
        {
            plot_pen_pixel(positive_tx, positive_ty);
            plot_pen_pixel(positive_tx, negative_ty);
            plot_pen_pixel(negative_tx, positive_ty);
            plot_pen_pixel(negative_tx, negative_ty);
            x--;
            t8 = t8 - t6;
            if (d2 < 0)
            {
                ++y;
                t9 = t9 + t3;
                d2 = d2 + (t9 + t5 - t8);
                negative_ty = center_y - y;
                positive_ty = center_y + y;
            }
            else
                d2 = d2 + (t5 - t8);
            negative_tx = center_x - x;
            positive_tx = center_x + x;
        }
        while (x >= 0);
    }

    void circle(int center_x, int center_y, int radius)
    {
        int x = 0;
        int d = (1 - radius) << 1;

        while (radius >= 0)
        {
            plot_pen_pixel(center_x + x, center_y + radius);
            plot_pen_pixel(center_x + x, center_y - radius);
            plot_pen_pixel(center_x - x, center_y + radius);
            plot_pen_pixel(center_x - x, center_y - radius);

            if ((d + radius) > 0)
                d -= ((--radius) << 1) - 1;
            if (x > d)
                d += ((++x) << 1) + 1;
        }
    }

    void plot_pen_pixel(int x, int y)
    {
        switch (pen_width_)
        {
        case 1:
            plot_pixel(x,y);
            break;
        case 2:
            plot_pixel(x    , y    );
            plot_pixel(x + 1, y    );
            plot_pixel(x + 1, y + 1);
            plot_pixel(x    , y + 1);
            break;
        case 3:
            plot_pixel(x    , y - 1);
            plot_pixel(x - 1, y - 1);
            plot_pixel(x + 1, y - 1);
            plot_pixel(x    , y    );
            plot_pixel(x - 1, y    );
            plot_pixel(x + 1, y    );
            plot_pixel(x    , y + 1);
            plot_pixel(x - 1, y + 1);
            plot_pixel(x + 1, y + 1);
            break;
        default:
            plot_pixel(x,y);
            break;
        }
    }

    void plot_pixel(int x, int y)
    {
        if (
            (x < 0) ||
            (y < 0) ||
            (x >= static_cast<int>(image_.width ())) ||
            (y >= static_cast<int>(image_.height())) )
            return;
        image_.set_pixel(x, y, pen_color_);
    }

    void pen_width(const unsigned int& width)
    {
        if ((width > 0) && (width < 4))
        {
            pen_width_ = width;
        }
    }

    void pen_color(const pixel_t v)
    {
        pen_color_ = v;
    }

private:
    image_drawer(const image_drawer& id);
    image_drawer& operator =(const image_drawer& id);

    BitmapImageType& image_;
    unsigned int  pen_width_;
    pixel_t pen_color_;
};

}
