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
#include <offscr_bmp_drw/convert.hpp>
// #include <offscr_bmp_drw/misc.hpp>

#include <offscr_bmp_drw/colormaps.hpp>
#include <offscr_bmp_drw/bitmap_image_generic.hpp>
#include <offscr_bmp_drw/bitmap_image_file.hpp>
#include <offscr_bmp_drw/cartesian_canvas.hpp>
#include <offscr_bmp_drw/plasma.hpp>
#include <offscr_bmp_drw/checkered_pattern.hpp>
#include <offscr_bmp_drw/zingl_image_drawer.hpp>

#include <vector>

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <cassert>


using namespace OffScreenBitmapDraw;

using rgb_pixel_t = bgr_t;
using BitmapRGBImage = bitmap_image_rgb<rgb_pixel_t>;
using BitmapFloatImage = bitmap_image_rgb<float>;
using BitmapRGBImageFile = image_io<BitmapRGBImage>;
using RGBDrawer = zingl_image_drawer<BitmapRGBImage>;
using FloatDrawer = zingl_image_drawer<BitmapFloatImage>;


const std::string file_name("image.bmp");

static const double pi_ = 3.14159265358979323846264338327950288419716939937510;

static rgb_t jet_like_cmap[1000];   // generate in test18(), utilized also in test23()
static bool generated_jet_like_cmap = false;

static void generate_jet_like_colormap()
{
    if (generated_jet_like_cmap)
        return;
    constexpr int num_colors = 6;
    int nc[num_colors - 1] = { 40, 60, 200, 300, 400 };
    rgb_t c[num_colors];
    set_rgb(c[0], 0, 0, 0);     // black
    set_rgb(c[1], 10, 10, 128); // blue
    set_rgb(c[2], 10, 50, 192); // blue -> green
    set_rgb(c[3], 50, 208, 50); // green
    set_rgb(c[4], 240, 240, 0); // yellow
    set_rgb(c[5], 255, 0, 0);   // red
    int sz = interpolate_color_counts(num_colors, nc, c, jet_like_cmap, 1000);
    assert( sz == 1000 );
    generated_jet_like_cmap = true;
}

static BitmapRGBImage& convert_(const BitmapFloatImage& float_image, BitmapRGBImage& rgb_image)
{
    generate_jet_like_colormap();
    convert_float_to_rgb<rgb_t, BitmapFloatImage, BitmapRGBImage>(
        float_image, rgb_image,
        jet_like_cmap, // jet_colormap or yarg_colormap
        1000,
        0.75F, false    // 0.75 makes red more often: sort of clipping
        );
    return rgb_image;
}


void test00()
{
    // PixelType could be rgb_t, bgr_t, rgba_t or abgr_t from colors.hpp
    bitmap_image_generic<rgb_t>    internal_test_rgb;
    bitmap_image_generic<bgr_t>    internal_test_bgr;
    bitmap_image_generic<rgba_t>   internal_test_rgba;
    bitmap_image_generic<abgr_t>   internal_test_abgr;
    bitmap_image_generic<float, float>   internal_test_float;
    (void)internal_test_rgb;
    (void)internal_test_bgr;
    (void)internal_test_rgba;
    (void)internal_test_abgr;
    (void)internal_test_float;
}

bool test01()
{
    BitmapRGBImage image = BitmapRGBImageFile::load(file_name);
    if (!image)
    {
        fprintf(stderr, "test01() - Error - Failed to open '%s'\n",file_name.c_str());
        return false;
    }
    BitmapRGBImageFile::save(image, "test01_saved.bmp");
    return true;
}

void test02()
{
    BitmapRGBImage image = BitmapRGBImageFile::load(file_name);
    if (!image)
    {
        fprintf(stderr, "test02() - Error - Failed to open '%s'\n",file_name.c_str());
        return;
    }
    BitmapRGBImageFile::save(image.vertical_flip(), "test02_saved_vert_flip.bmp");
    BitmapRGBImageFile::save(image.vertical_flip().horizontal_flip(), "test02_saved_horiz_flip.bmp");
}

void test03()
{
    const BitmapRGBImage img = BitmapRGBImageFile::load(file_name);
    if (!img)
    {
        fprintf(stderr, "test03() - Error - Failed to open '%s'\n",file_name.c_str());
        return;
    }
    BitmapRGBImage sub1, sub2, sub3;
    BitmapRGBImageFile::save(img.subsample_to(sub1), "test03_1xsubsampled_image.bmp");
    BitmapRGBImageFile::save(sub1.subsample_to(sub2), "test03_2xsubsampled_image.bmp");
    BitmapRGBImageFile::save(sub2.subsample_to(sub3), "test03_3xsubsampled_image.bmp");
}

void test04()
{
    const BitmapRGBImage img = BitmapRGBImageFile::load(file_name);
    if (!img)
    {
        fprintf(stderr, "test04() - Error - Failed to open '%s'\n",file_name.c_str());
        return;
    }
    BitmapRGBImage up1, up2, up3;
    BitmapRGBImageFile::save(img.upsample_to(up1), "test04_1xupsampled_image.bmp");
    BitmapRGBImageFile::save(up1.upsample_to(up2), "test04_2xupsampled_image.bmp");
    BitmapRGBImageFile::save(up2.upsample_to(up3), "test04_3xupsampled_image.bmp");
}

void test05()
{
    BitmapRGBImage image = BitmapRGBImageFile::load(file_name);
    if (!image)
    {
        fprintf(stderr, "test05() - Error - Failed to open '%s'\n",file_name.c_str());
        return;
    }
    BitmapRGBImageFile::save(image.set_all_ith_bits_low(0), "test05_lsb0_removed_saved.bmp");
    BitmapRGBImageFile::save(image.set_all_ith_bits_low(1), "test05_lsb01_removed_saved.bmp");
    BitmapRGBImageFile::save(image.set_all_ith_bits_low(2), "test05_lsb012_removed_saved.bmp");
    BitmapRGBImageFile::save(image.set_all_ith_bits_low(3), "test05_lsb0123_removed_saved.bmp");
    BitmapRGBImageFile::save(image.set_all_ith_bits_low(4), "test05_lsb01234_removed_saved.bmp");
    BitmapRGBImageFile::save(image.set_all_ith_bits_low(5), "test05_lsb012345_removed_saved.bmp");
    BitmapRGBImageFile::save(image.set_all_ith_bits_low(6), "test05_lsb0123456_removed_saved.bmp");
}

void test06()
{
    const BitmapRGBImage image = BitmapRGBImageFile::load(file_name);
    if (!image)
    {
        fprintf(stderr, "test06() - Error - Failed to open '%s'\n",file_name.c_str());
        return;
    }
    BitmapRGBImage red_ch, green_ch, blue_ch;
    BitmapRGBImageFile::save(image.export_color_plane(red_plane, red_ch),     "test06_red_channel_image.bmp");
    BitmapRGBImageFile::save(image.export_color_plane(green_plane, green_ch), "test06_green_channel_image.bmp");
    BitmapRGBImageFile::save(image.export_color_plane(blue_plane, blue_ch),   "test06_blue_channel_image.bmp");
}

void test07()
{
    BitmapRGBImage image = BitmapRGBImageFile::load(file_name);
    if (!image)
    {
        fprintf(stderr, "test07() - Error - Failed to open '%s'\n",file_name.c_str());
        return;
    }
    BitmapRGBImageFile::save(image.convert_to_grayscale(), "test07_grayscale_image.bmp");
}

void test08()
{
    const BitmapRGBImage image = BitmapRGBImageFile::load(file_name);
    if (!image)
    {
        fprintf(stderr, "test08() - Error - Failed to open '%s'\n",file_name.c_str());
        return;
    }
    BitmapRGBImage image1, image2, image3, image4;
    const unsigned int w = image.width();
    const unsigned int h = image.height();
    if (!image.copy_region_to(0, 0, w /2, h /2, image1))
        fprintf(stderr, "test08(): ERROR: upper_left_image\n");
    if (!image.copy_region_to((w - 1) /2, 0, w /2, h /2, image2))
        fprintf(stderr, "test08(): ERROR: upper_right_image\n");
    if (!image.copy_region_to(0, (h - 1) /2, w /2, h /2, image3))
        fprintf(stderr, "test08(): ERROR: lower_left_image\n");
    if (!image.copy_region_to((w - 1) /2, (h - 1) /2, w /2, h /2, image4))
        fprintf(stderr, "test08(): ERROR: lower_right_image\n");
    BitmapRGBImageFile::save(image1, "test08_upper_left_image.bmp");
    BitmapRGBImageFile::save(image2, "test08_upper_right_image.bmp");
    BitmapRGBImageFile::save(image3, "test08_lower_left_image.bmp");
    BitmapRGBImageFile::save(image4, "test08_lower_right_image.bmp");
}

void test09()
{
    const unsigned int dim = 1000;
    BitmapRGBImage image(dim, dim);
    for (unsigned x = 0; x < dim; ++x)
    {
        for (unsigned int y = 0; y < dim; ++y)
        {
            rgb_t col = jet_colormap[(x + y) % dim];
            image.set_pixel(x, y, col.red, col.green, col.blue);
        }
    }
    BitmapRGBImageFile::save(image, "test09_color_map_image.bmp");
}

void test10()
{
    BitmapRGBImage image = BitmapRGBImageFile::load(file_name);
    if (!image)
    {
        fprintf(stderr, "test10() - Error - Failed to open '%s'\n",file_name.c_str());
        return;
    }
    BitmapRGBImageFile::save(image.invert_color_planes(), "test10_inverted_color_image.bmp");
}

void test11()
{
    BitmapRGBImage image = BitmapRGBImageFile::load(file_name);
    if (!image)
    {
        fprintf(stderr, "test11() - Error - Failed to open '%s'\n",file_name.c_str());
        return;
    }
    for (unsigned int i = 0; i < 10; ++i)
    {
        image.add_to_color_plane(red_plane, 10);
        BitmapRGBImageFile::save(image, std::string("test11_") + static_cast<char>(48 + i) + std::string("_red_inc_image.bmp"));
    }
}

void test12()
{
    using Float = double;
    std::vector<Float> y, cb, cr;
    unsigned w, h, N;
    {
        const BitmapRGBImage image = BitmapRGBImageFile::load(file_name);
        if (!image)
        {
            fprintf(stderr, "test12() - Error - Failed to open '%s'\n",file_name.c_str());
            return;
        }
        w = image.width();
        h = image.height();
        N = image.pixel_count_unpadded();
        y.resize( N );
        cb.resize( N );
        cr.resize( N );
        image.export_ycbcr( y.data(), cb.data(), cr.data() );
    }
    for (unsigned i = 0; i < N; ++i)
        cb[i] = cr[i] = Float(0);
    BitmapRGBImage import_img(w, h);
    import_img.import_ycbcr( y.data(), cb.data(), cr.data() );
    BitmapRGBImageFile::save(import_img, "test12_only_y_image.bmp");
}

void test13()
{
    using Float = float;
    std::vector<Float> y, cb, cr;
    unsigned w, h, N;
    {
        const BitmapRGBImage image = BitmapRGBImageFile::load(file_name);
        if (!image)
        {
            fprintf(stderr, "test13() - Error - Failed to open '%s'\n",file_name.c_str());
            return;
        }
        w = image.width();
        h = image.height();
        N = image.pixel_count_unpadded();
        y.resize( N );
        cb.resize( N );
        cr.resize( N );
        image.export_ycbcr( y.data(), cb.data(), cr.data() );
    }
    BitmapRGBImage import_img(w, h);
    for (unsigned j = 0; j < 10; ++j)
    {
        for (unsigned i = 0; i < N; ++i)
            y[i] += 15.0F;
        import_img.import_ycbcr( y.data(), cb.data(), cr.data() );
        BitmapRGBImageFile::save(import_img, std::string("test13_") + static_cast<char>(48 + j) + std::string("_y_image.bmp"));
    }
}

void test14()
{
    BitmapRGBImage image(512, 512);
    image.clear();
    checkered_pattern<BitmapRGBImage>(64, 64, 220, red_plane, image);
    BitmapRGBImageFile::save(image, "test14_checkered_01.bmp");

    image.clear();
    rgb_pixel_t color;
    set_rgb(color, 100, 200, 50);
    checkered_pattern<BitmapRGBImage>(32, 64, color, image);
    BitmapRGBImageFile::save(image, "test14_checkered_02.bmp");
}

void test15(bool save_as_input = false)
{
    BitmapRGBImage image(1024,1024);
    image.clear();

    using Float = double;
    Float c1 = Float(0.9);
    Float c2 = Float(0.5);
    Float c3 = Float(0.3);
    Float c4 = Float(0.7);

    ::srand(0xA5AA5AA5);
    plasma<BitmapRGBImage, Float>(
        image,
        0, 0, image.width(), image.height(),
        c1, c2, c3, c4,
        Float(3), jet_colormap
        );
    BitmapRGBImageFile::save(image, save_as_input ? file_name : "test15_plasma.bmp");
}

void test16()
{
    BitmapRGBImage img = BitmapRGBImageFile::load(file_name);
    if (!img)
    {
        fprintf(stderr, "test16() - Error - Failed to open '%s'\n",file_name.c_str());
        return;
    }

    using Float = double;
    Float c1 = Float(0.9);
    Float c2 = Float(0.5);
    Float c3 = Float(0.3);
    Float c4 = Float(0.7);

    BitmapRGBImage plasma_img(img.width(),img.height());
    plasma<BitmapRGBImage, Float>(
        plasma_img,
        0, 0, plasma_img.width(), plasma_img.height(),
        c1, c2, c3, c4,
        Float(3), jet_colormap
        );
    BitmapRGBImageFile::save(plasma_img, "test16_alpha_0.0.bmp");

    BitmapRGBImage temp(img);
    BitmapRGBImageFile::save(temp, "test16_alpha_tmp.bmp");
    BitmapRGBImageFile::save(temp.alpha_blend(0.1, plasma_img), "test16_alpha_0.1.bmp"); temp = img;
    BitmapRGBImageFile::save(temp.alpha_blend(0.2, plasma_img), "test16_alpha_0.2.bmp"); temp = img;
    BitmapRGBImageFile::save(temp.alpha_blend(0.3, plasma_img), "test16_alpha_0.3.bmp"); temp = img;
    BitmapRGBImageFile::save(temp.alpha_blend(0.4, plasma_img), "test16_alpha_0.4.bmp"); temp = img;
    BitmapRGBImageFile::save(temp.alpha_blend(0.5, plasma_img), "test16_alpha_0.5.bmp"); temp = img;
    BitmapRGBImageFile::save(temp.alpha_blend(0.6, plasma_img), "test16_alpha_0.6.bmp"); temp = img;
    BitmapRGBImageFile::save(temp.alpha_blend(0.7, plasma_img), "test16_alpha_0.7.bmp"); temp = img;
    BitmapRGBImageFile::save(temp.alpha_blend(0.8, plasma_img), "test16_alpha_0.8.bmp"); temp = img;
    BitmapRGBImageFile::save(temp.alpha_blend(0.9, plasma_img), "test16_alpha_0.9.bmp");
}

void test17()
{
    BitmapRGBImage image(1024, 1024);
    image.clear({20, 20, 20});

    image_drawer<BitmapRGBImage> draw(image);

    draw.pen_width(3);
    draw.pen_color({255,0,0});  // red
    draw.circle(image.width() / 2 + 100, image.height() / 2, 100);

    draw.pen_width(2);
    draw.pen_color({0,255,255});
    draw.ellipse(image.width() / 2, image.height() / 2, 200,350);

    draw.pen_width(1);
    image_drawer<BitmapRGBImage>::pixel_t c = {255, 255, 0};  // yellow
    draw.pen_color(c);
    draw.rectangle(50,50,250,400);

    draw.pen_color({0,255,0});  // green
    draw.rectangle(450,250,850,880);

    BitmapRGBImageFile::save(image, "test17_image_drawer.bmp");
}

void test18()
{
    generate_jet_like_colormap();

    {
        const rgb_t* colormap[] = {
                                   autumn_colormap,
                                   copper_colormap,
                                     gray_colormap,
                                      hot_colormap,
                                      hsv_colormap,
                                    prism_colormap,
                                      vga_colormap,
                                     yarg_colormap,
                                      jet_colormap,
                                     jet_like_cmap,
                                 };
        constexpr int h_per_color = 50;
        constexpr int n_colormaps = sizeof(colormap) / sizeof(colormap[0]);
        BitmapRGBImage image(1000, n_colormaps * h_per_color);
        image_drawer<BitmapRGBImage> draw(image);

        for (unsigned x = 0; x < image.width(); ++x)
        {
            for (unsigned int j = 0; j < n_colormaps; ++j)
            {
                draw.pen_color( colormap[j][x] );
                draw.vertical_line_segment(x, j * h_per_color, (j + 1) * h_per_color);
            }
        }

        BitmapRGBImageFile::save(image, "test18_color_maps.bmp");
    }

    {
        BitmapRGBImage image(1000, 500);
        image_drawer<BitmapRGBImage> draw(image);

        std::size_t palette_colormap_size = sizeof(palette_colormap) / sizeof(palette_colormap[0]);
        std::size_t bar_width = image.width() / palette_colormap_size;

        for (std::size_t i = 0; i < palette_colormap_size; ++i)
        {
            for (std::size_t j = 0; j < bar_width; ++j)
            {
                draw.pen_color( palette_colormap[i] );
                draw.vertical_line_segment(static_cast<int>(i * bar_width + j), 0, image.height());
            }
        }

        BitmapRGBImageFile::save(image, "test18_palette_colormap.bmp");
    }
}

void test19()
{
    using Float = float;
    static const Float pi = Float(pi_);
    {
        cartesian_canvas<BitmapRGBImage, Float> canvas(1000, 1000);
        if (!canvas)
        {
            fprintf(stderr, "test19() - Error - Failed to instantiate cartesian canvas(1000x1000) [1]\n");
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
        BitmapRGBImageFile::save(canvas.image(), "test19_cartesian_canvas01.bmp");
    }

    {
        cartesian_canvas<BitmapRGBImage, Float> canvas(1000, 1000);
        if (!canvas)
        {
            fprintf(stderr, "test19() - Error - Failed to instantiate cartesian canvas(1000x1000) [2]\n");
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
        BitmapRGBImageFile::save(canvas.image(), "test19_cartesian_canvas02.bmp");
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
        BitmapRGBImage fractal_hsv  (fractal_width,fractal_height);
        BitmapRGBImage fractal_jet  (fractal_width,fractal_height);
        BitmapRGBImage fractal_prism(fractal_width,fractal_height);
        BitmapRGBImage fractal_vga  (fractal_width,fractal_height);

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

        BitmapRGBImageFile::save(fractal_hsv  , "test20_mandelbrot_set_hsv.bmp"  );
        BitmapRGBImageFile::save(fractal_jet  , "test20_mandelbrot_set_jet.bmp"  );
        BitmapRGBImageFile::save(fractal_prism, "test20_mandelbrot_set_prism.bmp");
        BitmapRGBImageFile::save(fractal_vga  , "test20_mandelbrot_set_vga.bmp"  );
    }

    {
        BitmapRGBImage fractal_hsv  (fractal_width,fractal_height);
        BitmapRGBImage fractal_jet  (fractal_width,fractal_height);
        BitmapRGBImage fractal_prism(fractal_width,fractal_height);
        BitmapRGBImage fractal_vga  (fractal_width,fractal_height);

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

        BitmapRGBImageFile::save(fractal_hsv  , "test20_julia_set_hsv.bmp"  );
        BitmapRGBImageFile::save(fractal_jet  , "test20_julia_set_jet.bmp"  );
        BitmapRGBImageFile::save(fractal_prism, "test20_julia_set_prism.bmp");
        BitmapRGBImageFile::save(fractal_vga  , "test20_julia_set_vga.bmp"  );
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
            fprintf(stderr, "test21() - Error - Failed to instantiate cartesian canvas(1000x1000) [1]\n");
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
        static const double pi = pi_;

        cartesian_canvas<bitmap_image_float> canvas(1000, 1000);
        if (!canvas)
        {
            fprintf(stderr, "test21() - Error - Failed to instantiate cartesian canvas(1000x1000) [2]\n");
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
    BitmapRGBImage sobel_img;
    const BitmapRGBImage image = BitmapRGBImageFile::load(file_name);
    if (!image)
    {
        fprintf(stderr, "test22() - Error - Failed to open '%s'\n",file_name.c_str());
        return;
    }
    sobel_operator(image, sobel_img, Float(0), Float(8));
    BitmapRGBImageFile::save(sobel_img, "test22_image_sobel.bmp");
}

static inline int rand_normal(float scale)
{
    return int( ::rand() * scale + ::rand() * scale + ::rand() * scale );
}

static inline std::pair<int, int> randn_point(int x0, int y0, float scale)
{
    return { x0 + rand_normal(scale), y0 + rand_normal(scale) };
}

void test23()
{
    constexpr int dim = 200;
    using Point = std::pair<int, int>;
    {
        BitmapRGBImage image(dim, dim);
        using Setter = RGBDrawer::PixelSetter;
        image.clear({20, 20, 20});
        RGBDrawer draw(image);
        draw.plotLine<Setter>(0, 2, dim - 1, dim - 3, {255, 255, 0});
        draw.plotLine<Setter>(2, 0, dim - 3, dim - 1, {255, 0, 0});
        BitmapRGBImageFile::save(image, "test23_zingl_drawer-0_rgb.bmp");
    }

    {
        BitmapRGBImage rgb_image(dim, dim);
        BitmapFloatImage float_image(dim, dim);
        using Setter = FloatDrawer::PixelAdder;  // previous was: Drawer::PixelSetter;
        FloatDrawer draw(float_image);
        auto convert = [&]() -> BitmapRGBImage& { return convert_(float_image, rgb_image); };

        const int n_lines = 300;
        const int x_p0 = dim / 4;
        const int x_p1 = dim - x_p0;
        const int y_p = dim / 2;
        const int distort = dim / 10;
        const int distort_off = (distort * 3) / 2;
        const float distort_scale = float(distort) / float(RAND_MAX -1);

        Point p0, p1{ x_p0, y_p };

        float_image.clear(0.1F);
        for (int k = 0; k < n_lines; ++k)
        {
            p0 = p1;
            p1 = randn_point(x_p1 - distort_off, y_p - distort_off, distort_scale);
            draw.plotLine<Setter>(p0, p1, 0.1F);
            p0 = p1;
            p1 = randn_point(x_p0 - distort_off, y_p - distort_off, distort_scale);
            draw.plotLine<Setter>(p0, p1, 0.1F);
        }
        BitmapRGBImageFile::save(convert(), "test23_zingl_drawer-1_float_as_rgb_lines.bmp");

        float_image.clear(0.1F);
        for (int k = 0; k < n_lines; ++k)
        {
            p1 = randn_point(x_p1 - distort_off, y_p - distort_off, distort_scale);
            draw.plotCross<Setter>(p1, 0.1F);
            p1 = randn_point(x_p0 - distort_off, y_p - distort_off, distort_scale);
            draw.plotCross<Setter>(p1, 0.1F);
        }
        BitmapRGBImageFile::save(convert(), "test23_zingl_drawer-2_float_as_rgb_cross.bmp");

        float_image.clear(0.1F);
        for (int k = 0; k < n_lines; ++k)
        {
            p1 = randn_point(x_p1 - distort_off, y_p - distort_off, distort_scale);
            draw.plotPoint<Setter>(p1, 0.1F);
            p1 = randn_point(x_p0 - distort_off, y_p - distort_off, distort_scale);
            draw.plotPoint<Setter>(p1, 0.1F);
        }
        BitmapRGBImageFile::save(convert(), "test23_zingl_drawer-3_float_as_rgb_points.bmp");
    }
}

void test24()
{
    constexpr int dim = 180;
    {
        BitmapRGBImage image(dim, dim);
        using Setter = RGBDrawer::PixelSetter;
        image.clear({20, 20, 20});
        RGBDrawer draw(image);

        draw.plotLineWidth<Setter>({10,  10}, {dim - 10,  11}, 2.0F, {255, 0, 0});
        draw.plotLineWidth<Setter>({10,  20}, {dim - 10,  30}, 2.0F, {0, 255, 0});
        draw.plotLineWidth<Setter>({10,  40}, {dim - 10,  60}, 3.0F, {0, 0, 255});
        draw.plotLineWidth<Setter>({10,  70}, {dim - 10, 100}, 4.0F, {255, 255, 0});
        draw.plotLineWidth<Setter>({10, 110}, {dim - 10, 150}, 5.0F, {0, 255, 255});
        BitmapRGBImageFile::save(image, "test24_zingl_draw-lines-0_rgb.bmp");
    }

    {
        BitmapRGBImage rgb_image(dim, dim);
        BitmapFloatImage float_image(dim, dim);
        //using Setter = FloatDrawer::PixelAdder;  // previous was: Drawer::PixelSetter;
        using Setter = FloatDrawer::PixelSetter;
        FloatDrawer draw(float_image);
        auto convert = [&]() -> BitmapRGBImage& { return convert_(float_image, rgb_image); };

        float_image.clear(0.1F);
        draw.plotLineWidth<Setter>({10,  10}, {dim - 10,  11}, 2.0F, 0.2F);
        draw.plotLineWidth<Setter>({10,  20}, {dim - 10,  30}, 2.0F, 0.2F);
        draw.plotLineWidth<Setter>({10,  40}, {dim - 10,  60}, 3.0F, 0.2F);
        draw.plotLineWidth<Setter>({10,  70}, {dim - 10, 100}, 4.0F, 0.2F);
        draw.plotLineWidth<Setter>({10, 110}, {dim - 10, 150}, 5.0F, 0.2F);
        BitmapRGBImageFile::save(convert(), "test24_zingl_draw-lines-1_float_as_rgb.bmp");
    }
}

void test25()
{
    constexpr unsigned dim = 600;
    constexpr unsigned d2 = dim/2, d4 = dim/4;
    constexpr unsigned rA = dim/6, rB = dim/15;
    {
        BitmapRGBImage image(dim, dim);
        BitmapRGBImage imgA(Slice{}, image, 0, 0), imgB(Slice{}, image, d2, 0);
        BitmapRGBImage imgC(Slice{}, image, 0, d2), imgD(Slice{}, image, d2, d2);
        RGBDrawer drawA(imgA), drawB(imgB), drawC(imgC), drawD(imgD);
        using Setter = RGBDrawer::PixelSetter;
        image.clear({20, 20, 20});

        drawA.plotEllipse<Setter>( {d4, d4}, {rA, rB}, {255, 0, 0});
        drawB.plotOptimizedEllipse<Setter>( {d4, d4}, {rB, rA}, {0, 255, 0});
        drawC.plotEllipseRect<Setter>({d4 -rA, d4 -rB}, {d4 +rA, d4 +rB}, {0, 0, 255});
        drawD.plotCircle<Setter>({d4, d4}, rA, {255, 255, 0});
        BitmapRGBImageFile::save(image, "test25_zingl_draw-ellipses-circle-0_rgb.bmp");
    }

    {
        BitmapRGBImage rgb_image(dim, dim);
        BitmapFloatImage float_image(dim, dim);
        BitmapFloatImage imgA(Slice{}, float_image, 0, 0), imgB(Slice{}, float_image, d2, 0);
        BitmapFloatImage imgC(Slice{}, float_image, 0, d2), imgD(Slice{}, float_image, d2, d2);
        using Setter = FloatDrawer::PixelAdder;  // previous was: Drawer::PixelSetter;
        FloatDrawer drawA(imgA), drawB(imgB), drawC(imgC), drawD(imgD);
        auto convert = [&]() -> BitmapRGBImage& { return convert_(float_image, rgb_image); };

        float_image.clear(0.1F);
        drawA.plotEllipse<Setter>( {d4, d4}, {rA, rB}, 0.2F);
        drawB.plotOptimizedEllipse<Setter>( {d4, d4}, {rB, rA}, 0.2F);
        drawC.plotEllipseRect<Setter>({d4 -rA, d4 -rB}, {d4 +rA, d4 +rB}, 0.2F);
        drawD.plotCircle<Setter>({d4, d4}, rA, 0.2F);
        BitmapRGBImageFile::save(convert(), "test25_zingl_draw-ellipses-circle-1_float_as_rgb.bmp");
    }
}

void test26()
{
    constexpr int d = 300;
    constexpr int c = d/2;
    constexpr int rA = d/3, rB = d/80, rC = rB*4;
    constexpr int off = 3 * rB, offC = 2*rC+3;
    {
        BitmapRGBImage image(2*d, 3*d);
        BitmapRGBImage imgA(Slice{}, image, 0, 0), imgB(Slice{}, image, d, 0);
        BitmapRGBImage imgC(Slice{}, image, 0, d), imgD(Slice{}, image, d, d);
        RGBDrawer drawA(imgA), drawB(imgB), drawC(imgC), drawD(imgD);
        using Setter = RGBDrawer::PixelSetter;
        image.clear({20, 20, 20});

        drawA.plotEllipse<Setter>( {c, c}, {rA, rB}, {255, 0, 0});
        drawA.fillEllipse<Setter>( {c, c+off}, {rA, rB}, {255, 0, 0});
        drawB.plotOptimizedEllipse<Setter>( {c, c}, {rB, rA}, {0, 255, 0});
        drawB.fillOptimizedEllipse<Setter>( {c+off, c}, {rB, rA}, {0, 255, 0});
        drawC.plotEllipseRect<Setter>({c -rA, c -rB}, {c +rA, c +rB}, {0, 0, 255});
        drawC.fillEllipseRect<Setter>({c -rA, c -rB +off}, {c +rA, c +rB +off}, {0, 0, 255});
        drawD.plotCircle<Setter>({c, c}, rA, {255, 255, 0});
        // drawD.fillCircle<Setter>({c, c}, rA, {255, 255, 0});
        BitmapRGBImageFile::save(image, "test26_zingl_draw-ellipses-circle-0_rgb.bmp");
    }

    {
        BitmapRGBImage rgb_image(2*d, 3*d);
        BitmapFloatImage float_image(2*d, 3*d);
        BitmapFloatImage imgA(Slice{}, float_image, 0, 0), imgB(Slice{}, float_image, d, 0);
        BitmapFloatImage imgC(Slice{}, float_image, 0, d), imgD(Slice{}, float_image, d, d);
        BitmapFloatImage imgE(Slice{}, float_image, 0, 2*d);
        FloatDrawer drawA(imgA), drawB(imgB), drawC(imgC), drawD(imgD), drawE(imgE);
        using Setter = FloatDrawer::PixelAdder;  // previous was: Drawer::PixelSetter;
        auto convert = [&]() -> BitmapRGBImage& { return convert_(float_image, rgb_image); };
        float_image.clear(0.1F);

        drawA.plotEllipse<Setter>( {c, c}, {rA, rB}, 0.2F);
        drawA.fillEllipse<Setter>( {c, c+off}, {rA, rB}, 0.2F);

        drawB.plotOptimizedEllipse<Setter>( {c, c}, {rB, rA}, 0.2F);
        drawB.fillOptimizedEllipse<Setter>( {c+off, c}, {rB, rA}, 0.2F);

        drawC.plotEllipseRect<Setter>({c -rA,   c -rB},        {c +rA,   c +rB}, 0.2F);
        drawC.fillEllipseRect<Setter>({c -rA,   c -rB +1*off}, {c +rA,   c +rB +1*off}, 0.2F);
        drawC.fillEllipseRect<Setter>({c -rA+1, c -rB +2*off}, {c +rA+1, c +rB +2*off}, 0.2F);
        drawC.fillEllipseRect<Setter>({c -rA,   c -rB +3*off}, {c +rA-1, c +rB +3*off}, 0.2F);
        drawC.fillEllipseRect<Setter>({c -rA+1, c -rB +4*off}, {c +rA,   c +rB +4*off}, 0.2F);

        drawD.plotEllipseRect<Setter>({c -rB,        c -rA  }, {c +rB,        c +rA  }, 0.2F);
        drawD.fillEllipseRect<Setter>({c -rB +1*off, c -rA  }, {c +rB +1*off, c +rA  }, 0.2F);
        drawD.fillEllipseRect<Setter>({c -rB +2*off, c -rA+1}, {c +rB +2*off, c +rA+1}, 0.2F);
        drawD.fillEllipseRect<Setter>({c -rB +3*off, c -rA  }, {c +rB +3*off, c +rA  }, 0.2F);
        drawD.fillEllipseRect<Setter>({c -rB +4*off, c -rA+1}, {c +rB +4*off, c +rA+1}, 0.2F);

        drawE.plotCircle<Setter>({c,           c     }, rC,   0.2F);
        drawE.fillCircle<Setter>({c +1*offC,   c     }, rC,   0.2F);
        drawE.fillCircle<Setter>({c +2*offC+1, c     }, rC,   0.2F);
        drawE.fillCircle<Setter>({c +3*offC,   c     }, rC-1, 0.2F);
        drawE.fillCircle<Setter>({c +4*offC+1, c     }, rC-1, 0.2F);

        drawE.fillCircle<Setter>({c        ,   c+offC}, rC,   0.2F);
        drawE.plotCircle<Setter>({c +1*offC,   c+offC}, rC,   0.2F);
        drawE.plotCircle<Setter>({c +2*offC+1, c+offC}, rC,   0.2F);
        drawE.plotCircle<Setter>({c +3*offC,   c+offC}, rC-1, 0.2F);
        drawE.plotCircle<Setter>({c +4*offC+1, c+offC}, rC-1, 0.2F);

        BitmapRGBImageFile::save(convert(), "test26_zingl_draw-ellipses-circle-1_float_as_rgb.bmp");
    }
}


const char *testDesc[] = {
    "compile test with rgb tests",      // 0
    "load() & save()",                  // 1
    "vertical/horizontal_flip()",       // 2
    "subsample_to()",                   // 3
    "upsample_to",                      // 4
    "set_all_ith_bits_low()",           // 5
    "export_color_plane()",             // 6
    "convert_to_grayscale()",           // 7
    "copy_region_to()",                 // 8
    "set_pixel() r/g/b jet_colormap",   // 9
    "invert_color_planes()",            // 10
    "add_to_color_plane()",             // 11
    "export_ycbcr() / import_ycbcr()",  // 12
    "export_ycbcr() / import_ycbcr()",  // 13
    "checkered_pattern()",              // 14
    "plasma< , double>()",              // 15
    "alpha_blend()",                    // 16
    "image_drawer<>::circle()/ellipse()/rectangle()",   // 17
    "generate_colours() colormaps",     // 18
    "cartesian_canvas<>::rectangle()/circle()", // 19
    "mandelbrot_set_hsv()",             // 20
    "cartesian_canvas<float>",          // 21
    "sobel_operator()",                 // 22
    "zingl_image_drawer<*> lines/points",       // 23
    "zingl_image_drawer<*>::plotLineWidth()",   // 24
    "zingl_image_drawer<*>::plotEllipses/Circle",   // 25
    "zingl_image_drawer<*>::plot on Slices"     // 26
};

int main(int argc, char* argv[])
{
    const int last_testno = 26;
    const int n_first = (argc == 1 ? 0 : 1);
    const int n_last = (argc == 1 ? last_testno : argc -1);
    if (argc == 2 && (!strcmp(argv[1], "help") || !strcmp(argv[1], "h"))) {
        for (int t = 0; t <= last_testno; ++t)
            fprintf(stderr, "test %2d: %s\n", t, testDesc[t]);
        return 0;
    }
    bool loadOK = true;
    for (int n = n_first; n <= n_last; ++n)
    {
        int t = (argc == 1) ? n : atoi(argv[n]);
        if (t >= 0 && t <= last_testno)
            fprintf(stderr, "running test %d ..\n", t);

        if (t == 0)     test00();
        if (t == 1)
        {
            loadOK = test01();
            if (!loadOK)
            {
                fprintf(stderr, "Note: Many of the tests need a bitmap image with the filename '%s'\n", file_name.c_str());
                fprintf(stderr, "is required. If not present these tests would fail!\n");
                fprintf(stderr, "generating plasma (test15) and using that file ..\n");
                test15(true);
                loadOK = test01();
                if (!loadOK)
                    fprintf(stderr, "Error generating or loading generated file!\n");
            }
        }
        if (t == 2 && loadOK)   test02();
        if (t == 3 && loadOK)   test03();
        if (t == 4 && loadOK)   test04();
        if (t == 5 && loadOK)   test05();
        if (t == 6 && loadOK)   test06();
        if (t == 7 && loadOK)   test07();
        if (t == 8 && loadOK)   test08();
        if (t == 9)     test09();
        if (t == 10 && loadOK)  test10();
        if (t == 11 && loadOK)  test11();
        if (t == 12 && loadOK)  test12();
        if (t == 13 && loadOK)  test13();
        if (t == 14)    test14();
        if (t == 15)    test15();
        if (t == 16 && loadOK)  test16();
        if (t == 17)    test17();
        if (t == 18)    test18();
        if (t == 19)    test19();
        if (t == 20)    test20();
        if (t == 21)    test21();
        if (t == 22 && loadOK) test22();
        if (t == 23)    test23();
        if (t == 24)    test24();
        if (t == 25)    test25();
        if (t == 26)    test26();
    }

    if (argc == 1)
    {
        rgba_t colortab_unpacked[10];
        rgb_t colortab_packed[10];

        printf("sizeof(rgba_t) = %u bytes\n", unsigned(sizeof(rgb_t)) );
        printf("sizeof(rgb_t) = %u bytes\n", unsigned(sizeof(rgb_t)) );
        printf("sizeof(10 * rgb_t) = %u bytes\n", unsigned(sizeof(colortab_packed)) );
        printf("sizeof(10 * rgba_t) = %u bytes\n", unsigned(sizeof(colortab_unpacked)) );
    }

    return 0;
}
