/********************************************************************************
  $Header$

  purpose:
    Bitmap class definition. A bitmap can be used for any two-dimensional
    image data of arbitrary types. Bitmaps can easily be created in main
    memory given its size, optional alignment and surrounding border.
    For special purpose applications, BitmapProviders can be defined which
    create an envelope around existing memory blocks to allow access to them
    through the standard Bitmap API.

  notes:
    This class cannot be named Pixmap because this is already used in X11.

  to do:

  author(s):
   - Dirk Farin, dirk.farin@gmx.de

  modifications:
    23/Jan/2002 - Dirk Farin - Complete reimplementation based on old Bitmap type.
    02/Jun/1999 - Dirk Farin - First implementation based on DVDview code.
 ********************************************************************************
    LibVideoGfx - video processing library
    Copyright (C) 2002  Dirk Farin

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 ********************************************************************************/

#ifndef LIBVIDEOGFX_GRAPHICS_BASIC_BITMAP_HH
#define LIBVIDEOGFX_GRAPHICS_BASIC_BITMAP_HH

#include <libvideogfx/error.hh>
#include <libvideogfx/types.hh>
#include <libvideogfx/utility/math.hh>
#include <string.h>
#include <algorithm>


namespace videogfx {

  /* Set global defaults for bitmap alignment. Every new bitmap created by a
     BitmapProvider that supports user-defined alignment will be aligned to
     at least these defaults. I.e. if you globally specify a border of 4 and
     an alignment of 3, and a bitmap should be created with a border of 2
     and am alignment of 4, the bitmap will have a border of size 4 and an
     alignment of size 12.
  */
  void SetAlignmentDefaults(int border,int halign,int valign);
  void AskAlignmentDefaults(int& border,int& halign,int& valign);


  template <class Pel> class BitmapProvider;


  template <class Pel> class Bitmap
  {
  public:
    Bitmap(); /* Create an empty bitmap. No provider is attached. */
    Bitmap(const Bitmap&);
    Bitmap(int w,int h,int border=0,int halign=1,int valign=1);
    Bitmap(BitmapProvider<Pel>*); /* Attach the full image of the provider to the Bitmap. */
    ~Bitmap();

    void Create(int w,int h,int border=0,int halign=1,int valign=1);
    void Release(); /* Detach bitmap content from the object. If this was the
		       last handle to the bitmap data, the data will be removed
		       from main memory. */

    /* Attach the full image of a new provider. The provider may be NULL, which will result
       in a empty bitmap. */
    void AttachBitmapProvider(BitmapProvider<Pel>*);

    Bitmap<Pel> operator=(const Bitmap<Pel>&);
    Bitmap<Pel> CreateSubView  (int x0,int y0,int w,int h) const;
    Bitmap<Pel> CreateFieldView(bool top) const;

    /* Copy the bitmap into a new, independent memory area. Alignments can be changed
       on the fly. If border<0, the old border size is used for the new bitmap. */
    Bitmap<Pel> Clone(int border=-1,int halign=1,int valign=1) const;
 
    int AskWidth()  const;
    int AskHeight() const;
    int AskLogicalWidth() const;
    int AskLogicalHeight() const;
    int AskTotalWidth() const;
    int AskTotalHeight() const;
    int AskBorder() const;
    int AskStride() const { return AskFrame()[1]-AskFrame()[0]; }  // NOTE: this is != TotalWidth (field view)

    bool IsEmpty() const { return d_parent==NULL; }

    /* Get a pointer-array to the actual bitmap data ([y][x]). Pixel [0][0] is
       the top-left pixel _inside_ the image. I.e. if the bitmap has a border
       around it, the border pixels can be access by negative positions or with
       x,y coordinates above AskWidth()/AskHeight(). */
    Pel*const* AskFrame();
    const Pel*const* AskFrame() const;

    bool IsShared() const
    {
      if (!d_parent) return false;
      if (d_parent->RefCntr()>1) return true;
      return false;
    }

#if 0
    void Debug() const
    {
      cout << d_width << "x" << d_height << "/" << d_border << endl
	   << "dataptr: " << (d_dataptr_reused ? "reused\n" : "own\n") << d_data << endl << d_parent << endl;
    }
#endif

  private:
    BitmapProvider<Pel>* d_parent;

    int d_width;
    int d_height;
    int d_border;
    int d_internal_width;
    int d_internal_height;
    int d_total_width;
    int d_total_height;

    Pel** d_data;
    bool  d_dataptr_reused;
  };


  template <class Pel> class BitmapProvider
  {
  public:
    BitmapProvider();
    virtual ~BitmapProvider();

    int AskLogicalWidth()  const { return d_width; }
    int AskLogicalHeight() const { return d_height; }
    int AskWidth()  const { return d_internal_width; }
    int AskHeight() const { return d_internal_height; }
    int AskTotalWidth()  const { return d_total_width; }
    int AskTotalHeight() const { return d_total_height; }
    int AskBorder() const { return d_border; }

    Pel** AskFrame()       { return d_frame_ptr; }
    const Pel** AskFrame() const { return d_frame_ptr; }

    void IncrRef() { d_ref_cntr++; }
    void DecrRef() { d_ref_cntr--; assert(d_ref_cntr>=0); }
    int  RefCntr() const { return d_ref_cntr; }

  private:
    int d_ref_cntr;

    Pel** d_frame_ptr;

  protected:
    /* Each derived class is responsible for setting the following attributes
       and has to call SetFramePtrs() afterwards. */

    int  d_width,d_height;   // logical size
    int  d_border;

    int  d_internal_width;   // size of bitmap area inclusive alignments
    int  d_internal_height;

    int  d_total_width;
    int  d_total_height;

    Pel* d_bitmap_ptr;

    void SetFramePtrs();
  };


  inline void CalcInternalSizes(int w,int h,int border,int halign,int valign,
				int& intw,int& inth,int& intb)
  {
    assert(border>=0);
    assert(halign>=1);
    assert(valign>=1);

    // integrate default alignments into alignment specification

    int def_border,def_halign,def_valign;
    AskAlignmentDefaults(def_border, def_halign, def_valign);

    border = std::max(border,def_border);
    halign = LeastCommonMultiple(halign,def_halign);
    valign = LeastCommonMultiple(valign,def_valign);


    // increase size of internal bitmap area

    intw = w;
    inth = h;
    intb = border;

    intw = AlignUp(intw, halign);
    inth = AlignUp(inth, valign);
    intb = AlignUp(intb, halign);       // align border to halign to provide aligned memory access
  }


  template <class Pel> class BitmapProvider_Mem : public BitmapProvider<Pel>
  {
  public:
    BitmapProvider_Mem() { }
    BitmapProvider_Mem(int w,int h,int border=0,int halign=1,int valign=1)
    {
      Create(w,h,border,halign,valign);
    }
    ~BitmapProvider_Mem() { if (d_bitmap_ptr) delete[] d_bitmap_ptr; }

    void Create(int w,int h,int border=0,int halign=1,int valign=1)
    {
      CalcInternalSizes(w,h,border,halign,valign,
			d_internal_width,d_internal_height,border);

      // total size including border

      d_total_width  = d_internal_width +2*border;
      d_total_height = d_internal_height+2*border;

      if (d_bitmap_ptr)
	delete[] d_bitmap_ptr;

      d_bitmap_ptr = new Pel[d_total_width * d_total_height];
      d_width  = w;
      d_height = h;
      d_border = border;
      SetFramePtrs();
    }
  };


  // ---------------------- BitmapProvider implementation --------------------

  template <class Pel> inline BitmapProvider<Pel>::BitmapProvider()
    : d_ref_cntr(0),
      d_frame_ptr(NULL),
      d_bitmap_ptr(NULL)
  {
  }

  template <class Pel> inline BitmapProvider<Pel>::~BitmapProvider()
  {
    assert(d_ref_cntr==0);

    if (d_frame_ptr)
      delete[] d_frame_ptr;
  }

  template <class Pel> void BitmapProvider<Pel>::SetFramePtrs()
  {
    if (d_frame_ptr)
      delete[] d_frame_ptr;

    d_frame_ptr = new Pel* [d_total_height];

    for (int i=0;i<d_total_height;i++)
      d_frame_ptr[i] = d_bitmap_ptr + i*d_total_width + d_border;
  }


  // ---------------------- Bitmap implementation --------------------

  template <class Pel> Bitmap<Pel>::Bitmap()
    : d_parent(NULL),
      d_data(NULL),
      d_dataptr_reused(true)
  {
  }

  template <class Pel> Bitmap<Pel>::Bitmap(int w,int h,int border,int halign,int valign)
    : d_parent(NULL),
      d_data(NULL),
      d_dataptr_reused(true)
  {
    AttachBitmapProvider(new BitmapProvider_Mem<Pel>(w,h,border,halign,valign));
  }

  template <class Pel> Bitmap<Pel>::Bitmap(BitmapProvider<Pel>* p)
    : d_parent(NULL),
      d_data(NULL),
      d_dataptr_reused(true)
  {
    AttachBitmapProvider(p);
  }

  template <class Pel> Bitmap<Pel>::Bitmap(const Bitmap& pm)
  {
    d_parent = pm.d_parent;
    d_data=NULL; // in case "pm" is an empty bitmap
    d_dataptr_reused=true;

    if (d_parent)
      {
	d_parent->IncrRef();

	d_width = pm.d_width;
	d_height = pm.d_height;
	d_border = pm.d_border;
	d_internal_width  = pm.d_internal_width;
	d_internal_height = pm.d_internal_height;
	d_total_width  = pm.d_total_width;
	d_total_height = pm.d_total_height;

	if (pm.d_dataptr_reused)
	  {
	    d_data = pm.d_data;
	    d_dataptr_reused = true;
	  }
	else
	  {
	    d_data = new Pel* [d_total_height];
	    for (int y=0;y<d_total_height;y++)
	      d_data[y] = pm.d_data[y];

	    d_dataptr_reused = false;
	  }
      }
  }

  template <class Pel> Bitmap<Pel>::~Bitmap()
  {
    Release();
  }

  template <class Pel> void Bitmap<Pel>::Create(int w,int h,int border,int halign,int valign)
  {
    bool compatible=false;
    int intw,inth,intb;

    if (d_parent)
      {
	compatible=true;

	CalcInternalSizes(w,h,border,halign,valign,
			  intw,inth,intb);

	if (d_total_width  < intw+2*intb) compatible=false;
	if (d_total_height < inth+2*intb) compatible=false;
	if (d_parent->RefCntr()>1)
	  compatible = false;
      }

    if (compatible)
      {
	d_width  = w;
	d_height = h;
	d_internal_width  = intw;
	d_internal_height = inth;
	d_border = border;
      }
    else
      {
	AttachBitmapProvider(new BitmapProvider_Mem<Pel>(w,h,border,halign,valign));
      }
  }

  template <class Pel> void Bitmap<Pel>::Release()
  {
    // Release bitmap-provider and destroy if this was the last reference.

    if (d_parent)
      {
	d_parent->DecrRef();
	if (d_parent->RefCntr()==0) { delete d_parent; }

	d_parent=NULL;
      }

    // Release row-ptr array if the bitmap-provider array was not reused.

    if (d_data && !d_dataptr_reused)
      delete[] d_data;

    d_data=NULL;
    d_dataptr_reused=true;
  }

  template <class Pel>  void Bitmap<Pel>::AttachBitmapProvider(BitmapProvider<Pel>* p)
  {
    if (p==d_parent)
      return;


    // Lock new bitmap-provider

    if (p)
      p->IncrRef();


    Release();
    assert(d_parent==NULL);


    // Set new bitmap-provider.

    d_parent = p;

    if (p)
      {
	d_data=p->AskFrame();
	d_dataptr_reused = true;

	d_width  = p->AskLogicalWidth();
	d_height = p->AskLogicalHeight();
	d_internal_width  = p->AskWidth();
	d_internal_height = p->AskHeight();
	d_total_width  = p->AskTotalWidth();
	d_total_height = p->AskTotalHeight();
	d_border = p->AskBorder();
      }
  }

  template <class Pel> Bitmap<Pel> Bitmap<Pel>::operator=(const Bitmap<Pel>& pm)
  {
    if (this == &pm)
      return *this;


    // Lock new bitmap-provider

    if (pm.d_parent)
      pm.d_parent->IncrRef();


    Release();


    // Set new bitmap-provider.

    d_parent = pm.d_parent;


    // Copy bitmap-access data.

    if (d_parent)
      {
	d_width  = pm.AskWidth();
	d_height = pm.AskHeight();
	d_border = pm.AskBorder();
	d_internal_width  = pm.d_internal_width;
	d_internal_height = pm.d_internal_height;
	d_total_width  = pm.d_total_width;
	d_total_height = pm.d_total_height;

	if (pm.d_dataptr_reused)
	  {
	    d_data = pm.d_data;
	    d_dataptr_reused=true;
	  }
	else
	  {
	    d_data = new Pel*[d_total_height];
	    for (int y=0;y<d_total_height;y++)
	      d_data[y] = pm.d_data[y];

	    d_dataptr_reused = false;
	  }
      }

    return *this;
  }

  template <class Pel> Bitmap<Pel> Bitmap<Pel>::CreateSubView(int x0,int y0,int w,int h) const
  {
    if (d_parent==NULL)
      return Bitmap<Pel>();

    AssertDescr(!(x0<0 ||
		  y0<0 ||
		  x0+w > d_internal_width ||
		  y0+h > d_internal_height),
		"sub-view range not within bitmap");

    Bitmap<Pel> pm;

    pm.d_parent = d_parent;
    pm.d_width = w;
    pm.d_height = h;
    pm.d_border = 0;
    pm.d_internal_width  = w;
    pm.d_internal_height = h;
    pm.d_total_width  = w;
    pm.d_total_height = h;
    pm.d_dataptr_reused = false;
    pm.d_data = new Pel* [h];

    for (int y=0;y<h;y++)
      pm.d_data[y] = &d_data[y+y0+d_border][x0];

    d_parent->IncrRef();

    return pm;
  }


  template <class Pel> Bitmap<Pel> Bitmap<Pel>::CreateFieldView(bool top) const
  {
    if (d_parent==NULL)
      return Bitmap<Pel>();

    Bitmap<Pel> pm;

    int newborder = d_border/2; // left/right border are the same, but at top/bottom they are half size
    if (d_internal_height%2 && d_border%2 && !top) newborder++;

    int newheight,newintheight;
    int firstline;
    if (top)
      {
	newheight    = (d_height+1)/2;
	newintheight = (d_internal_height+1)/2;
	firstline = d_border-2*newborder;
      }
    else
      {
	newheight    = d_height/2;
	newintheight = d_internal_height/2;
	firstline = d_border+1 -2*newborder;
      }

    pm.d_parent = d_parent;
    pm.d_width = d_width;
    pm.d_height = newheight;
    pm.d_border = newborder;
    pm.d_internal_width  = d_internal_width;
    pm.d_internal_height = newintheight;
    pm.d_total_width  = d_total_width;
    pm.d_total_height = newintheight+ 2*newborder;
    pm.d_dataptr_reused = false;
    pm.d_data = new Pel* [pm.d_total_height];

    for (int y=0;y<pm.d_total_height;y++)
      {
	pm.d_data[y] = d_data[2*y+firstline];
      }

    d_parent->IncrRef();

    return pm;
  }

  template <class Pel> Bitmap<Pel> Bitmap<Pel>::Clone(int border,int halign,int valign) const
  {
    assert(halign>=1);
    assert(valign>=1);

    Bitmap<Pel> pm;

    // cloning an empty bitmap creates an empty bitmap
    if (!d_parent)
      return pm;


    // use old border if none has been specified
    if (border<0) border = d_border;

    // create new bitmap with in a new memory area
    pm.AttachBitmapProvider(new BitmapProvider_Mem<Pel>(d_width,d_height,border, halign,valign));


    // copy old bitmap content into new bitmap
    int minwidth  = std::min(d_total_width , pm.AskTotalWidth() );
    int minheight = std::min(d_total_height, pm.AskTotalHeight());
    int minborder = std::min(d_border, pm.AskBorder());

    const Pel*const* src = AskFrame();
    Pel*const* dst = pm.AskFrame();

    for (int y=0;y<minheight;y++)
      memcpy(&dst[y-minborder][-minborder],
	     &src[y-minborder][-minborder],
	     minwidth*sizeof(Pel));

    return pm;
  }

  template <class Pel> inline int Bitmap<Pel>::AskWidth()  const
  {
    AssertDescr(d_parent,"no bitmap-provider attached to bitmap");
    return d_internal_width;
  }

  template <class Pel> inline int Bitmap<Pel>::AskHeight() const
  {
    AssertDescr(d_parent,"no bitmap-provider attached to bitmap");
    return d_internal_height;
  }

  template <class Pel> inline int Bitmap<Pel>::AskLogicalWidth() const
  {
    AssertDescr(d_parent,"no bitmap-provider attached to bitmap");
    return d_width;
  }

  template <class Pel> inline int Bitmap<Pel>::AskLogicalHeight() const
  {
    AssertDescr(d_parent,"no bitmap-provider attached to bitmap");
    return d_height;
  }

  template <class Pel> inline int Bitmap<Pel>::AskTotalWidth() const
  {
    AssertDescr(d_parent,"no bitmap-provider attached to bitmap");
    return d_total_width;
  }

  template <class Pel> inline int Bitmap<Pel>::AskTotalHeight() const
  {
    AssertDescr(d_parent,"no bitmap-provider attached to bitmap");
    return d_total_height;
  }

  template <class Pel> inline int Bitmap<Pel>::AskBorder() const
  {
    AssertDescr(d_parent,"no bitmap-provider attached to bitmap");
    return d_border;
  }

  template <class Pel> inline Pel*const* Bitmap<Pel>::AskFrame()
  {
    AssertDescr(d_parent,"no bitmap-provider attached to bitmap");
    return &d_data[d_border];
  }

  template <class Pel> inline const Pel*const* Bitmap<Pel>::AskFrame() const
  {
    AssertDescr(d_parent,"no bitmap-provider attached to bitmap");
    return &d_data[d_border];
  }

}

#endif
