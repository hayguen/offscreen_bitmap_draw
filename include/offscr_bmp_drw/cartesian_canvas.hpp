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
#include "image_drawer.hpp"

#include <array>


namespace OffScreenBitmapDraw
{

/// have mathematical coordinates:
///   x grows from left to right
///   y grows from bottom to top
/// coordinates are only shifted, that point (0|0) is at center,
/// but there's NO scaling
template <class BitmapImageType = bitmap_image_rgb<>, class Float = double>
class cartesian_canvas
{
public:

    static constexpr Float zero = Float(0.0);
    static constexpr Float half = Float(0.5);
    static constexpr Float one  = Float(1.0);
    static constexpr Float two  = Float(2.0);
    static constexpr Float eps  = Float(0.00001);

    typedef typename BitmapImageType::pixel_t pixel_t;
    typedef typename BitmapImageType::component_t component_t;

    typedef std::pair<Float,Float> point_t;


   cartesian_canvas(const Float x_length, const Float y_length)
   : width_div2_ (zero)
   , height_div2_(zero)
   , min_x_      (zero)
   , min_y_      (zero)
   , max_x_      (zero)
   , max_y_      (zero)
   , image_      ()
   , draw_       (image_)
   {
      setup_canvas(x_length,y_length);
   }

   inline bool operator!()
   {
      return !image_;
   }

   void rectangle(Float x1, Float y1, Float x2, Float y2)
   {
      line_segment(x1, y1, x2, y1);
      line_segment(x2, y1, x2, y2);
      line_segment(x2, y2, x1, y2);
      line_segment(x1, y2, x1, y1);
   }

   void triangle(Float x1, Float y1, Float x2, Float y2, Float x3, Float y3)
   {
      line_segment(x1, y1, x2, y2);
      line_segment(x2, y2, x3, y3);
      line_segment(x3, y3, x1, y1);
   }

   void quadix(Float x1, Float y1, Float x2, Float y2, Float x3, Float y3, Float x4, Float y4)
   {
      line_segment(x1, y1, x2, y2);
      line_segment(x2, y2, x3, y3);
      line_segment(x3, y3, x4, y4);
      line_segment(x4, y4, x1, y1);
   }

   void line_segment(Float x1, Float y1, Float x2, Float y2)
   {
      if (clip(x1, y1, x2, y2))
      {
         const int sc_x1 = static_cast<int>(cart_to_screen_x(x1));
         const int sc_x2 = static_cast<int>(cart_to_screen_x(x2));
         const int sc_y1 = static_cast<int>(cart_to_screen_y(y1));
         const int sc_y2 = static_cast<int>(cart_to_screen_y(y2));

         draw_.line_segment(sc_x1, sc_y1, sc_x2, sc_y2);
      }
   }

   void horiztonal_line_segment(Float x1, Float x2, Float y)
   {
      x1 = clamp_x(x1);
      x2 = clamp_x(x2);
      y  = clamp_y( y);

      const int sc_x1 = static_cast<int>(cart_to_screen_x(x1));
      const int sc_x2 = static_cast<int>(cart_to_screen_x(x2));
      const int sc_y  = static_cast<int>(cart_to_screen_y(y ));

      draw_.horiztonal_line_segment(sc_x1, sc_x2, sc_y);
   }

   void vertical_line_segment(Float y1, Float y2, Float x)
   {
      y1 = clamp_y(y1);
      y2 = clamp_y(y2);
      x  = clamp_x( x);

      const int sc_y1 = static_cast<int>(cart_to_screen_y(y1));
      const int sc_y2 = static_cast<int>(cart_to_screen_y(y2));
      const int sc_x  = static_cast<int>(cart_to_screen_x(x ));

      draw_.vertical_line_segment(sc_y1, sc_y2, sc_x);
   }

   void ellipse(Float center_x, Float center_y, Float a, Float b)
   {

      const int sc_cx = static_cast<int>(cart_to_screen_x(center_x));
      const int sc_cy = static_cast<int>(cart_to_screen_y(center_y));

      draw_.ellipse(sc_cx, sc_cy, static_cast<int>(a), static_cast<int>(b));
   }

   void circle(Float center_x, Float center_y, Float radius)
   {
      const int sc_cx = static_cast<int>(cart_to_screen_x(center_x));
      const int sc_cy = static_cast<int>(cart_to_screen_y(center_y));

      draw_.circle(sc_cx, sc_cy, static_cast<int>(radius));
   }

   void fill_rectangle(Float x1, Float y1, Float x2, Float y2)
   {
      if (y1 > y2)
         std::swap(y1, y2);

      for (Float y = y1; y <= y2; y += half)
      {
        line_segment(x1, y, x2, y);
      }
   }

   void fill_triangle(Float x1, Float y1, Float x2, Float y2, Float x3, Float y3)
   {
      std::array<point_t, 3> p { std::make_pair(x1,y1), std::make_pair(x2,y2), std::make_pair(x3,y3) };
      // p.push_back(std::make_pair(x1,y1));
      // p.push_back(std::make_pair(x2,y2));
      // p.push_back(std::make_pair(x3,y3));

      if (p[0].second > p[1].second)
         std::swap(p[0],p[1]);
      if (p[0].second > p[2].second)
         std::swap(p[0],p[2]);
      if (p[1].second > p[2].second)
         std::swap(p[1],p[2]);

      class draw_modes
      {
      private:

         cartesian_canvas& canvas;

         // Needed for incompetent and broken msvc compiler versions
         #ifdef _MSC_VER
            #pragma warning(push)
            #pragma warning(disable: 4822)
         #endif
         draw_modes& operator=(const draw_modes&);
         #ifdef _MSC_VER
            #pragma warning(pop)
         #endif

      public:

         draw_modes(cartesian_canvas& c)
         : canvas(c)
         {}

         void bottom(const point_t& p0, const point_t& p1, const point_t& p2)
         {
             const Float m0 = (p1.first - p0.first) / (two * (p1.second - p0.second));
             const Float m1 = (p2.first - p0.first) / (two * (p2.second - p0.second));

            Float x0 = p0.first;
            Float x1 = p0.first;

            for (Float y = p0.second; y <= p1.second; y += half)
            {
               canvas.horiztonal_line_segment(x0, x1, y);

               x0 += m0;
               x1 += m1;
            }
         }

         void top(const point_t& p0, const point_t& p1, const point_t& p2)
         {
            const Float m0 = (p2.first - p0.first) / (two * (p2.second - p0.second));
            const Float m1 = (p2.first - p1.first) / (two * (p2.second - p1.second));

            Float x0 = p2.first;
            Float x1 = p2.first;

            for (Float y = p2.second; y >= p0.second; y -= half)
            {
               canvas.horiztonal_line_segment(x0, x1, y);

               x0 -= m0;
               x1 -= m1;
            }
         }
      };

      draw_modes dm(*this);

      if (std::abs(p[1].second - p[2].second) < eps)
         dm.bottom(p[0], p[1], p[2]);
      else if (std::abs(p[0].second - p[1].second) < eps)
         dm.top(p[0], p[1], p[2]);
      else
      {
         point_t p3;

         p3.first  = (p[0].first + ((p[1].second - p[0].second) / (p[2].second - p[0].second)) * (p[2].first - p[0].first));
         p3.second = p[1].second;

         dm.bottom(p[0], p[1], p3  );
         dm.top   (p[1], p3  , p[2]);
      }
   }

   void fill_quadix(Float x1, Float y1, Float x2, Float y2, Float x3, Float y3, Float x4, Float y4)
   {
      fill_triangle(x1, y1, x2, y2, x3, y3);
      fill_triangle(x1, y1, x3, y3, x4, y4);
   }

   void fill_circle(Float cx, Float cy, Float radius)
   {
      const Float delta = one;
      Float  x = radius;
      Float  y = zero;
      Float dx = delta - (two * delta * radius);
      Float dy = zero;
      Float dr = zero;

      while (x >= y)
      {
         for (Float i = cx - x; i <= cx + x; i += delta)
         {
            horiztonal_line_segment(cx - x, cx + x, cy + y);
            horiztonal_line_segment(cx - x, cx + x, cy - y);
         }

         for (Float i = cx - y; i <= cx + y; i += delta)
         {
            horiztonal_line_segment(cx - y, cx + y, cy + x);
            horiztonal_line_segment(cx - y, cx + y, cy - x);
         }

         y += delta;

         dr += dy;
         dy += two * delta;

         if ((two * delta * dr + dx) > 0)
         {
             x -= delta;
            dr +=  dx;
            dx += two * delta;
         }
      }
   }

   void plot_pen_pixel(Float x, Float y)
   {
      if ((x < min_x_) || (x > max_x_)) return;
      if ((y < min_y_) || (y > max_y_)) return;

      const int sc_x = static_cast<int>(cart_to_screen_x(x));
      const int sc_y = static_cast<int>(cart_to_screen_y(y));

      draw_.plot_pen_pixel(sc_x, sc_y);
   }

   void plot_pixel(Float x, Float y)
   {
      if ((x < min_x_) || (x > max_x_)) return;
      if ((y < min_y_) || (y > max_y_)) return;

      const int sc_x = static_cast<int>(cart_to_screen_x(x));
      const int sc_y = static_cast<int>(cart_to_screen_y(y));

      draw_.plot_pixel(sc_x, sc_y);
   }

   void pen_width(const unsigned int& width)
   {
      draw_.pen_width(width);
   }

   void pen_color(const pixel_t color)
   {
      draw_.pen_color(color);
   }

   const BitmapImageType& image() const
   {
      return image_;
   }

   BitmapImageType& image()
   {
      return image_;
   }

   void set_widthheight(const Float x_length, const Float y_length)
   {
      setup_canvas(x_length, y_length);
   }

   Float min_x() const { return min_x_; }
   Float min_y() const { return min_y_; }
   Float max_x() const { return max_x_; }
   Float max_y() const { return max_y_; }

private:

   void setup_canvas(const Float x_length, const Float y_length)
   {
      if ((x_length < two) || (y_length < two))
         return;

      width_div2_  = x_length / two;
      height_div2_ = y_length / two;

      min_x_ = -width_div2_ ;
      min_y_ = -height_div2_;
      max_x_ =  width_div2_ ;
      max_y_ =  height_div2_;

      image_.setwidth_height(static_cast<unsigned int>(x_length) + 1, static_cast<unsigned int>(y_length) + 1);

      pixel_t color;
      set_white(color);
      image_.clear(color);
   }

   Float clamp_x(const Float& x) const
   {
           if (x < min_x_)  return min_x_;
      else if (x > max_x_)  return max_x_;
      else                  return x;
   }

   Float clamp_y(const Float& y) const
   {
           if (y < min_y_)  return min_y_;
      else if (y > max_y_)  return max_y_;
      else                  return y;
   }

   Float cart_to_screen_x(const Float& x) const
   {
      return x + width_div2_;
   }

   Float cart_to_screen_y(const Float& y) const
   {
      return height_div2_ - y;
   }

   enum clip_code
   {
      e_clip_bottom = 1,
      e_clip_top    = 2,
      e_clip_left   = 4,
      e_clip_right  = 8
   };

   int out_code(
                 const Float&  x, const Float&  y,
                 const Float& x1, const Float& y1,
                 const Float& x2, const Float& y2
               )
   {
      int result = 0;
      if (y < y1)      result |= e_clip_bottom;
      else if (y > y2) result |= e_clip_top;

      if (x < x1)      result |= e_clip_left;
      else if (x > x2) result |= e_clip_right;

      return result;
   }

   bool clip(Float& x1, Float& y1, Float& x2, Float& y2)
   {
      bool   result = false;
      Float x      = zero;
      Float y      = zero;

      int outcode0   = out_code(x1, y1, min_x_, min_y_, max_x_, max_y_);
      int outcode1   = out_code(x2, y2, min_x_, min_y_, max_x_, max_y_);
      int outcodeout = 0;

      while ((outcode0 != 0) || (outcode1 != 0))
      {
         if ((outcode0 & outcode1) != 0)
            return result;
         else
         {
            if (outcode0 != 0)
               outcodeout = outcode0;
            else
               outcodeout = outcode1;

            Float dx = (x2 - x1);
            Float dy = (y2 - y1);

            if ((outcodeout & e_clip_bottom) == e_clip_bottom)
            {
               x = x1 + dx * (min_y_ - y1) / dy;
               y = min_y_;
            }
            else if ((outcodeout & e_clip_top) == e_clip_top)
            {
               x = x1 + dx * (max_y_ - y1) / dy;
               y = max_y_;
            }
            else if ((outcodeout & e_clip_right) == e_clip_right)
            {
               y = y1 + dy * (max_x_ - x1) / dx;
               x = max_x_;
            }
            else if ((outcodeout & e_clip_left) == e_clip_left)
            {
               y = y1 + dy * (min_x_ - x1) / dx;
               x = min_x_;
            }

            if (outcodeout == outcode0)
            {
               x1 = x;
               y1 = y;
               outcode0 = out_code(x1, y1, min_x_, min_y_, max_x_, max_y_);
            }
            else
            {
               x2 = x;
               y2 = y;
               outcode1 = out_code(x2, y2, min_x_, min_y_, max_x_, max_y_);
            }
         }
      }

      return true;
   }

   cartesian_canvas(const cartesian_canvas&);
   cartesian_canvas operator=(const cartesian_canvas&);

   Float width_div2_;
   Float height_div2_;
   Float min_x_;
   Float min_y_;
   Float max_x_;
   Float max_y_;
   BitmapImageType image_;
   image_drawer<BitmapImageType> draw_;
};

}
