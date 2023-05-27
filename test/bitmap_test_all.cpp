/*
 *****************************************************************************
 *                                                                           *
 *                          Platform Independent                             *
 *                     Bitmap Image Reader Writer Library                    *
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


// manually check with every single header, if include dependencies are met
#include <offscr_bmp_drw/bitmap_image_rgb.hpp>
#include <offscr_bmp_drw/response_image.hpp>
#include <offscr_bmp_drw/sobel.hpp>
// #include <offscr_bmp_drw/convert.hpp>
// #include <offscr_bmp_drw/misc.hpp>

#include <offscr_bmp_drw/colormaps.hpp>
#include <offscr_bmp_drw/bitmap_image_generic.hpp>
#include <offscr_bmp_drw/bitmap_image_file.hpp>
#include <offscr_bmp_drw/cartesian_canvas.hpp>
#include <offscr_bmp_drw/plasma.hpp>
#include <offscr_bmp_drw/checkered_pattern.hpp>

#include <vector>

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>


using namespace OffScreenBitmapDraw;

using pixel_t = bgr_t;
using BitmapImage = bitmap_image_rgb<pixel_t>;
using BitmapImageFile = image_io<BitmapImage>;
//using BitmapImageFile = bitmap_image_file<>;
//class bitmap_image_file


const std::string file_name("image.bmp");


void test01()
{
    BitmapImage image = BitmapImageFile::load(file_name);
    if (!image)
    {
        printf("test01() - Error - Failed to open '%s'\n",file_name.c_str());
        return;
    }
    BitmapImageFile::save(image, "test01_saved.bmp");
}

void test02()
{
    BitmapImage image = BitmapImageFile::load(file_name);
    if (!image)
    {
        printf("test02() - Error - Failed to open '%s'\n",file_name.c_str());
        return;
    }
    BitmapImageFile::save(image, "test02_saved.bmp");

    BitmapImageFile::save(image.vertical_flip(), "test02_saved_vert_flip.bmp");
    image.vertical_flip();  // flip back

    BitmapImageFile::save(image.horizontal_flip(), "test02_saved_horiz_flip.bmp");
}

void test03()
{
    BitmapImage img = BitmapImageFile::load(file_name);
    if (!img)
    {
        printf("test03() - Error - Failed to open '%s'\n",file_name.c_str());
        return;
    }

    BitmapImage sub1, sub2, sub3;
    BitmapImageFile::save(img.subsample_to(sub1), "test03_1xsubsampled_image.bmp");
    BitmapImageFile::save(sub1.subsample_to(sub2), "test03_2xsubsampled_image.bmp");
    BitmapImageFile::save(sub2.subsample_to(sub3), "test03_3xsubsampled_image.bmp");
}

void test04()
{
    BitmapImage img = BitmapImageFile::load(file_name);
    if (!img)
    {
        printf("test04() - Error - Failed to open '%s'\n",file_name.c_str());
        return;
    }

    BitmapImage up1, up2, up3;
    BitmapImageFile::save(img.upsample_to(up1), "test04_1xupsampled_image.bmp");
    BitmapImageFile::save(up1.upsample_to(up2), "test04_2xupsampled_image.bmp");
    BitmapImageFile::save(up2.upsample_to(up3), "test04_3xupsampled_image.bmp");
}

void test05()
{
    BitmapImage image = BitmapImageFile::load(file_name);
    if (!image)
    {
        printf("test05() - Error - Failed to open '%s'\n",file_name.c_str());
        return;
    }

    BitmapImageFile::save(image.set_all_ith_bits_low(0), "test05_lsb0_removed_saved.bmp");
    BitmapImageFile::save(image.set_all_ith_bits_low(1), "test05_lsb01_removed_saved.bmp");
    BitmapImageFile::save(image.set_all_ith_bits_low(2), "test05_lsb012_removed_saved.bmp");
    BitmapImageFile::save(image.set_all_ith_bits_low(3), "test05_lsb0123_removed_saved.bmp");
    BitmapImageFile::save(image.set_all_ith_bits_low(4), "test05_lsb01234_removed_saved.bmp");
    BitmapImageFile::save(image.set_all_ith_bits_low(5), "test05_lsb012345_removed_saved.bmp");
    BitmapImageFile::save(image.set_all_ith_bits_low(6), "test05_lsb0123456_removed_saved.bmp");
}

void test06()
{
    BitmapImage image = BitmapImageFile::load(file_name);
    if (!image)
    {
        printf("test06() - Error - Failed to open '%s'\n",file_name.c_str());
        return;
    }

    BitmapImage red_ch, green_ch, blue_ch;
    BitmapImageFile::save(image.export_color_plane(red_plane, red_ch),     "test06_red_channel_image.bmp");
    BitmapImageFile::save(image.export_color_plane(green_plane, green_ch), "test06_green_channel_image.bmp");
    BitmapImageFile::save(image.export_color_plane(blue_plane, blue_ch),   "test06_blue_channel_image.bmp");
}

void test07()
{
    BitmapImage image = BitmapImageFile::load(file_name);
    if (!image)
    {
        printf("test07() - Error - Failed to open '%s'\n",file_name.c_str());
        return;
    }
    BitmapImageFile::save(image.convert_to_grayscale(), "test07_grayscale_image.bmp");
}

void test08()
{
    BitmapImage image = BitmapImageFile::load(file_name);
    if (!image)
    {
        printf("test08() - Error - Failed to open '%s'\n",file_name.c_str());
        return;
    }

    BitmapImage image1, image2, image3, image4;
    const unsigned int w = image.width();
    const unsigned int h = image.height();

    if (!image.region_to(0,0, w / 2, h / 2, image1))
    {
        std::cout << "ERROR: upper_left_image" << std::endl;
    }

    if (!image.region_to((w - 1) / 2, 0, w / 2, h / 2, image2))
    {
        std::cout << "ERROR: upper_right_image" << std::endl;
    }

    if (!image.region_to(0,(h - 1) / 2, w / 2, h / 2, image3))
    {
        std::cout << "ERROR: lower_left_image" << std::endl;
    }

    if (!image.region_to((w - 1) / 2, (h - 1) / 2, w / 2, h / 2, image4))
    {
        std::cout << "ERROR: lower_right_image" << std::endl;
    }

    BitmapImageFile::save(image1, "test08_upper_left_image.bmp");
    BitmapImageFile::save(image2, "test08_upper_right_image.bmp");
    BitmapImageFile::save(image3, "test08_lower_left_image.bmp");
    BitmapImageFile::save(image4, "test08_lower_right_image.bmp");
}

void test09()
{
    const unsigned int dim = 1000;
    BitmapImage image(dim,dim);

    for (unsigned int x = 0; x < dim; ++x)
    {
        for (unsigned int y = 0; y < dim; ++y)
        {
            rgb_t col = jet_colormap[(x + y) % dim];
            image.set_pixel(x, y, col.red, col.green, col.blue);
        }
    }

    BitmapImageFile::save(image, "test09_color_map_image.bmp");
}

void test10()
{
    BitmapImage image = BitmapImageFile::load(file_name);
    if (!image)
    {
        printf("test10() - Error - Failed to open '%s'\n",file_name.c_str());
        return;
    }

    BitmapImageFile::save(image.invert_color_planes(), "test10_inverted_color_image.bmp");
}

void test11()
{
    BitmapImage image = BitmapImageFile::load(file_name);
    if (!image)
    {
        printf("test11() - Error - Failed to open '%s'\n",file_name.c_str());
        return;
    }

    for (unsigned int i = 0; i < 10; ++i)
    {
        image.add_to_color_plane(red_plane,10);
        BitmapImageFile::save(image, std::string("test11_") + static_cast<char>(48 + i) + std::string("_red_inc_image.bmp"));
    }
}

void test12()
{
    BitmapImage image = BitmapImageFile::load(file_name);
    if (!image)
    {
        printf("test12() - Error - Failed to open '%s'\n",file_name.c_str());
        return;
    }

    using Float = double;
    std::vector<Float> y( image.pixel_count_unpadded() );
    std::vector<Float> cb( image.pixel_count_unpadded() );
    std::vector<Float> cr( image.pixel_count_unpadded() );

    image.export_ycbcr( y.data(), cb.data(), cr.data() );

    for (unsigned int i = 0; i < image.pixel_count_unpadded(); ++i)
    {
        cb[i] = cr[i] = Float(0);
    }

    image.import_ycbcr( y.data(), cb.data(), cr.data() );
    BitmapImageFile::save(image, "test12_only_y_image.bmp");
}

void test13()
{
    BitmapImage image = BitmapImageFile::load(file_name);
    if (!image)
    {
        printf("test13() - Error - Failed to open '%s'\n",file_name.c_str());
        return;
    }

    using Float = double;
    std::vector<Float> y( image.pixel_count_unpadded() );
    std::vector<Float> cb( image.pixel_count_unpadded() );
    std::vector<Float> cr( image.pixel_count_unpadded() );

    image.export_ycbcr( y.data(), cb.data(), cr.data() );

    for (unsigned int j = 0; j < 10; ++j)
    {
        for (unsigned int i = 0; i < image.pixel_count_unpadded(); ++i)
        {
            y[i] += 15.0;
        }

        image.import_ycbcr( y.data(), cb.data(), cr.data() );
        BitmapImageFile::save(image, std::string("test13_") + static_cast<char>(48 + j) + std::string("_y_image.bmp"));
    }
}

void test14()
{
    BitmapImage image(512,512);

    image.clear();
    checkered_pattern<BitmapImage>(64,64,220,red_plane,image);
    BitmapImageFile::save(image, "test14_checkered_01.bmp");

    image.clear();
    checkered_pattern<BitmapImage>(32,64,100,200,50,image);
    BitmapImageFile::save(image, "test14_checkered_02.bmp");
}

void test15()
{
    BitmapImage image(1024,1024);
    image.clear();

    using Float = double;
    Float c1 = Float(0.9);
    Float c2 = Float(0.5);
    Float c3 = Float(0.3);
    Float c4 = Float(0.7);

    ::srand(0xA5AA5AA5);
    plasma<BitmapImage, Float>(
        image,
        0, 0, image.width(), image.height(),
        c1, c2, c3, c4,
        Float(3), jet_colormap
        );
    BitmapImageFile::save(image, "test15_plasma.bmp");
}

void test16()
{
    BitmapImage img = BitmapImageFile::load(file_name);
    if (!img)
    {
        printf("test16() - Error - Failed to open '%s'\n",file_name.c_str());
        return;
    }

    using Float = double;
    Float c1 = Float(0.9);
    Float c2 = Float(0.5);
    Float c3 = Float(0.3);
    Float c4 = Float(0.7);

    BitmapImage plasma_img(img.width(),img.height());
    plasma<BitmapImage, Float>(
        plasma_img,
        0, 0, plasma_img.width(), plasma_img.height(),
        c1, c2, c3, c4,
        Float(3), jet_colormap
        );
    BitmapImageFile::save(plasma_img, "test16_alpha_0.0.bmp");

    BitmapImage temp(img);
    BitmapImageFile::save(temp, "test16_alpha_tmp.bmp");
    BitmapImageFile::save(temp.alpha_blend(0.1, plasma_img), "test16_alpha_0.1.bmp"); temp = img;
    BitmapImageFile::save(temp.alpha_blend(0.2, plasma_img), "test16_alpha_0.2.bmp"); temp = img;
    BitmapImageFile::save(temp.alpha_blend(0.3, plasma_img), "test16_alpha_0.3.bmp"); temp = img;
    BitmapImageFile::save(temp.alpha_blend(0.4, plasma_img), "test16_alpha_0.4.bmp"); temp = img;
    BitmapImageFile::save(temp.alpha_blend(0.5, plasma_img), "test16_alpha_0.5.bmp"); temp = img;
    BitmapImageFile::save(temp.alpha_blend(0.6, plasma_img), "test16_alpha_0.6.bmp"); temp = img;
    BitmapImageFile::save(temp.alpha_blend(0.7, plasma_img), "test16_alpha_0.7.bmp"); temp = img;
    BitmapImageFile::save(temp.alpha_blend(0.8, plasma_img), "test16_alpha_0.8.bmp"); temp = img;
    BitmapImageFile::save(temp.alpha_blend(0.9, plasma_img), "test16_alpha_0.9.bmp");
}

void test17()
{
    BitmapImage image(1024,1024);
    image.clear({20, 20, 20});

    image_drawer<BitmapImage> draw(image);

    draw.pen_width(3);
    draw.pen_color({255,0,0});  // red
    draw.circle(image.width() / 2 + 100, image.height() / 2, 100);

    draw.pen_width(2);
    draw.pen_color({0,255,255});
    draw.ellipse(image.width() / 2, image.height() / 2, 200,350);

    draw.pen_width(1);
    image_drawer<BitmapImage>::pixel_t c = {255, 255, 0};  // yellow
    draw.pen_color(c);
    draw.rectangle(50,50,250,400);

    draw.pen_color({0,255,0});  // green
    draw.rectangle(450,250,850,880);

    BitmapImageFile::save(image, "test17_image_drawer.bmp");
}

void test18()
{
    constexpr int h_per_color = 50;
    {
        BitmapImage image(1000, 9 * h_per_color);
        image_drawer<BitmapImage> draw(image);
        const rgb_t* colormap[9] = {
                                   autumn_colormap,
                                   copper_colormap,
                                     gray_colormap,
                                      hot_colormap,
                                      hsv_colormap,
                                      jet_colormap,
                                    prism_colormap,
                                      vga_colormap,
                                     yarg_colormap
                                 };

        for (unsigned int i = 0; i < image.width(); ++i)
        {
            for (unsigned int j = 0; j < 9; ++j)
            {
                draw.pen_color( colormap[j][i] );
                draw.vertical_line_segment(j * h_per_color, (j + 1) * h_per_color, i);
            }
        }

        BitmapImageFile::save(image, "test18_color_maps.bmp");
    }

    {
        BitmapImage image(1000,500);
        image_drawer<BitmapImage> draw(image);

        std::size_t palette_colormap_size = sizeof(palette_colormap) / sizeof(palette_colormap[0]);
        std::size_t bar_width = image.width() / palette_colormap_size;

        for (std::size_t i = 0; i < palette_colormap_size; ++i)
        {
            for (std::size_t j = 0; j < bar_width; ++j)
            {
                draw.pen_color( palette_colormap[i] );
                draw.vertical_line_segment(0, image.height(), static_cast<int>(i * bar_width + j));
            }
        }

        BitmapImageFile::save(image, "test18_palette_colormap.bmp");
    }
}

void test19()
{
    using Float = float;
    static const Float pi = Float(3.14159265358979323846264338327950288419716939937510);
    {
        cartesian_canvas<BitmapImage, Float> canvas(1000, 1000);
        if (!canvas)
        {
            printf("test19() - Error - Failed to instantiate cartesian canvas(1000x1000) [1]\n");
            return;
        }
        canvas.rectangle(canvas.min_x(), canvas.min_y(), canvas.max_x(), canvas.max_y());
        canvas.horiztonal_line_segment(canvas.min_x(), canvas.max_x(), -400);
        canvas.line_segment(-500, 600, 600, -500);
        canvas.pen_width(3);
        for (std::size_t i = 0; i < 160; i++)
        {
            std::size_t c_idx = i % (sizeof(palette_colormap) / sizeof(palette_colormap[0]));
            canvas.pen_color( palette_colormap[c_idx] );
            canvas.circle(0, 0, 3 * i);
        }
        BitmapImageFile::save(canvas.image(), "test19_cartesian_canvas01.bmp");
    }

    {
        cartesian_canvas<BitmapImage, Float> canvas(1000, 1000);
        if (!canvas)
        {
            printf("test19() - Error - Failed to instantiate cartesian canvas(1000x1000) [2]\n");
            return;
        }
        canvas.image().set_all_channels(0xFF);
        canvas.pen_width(2);
        unsigned int i = 0;
        for (int x = -500; x < 500; x += 3, ++i)
        {
            std::size_t c_idx = i % (sizeof(palette_colormap) / sizeof(palette_colormap[0]));
            canvas.pen_color( palette_colormap[c_idx] );
            Float radius = std::max(Float(10), std::abs(Float(80) * std::sin((Float(1) / Float(80)) * pi * x)));
            Float y = 400.0 * std::sin((Float(1) / Float(200)) * pi * x);
            canvas.circle(x, y, radius);
        }
        BitmapImageFile::save(canvas.image(), "test19_cartesian_canvas02.bmp");
    }
}

void test20()
{
    const rgb_t* colormap[4] = {
                                   hsv_colormap,
                                   jet_colormap,
                                 prism_colormap,
                                   vga_colormap
    };

    const unsigned int fractal_width  = 1200;
    const unsigned int fractal_height =  800;

    {
        BitmapImage fractal_hsv  (fractal_width,fractal_height);
        BitmapImage fractal_jet  (fractal_width,fractal_height);
        BitmapImage fractal_prism(fractal_width,fractal_height);
        BitmapImage fractal_vga  (fractal_width,fractal_height);

        fractal_hsv  .clear();
        fractal_jet  .clear();
        fractal_prism.clear();
        fractal_vga  .clear();

        double    cr,    ci;
        double nextr, nexti;
        double prevr, previ;

        const unsigned int max_iterations = 1000;

        for (unsigned int y = 0; y < fractal_height; ++y)
        {
            for (unsigned int x = 0; x < fractal_width; ++x)
            {
                cr = 1.5 * (2.0 * x / fractal_width  - 1.0) - 0.5;
                ci =       (2.0 * y / fractal_height - 1.0);
                nextr = nexti = 0;
                prevr = previ = 0;

                for (unsigned int i = 0; i < max_iterations; i++)
                {
                    prevr = nextr;
                    previ = nexti;
                    nextr =     prevr * prevr - previ * previ + cr;
                    nexti = 2 * prevr * previ + ci;

                    if (((nextr * nextr) + (nexti * nexti)) > 4)
                    {
                        if (max_iterations != i)
                        {
                            double z = sqrt(nextr * nextr + nexti * nexti);
                            #define log2(x) (std::log(1.0 * x) / std::log(2.0))
                            unsigned int index = static_cast<unsigned int>(
                                1000.0 * log2(1.75 + i - log2(log2(z))) / log2(max_iterations));
                            #undef log2
                            rgb_t c0 = colormap[0][index];
                            rgb_t c1 = colormap[1][index];
                            rgb_t c2 = colormap[2][index];
                            rgb_t c3 = colormap[3][index];
                            fractal_hsv  .set_pixel(x, y, c0.red, c0.green, c0.blue);
                            fractal_jet  .set_pixel(x, y, c1.red, c1.green, c1.blue);
                            fractal_prism.set_pixel(x, y, c2.red, c2.green, c2.blue);
                            fractal_vga  .set_pixel(x, y, c3.red, c3.green, c3.blue);
                        }
                        break;
                    }
                }
            }
        }

        BitmapImageFile::save(fractal_hsv  , "test20_mandelbrot_set_hsv.bmp"  );
        BitmapImageFile::save(fractal_jet  , "test20_mandelbrot_set_jet.bmp"  );
        BitmapImageFile::save(fractal_prism, "test20_mandelbrot_set_prism.bmp");
        BitmapImageFile::save(fractal_vga  , "test20_mandelbrot_set_vga.bmp"  );
    }

    {
        BitmapImage fractal_hsv  (fractal_width,fractal_height);
        BitmapImage fractal_jet  (fractal_width,fractal_height);
        BitmapImage fractal_prism(fractal_width,fractal_height);
        BitmapImage fractal_vga  (fractal_width,fractal_height);

        fractal_hsv  .clear();
        fractal_jet  .clear();
        fractal_prism.clear();
        fractal_vga  .clear();

        const unsigned int max_iterations = 300;

        const double cr = -0.70000;
        const double ci =  0.27015;

        double prevr, previ;

        for (unsigned int y = 0; y < fractal_height; ++y)
        {
            for (unsigned int x = 0; x < fractal_width; ++x)
            {
                double nextr = 1.5 * (2.0 * x / fractal_width  - 1.0);
                double nexti =       (2.0 * y / fractal_height - 1.0);

                for (unsigned int i = 0; i < max_iterations; i++)
                {
                    prevr = nextr;
                    previ = nexti;
                    nextr =     prevr * prevr - previ * previ + cr;
                    nexti = 2 * prevr * previ + ci;

                    if (((nextr * nextr) + (nexti * nexti)) > 4)
                    {
                        if (max_iterations != i)
                        {
                            unsigned int index = static_cast<int>((1000.0 * i) / max_iterations);
                            rgb_t c0 = colormap[0][index];
                            rgb_t c1 = colormap[1][index];
                            rgb_t c2 = colormap[2][index];
                            rgb_t c3 = colormap[3][index];
                            fractal_hsv  .set_pixel(x, y, c0.red, c0.green, c0.blue);
                            fractal_jet  .set_pixel(x, y, c1.red, c1.green, c1.blue);
                            fractal_prism.set_pixel(x, y, c2.red, c2.green, c2.blue);
                            fractal_vga  .set_pixel(x, y, c3.red, c3.green, c3.blue);
                        }
                        break;
                    }
                }
            }
        }

        BitmapImageFile::save(fractal_hsv  , "test20_julia_set_hsv.bmp"  );
        BitmapImageFile::save(fractal_jet  , "test20_julia_set_jet.bmp"  );
        BitmapImageFile::save(fractal_prism, "test20_julia_set_prism.bmp");
        BitmapImageFile::save(fractal_vga  , "test20_julia_set_vga.bmp"  );
    }
}

void test21()
{
    typedef bitmap_image_generic<float, float> bitmap_image_float;
    const std::size_t palette_colormap_size = sizeof(palette_colormap) / sizeof(palette_colormap[0]);
    const float col_idx_mul = 1.0F / palette_colormap_size;

    {
        cartesian_canvas<bitmap_image_float> canvas(1000, 1000);
        if (!canvas)
        {
            printf("test21() - Error - Failed to instantiate cartesian canvas(1000x1000) [1]\n");
            return;
        }

        canvas.image().clear(0.0F);
        canvas.rectangle(canvas.min_x(), canvas.min_y(), canvas.max_x(), canvas.max_y());
        canvas.horiztonal_line_segment(canvas.min_x(), canvas.max_x(), -400.0);
        canvas.line_segment(-500.0, 600.0, 600.0, -500.0);
        canvas.pen_width(3);

        for (std::size_t i = 0; i < 160; i++)
        {
            std::size_t c_idx = i % palette_colormap_size;
            canvas.pen_color( c_idx * col_idx_mul );
            canvas.circle(0.0, 0.0, 3.0 * i);
        }
        //canvas.image().save_image("test21_cartesian_canvas01.bmp");
    }

    {
        static const double pi = 3.14159265358979323846264338327950288419716939937510;

        cartesian_canvas<bitmap_image_float> canvas(1000, 1000);
        if (!canvas)
        {
            printf("test21() - Error - Failed to instantiate cartesian canvas(1000x1000) [2]\n");
            return;
        }

        canvas.image().clear(0.0F);
        canvas.pen_width(2);

        unsigned int i = 0;

        for (double x = -500; x < 500; x += 3, ++i)
        {
            std::size_t c_idx = i % palette_colormap_size;
            canvas.pen_color( c_idx * palette_colormap_size );
            double radius = std::max(10.0,std::abs(80.0 * std::sin((1.0 / 80.0) * pi * x)));
            double y = 400.0 * std::sin((1.0 / 200.0) * pi * x);
            canvas.circle(x, y, radius);
        }
        //canvas.image().save_image("test21_cartesian_canvas02.bmp");
    }
}

void test22()
{
    using Float = double;
    BitmapImage sobel_img;
    BitmapImage image = BitmapImageFile::load(file_name);
    if (!image)
    {
        printf("test22() - Error - Failed to open '%s'\n",file_name.c_str());
        return;
    }
    sobel_operator(image, sobel_img, Float(0), Float(8));
    BitmapImageFile::save(sobel_img, "test22_image_sobel.bmp");
}


int main()
{
    test01();
    test02();
    test03();
    test04();
    test05();
    test06();
    test07();
    test08();
    test09();
    test10();
    test11();
    test12();
    test13();
    test14();
    test15();
    test16();
    test17();
    test18();
    test19();
    test20();
    test21();
    test22();

    rgba_t colortab_unpacked[10];
    rgb_t colortab_packed[10];

    printf("sizeof(rgba_t) = %u bytes\n", unsigned(sizeof(rgb_t)) );
    printf("sizeof(rgb_t) = %u bytes\n", unsigned(sizeof(rgb_t)) );
    printf("sizeof(10 * rgb_t) = %u bytes\n", unsigned(sizeof(colortab_packed)) );
    printf("sizeof(10 * rgba_t) = %u bytes\n", unsigned(sizeof(colortab_unpacked)) );

    return 0;
}


/*
   Note: In some of the tests a bitmap image by the name of 'image.bmp'
         is required. If not present the test will fail.
*/
