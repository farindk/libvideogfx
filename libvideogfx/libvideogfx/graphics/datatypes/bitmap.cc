
#include <libvideogfx/graphics/datatypes/bitmap.hh>

namespace videogfx {

  static int default_align_border=0;
  static int default_align_halign=1;
  static int default_align_valign=1;

  void SetAlignmentDefaults(int border,int halign,int valign)
  {
    assert(border>=0);
    assert(halign>=1);
    assert(valign>=1);

    default_align_border=border;
    default_align_halign=halign;
    default_align_valign=valign;
  }

  void AskAlignmentDefaults(int& border,int& halign,int& valign)
  {
    border = default_align_border;
    halign = default_align_halign;
    valign = default_align_valign;
  }

}
