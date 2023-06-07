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

#include <fstream>
#include <vector>


namespace OffScreenBitmapDraw
{

template <class BitmapImageType = bitmap_image_rgb<> >
class image_io
{
    using ImageT = BitmapImageType;
public:

    static BitmapImageType load(const std::string& filename)
    {
        BitmapImageType image;
        bool success = load(filename, image);
        if (!success)
            image.reset();
        return image;
    }

    static bool load(const std::string& filename, BitmapImageType &image)
    {
        std::ifstream stream(filename.c_str(),std::ios::binary);
        if (!stream)
        {
            std::cerr << "image_io::load() ERROR: BitmapImageType - "
                      << "file " << filename << " not found!" << std::endl;
            return false;
        }

        image.reset();

        bitmap_file_header bfh;
        bitmap_information_header bih;

        bfh.clear();
        bih.clear();

        read_bfh(stream,bfh);
        read_bih(stream,bih);

        if (bfh.type != 19778)
        {
            std::cerr << "image_io::load() ERROR: BitmapImageType - "
                      << "Invalid type value " << bfh.type << " expected 19778." << std::endl;
            return false;
        }

        if (bih.bit_count != 24)
        {
            std::cerr << "image_io::load() ERROR: BitmapImageType - "
                      << "Invalid bit depth " << bih.bit_count << " expected 24." << std::endl;
            return false;
        }

        if (bih.size != bih.struct_size())
        {
            std::cerr << "image_io::load() ERROR: BitmapImageType - "
                      << "Invalid BIH size " << bih.size
                      << " expected " << bih.struct_size() << std::endl;
            return false;
        }

        if (ImageT::bytes_per_pixel() != unsigned(bih.bit_count >> 3))
        {
            std::cerr << "image_io::load() ERROR: BitmapImageType - "
                      << "Invalid bit count " << bih.bit_count
                      << " expected " << 24 << std::endl;
            return false;
        }

        const unsigned l_width  = bih.width;
        const unsigned l_height = bih.height;

        unsigned padding = (4 - ((3 * l_width) % 4)) % 4;
        char padding_data[4] = { 0x00, 0x00, 0x00, 0x00 };

        std::size_t bitmap_file_size = file_size(filename);

        std::size_t bitmap_logical_size = (l_height * l_width * ImageT::bytes_per_pixel()) +
                                          (l_height * padding)                   +
                                          bih.struct_size()                    +
                                          bfh.struct_size()                    ;

        if (bitmap_file_size != bitmap_logical_size)
        {
            std::cerr << "image_io::load() ERROR: BitmapImageType - "
                      << "Mismatch between logical and physical sizes of bitmap. "
                      << "Logical: "  << bitmap_logical_size << " "
                      << "Physical: " << bitmap_file_size    << std::endl;
            return false;
        }

        image.setwidth_height(l_width, l_height);

        if (sizeof(typename ImageT::pixel_t) == sizeof(bgr_t) && cbegin(*image.row(0)) != blue(*image.row(0)) )
        {
            std::vector<bgr_t> line_buffer(image.width());
            for (unsigned i = 0; i < image.height(); ++i)
            {
                typename ImageT::pixel_t * row_ptr = image.row(image.height() - i - 1);
                unsigned char* data_ptr = begin(*line_buffer.data());
                stream.read(reinterpret_cast<char*>(data_ptr), sizeof(char) * ImageT::bytes_per_pixel() * image.width());
                stream.read(padding_data, padding);

                for (unsigned x = 0; x < image.width(); ++x)
                {
                    *blue(row_ptr[x]) = *blue(line_buffer[x]);
                    *green(row_ptr[x]) = *green(line_buffer[x]);
                    *red(row_ptr[x]) = *red(line_buffer[x]);
                }
            }
        }
        else
        {
            for (unsigned i = 0; i < image.height(); ++i)
            {
                typename ImageT::pixel_t * row_ptr = image.row(image.height() - i - 1); // read in inverted row order
                unsigned char* data_ptr = begin(*row_ptr);
                stream.read(reinterpret_cast<char*>(data_ptr), sizeof(char) * ImageT::bytes_per_pixel() * image.width());
                stream.read(padding_data, padding);
            }
        }

        return true;
    }

    static bool save(const BitmapImageType &image, const std::string& file_name)
    {
        std::ofstream stream(file_name.c_str(),std::ios::binary);
        if (!stream)
        {
            std::cerr << "image_io::save(): Error - Could not open file "
                      << file_name << " for writing!" << std::endl;
            return false;
        }

        bitmap_information_header bih;
        bih.width            = image.width();
        bih.height           = image.height();
        bih.bit_count        = static_cast<unsigned short>(ImageT::bytes_per_pixel() << 3);
        bih.clr_important    = 0;
        bih.clr_used         = 0;
        bih.compression      = 0;
        bih.planes           = 1;
        bih.size             = bih.struct_size();
        bih.x_pels_per_meter = 0;
        bih.y_pels_per_meter = 0;
        bih.size_image       = (((bih.width * ImageT::bytes_per_pixel()) + 3) & 0x0000FFFC) * bih.height;

        bitmap_file_header bfh;
        bfh.type             = 19778;
        bfh.size             = bfh.struct_size() + bih.struct_size() + bih.size_image;
        bfh.reserved1        = 0;
        bfh.reserved2        = 0;
        bfh.off_bits         = bih.struct_size() + bfh.struct_size();

        write_bfh(stream,bfh);
        write_bih(stream,bih);

        unsigned padding = (4 - ((3 * image.width()) % 4)) % 4;
        char padding_data[4] = { 0x00, 0x00, 0x00, 0x00 };

        if (sizeof(typename ImageT::pixel_t) == sizeof(bgr_t) && begin(*image.row(0)) != blue(*image.row(0)) )
        {
            std::vector<bgr_t> line_buffer(image.width());
            for (unsigned i = 0; i < image.height(); ++i)
            {
                const typename ImageT::pixel_t * row_ptr = image.row(image.height() - i - 1);
                for (unsigned x = 0; x < image.width(); ++x)
                {
                    *blue(line_buffer[x]) = *blue(row_ptr[x]);
                    *green(line_buffer[x]) = *green(row_ptr[x]);
                    *red(line_buffer[x]) = *red(row_ptr[x]);
                }
                const unsigned char* data_ptr = begin(*line_buffer.data());
                stream.write(reinterpret_cast<const char*>(data_ptr), sizeof(unsigned char) * ImageT::bytes_per_pixel() * image.width());
                stream.write(padding_data,padding);
            }
        }
        else
        {
            for (unsigned i = 0; i < image.height(); ++i)
            {
                const typename ImageT::pixel_t * row_ptr = image.row(image.height() - i - 1);
                const unsigned char* data_ptr = begin(*row_ptr);
                stream.write(reinterpret_cast<const char*>(data_ptr), sizeof(unsigned char) * ImageT::bytes_per_pixel() * image.width());
                stream.write(padding_data,padding);
            }
        }

        stream.close();
        return true;
    }

private:

    struct bitmap_file_header
    {
        unsigned short type;
        unsigned int   size;
        unsigned short reserved1;
        unsigned short reserved2;
        unsigned int   off_bits;

        unsigned int struct_size() const
        {
            return sizeof(type     ) +
                   sizeof(size     ) +
                   sizeof(reserved1) +
                   sizeof(reserved2) +
                   sizeof(off_bits ) ;
        }

        void clear()
        {
            std::memset(this, 0x00, sizeof(bitmap_file_header));
        }
    };

    struct bitmap_information_header
    {
        unsigned int   size;
        unsigned int   width;
        unsigned int   height;
        unsigned short planes;
        unsigned short bit_count;
        unsigned int   compression;
        unsigned int   size_image;
        unsigned int   x_pels_per_meter;
        unsigned int   y_pels_per_meter;
        unsigned int   clr_used;
        unsigned int   clr_important;

        unsigned int struct_size() const
        {
            return sizeof(size            ) +
                   sizeof(width           ) +
                   sizeof(height          ) +
                   sizeof(planes          ) +
                   sizeof(bit_count       ) +
                   sizeof(compression     ) +
                   sizeof(size_image      ) +
                   sizeof(x_pels_per_meter) +
                   sizeof(y_pels_per_meter) +
                   sizeof(clr_used        ) +
                   sizeof(clr_important   ) ;
        }

        void clear()
        {
            std::memset(this, 0x00, sizeof(bitmap_information_header));
        }
    };

    template <typename T>
    static inline void read_from_stream(std::ifstream& stream,T& t)
    {
        stream.read(reinterpret_cast<char*>(&t),sizeof(T));
    }

    template <typename T>
    static inline void write_to_stream(std::ofstream& stream,const T& t)
    {
        stream.write(reinterpret_cast<const char*>(&t),sizeof(T));
    }

    static inline void read_bfh(std::ifstream& stream, bitmap_file_header& bfh)
    {
        read_from_stream(stream, bfh.type     );
        read_from_stream(stream, bfh.size     );
        read_from_stream(stream, bfh.reserved1);
        read_from_stream(stream, bfh.reserved2);
        read_from_stream(stream, bfh.off_bits );

        if (BitmapImageType::big_endian())
        {
            bfh.type      = ImageT::flip(bfh.type     );
            bfh.size      = ImageT::flip(bfh.size     );
            bfh.reserved1 = ImageT::flip(bfh.reserved1);
            bfh.reserved2 = ImageT::flip(bfh.reserved2);
            bfh.off_bits  = ImageT::flip(bfh.off_bits );
        }
    }

    static inline void write_bfh(std::ofstream& stream, const bitmap_file_header& bfh)
    {
        if (BitmapImageType::big_endian())
        {
            write_to_stream(stream, ImageT::flip(bfh.type     ));
            write_to_stream(stream, ImageT::flip(bfh.size     ));
            write_to_stream(stream, ImageT::flip(bfh.reserved1));
            write_to_stream(stream, ImageT::flip(bfh.reserved2));
            write_to_stream(stream, ImageT::flip(bfh.off_bits ));
        }
        else
        {
            write_to_stream(stream, bfh.type     );
            write_to_stream(stream, bfh.size     );
            write_to_stream(stream, bfh.reserved1);
            write_to_stream(stream, bfh.reserved2);
            write_to_stream(stream, bfh.off_bits );
        }
    }

    static inline void read_bih(std::ifstream& stream,bitmap_information_header& bih)
    {
        read_from_stream(stream, bih.size            );
        read_from_stream(stream, bih.width           );
        read_from_stream(stream, bih.height          );
        read_from_stream(stream, bih.planes          );
        read_from_stream(stream, bih.bit_count       );
        read_from_stream(stream, bih.compression     );
        read_from_stream(stream, bih.size_image      );
        read_from_stream(stream, bih.x_pels_per_meter);
        read_from_stream(stream, bih.y_pels_per_meter);
        read_from_stream(stream, bih.clr_used        );
        read_from_stream(stream, bih.clr_important   );

        if (BitmapImageType::big_endian())
        {
            bih.size          = ImageT::flip(bih.size               );
            bih.width         = ImageT::flip(bih.width              );
            bih.height        = ImageT::flip(bih.height             );
            bih.planes        = ImageT::flip(bih.planes             );
            bih.bit_count     = ImageT::flip(bih.bit_count          );
            bih.compression   = ImageT::flip(bih.compression        );
            bih.size_image    = ImageT::flip(bih.size_image         );
            bih.x_pels_per_meter = ImageT::flip(bih.x_pels_per_meter);
            bih.y_pels_per_meter = ImageT::flip(bih.y_pels_per_meter);
            bih.clr_used      = ImageT::flip(bih.clr_used           );
            bih.clr_important = ImageT::flip(bih.clr_important      );
        }
    }

    static inline void write_bih(std::ofstream& stream, const bitmap_information_header& bih)
    {
        if (BitmapImageType::big_endian())
        {
            write_to_stream(stream, ImageT::flip(bih.size            ));
            write_to_stream(stream, ImageT::flip(bih.width           ));
            write_to_stream(stream, ImageT::flip(bih.height          ));
            write_to_stream(stream, ImageT::flip(bih.planes          ));
            write_to_stream(stream, ImageT::flip(bih.bit_count       ));
            write_to_stream(stream, ImageT::flip(bih.compression     ));
            write_to_stream(stream, ImageT::flip(bih.size_image      ));
            write_to_stream(stream, ImageT::flip(bih.x_pels_per_meter));
            write_to_stream(stream, ImageT::flip(bih.y_pels_per_meter));
            write_to_stream(stream, ImageT::flip(bih.clr_used        ));
            write_to_stream(stream, ImageT::flip(bih.clr_important   ));
        }
        else
        {
            write_to_stream(stream, bih.size            );
            write_to_stream(stream, bih.width           );
            write_to_stream(stream, bih.height          );
            write_to_stream(stream, bih.planes          );
            write_to_stream(stream, bih.bit_count       );
            write_to_stream(stream, bih.compression     );
            write_to_stream(stream, bih.size_image      );
            write_to_stream(stream, bih.x_pels_per_meter);
            write_to_stream(stream, bih.y_pels_per_meter);
            write_to_stream(stream, bih.clr_used        );
            write_to_stream(stream, bih.clr_important   );
        }
    }

    static inline std::size_t file_size(const std::string& file_name)
    {
        std::ifstream file(file_name.c_str(),std::ios::in | std::ios::binary);
        if (!file) return 0;
        file.seekg (0, std::ios::end);
        return static_cast<std::size_t>(file.tellg());
    }

};

}
