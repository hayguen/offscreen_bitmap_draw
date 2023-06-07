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
#include <offscr_bmp_drw/bitmap_image_file.hpp>
#include <offscr_bmp_drw/image_drawer.hpp>
#include <offscr_bmp_drw/zingl_image_drawer.hpp>
#include <offscr_bmp_drw/colormaps.hpp>

#include <array>

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cassert>


using namespace OffScreenBitmapDraw;

using rgb_pixel_t = bgr_t;
using BitmapRGBImage = bitmap_image_rgb<rgb_pixel_t>;
using BitmapRGBImageFile = image_io<BitmapRGBImage>;

#define USE_ZINGL_DRAWER    1

// need same type as reference colormap
static rgb_pixel_t cmapRef[1000];   // for a copy of jet_colormap[]
static rgb_pixel_t cmapA[1000];
static rgb_pixel_t cmapB[1000];

static void generate_colormaps()
{
    constexpr int max_num_colors = 16;
    int color_idx[max_num_colors] = { 0 };
    rgb_pixel_t c[max_num_colors];
    int num_colors = 0;

    // play here to experiment with colormap
    // remove or add lines setting desired color at defined positions (color_idx)
    //   and modify the rgb values in the set_rgb()-call
    //   the color indices in between are interpolated - in the RGB domain
    set_rgb(c[num_colors],   0,   0,   0);  color_idx[num_colors++] = 0;    // first entry
    set_rgb(c[num_colors],  50,  50, 192);  color_idx[num_colors++] = 100;
    set_rgb(c[num_colors],  50, 128, 255);  color_idx[num_colors++] = 200;
    set_rgb(c[num_colors],  50, 208,  50);  color_idx[num_colors++] = 500;
    set_rgb(c[num_colors], 240, 240,   0);  color_idx[num_colors++] = 700;
    set_rgb(c[num_colors], 255,   0,   0);  color_idx[num_colors++] = 999;  // last entry

    interpolate_color_points(num_colors, color_idx, c, cmapA);
    interpolate_color_points(num_colors, color_idx, c, cmapB);
    for (int k = 1; k < num_colors - 1; ++k)
        set_black(cmapB[ color_idx[k] ]);   // set given color points to black

    convert_colors(1000, jet_colormap, cmapRef);
}


int main(int, char* [])
{
    generate_colormaps();
    // show 3 bars - from top to bottom:
    //  * a reference colormap
    //  * the colormap, defined in generate_colormaps()
    //  * the same colormap, defined in generate_colormaps()
    //     - but with black lines at the given color points
    const rgb_pixel_t* colormap[] = { cmapRef, cmapA, cmapB };
    constexpr int h_per_color = 50;
    constexpr int n_colormaps = sizeof(colormap) / sizeof(colormap[0]);
    BitmapRGBImage image(1000, n_colormaps * h_per_color);
#if USE_ZINGL_DRAWER
    using RGBDrawer = zingl_image_drawer<BitmapRGBImage>;
    using Setter = RGBDrawer::PixelSetter;
    RGBDrawer draw(image);
#else
    image_drawer<BitmapRGBImage> draw(image);
#endif

    image.clear();
    for (unsigned int x = 0; x < image.width(); ++x)
    {
        for (unsigned int j = 0; j < n_colormaps; ++j)
        {
        #if USE_ZINGL_DRAWER
            draw.plotVLine<Setter>(x, j * h_per_color, (j + 1) * h_per_color -2, colormap[j][x]);
        #else
            draw.pen_color( colormap[j][x] );
            draw.vertical_line_segment(x, j * h_per_color, (j + 1) * h_per_color -2);
        #endif
        }
    }
    BitmapRGBImageFile::save(image, "test_cmap.bmp");
    return 0;
}
