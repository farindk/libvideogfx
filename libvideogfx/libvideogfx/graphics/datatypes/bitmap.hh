/********************************************************************************
  $Header$
*/
/*
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

  template <class Pel> class BitmapProvider;

  /** Bitmap data-type definition.
      A Bitmap can be used for any two-dimensional image data using
      arbitrary types for the pixels. In that sense, a Bitmap is nothing
      but a two-dimensional array. What makes the Bitmap special is
      <ul>
      <li>automatic reference-counter based memory handling, making
          copying efficient,
      <li>transparent alignment of bitmap sizes to integer multiples
          of some constants (e.g., 16 pixels to simplify MPEG coders),
      <li>transparent border around the bitmap to allow algorithms,
          that consider a context around a current pixel, to ignore
          boundary conditions,
      <li>multiple views into a single bitmap, to use a small part
          of a bitmap like a bitmap of its own, or to have acces to
          interlaced fields just like full-resolution frames (which
          for example simplifies MPEG field-MC),
      <li>ability to wrap fixed memory ranges (like your gfx-card
          memory) into a Bitmap (not written yet).
      </ul>

      Internally, Bitmap is only the \em envelope class to the \em letter
      class BitmapProvider, which contains the actual data. When a bitmap
      is copied, the data is not copied! Instead, copying will result in
      a second Bitmap-envelope object using the same bitmap data. Changes
      made to one bitmap will hence also influence the other. If you want
      to make an independent copy of your bitmap, use Clone() instead.

      Usually, a Bitmap is created in main memory by specifying its size
      and optionally alignment and a surrounding border.
      For special purpose applications, BitmapProviders can be defined which
      wrap the Bitmap-API around existing memory blocks.

      When creating bitmaps in main memory, optionally to the bitmap size, a
      border and horizontal, vertical alignments can be specified. The border
      is a safety memory around the actual bitmap data. I.e., you may access
      bitmap content if indexes in the range \f$[-border; width+border[\f$.
      An alignment rounds the bitmap up to a multiple of the alignment factor
      (but not necessarily the next multiple). Be warned that you may get
      differing values for border and alignment back from the bitmap class.
      However, these will never be smaller than the values you specified.
      Horizontal border sizes are added before doing the alignment. Hence,
      the total width and stride (number of data elements, usually bytes,
      between two lines) will be a multiple of the horizontal alignment.
      \image latex bitmap.eps "bitmap border, alignments and corresponding sizes", width=8cm
      \image html bitmap.png  "bitmap border, alignments and corresponding sizes"
      The alignment layout and the involved widths (height are named similarly) are
      depicted in the above figure. The border size is aligned to the horizontal
      alignment factor only. Vertical border size is not increased.
      Note that even though the border is here shown
      as beginning behind the alignment at the right and bottom image border, the
      usage convention is that the border area starts \em immediately beneath the
      image area on all four sides. Also note that you can access all image data
      in the border and the alignment area just like normal bitmap area.
      The total image size that you may access is AskTotalWidth() times
      AskTotalHeight(). Do not modify the more even if AskStride() is larger.
	
      A Bitmap is not intended as standard image storage (even for
      greyscale images). Instead, use the Image class, which adds more
      information to the Bitmaps like the used colorspace.
      
      BTW., this class could not be named Pixmap since this collides with X11.
  */
  template <class Pel> class Bitmap
  {
  public:
    Bitmap(); ///< Create an empty bitmap. No provider is attached.
    Bitmap(const Bitmap&); /**< Copy-constructor. Note that image content is not
			      copied. Both bitmaps will share the same memory. */
    /// Create a new bitmap in main memory.
    Bitmap(int w,int h,int border=0,int halign=1,int valign=1);
    /// Initialize a new bitmap based on the full data of the specified provider.
    Bitmap(BitmapProvider<Pel>*);
    /** Destructor. If this was the last reference to the bitmap provider
	it is destroyed. */
    ~Bitmap();

    /** Create a new bitmap. The old bitmap data is lost. If the new bitmap
	fits into the old bitmap memory space and it is not shared, the old
	space is reused. This is much faster than recreating a new bitmap. */
    void Create(int w,int h,int border=0,int halign=1,int valign=1);

    /** Detach bitmap content from the object. If this was the
	last handle to the bitmap data, the provider will be detroyed. */
    void Release();

    /** Attach the full data of a new provider. The provider may be NULL,
	which will result in a empty bitmap. */
    void AttachBitmapProvider(BitmapProvider<Pel>*);

    /// Make a copy of the specified bitmap by sharing the data.
    Bitmap<Pel> operator=(const Bitmap<Pel>&);

    /** Create a bitmap that shares only part of the current bitmap.
	The created bitmap will have no border and no alignment. */
    Bitmap<Pel> CreateSubView  (int x0,int y0,int w,int h) const;

    /** Create a bitmap that shares only a single field from the
	current (frame based) bitmap. In the created bitmap, the
	field lines are accessed using consecutive lines, instead of
	offsets of two. The border sizes an alignment sizes are
	adapted correspondingly. */
    Bitmap<Pel> CreateFieldView(bool top) const;

    /** Copy the bitmap into a new, independent memory area. Alignments can be changed
	on the fly. If border<0, the old border size is used for the new bitmap. */
    Bitmap<Pel> Clone(int border=-1,int halign=1,int valign=1) const;

    int AskWidth()  const;         ///< Width of bitmap excluding border and alignments.
    int AskHeight() const;         ///< Height of bitmap excluding border and alignments.
    int AskAlignedWidth() const;   ///< Width of bitmap including alignments.
    int AskAlignedHeight() const;  ///< Height of bitmap including alignments.
    int AskTotalWidth() const;     ///< Width of bitmap including borders and alignments.
    int AskTotalHeight() const;    ///< Height of bitmap including borders and alignments.
    int AskBorder() const;         ///< Border width (as requested).
    int AskAlignedBorder() const;  ///< Actual border width including alignments.
    /** Bitmap stride (number of data elements to get from one line to the next).
	Note that this is not necessarily equal to AskTotalWidth() since a SubView or
	FieldView bitmap has additional data between its lines.
    */
    int AskStride() const { return AskFrame()[1]-AskFrame()[0]; }

    /// Check if bitmap has any data associated with it.
    bool IsEmpty() const { return d_parent==NULL; }

    /** Get a pointer-array to the actual bitmap data ([y][x]). Pixel [0][0] is
	the top-left pixel _inside_ the image. I.e. if the bitmap has a border
	around it, the border pixels can be access by negative positions or with
	x,y coordinates above AskWidth()/AskHeight(). This method comes
	additionally in a const flavour.
    */
    Pel*const* AskFrame();
    /** Const flavour of the non-const AskFrame(). Returns same value, but
	bitmap data may not be modified.
     */
    const Pel*const* AskFrame() const;

    /// Return true iff the bitmap data is also used by another Bitmap object.
    bool IsShared() const
    {
      if (!d_parent) return false;
      if (d_parent->RefCntr()>1) return true;
      return false;
    }


    /** Set global defaults for bitmap alignment. Every new bitmap created by a
	BitmapProvider that supports user-defined alignment will be aligned to
	at least these defaults. I.e. if you globally specify a border of 4 and
	an alignment of 3, and a bitmap should be created with a border of 2
	and am alignment of 4, the bitmap will have a border of size 4 and an
	alignment of size 12.
    */
    static void SetAlignmentDefaults(int border,int halign,int valign);
    /// Ask global alignment settings.
    static void AskAlignmentDefaults(int& border,int& halign,int& valign);

  private:
    BitmapProvider<Pel>* d_parent;

    /* The following size values cannot be taken from d_parent, since a
       Bitmap can also be a sub-view into a larger bitmap.
    */

    int d_width;
    int d_height;
    int d_border;
    int d_aligned_border;
    int d_aligned_width;
    int d_aligned_height;
    int d_total_width;
    int d_total_height;

    /* d_data is the line-pointer array to the actual bitmap data. This pointer
       can come in two variations. If the Bitmap is simply the whole bitmap data
       (and no sub-view), d_data is set to the line-pointer array of the provider
       object. Since this is the most common case, it is worth to provide a special
       case for it.
       If the Bitmap is a sub-view into the bitmap data, an array of its own is
       allocated and filled with custom pointers. In this case, d_dataptr_reused is
       set to false and d_data must be freed in the destructor. Note that a reuse
       of sub-view pointer-arrays is not possible.
    */
    Pel** d_data;
    bool  d_dataptr_reused;


    static int default_align_border;
    static int default_align_halign;
    static int default_align_valign;
  };


  /** Bitmap data-provider abstract base-class.
      Letter class of the envelope/letter design. See class Bitmap for
      details. This class is not functionally as-is, you have to derive
      it to make a specific implementation.
      Each derived class is responsible for setting all the protected attributes
      and has to call SetFramePtrs() afterwards.

      The basic architecture is that the derived object provides a pointer to
      the memory area and the memory layout. Based on this, this base-class
      creates an array with pointers to the beginning of the individual lines.
      Using this pointer-array, the bitmap data can be accessed like a normal
      two-dimensional array.
   */
  template <class Pel> class BitmapProvider
  {
  public:
    /// create bitmap-provider without any data
    BitmapProvider();
    /// checks that the provider is not used any more and destroys the line-pointer array
    virtual ~BitmapProvider();

    /// see Bitmap
    int AskWidth()  const { return d_width; }
    /// see Bitmap
    int AskHeight() const { return d_height; }
    /// see Bitmap
    int AskAlignedWidth()  const { return d_aligned_width; }
    /// see Bitmap
    int AskAlignedHeight() const { return d_aligned_height; }
    /// see Bitmap
    int AskTotalWidth()  const { return d_total_width; }
    /// see Bitmap
    int AskTotalHeight() const { return d_total_height; }
    /// see Bitmap
    int AskBorder() const { return d_border; }
    /// see Bitmap
    int AskAlignedBorder() const { return d_aligned_border; }

    /// see Bitmap
    Pel** AskFrame()       { return d_frame_ptr; }
    /// see Bitmap
    const Pel** AskFrame() const { return d_frame_ptr; }

    /// increase reference counter of this letter object
    void IncrRef() { d_ref_cntr++; }
    /// decrease reference counter of this letter object
    void DecrRef() { d_ref_cntr--; assert(d_ref_cntr>=0); }
    /// get current reference counter value
    int  RefCntr() const { return d_ref_cntr; }

  private:
    int d_ref_cntr;

    Pel** d_frame_ptr;

  protected:
    /* Each derived class has to set all these values and
       call SetFramePtrs() afterwards. */

    int  d_width;  ///< logical width of the bitmap (excluding borders and alignments)
    int  d_height; ///< logical height of the bitmap (excluding borders and alignments)
    int  d_border; ///< size of border around the actual image data

    int  d_aligned_width;   ///< width of bitmap area inclusive alignments
    int  d_aligned_height;  ///< height of bitmap area inclusive alignments
    int  d_aligned_border;  ///< size of border inclusive alignments

    int  d_total_width;  ///< total width of the bitmap data area
    int  d_total_height; ///< total height of the bitmap data area

    Pel* d_bitmap_ptr;  ///< pointer to the very beginning of the bitmap data

    /** After setting all of the protected data members, the derived
	class has to call this method. The method will then create the
	array with pointers to the beginning of each line.
    */
    void SetFramePtrs();
  };


  inline void CalcAlignedSizes(int w,int h,                       // original bitmap size
			       int border,int halign,int valign,  // alignments
			       int& intw,int& inth,int& intb)     // aligned sizes
  {
    assert(border>=0);
    assert(halign>=1);
    assert(valign>=1);

    // integrate default alignments into alignment specification

    int def_border,def_halign,def_valign;
    Bitmap<class Pel>::AskAlignmentDefaults(def_border, def_halign, def_valign);

    border = std::max(border,def_border);
    halign = LeastCommonMultiple(halign,def_halign);
    valign = LeastCommonMultiple(valign,def_valign);


    // increase size of internal bitmap area

    intw = AlignUp(w,      halign);
    inth = AlignUp(h,      valign);
    intb = AlignUp(border, halign); // align border to halign to provide aligned memory access
  }


  /** A bitmap provider that allocates main memory to store the bitmap in.
   */
  template <class Pel> class BitmapProvider_Mem : public BitmapProvider<Pel>
  {
  public:
    /// create provider without allocating memory
    BitmapProvider_Mem() { }

    /// create the provider and allocate bitmap memory accordings to specified parameters
    BitmapProvider_Mem(int w,int h,int border=0,int halign=1,int valign=1)
    {
      Create(w,h,border,halign,valign);
    }

    /// the destructor frees the bitmap-data memory (if previously allocated)
    ~BitmapProvider_Mem() { if (d_bitmap_ptr) delete[] d_bitmap_ptr; }

    /// Create a new bitmap-data in memory, old data is never reused.
    void Create(int w,int h,int border=0,int halign=1,int valign=1)
    {
      d_width  = w;
      d_height = h;
      d_border = border;

      CalcAlignedSizes(w,h,border,halign,valign,
		       d_aligned_width,d_aligned_height,d_aligned_border);

      // total size including border

      d_total_width  = d_aligned_width +2*d_aligned_border;
      d_total_height = d_aligned_height+2*d_border;

      if (d_bitmap_ptr)
	delete[] d_bitmap_ptr;

      d_bitmap_ptr = new Pel[d_total_width * d_total_height];
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
      {
	d_frame_ptr[i] = d_bitmap_ptr + i*d_total_width + d_aligned_border;
      }
  }


  // ---------------------- Bitmap implementation --------------------

  template <class Pel> Bitmap<Pel>::Bitmap()
    : d_parent(NULL),
      d_data(NULL),
      d_dataptr_reused(true)  // could be any of both
  {
  }

  template <class Pel> Bitmap<Pel>::Bitmap(int w,int h,int border,int halign,int valign)
    : d_parent(NULL),
      d_data(NULL),
      d_dataptr_reused(true)  // could be any of both
  {
    AttachBitmapProvider(new BitmapProvider_Mem<Pel>(w,h,border,halign,valign));
  }

  template <class Pel> Bitmap<Pel>::Bitmap(BitmapProvider<Pel>* p)
    : d_parent(NULL),
      d_data(NULL),
      d_dataptr_reused(true)  // could be any of both
  {
    AttachBitmapProvider(p);
  }

  template <class Pel> Bitmap<Pel>::Bitmap(const Bitmap& pm)
  {
    d_parent = pm.d_parent;  // use same provider

    if (d_parent)
      {
	d_parent->IncrRef();  // we are now also using the provider


	// make copy of all size parameters

	d_width = pm.d_width;
	d_height = pm.d_height;
	d_border = pm.d_border;
	d_aligned_width  = pm.d_aligned_width;
	d_aligned_height = pm.d_aligned_height;
	d_aligned_border = pm.d_aligned_border;
	d_total_width  = pm.d_total_width;
	d_total_height = pm.d_total_height;


	if (pm.d_dataptr_reused)
	  {
	    // if line-pointer array was used from the provider, we can also do so

	    d_data = pm.d_data;
	    d_dataptr_reused = true;
	  }
	else
	  {
	    // else, we have to copy to line-pointer array

	    d_data = new Pel* [d_total_height];
	    for (int y=0;y<d_total_height;y++)
	      d_data[y] = pm.d_data[y];

	    d_dataptr_reused = false;
	  }
      }
    else
      {
	// in case "pm" is an empty bitmap

	d_data=NULL;
	d_dataptr_reused=true;
      }
  }

  template <class Pel> Bitmap<Pel>::~Bitmap()
  {
    Release();
  }

  template <class Pel> void Bitmap<Pel>::Create(int w,int h,
						int border,int halign,int valign)
  {
    bool compatible=false;  // can use old memory bitmap without reallocation
    int intw,inth,intb;     // aligned sizes

    if (d_parent)
      {
	compatible=true;

	// let's see what our requirements are
	CalcAlignedSizes(w,h,border,halign,valign,
			 intw,inth,intb);

	// new bitmap memory has to fit into old bitmap
	if (d_total_width  < intw+2*intb)   compatible=false;
	if (d_total_height < inth+2*border) compatible=false;

	// old bitmap must be suitably aligned
	if ((d_total_width  - (intw+2*intb  )) % halign) compatible=false;
	if ((d_total_height - (inth+2*border)) % valign) compatible=false;

	/* If new border is larger, there is not enough space to the left/top.
	   Well, we could move all line pointers forward, but this infrequent
	   case is not worth the additional code.
	*/
	if (intb   > d_aligned_border) compatible=false;  // horizontal
	if (border > d_border)         compatible=false;  // vertical

	// cannot reuse if bitmap is still shared
	if (d_parent->RefCntr()>1)
	  compatible = false;

	// can only reuse if bitmap was stored in main memory
	if (dynamic_cast<BitmapProvider_Mem<Pel>*>(d_parent)==0)
	  compatible=false;

	// We don't reuse if this is a sub-view. Too much memory could be wasted.
	if (!d_dataptr_reused)
	  compatible = false;
      }

    if (compatible)
      {
	d_width  = w;
	d_height = h;
	d_border = border;
	d_aligned_width  = intw;
	d_aligned_height = inth;
	d_aligned_border = intb;
	d_total_width  = d_aligned_width  + 2*d_aligned_border;
	d_total_height = d_aligned_height + 2*d_border;
      }
    else
      {
	AttachBitmapProvider(new BitmapProvider_Mem<Pel>(w,h,border,halign,valign));
      }
  }

  template <class Pel> void Bitmap<Pel>::Release()
  {
    // Release bitmap-provider and destroy it if this was the last reference.

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
    // Already using this provider.
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

	d_width  = p->AskWidth();
	d_height = p->AskHeight();
      	d_border = p->AskBorder();
	d_aligned_width  = p->AskAlignedWidth();
	d_aligned_height = p->AskAlignedHeight();
	d_aligned_border = p->AskAlignedBorder();
	d_total_width  = p->AskTotalWidth();
	d_total_height = p->AskTotalHeight();
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
	d_aligned_width  = pm.d_aligned_width;
	d_aligned_height = pm.d_aligned_height;
	d_aligned_border = pm.AskAlignedBorder();
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
		  x0+w > d_aligned_width ||
		  y0+h > d_aligned_height),
		"sub-view range not within bitmap");

    Bitmap<Pel> pm;

    // sub-view size is exactly the requested area, without any borders or alignments

    pm.d_parent = d_parent;
    pm.d_width = w;
    pm.d_height = h;
    pm.d_border = 0;
    pm.d_aligned_width  = w;
    pm.d_aligned_height = h;
    pm.d_aligned_border = 0;
    pm.d_total_width  = w;
    pm.d_total_height = h;

    // create a line-pointer array for the sub-view

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
    if (d_aligned_height%2 && d_border%2 && !top) newborder++;

    int newheight,newintheight;
    int firstline;
    if (top)
      {
	newheight    = (d_height+1)/2;
	newintheight = (d_aligned_height+1)/2;
	firstline = d_border-2*newborder;
      }
    else
      {
	newheight    = d_height/2;
	newintheight = d_aligned_height/2;
	firstline = d_border+1 -2*newborder;
      }

    pm.d_parent = d_parent;
    pm.d_width = d_width;
    pm.d_height = newheight;
    pm.d_border = newborder;
    pm.d_aligned_border = d_aligned_border;
    pm.d_aligned_width  = d_aligned_width;
    pm.d_aligned_height = newintheight;
    pm.d_total_width  = d_total_width;
    pm.d_total_height = newintheight+ 2*newborder;


    // create new line-pointer array

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

    // create new bitmap in a new memory area
    pm.AttachBitmapProvider(new BitmapProvider_Mem<Pel>(d_width,d_height,border, halign,valign));


    // copy old bitmap content into new bitmap
    int minwidth  = std::min(d_width , pm.AskWidth() );
    int minheight = std::min(d_height, pm.AskHeight());
    int minhborder = std::min(d_aligned_border, pm.AskAlignedBorder());
    int minvborder = std::min(d_border, pm.AskBorder());

    const Pel*const* src = AskFrame();
    Pel*const* dst = pm.AskFrame();

    for (int y=0;y<minheight +2*minvborder;y++)
      {
	memcpy(&dst[y-minvborder][-minhborder],
	       &src[y-minvborder][-minhborder],
	       (minwidth+2*minhborder)*sizeof(Pel));
      }

    return pm;
  }

  template <class Pel> inline int Bitmap<Pel>::AskWidth()  const
  {
    AssertDescr(d_parent,"no bitmap-provider attached to bitmap");
    return d_width;
  }

  template <class Pel> inline int Bitmap<Pel>::AskHeight() const
  {
    AssertDescr(d_parent,"no bitmap-provider attached to bitmap");
    return d_height;
  }

  template <class Pel> inline int Bitmap<Pel>::AskAlignedWidth() const
  {
    AssertDescr(d_parent,"no bitmap-provider attached to bitmap");
    return d_aligned_width;
  }

  template <class Pel> inline int Bitmap<Pel>::AskAlignedHeight() const
  {
    AssertDescr(d_parent,"no bitmap-provider attached to bitmap");
    return d_aligned_height;
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

  template <class Pel> inline int Bitmap<Pel>::AskAlignedBorder() const
  {
    AssertDescr(d_parent,"no bitmap-provider attached to bitmap");
    return d_aligned_border;
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

  template <class Pel> int Bitmap<Pel>::default_align_border=0;
  template <class Pel> int Bitmap<Pel>::default_align_halign=1;
  template <class Pel> int Bitmap<Pel>::default_align_valign=1;

  template <class Pel> void Bitmap<Pel>::SetAlignmentDefaults(int border,int halign,int valign)
  {
    assert(border>=0);
    assert(halign>=1);
    assert(valign>=1);

    default_align_border=border;
    default_align_halign=halign;
    default_align_valign=valign;
  }

  template <class Pel> void Bitmap<Pel>::AskAlignmentDefaults(int& border,int& halign,int& valign)
  {
    border = default_align_border;
    halign = default_align_halign;
    valign = default_align_valign;
  }

}

#endif
