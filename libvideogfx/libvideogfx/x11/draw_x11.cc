
#include "libvideogfx/x11/draw_x11.hh"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "libvideogfx/x11/server.hh"


void WriteText_X11(Bitmap<Pixel>& bm,const char* txt,int x0,int y0,
		   Pixel front,Pixel back,
		   const char* fontname,
		   HTextAlign halign,VTextAlign valign,
		   TextDrawMode mode)
{
  int bmwidth  = bm.AskWidth();
  int bmheight = bm.AskHeight();

  Pixel*const* yp = bm.AskFrame();

  // Get access to font.

  X11ServerConnection server;
  Display* display = server.AskDisplay();

  Font font = XLoadFont(display,fontname);
  XFontStruct* fontstr = XQueryFont(display,font);


  // Calculate pixel dimensions of text if written in the font specified.

  int dummy;
  XCharStruct overallsize;
  XTextExtents(fontstr,txt,strlen(txt),&dummy,&dummy,&dummy,&overallsize);

  const int w = overallsize.width;
  const int h = overallsize.ascent+overallsize.descent;


  // Create a pixmap to draw the text into.

  Pixmap pixmap = XCreatePixmap(display,RootWindow(display,DefaultScreen(display)),w,h,1);
  GC gc = XCreateGC(display,pixmap,0,NULL);


  // Clear pixmap.

  XSetForeground(display,gc,0);
  XSetBackground(display,gc,0);
  XFillRectangle(display,pixmap,gc,0,0,w,h);


  // Draw text.

  XSetForeground(display,gc,1);
  XSetFont(display,gc,font);
  XDrawString(display,pixmap,gc,0,overallsize.ascent,txt,strlen(txt));


  // Transfer pixmap data into X11-client XImage.

  XImage* ximg = XGetImage(display,pixmap,0,0,w,h,0x01,ZPixmap);

  if (halign==HAlign_Center) x0 -= w/2;
  if (halign==HAlign_Right)  x0 += (bm.AskWidth()-w);

  if (valign==VAlign_Center) y0 -= h/2;
  if (valign==VAlign_Bottom) y0 += (bm.AskHeight()-h);

  if (mode==TextDrawMode_Opaque)
    {
      for (int y=0;y<ximg->height;y++)
	for (int x=0;x<ximg->width;x++)
	  if (y+y0<bmheight && x+x0<bmwidth && y+y0>=0 && x+x0>=0)
	    {
	      unsigned char bitpos;
	      if (ximg->bitmap_bit_order==LSBFirst)
		bitpos = 0x1<<(x%8);
	      else
		bitpos = 0x80>>(x%8);

	      yp[y+y0][x+x0] = ((ximg->data[ximg->bytes_per_line*y+x/8]&bitpos) ? front : back );
	    }
    }
  else
    {
      assert(mode==TextDrawMode_Transparent);

      for (int y=0;y<ximg->height;y++)
	for (int x=0;x<ximg->width;x++)
	  {
	    unsigned char bitpos;
	    if (ximg->bitmap_bit_order==LSBFirst)
	      bitpos = 0x1<<(x%8);
	    else
	      bitpos = 0x80>>(x%8);

	    if (ximg->data[ximg->bytes_per_line*y+x/8]&bitpos &&
		y+y0<bmheight &&
		x+x0<bmwidth &&
		y+y0>=0 && x+x0>=0)
	      {
		yp[y+y0][x+x0] = front;
	      }
	  }
    }

  // Clean up

  XFreeGC(display,gc);
  XFreePixmap(display,pixmap);
  XDestroyImage(ximg);
  XFreeFontInfo(NULL,fontstr,1);
  XUnloadFont(display,font);
}
