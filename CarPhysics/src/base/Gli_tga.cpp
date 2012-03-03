/*
 * Copyright (c) 2012 Valentin Galea
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

//!MODIF Modified by Valentin Galea on 29.02.2004
//!MODIF all modifications have '!MODIF' signature

//!MODIF #ifndef __gli_tga_h__
//!MODIF #define __gli_tga_h__

/* gli_tga.h - interface for TrueVision (TGA) image file loader */

/* Copyright NVIDIA Corporation, 1999. */

/* $Id: Gli_tga.cpp,v 1.1 2010/02/28 19:29:35 alex Exp $ */

/* A lightweight TGA image file loader for OpenGL programs. */

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;

typedef struct {
  uint8 idLength;
  uint8 colorMapType;

  /* The image type. */
#define TGA_TYPE_MAPPED 1
#define TGA_TYPE_COLOR 2
#define TGA_TYPE_GRAY 3
#define TGA_TYPE_MAPPED_RLE 9
#define TGA_TYPE_COLOR_RLE 10
#define TGA_TYPE_GRAY_RLE 11
  uint8 imageType;

  /* Color Map Specification. */
  /* We need to separately specify high and low bytes to avoid endianness
     and alignment problems. */
  uint8 colorMapIndexLo, colorMapIndexHi;
  uint8 colorMapLengthLo, colorMapLengthHi;
  uint8 colorMapSize;

  /* Image Specification. */
  uint8 xOriginLo, xOriginHi;
  uint8 yOriginLo, yOriginHi;

  uint8 widthLo, widthHi;
  uint8 heightLo, heightHi;

  uint8 bpp;

  /* Image descriptor.
     3-0: attribute bpp
     4:   left-to-right ordering
     5:   top-to-bottom ordering
     7-6: zero
     */
#define TGA_DESC_ABITS 0x0f
#define TGA_DESC_HORIZONTAL 0x10
#define TGA_DESC_VERTICAL 0x20
  uint8 descriptor;

} TgaHeader;

typedef struct {
  uint32 extensionAreaOffset;
  uint32 developerDirectoryOffset;
#define TGA_SIGNATURE "TRUEVISION-XFILE"
  uint8 signature[16];
  uint8 dot;
  uint8 null;
} TgaFooter;

//!MODIF #endif /* __gli_tga_h__ */

/* This file is derived from (actually an earlier version of)... */

/* The GIMP -- an image manipulation program
 * Copyright (C) 1995 Spencer Kimball and Peter Mattis
 *
 * $Id: Gli_tga.cpp,v 1.1 2010/02/28 19:29:35 alex Exp $
 * TrueVision Targa loading and saving file filter for the Gimp.
 * Targa code Copyright (C) 1997 Raphael FRANCOIS and Gordon Matzigkeit
 *
 * The Targa reading and writing code was written from scratch by
 * Raphael FRANCOIS <fraph@ibm.net> and Gordon Matzigkeit
 * <gord@gnu.ai.mit.edu> based on the TrueVision TGA File Format
 * Specification, Version 2.0:
 *
 *   <URL:ftp://ftp.truevision.com/pub/TGA.File.Format.Spec/>
 *
 * It does not contain any code written for other TGA file loaders.
 * Not even the RLE handling. ;)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

//!MODIF #include <stdio.h>
//!MODIF #include <stdlib.h>
#include <string.h>

/* Include stuff that should be in <GL/gl.h> that might not be in your version. */

/* EXT_bgra defines from <GL/gl.h> */
#ifndef GL_EXT_bgra
#define GL_BGR_EXT			    0x80E0
#define GL_BGRA_EXT			    0x80E1
#endif

//!MODIF #include "gli.h"
//!MODIF #include "gli_internal.h"
//!MODIF #include "gli_tga.h"

//!MODIF
#include "window.h"

//!MODIF from gli.h
typedef struct {

  GLsizei  width;
  GLsizei  height;
  GLint    components;
  GLenum   format;

  GLsizei  cmapEntries;
  GLenum   cmapFormat;
  GLubyte *cmap;

  GLubyte *pixels;
  
} gliGenericImage;

//!MODIF stdio wraper begin
#include "filesys.h"
#define FILE binaryFile_t
#define fread(d, s, l, f) f->read(d, s, l)
#define fgetc(f) f->getChar()
#define fseek(f, p, m) f->seek(p, m)
#define ftell(f) f->tell()
//!MODIF stdio wraper end

//!MODIF
//#include "mmgr/mmgr.h"

//!MODIF malloc/free wraper begin
/*
inline static void * myFuckUpMalloc(size_t n)
{
	void *ptr = 0;
	ptr = new unsigned char[n];
	return ptr;
}

inline static void myFuckUpFree(void *ptr)
{
	delete []ptr;
}
*/
#define myFuckUpMalloc malloc
#define myFuckUpFree free
//!MODIF malloc/free wraper end

//!MODIF
char __gliError[1024];
//!MODIF
int __gliVerbose = true;

static int totbytes = 0;

typedef struct {
  unsigned char *statebuf;
  int statelen;
  int laststate;
} RLEstate;

static int
std_fread(RLEstate *rleInfo, void *buf, size_t datasize, size_t nelems, FILE *fp)
{
  if (__gliVerbose > 1) {
    totbytes += nelems * datasize;
    printf("TGA: std_fread %d (total %d)\n",
      nelems * datasize, totbytes);
  }
  return fread(buf, datasize, nelems, fp);
}

#define MIN(a,b) (((a) < (b)) ? (a) : (b))

#define RLE_PACKETSIZE 0x80

/* Decode a bufferful of file. */
static int
rle_fread(RLEstate *rleInfo, void *vbuf, size_t datasize, size_t nelems, FILE *fp)
{

  unsigned char *buf = /*!MODIF*/(unsigned char *)vbuf;
  int j, k;
  int buflen, count, bytes, curbytes;
  unsigned char *p;

  /* Scale the buffer length. */
  buflen = nelems * datasize;

  j = 0;
  curbytes = totbytes;
  while (j < buflen) {
    if (rleInfo->laststate < rleInfo->statelen) {
      /* Copy bytes from our previously decoded buffer. */
      bytes = MIN(buflen - j, rleInfo->statelen - rleInfo->laststate);
      memcpy(buf + j, rleInfo->statebuf + rleInfo->laststate, bytes);
      j += bytes;
      rleInfo->laststate += bytes;

      /* If we used up all of our state bytes, then reset them. */
      if (rleInfo->laststate >= rleInfo->statelen) {
	rleInfo->laststate = 0;
	rleInfo->statelen = 0;
      }

      /* If we filled the buffer, then exit the loop. */
      if (j >= buflen) break;
    }

    /* Decode the next packet. */
    count = fgetc(fp);
    if (count == EOF) {
      if (__gliVerbose) printf("TGA: hit EOF while looking for count\n");
      return j / datasize;
    }

    /* Scale the byte length to the size of the data. */
    bytes = ((count & ~RLE_PACKETSIZE) + 1) * datasize;

    if (j + bytes <= buflen) {
      /* We can copy directly into the image buffer. */
      p = buf + j;
    } else {
#ifdef PROFILE
      printf("TGA: needed to use statebuf for %d bytes\n", buflen - j);
#endif
      /* Allocate the state buffer if we haven't already. */
      if (!rleInfo->statebuf) {
	rleInfo->statebuf = (unsigned char *) myFuckUpMalloc(RLE_PACKETSIZE * datasize);
      }
      p = rleInfo->statebuf;
    }

    if (count & RLE_PACKETSIZE) {
      /* Fill the buffer with the next value. */
      if (fread(p, datasize, 1, fp) != 1) {
	if (__gliVerbose) {
	  printf("TGA: EOF while reading %d/%d element RLE packet\n",
	    bytes, datasize);
	}
	return j / datasize;
      }

      /* Optimized case for single-byte encoded data. */
      if (datasize == 1) {
	memset(p + 1, *p, bytes - 1);
      } else {
	for (k = datasize; k < bytes; k += datasize) {
	  memcpy(p + k, p, datasize);
	}
      }
    } else {
      /* Read in the buffer. */
      if (fread(p, bytes, 1, fp) != 1) {
	if (__gliVerbose) {
	  printf("TGA: EOF while reading %d/%d element raw packet\n",
	    bytes, datasize);
	}
	return j / datasize;
      }
    }

    if (__gliVerbose > 1) {
      totbytes += bytes;
      if (__gliVerbose > 2) {
	printf("TGA: %s packet %d/%d\n",
	  (count & RLE_PACKETSIZE) ? "RLE" : "raw",
	  bytes, totbytes);
      }
    }

    /* We may need to copy bytes from the state buffer. */
    if (p == rleInfo->statebuf) {
      rleInfo->statelen = bytes;
    } else {
      j += bytes;
    }
  }

  if (__gliVerbose > 1) {
    printf("TGA: rle_fread %d/%d (total %d)\n",
      nelems * datasize, totbytes - curbytes, totbytes);
  }
  return nelems;
}

gliGenericImage *
gliReadTGA(FILE *fp, char *name, int yFlip)
{
  TgaHeader tgaHeader;
  TgaFooter tgaFooter;
  char horzrev, vertrev;
  int width, height, bpp;
  int start, end, dir;
  int i, j, k;
  int pelbytes, wbytes;
  GLenum format;
  int components;
  RLEstate rleRec;
  RLEstate *rleInfo;
  int rle;
  int index, colors, length;
  GLubyte *cmap, *pixels, *data;
  int (*myfread)(RLEstate *rleInfo, void*, size_t, size_t, FILE*);
  gliGenericImage *genericImage;

  /* Check the footer. */
  if (fseek(fp, 0L - sizeof(tgaFooter), SEEK_END)
      || fread(&tgaFooter, sizeof(tgaFooter), 1, fp) != 1) {
    sprintf(__gliError, "TGA: Cannot read footer from \"%s\"", name);
    if (__gliVerbose) printf("%s\n", __gliError);
    return NULL;
  }

  /* Check the signature. */
  if (memcmp(tgaFooter.signature, TGA_SIGNATURE,
	     sizeof(tgaFooter.signature)) == 0) {
    if (__gliVerbose) printf("TGA: found New TGA\n");
  } else {
    if (__gliVerbose) printf("TGA: found Original TGA\n");
  }

  if (fseek(fp, 0, SEEK_SET) ||
      fread(&tgaHeader, sizeof(tgaHeader), 1, fp) != 1) {
    sprintf(__gliError, "TGA: Cannot read header from \"%s\"", name);
    if (__gliVerbose) printf("%s\n", __gliError);
    return NULL;
  }

  if (__gliVerbose) {
    printf("TGA: idLength=%d, colorMapType=%d\n",
      tgaHeader.idLength, tgaHeader.colorMapType);
  }
  if (__gliVerbose && tgaHeader.idLength) {
    char *idString = (char*) myFuckUpMalloc(tgaHeader.idLength);

    if (fread(idString, tgaHeader.idLength, 1, fp) != 1) {
      sprintf(__gliError, "TGA: Cannot read ID field in \"%s\"", name);
      printf("%s\n", __gliError);
    } else {
      printf("TGA: ID field: \"%*s\"\n", tgaHeader.idLength, idString);
    }
    myFuckUpFree(idString);
  } else {
    /* Skip the image ID field. */
    if (tgaHeader.idLength && fseek(fp, tgaHeader.idLength, SEEK_CUR)) {
      sprintf(__gliError, "TGA: Cannot skip ID field in \"%s\"", name);
      if (__gliVerbose) printf("%s\n", __gliError);
      return NULL;
    }
  }

  /* Reassemble the multi-byte values correctly, regardless of
     host endianness. */
  width = (tgaHeader.widthHi << 8) | tgaHeader.widthLo;
  height = (tgaHeader.heightHi << 8) | tgaHeader.heightLo;
  bpp = tgaHeader.bpp;
  if (__gliVerbose) {
    printf("TGA: width=%d, height=%d, bpp=%d\n", width, height, bpp);
  }
  if (width == 0 || height == 0 || bpp == 0) {
    sprintf(__gliError, "TGA: width, height, or bpp is zero in \"%s\"", name);
    if (__gliVerbose) printf("%s\n", __gliError);
    return NULL;
  }

  horzrev = tgaHeader.descriptor & TGA_DESC_HORIZONTAL;
  vertrev = !(tgaHeader.descriptor & TGA_DESC_VERTICAL);
  if (__gliVerbose && horzrev) printf("TGA: horizontal reversed\n");
  if (__gliVerbose && vertrev) printf("TGA: vertical reversed\n");
  if (yFlip) {
    if (__gliVerbose) printf("TGA: vertical flip requested\n");
    vertrev = !vertrev;
  }

  rle = 0;
  switch (tgaHeader.imageType) {
  case TGA_TYPE_MAPPED_RLE:
    rle = 1;
    if (__gliVerbose) printf("TGA: run-length encoded\n");
  case TGA_TYPE_MAPPED:
    /* Test for alpha channel. */
    format = GL_COLOR_INDEX;
    components = 1;
    if (__gliVerbose) {
      printf("TGA: %d bit indexed image (%d bit palette)\n",
	tgaHeader.colorMapSize, bpp);
    }
    break;

  case TGA_TYPE_GRAY_RLE:
    rle = 1;
    if (__gliVerbose) printf("TGA: run-length encoded\n");
  case TGA_TYPE_GRAY:
    format = GL_LUMINANCE;
    components = 1;
    if (__gliVerbose) printf("TGA: %d bit grayscale image\n", bpp);
    break;

  case TGA_TYPE_COLOR_RLE:
    rle = 1;
    if (__gliVerbose) printf("TGA: run-length encoded\n");
  case TGA_TYPE_COLOR:
    /* Test for alpha channel. */
    if (bpp == 32) {
      format = GL_BGRA_EXT;
      components = 4;
      if (__gliVerbose) {
	printf("TGA: %d bit color image with alpha channel\n", bpp);
      }
    } else {
      format = GL_BGR_EXT;
      components = 3;
      if (__gliVerbose) printf("TGA: %d bit color image\n", bpp);
    }
    break;

  default:
    sprintf(__gliError,
      "TGA: unrecognized image type %d\n", tgaHeader.imageType);
    if (__gliVerbose) printf("%s\n", __gliError);
    return NULL;
  }

  if ((format == GL_BGRA_EXT && bpp != 32) ||
      (format == GL_BGR_EXT && bpp != 24) ||
      ((format == GL_LUMINANCE || format == GL_COLOR_INDEX) && bpp != 8)) {
    /* FIXME: We haven't implemented bit-packed fields yet. */
    sprintf(__gliError, "TGA: channel sizes other than 8 bits are unimplemented");
    if (__gliVerbose) printf("%s\n", __gliError);
    return NULL;
  }

  /* Check that we have a color map only when we need it. */
  if (format == GL_COLOR_INDEX) {
    if (tgaHeader.colorMapType != 1) {
      sprintf(__gliError, "TGA: indexed image has invalid color map type %d\n",
	tgaHeader.colorMapType);
      if (__gliVerbose) printf("%s\n", __gliError);
      return NULL;
    }
  } else if (tgaHeader.colorMapType != 0) {
    sprintf(__gliError, "TGA: non-indexed image has invalid color map type %d\n",
      tgaHeader.colorMapType);
    if (__gliVerbose) printf("%s\n", __gliError);
    return NULL;
  }

  if (tgaHeader.colorMapType == 1) {
    /* We need to read in the colormap. */
    index = (tgaHeader.colorMapIndexHi << 8) | tgaHeader.colorMapIndexLo;
    length = (tgaHeader.colorMapLengthHi << 8) | tgaHeader.colorMapLengthLo;

    if (__gliVerbose) {
      printf("TGA: reading color map (%d + %d) * (%d / 8)\n",
	index, length, tgaHeader.colorMapSize);
    }
    if (length == 0) {
      sprintf(__gliError, "TGA: invalid color map length %d", length);
      if (__gliVerbose) printf("%s\n", __gliError);
      return NULL;
    }
    if (tgaHeader.colorMapSize != 24) {
      /* We haven't implemented bit-packed fields yet. */
      sprintf(__gliError, "TGA: channel sizes other than 8 bits are unimplemented");
      if (__gliVerbose) printf("%s\n", __gliError);
      return NULL;
    }

    pelbytes = tgaHeader.colorMapSize / 8;
    colors = length + index;
    cmap = /*!MODIF*/(unsigned char *)myFuckUpMalloc (colors * pelbytes);

    /* Zero the entries up to the beginning of the map. */
    memset(cmap, 0, index * pelbytes);

    /* Read in the rest of the colormap. */
    if (fread(cmap, pelbytes, length, fp) != (size_t) length) {
      sprintf(__gliError, "TGA: error reading colormap (ftell == %ld)\n",
	ftell (fp));
      if (__gliVerbose) printf("%s\n", __gliError);
      return NULL;
    }

    if (pelbytes >= 3) {
      /* Rearrange the colors from BGR to RGB. */
      int tmp;
      for (j = index; j < length * pelbytes; j += pelbytes) {
	tmp = cmap[j];
	cmap[j] = cmap[j + 2];
	cmap[j + 2] = tmp;
      }
    }
  } else {
    colors = 0;
    cmap = NULL;
  }

  /* Allocate the data. */
  pelbytes = bpp / 8;
  pixels = (unsigned char *) myFuckUpMalloc (width * height * pelbytes);

  if (rle) {
    rleRec.statebuf = 0;
    rleRec.statelen = 0;
    rleRec.laststate = 0;
    rleInfo = &rleRec;
    myfread = rle_fread;
  } else {
    rleInfo = NULL;
    myfread = std_fread;
  }

  wbytes = width * pelbytes;

  if (vertrev) {
    start = 0;
    end = height;
    dir = 1;
  } else {
    /* We need to reverse loading order of rows. */
    start = height-1;
    end = -1;
    dir = -1;
  }

  for (i = start; i != end; i += dir) {
    data = pixels + i*wbytes;

    /* Suck in the data one row at a time. */
    if (myfread(rleInfo, data, pelbytes, width, fp) != width) {
      /* Probably premature end of file. */
      if (__gliVerbose) {
	printf ("TGA: error reading (ftell == %ld, width=%d)\n",
	  ftell(fp), width);
      }
      return NULL;
    }

    if (horzrev) {
      /* We need to mirror row horizontally. */
      for (j = 0; j < width/2; j++) {
	GLubyte tmp;

	for (k = 0; k < pelbytes; k++) {
	  tmp = data[j*pelbytes+k];
	  data[j*pelbytes+k] = data[(width-j-1)*pelbytes+k];
	  data[(width-j-1)*pelbytes+k] = tmp;
	}
      }
    }
  }

  if (rle) {
    myFuckUpFree(rleInfo->statebuf);
  }

  if (fgetc (fp) != EOF) {
    if (__gliVerbose) printf ("TGA: too much input data, ignoring extra...\n");
  }

  genericImage = (gliGenericImage*) myFuckUpMalloc(sizeof(gliGenericImage));
  genericImage->width = width;
  genericImage->height = height;
  genericImage->format = format;
  genericImage->components = components;
  genericImage->cmapEntries = colors;
  genericImage->cmap = cmap;
  if (cmap) {
    genericImage->cmapFormat = GL_RGB;
  } else {
    genericImage->cmapFormat = GL_NONE;
  }
  genericImage->pixels = pixels;

  return genericImage;
}

//!MODIF begin
#include "texture.h"
#include "error.h"

void gliReadTGAWrapper(texture_t &tex, const char *filename)
{
	gliGenericImage *gli = 0;
	binaryFile_t *f = 0;

	try
	{
		f = stdio::instance().openBinary(filename, IOflags::read);

		gli = gliReadTGA(f, const_cast<char*>(filename), 1);
		if ( !gli )
		{
			printf("%s", __gliError);
			throw error("TGA: loading aborted");
		}

		switch( gli->format )
		{
		case GL_RGB:
		case GL_RGBA:
		case GL_LUMINANCE:
			tex.pixFormatIntern = gli->format;
			tex.pixFormatSrc = gli->format;
			tex.pixSize = GL_UNSIGNED_BYTE;
			break;

		case GL_BGR_EXT:
			tex.pixFormatIntern = GL_RGB;
			tex.pixFormatSrc = gli->format;
			tex.pixSize = GL_UNSIGNED_BYTE;
			break;

		case GL_BGRA_EXT:
		//case GL_ABGR_EXT:
			tex.pixFormatIntern = GL_RGBA;
			tex.pixFormatSrc = gli->format;
			tex.pixSize = GL_UNSIGNED_BYTE;
			break;

		default:
			throw error("TGA: unsupported format");
		}
		
		tex.width = gli->width;
		tex.height = gli->height;
		tex.data = gli->pixels;
		tex.id = 1; //dummy value just to pass non-0 test in setParams()

		delete f;
		myFuckUpFree(gli);
	}
	catch(...)
	{
		if ( gli )
		{
			if ( gli->cmap )
				myFuckUpFree(gli->cmap);
			if ( gli->pixels )
				myFuckUpFree(gli->pixels);
			myFuckUpFree(gli);
		}

		delete f;

		/*re*/throw;
	}
}
//!MODIF end
