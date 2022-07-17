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

#include <vector>


namespace OffScreenBitmapDraw
{

template <typename T>
class response_image
{
public:

   response_image(const std::size_t& width, const std::size_t& height, const T null = T(0))
   : width_ (width ),
     height_(height),
     null_  (null  )
   {
      data_.resize(width_ * height_);
   }

   std::size_t width () const { return  width_; }
   std::size_t height() const { return height_; }

   void set_all(const T& t)
   {
      std::fill_n(data_.begin(), data_.size(), t);
   }

   const T& operator()(const std::size_t& x, const std::size_t& y) const
   {
      if (y >= height_) return null_;
      if (x >= width_ ) return null_;

      return data_[width_ * y + x];
   }

   T& operator()(const std::size_t& x, const std::size_t& y)
   {
      if (y >= height_) return null_;
      if (x >= width_ ) return null_;

      return data_[width_ * y + x];
   }

   bool valid(const std::size_t& x, const std::size_t& y)
   {
      return ((x < width_ ) || (y < height_));
   }

   void inc_all(const T& v)
   {
      for (std::size_t i = 0; i < data_.size(); ++i)
      {
         data_[i] += v;
      }
   }

   void mul_all(const T& v)
   {
      for (std::size_t i = 0; i < data_.size(); ++i)
      {
         data_[i] *= v;
      }
   }

   T* row (const std::size_t& row_index)
   {
      if (row_index < height_)
         return &data_[width_ * row_index];
      else
         return reinterpret_cast<T*>(0);
   }

   const T* row (const std::size_t& row_index) const
   {
      if (row_index < height_)
         return data_[width_ * row_index];
      else
         return reinterpret_cast<T*>(0);
   }

private:

   std::size_t    width_;
   std::size_t    height_;
   std::vector<T> data_;
   T              null_;
};


}
