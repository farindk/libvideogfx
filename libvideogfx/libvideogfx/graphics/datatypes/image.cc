/********************************************************************************
    Copyright (C) 2002 Dirk Farin

    This program is distributed under GNU Public License (GPL) as
    outlined in the COPYING file that comes with the source distribution.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 ********************************************************************************/

#include <libvideogfx/graphics/datatypes/image.hh>

namespace videogfx {

  void ImageParam::AskChromaSizes(int& w,int &h) const
  {
    h = AskChromaHeight();
    w = AskChromaWidth();
  }

  int ImageParam::AskChromaBorder() const
  {
    if (chroma_border >= 0)
      return chroma_border;
    else
      return std::max((border+ChromaSubH(chroma)-1)/ChromaSubH(chroma),
		      (border+ChromaSubV(chroma)-1)/ChromaSubV(chroma));
  }

  int ImageParam::AskChromaHAlign() const
  {
    if (chroma_halign > 0)
      return chroma_halign;
    else if (halign==1)
      return 1;
    else
      {
	assert((halign % ChromaSubH(chroma))==0);
	return halign / ChromaSubH(chroma);
      }
  }

  int ImageParam::AskChromaVAlign() const
  {
    if (chroma_valign > 0)
      return chroma_valign;
    else if (valign==1)
      return 1;
    else
      {
	assert((valign % ChromaSubV(chroma))==0);
	return valign / ChromaSubV(chroma);
      }
  }

}
