#ifndef atlas_element__h__
#define atlas_element__h__

#include <aribeiro/aribeiro.h>
#include <vector>

#include "AtlasRect.h"

namespace DataModel {
    
    /// \brief Single element inside an #Atlas
    ///
    /// This class holds the RGBA of a single sprite when using for atlas creation.
    ///
    /// It could hold just the rectangle information and name of a single element of a already created atlas.
    ///
    /// \author Alessandro Ribeiro
    ///
    class AtlasElement{
        //private copy constructores, to avoid copy...
        AtlasElement(const AtlasElement& v);
        void operator=(const AtlasElement& v);
    public:
        AtlasRect rect;///< Rectangle of this element inside an #Atlas
        uint8_t *buffer;///< The RGBA raw buffer
        std::string name;///< The sprite name
        
        /// \brief Constructor with a width and height
        ///
        /// When this constructor is called, it will create the RGBA buffer with it.
        /// 
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// #include <data-model/data-model.h>
        /// using namespace aRibeiro;
        /// using namespace DataModel;
        ///
        /// AtlasElement* atlasElement = new AtlasElement(width, height);
        ///
        /// //it can be used the atlasElement->buffer to store this element buffer
        /// ...
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param w width
        /// \param h height
        ///
        AtlasElement(int w,int h);

        /// \brief Constructor
        ///
        /// This constructor does not create the RGBA buffer.
        /// 
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// #include <data-model/data-model.h>
        /// using namespace aRibeiro;
        /// using namespace DataModel;
        ///
        /// AtlasElement* atlasElement = new AtlasElement();
        ///
        /// //it can be used the atlasElement->read(...) to load element information
        /// ...
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param w width
        /// \param h height
        ///
        AtlasElement();
        
        virtual ~AtlasElement();
        
        /// \brief Read this single atlas internal rectangle and name references
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// #include <data-model/data-model.h>
        /// using namespace aRibeiro;
        /// using namespace DataModel;
        ///
        /// AtlasElement* atlasElement = new AtlasElement();
        ///
        /// ...
        ///
        /// BinaryReader reader;
        /// reader.readFromFile("file.input");
        ///
        /// atlasElement->read(&reader);
        ///
        /// reader.close();
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param reader The #aRibeiro::BinaryReader instance
        ///
        void read(aRibeiro::BinaryReader *reader);

        /// \brief Write this single atlas internal rectangle and name references
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// #include <data-model/data-model.h>
        /// using namespace aRibeiro;
        /// using namespace DataModel;
        ///
        /// AtlasElement* atlasElement = new AtlasElement( width, height );
        ///
        /// ...
        ///
        /// BinaryWriter writer;
        /// writer.writeToFile("file.output");
        ///
        /// atlasElement->write(&writer);
        ///
        /// writer.close();
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param writer The #aRibeiro::BinaryWriter instance
        ///
        void write(aRibeiro::BinaryWriter *writer)const;
        
        /// \brief Copy the RGBA from the parameter to the internal buffer.
        ///
        /// The input buffer need to be at least the size of the created width and height.
        ///
        /// This copy method do not verify the input buffer size.
        ///
        /// It just read the memory blocks and copy them.
        ///
        /// It could be used with PNGHelper to load bitmaps to this atlas element.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// #include <data-model/data-model.h>
        /// using namespace aRibeiro;
        /// using namespace DataModel;
        ///
        /// AtlasElement* atlasElement = new AtlasElement( width, height );
        ///
        /// ...
        ///
        /// uint8_t *raw_rgba_buffer = new [ width * height * 4 ];
        ///
        /// // can load bitmap image in the raw_rgba_buffer
        /// ...
        ///
        /// atlasElement->copyFromRGBABuffer(raw_rgba_buffer, width * 4);
        ///
        /// delete[] raw_rgba_buffer;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param src Source (input) buffer
        /// \param strideX The amount of bytes that need to skip 1 line in the src buffer
        ///
        void copyFromRGBABuffer(uint8_t *src, int strideX);

        /// \brief Copy the RGBA from internal buffer to the parameter.
        ///
        /// This method is meant to be used to copy this buffer to the final created #Atlas.
        ///
        /// It uses the rectangle reference information to write the RGBA buffer in the right region of the #Atlas.
        ///
        /// It does not check the output buffer size.
        ///
        /// It will fill the border of the bitmap according the xspacing and yspacing parameters.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// #include <data-model/data-model.h>
        /// using namespace aRibeiro;
        /// using namespace DataModel;
        ///
        /// AtlasElement* atlasElement = new AtlasElement( width, height );
        ///
        /// ...
        ///
        /// uint8_t *atlas_rgba_buffer = new [ atlas_width * atlas_height * 4 ];
        ///
        /// atlasElement->copyToRGBABuffer(atlas_rgba_buffer, atlas_width * 4, 8, 8 );
        ///
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param dst Target (output) buffer
        /// \param strideX The amount of bytes that need to skip 1 line in the dst buffer
        /// \param xspacing The spacing between the sprites (horizontal)
        /// \param yspacing The spacing between the sprites (vertical)
        ///
        void copyToRGBABuffer(uint8_t *dst, int strideX, int xspacing, int yspacing);

        /// \brief Copy the Alpha from internal buffer to the parameter.
        ///
        /// This method is meant to be used to copy this buffer to the final created #Atlas.
        ///
        /// It uses the rectangle reference information to write the Alpha buffer in the right region of the #Atlas.
        ///
        /// It does not check the output buffer size.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// #include <data-model/data-model.h>
        /// using namespace aRibeiro;
        /// using namespace DataModel;
        ///
        /// AtlasElement* atlasElement = new AtlasElement( width, height );
        ///
        /// ...
        ///
        /// uint8_t *atlas_a_buffer = new [ atlas_width * atlas_height ];
        ///
        /// atlasElement->copyToABuffer(atlas_rgba_buffer, atlas_width, 8, 8 );
        ///
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param dst Target (output) buffer
        /// \param strideX The amount of bytes that need to skip 1 line in the dst buffer
        /// \param xspacing The spacing between the sprites (horizontal)
        /// \param yspacing The spacing between the sprites (vertical)
        ///
        void copyToABuffer(uint8_t *dst, int strideX, int xspacing, int yspacing);
        
        /// \brief Save the RGBA internal buffer to a PNG file.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// #include <data-model/data-model.h>
        /// using namespace aRibeiro;
        /// using namespace DataModel;
        ///
        /// AtlasElement* atlasElement = new AtlasElement( width, height );
        ///
        /// ...
        ///
        /// atlasElement->savePNG("output.png");
        ///
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param filename PNG File to write to.
        ///
        void savePNG(const std::string& filename);

    };
    
}

#endif
