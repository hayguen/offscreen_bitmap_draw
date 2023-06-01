# Offscreen Bitmap Drawing C++ Header-Only Library

## Origin

This is a fork of http://www.partow.net/downloads/bitmap.zip \
an MIT licensed C++ Bitmap Library from Arash Partow.

See http://www.partow.net/programming/bitmap/index.html
and the referenced documentation there: \
https://www.partow.net/programming/bitmap/documentation/annotated.html


## Purpose

Perform inlined in-memory drawing operations with normal CPU - not using GPU,
which don't need library operations or function calls.

Thus, it's header-only.


## Changes from Arash Partow's version

* splitted/organized single header into multiple headers in `offscr_bmp_drw/` directory
* added (still) minimalistic cmake support
* moved all classes/functions into namespace `OffScreenBitmapDraw`
* added zingl_image_drawer and templates for setting or adding pixels - later one for building heatmaps
* other incompatible changes planned!


## Related / Similar C++ Libraries

* Tools for offscreen bitmap drawing - with Adafruit GFX Library
  * https://github.com/leftCoast/LC_offscreen
  * https://github.com/adafruit/Adafruit-GFX-Library
* Simple C++ off screen raster graphics library
  * https://github.com/richmit/mraster
* Bresenham Code/Site of Alois Zingl
  * https://zingl.github.io/bresenham.html
  * https://github.com/zingl/Bresenham
