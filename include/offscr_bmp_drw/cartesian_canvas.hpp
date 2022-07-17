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

#include "bitmap_image.hpp"
#include "image_drawer.hpp"


namespace OffScreenBitmapDraw
{

class cartesian_canvas
{
public:

   cartesian_canvas(const double x_length, const double y_length)
   : width_div2_ (0.0),
     height_div2_(0.0),
     min_x_      (0.0),
     min_y_      (0.0),
     max_x_      (0.0),
     max_y_      (0.0),
     draw_       (image_)
   {
      setup_canvas(x_length,y_length);
   }

   inline bool operator!()
   {
      return !image_;
   }

   void rectangle(double x1, double y1, double x2, double y2)
   {
      line_segment(x1, y1, x2, y1);
      line_segment(x2, y1, x2, y2);
      line_segment(x2, y2, x1, y2);
      line_segment(x1, y2, x1, y1);
   }

   void triangle(double x1, double y1, double x2, double y2, double x3, double y3)
   {
      line_segment(x1, y1, x2, y2);
      line_segment(x2, y2, x3, y3);
      line_segment(x3, y3, x1, y1);
   }

   void quadix(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4)
   {
      line_segment(x1, y1, x2, y2);
      line_segment(x2, y2, x3, y3);
      line_segment(x3, y3, x4, y4);
      line_segment(x4, y4, x1, y1);
   }

   void line_segment(double x1, double y1, double x2, double y2)
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

   void horiztonal_line_segment(double x1, double x2, double y)
   {
      x1 = clamp_x(x1);
      x2 = clamp_x(x2);
      y  = clamp_y( y);

      const int sc_x1 = static_cast<int>(cart_to_screen_x(x1));
      const int sc_x2 = static_cast<int>(cart_to_screen_x(x2));
      const int sc_y  = static_cast<int>(cart_to_screen_y(y ));

      draw_.horiztonal_line_segment(sc_x1, sc_x2, sc_y);
   }

   void vertical_line_segment(double y1, double y2, double x)
   {
      y1 = clamp_y(y1);
      y2 = clamp_y(y2);
      x  = clamp_x( x);

      const int sc_y1 = static_cast<int>(cart_to_screen_y(y1));
      const int sc_y2 = static_cast<int>(cart_to_screen_y(y2));
      const int sc_x  = static_cast<int>(cart_to_screen_x(x ));

      draw_.vertical_line_segment(sc_y1, sc_y2, sc_x);
   }

   void ellipse(double centerx, double centery, double a, double b)
   {

      const int sc_cx = static_cast<int>(cart_to_screen_x(centerx));
      const int sc_cy = static_cast<int>(cart_to_screen_y(centery));

      draw_.ellipse(sc_cx, sc_cy, static_cast<int>(a), static_cast<int>(b));
   }

   void circle(double centerx, double centery, double radius)
   {
      const int sc_cx = static_cast<int>(cart_to_screen_x(centerx));
      const int sc_cy = static_cast<int>(cart_to_screen_y(centery));

      draw_.circle(sc_cx, sc_cy, static_cast<int>(radius));
   }

   void fill_rectangle(double x1, double y1, double x2, double y2)
   {
      if (y1 > y2)
         std::swap(y1, y2);

      for (double y = y1; y <= y2; y += 0.5)
      {
        line_segment(x1, y, x2, y);
      }
   }

   void fill_triangle(double x1, double y1, double x2, double y2, double x3, double y3)
   {
      typedef std::pair<double,double> point_t;

      std::vector<point_t> p;

      p.push_back(std::make_pair(x1,y1));
      p.push_back(std::make_pair(x2,y2));
      p.push_back(std::make_pair(x3,y3));

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
            const double m0 = (p1.first - p0.first) / (2.0 * (p1.second - p0.second));
            const double m1 = (p2.first - p0.first) / (2.0 * (p2.second - p0.second));

            double x0 = p0.first;
            double x1 = p0.first;

            for (double y = p0.second; y <= p1.second; y += 0.5)
            {
               canvas.horiztonal_line_segment(x0, x1, y);

               x0 += m0;
               x1 += m1;
            }
         }

         void top(const point_t& p0, const point_t& p1, const point_t& p2)
         {
            const double m0 = (p2.first - p0.first) / (2.0 * (p2.second - p0.second));
            const double m1 = (p2.first - p1.first) / (2.0 * (p2.second - p1.second));

            double x0 = p2.first;
            double x1 = p2.first;

            for (double y = p2.second; y >= p0.second; y -= 0.5)
            {
               canvas.horiztonal_line_segment(x0, x1, y);

               x0 -= m0;
               x1 -= m1;
            }
         }
      };

      draw_modes dm(*this);

      const double eps = 0.00001;

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

   void fill_quadix(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4)
   {
      fill_triangle(x1, y1, x2, y2, x3, y3);
      fill_triangle(x1, y1, x3, y3, x4, y4);
   }

   void fill_circle(double cx, double cy, double radius)
   {
      const double delta = 1.0;
      double  x = radius;
      double  y = 0.0;
      double dx = delta - (2.0 * delta * radius);
      double dy = 0.0;
      double dr = 0.0;

      while (x >= y)
      {
         for (double i = cx - x; i <= cx + x; i += delta)
         {
            horiztonal_line_segment(cx - x, cx + x, cy + y);
            horiztonal_line_segment(cx - x, cx + x, cy - y);
         }

         for (double i = cx - y; i <= cx + y; i += delta)
         {
            horiztonal_line_segment(cx - y, cx + y, cy + x);
            horiztonal_line_segment(cx - y, cx + y, cy - x);
         }

         y += delta;

         dr += dy;
         dy += 2.0 * delta;

         if ((2.0 * delta * dr + dx) > 0)
         {
             x -= delta;
            dr +=  dx;
            dx += 2.0 * delta;
         }
      }
   }

   void plot_pen_pixel(double x, double y)
   {
      if ((x < min_x_) || (x > max_x_)) return;
      if ((y < min_y_) || (y > max_y_)) return;

      const int sc_x = static_cast<int>(cart_to_screen_x(x));
      const int sc_y = static_cast<int>(cart_to_screen_y(y));

      draw_.plot_pen_pixel(sc_x, sc_y);
   }

   void plot_pixel(double x, double y)
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

   void pen_color(const unsigned char&   red,
                  const unsigned char& green,
                  const unsigned char&  blue)
   {
      draw_.pen_color(red,green,blue);
   }

   template <typename RGB>
   void pen_color(const RGB colour)
   {
      draw_.pen_color(colour);
   }

   const bitmap_image& image() const
   {
      return image_;
   }

   bitmap_image& image()
   {
      return image_;
   }

   void set_widthheight(const double x_length, const double y_length)
   {
      setup_canvas(x_length, y_length);
   }

   double min_x() const { return min_x_; }
   double min_y() const { return min_y_; }
   double max_x() const { return max_x_; }
   double max_y() const { return max_y_; }

private:

   void setup_canvas(const double x_length, const double y_length)
   {
      if ((x_length < 2.0) || (y_length < 2.0))
         return;

      width_div2_  = x_length / 2.0;
      height_div2_ = y_length / 2.0;

      min_x_ = -width_div2_ ;
      min_y_ = -height_div2_;
      max_x_ =  width_div2_ ;
      max_y_ =  height_div2_;

      image_.setwidth_height(static_cast<unsigned int>(x_length) + 1, static_cast<unsigned int>(y_length) + 1);

      image_.clear(0xFF);
   }

   double clamp_x(const double& x)
   {
           if (x < min_x_)  return min_x_;
      else if (x > max_x_)  return max_x_;
      else                  return x;
   }

   double clamp_y(const double& y)
   {
           if (y < min_y_)  return min_y_;
      else if (y > max_y_)  return max_y_;
      else                  return y;
   }

   double cart_to_screen_x(const double& x)
   {
      return x + width_div2_;
   }

   double cart_to_screen_y(const double& y)
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
                 const double&  x, const double&  y,
                 const double& x1, const double& y1,
                 const double& x2, const double& y2
               )
   {
      int result = 0;
      if (y < y1)      result |= e_clip_bottom;
      else if (y > y2) result |= e_clip_top;

      if (x < x1)      result |= e_clip_left;
      else if (x > x2) result |= e_clip_right;

      return result;
   }

   bool clip(double& x1, double& y1, double& x2, double& y2)
   {
      bool   result = false;
      double x      = 0.0;
      double y      = 0.0;

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

            double dx = (x2 - x1);
            double dy = (y2 - y1);

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

   double width_div2_;
   double height_div2_;
   double min_x_;
   double min_y_;
   double max_x_;
   double max_y_;
   bitmap_image image_;
   image_drawer draw_;
};

}
