/*********************************************************************
  unified_writer.hh

  purpose:
    Write image in arbitrary formats.

  notes:

  to do:

  author(s):
   - Dirk Farin, dirk.farin@gmx.de

  modifications:
   05/Apr/2002 - Dirk Farin - first implementation
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

#ifndef LIBVIDEOGFX_GRAPHICS_FILEIO_UNIFIED_WRITER_HH
#define LIBVIDEOGFX_GRAPHICS_FILEIO_UNIFIED_WRITER_HH

#include <libvideogfx/graphics/fileio/base.hh>
#include <libvideogfx/graphics/fileio/unified_reader.hh>

namespace videogfx {

  /* A WriterStage implements a file writer or an image filter.
   * WriterStages can be concatenated to a chain of filters vis SetPrevious().
   */
  class WriterStage
  {
  public:
    WriterStage() : next(NULL) { }
    virtual ~WriterStage() { if (next) delete next; }

    /* Append a stage at the end of this pipeline. */
    void AppendAtEnd(WriterStage* next);

    virtual void WriteImage(const Image<Pixel>&) = 0;

  protected:
    WriterStage* next;
  };


  /* Allocate and configure a WriterStage based on a specification string.
     If the WriterStageFactory object recognizes the first option in the
     specification string, it removes these options and returns a
     corresponding WriterStage. Otherwise, it returns NULL and leaves
     the specification string unmodified.

     The Factory is a singleton class. Derived classes should not be exported
     and exactly one object should be allocated. This object will register
     itself as plugin.
  */
  class WriterStageFactory
  {
  public:
    WriterStageFactory();
    virtual ~WriterStageFactory() { }

    /* Parse the specification. If the loader factory can handle it, it removes
       the option from the specification and appends it to the plugin pipeline. */
    virtual WriterStage* ParseSpec(char** spec) const = 0;
    virtual const char* AskName() const { return "noname"; }
  };


#define MAX_WRITER_PLUGINS 100

  /* A universal writer that uses a specification string to build a writer pipeline.
     The available loaders can be extended with plugins that implement a WriterStageFactory.
   */
  class UnifiedImageWriter : public ImageWriter
  {
  public:
    UnifiedImageWriter() : d_writer_pipeline(NULL) { }
    ~UnifiedImageWriter() { if (d_writer_pipeline) delete d_writer_pipeline; }

    bool SetOutput(const char* output_specification);

    // usage

    void WriteImage(const Image<Pixel>&);

    // plugin handling

    static const char* AskPluginName(int idx); // returns name of plugin or NULL if idx exceeds the number of plugins

  private:
    static void RegisterPlugin(const WriterStageFactory*);
    friend class WriterStageFactory;

    WriterStage* d_writer_pipeline;

    static const WriterStageFactory* s_plugins[MAX_WRITER_PLUGINS];
    static int s_nplugins;
  };
}

#endif
