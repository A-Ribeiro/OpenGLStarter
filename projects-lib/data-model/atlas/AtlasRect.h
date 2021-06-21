#ifndef atlas_rect__h__
#define atlas_rect__h__

#include <aribeiro/aribeiro.h>
#include <vector>

namespace DataModel {
    
    /// \brief Rectangle reference to be used in an #Atlas image.
    ///
    /// The origin of x,y coords are in top left corner of the image.
    ///
    /// \author Alessandro Ribeiro
    ///
    class AtlasRect{
    public:
        int x;///< origin x
        int y;///< origin y
        int w;///< width
        int h;///< height

        /// \brief Constructor with no arguments
        ///
        /// Initialize this rect with: `x = y = w = h = 0`.
        /// 
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// #include <data-model/data-model.h>
        /// using namespace aRibeiro;
        /// using namespace DataModel;
        ///
        /// AtlasRect rect = AtlasRect();
        ///
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        AtlasRect();

        /// \brief Full argument constructor
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// #include <data-model/data-model.h>
        /// using namespace aRibeiro;
        /// using namespace DataModel;
        ///
        /// int x,y,width,height;
        ///
        /// AtlasRect rect = AtlasRect( x, y, width, height );
        ///
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param _x origin left
        /// \param _y origin top
        /// \param _w width
        /// \param _h height
        ///
        AtlasRect(int _x,int _y,int _w,int _h);

        /// \brief Width/Height constructor
        ///
        /// The origin (x,y) are set to 0.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// #include <data-model/data-model.h>
        /// using namespace aRibeiro;
        /// using namespace DataModel;
        ///
        /// int width,height;
        ///
        /// AtlasRect rect = AtlasRect( width, height );
        ///
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param _w width
        /// \param _h height
        ///
        AtlasRect(int _w,int _h);

        /// \brief Set the origin x and y of this rect
        ///
        /// Can be used to reposition this rectangle inside an #Atlas in one method.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// #include <data-model/data-model.h>
        /// using namespace aRibeiro;
        /// using namespace DataModel;
        ///
        /// AtlasRect rect = ...;
        ///
        /// int x,y;
        ///
        /// rect.setXY( x, y );
        ///
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param _x origin left
        /// \param _h origin top
        ///
        void setXY(int _x,int _y);

        /// \brief Compute the inside right corner of this rectangle
        ///
        /// The value contains the last valid column of this rectangle.
        ///
        /// Uses the equation: `maxX = x + w - 1`.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// #include <data-model/data-model.h>
        /// using namespace aRibeiro;
        /// using namespace DataModel;
        ///
        /// AtlasRect rect = ...;
        ///
        /// int lastValidColumn;
        ///
        /// lastValidColumn = rect.maxXInclusive();
        ///
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \return the last valid column of this rectangle
        ///
        int maxXInclusive()const;

        /// \brief Compute the inside bottom corner of this rectangle
        ///
        /// The value contains the last valid row of this rectangle.
        ///
        /// Uses the equation: `maxY = y + h - 1`.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// #include <data-model/data-model.h>
        /// using namespace aRibeiro;
        /// using namespace DataModel;
        ///
        /// AtlasRect rect = ...;
        ///
        /// int lastValidRow;
        ///
        /// lastValidRow = rect.maxYInclusive();
        ///
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \return the last valid row of this rectangle
        ///
        int maxYInclusive()const;

        /// \brief Test if this rectangle overlaps another rectangle.
        ///
        /// It consideres the space between the rectangle from the parameters xspacing and yspacing.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// #include <data-model/data-model.h>
        /// using namespace aRibeiro;
        /// using namespace DataModel;
        ///
        /// AtlasRect rectA = ...;
        /// AtlasRect rectB = ...;
        ///
        /// ...
        ///
        /// int spacing_between_rects = 8;
        /// if ( rectA.overlaps(rectB, spacing_between_rects, spacing_between_rects) ){
        ///     ...
        /// }
        ///
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param other The #AtlasRect to check overlap
        /// \param xspacing Horizontal spacing to consider
        /// \param yspacing Vertical spacing to consider
        /// \return true if this rect overlaps the other parameter.
        ///
        bool overlaps(const AtlasRect &other, int xspacing = 5, int yspacing = 5)const;

        /// \brief Test if this rectangle is inside another rectangle.
        ///
        /// It consideres the space between the rectangle from the parameters xspacing and yspacing.
        ///
        /// It can be used to check if a rectangle is inside the #Atlas area or screen area.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// #include <data-model/data-model.h>
        /// using namespace aRibeiro;
        /// using namespace DataModel;
        ///
        /// AtlasRect rect = ...;
        /// AtlasRect rectAtlas = ...;
        ///
        /// ...
        ///
        /// int spacing_between_rects = 8;
        /// if ( rectA.inside(rectAtlas, spacing_between_rects/2, spacing_between_rects/2) ){
        ///     ...
        /// }
        ///
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param other The #AtlasRect to check if this is inside it
        /// \param xspacing Horizontal spacing to consider
        /// \param yspacing Vertical spacing to consider
        /// \return true if this rect is inside the other parameter.
        ///
        bool inside(const AtlasRect &other, int xspacing = 5, int yspacing = 5)const;
        
        /// \brief Write this rectangle to a #aRibeiro::BinaryWriter
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// #include <data-model/data-model.h>
        /// using namespace aRibeiro;
        /// using namespace DataModel;
        ///
        /// AtlasRect rect = ...;
        ///
        /// ...
        ///
        /// BinaryWriter writer;
        /// writer.writeToFile("file.output");
        ///
        /// rect.write(&writer);
        ///
        /// writer.close();
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param writer The #aRibeiro::BinaryWriter instance
        ///
        void write(aRibeiro::BinaryWriter *writer)const;

        /// \brief Read this rectangle from a #aRibeiro::BinaryReader
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// #include <data-model/data-model.h>
        /// using namespace aRibeiro;
        /// using namespace DataModel;
        ///
        /// AtlasRect rect = ...;
        ///
        /// ...
        ///
        /// BinaryReader reader;
        /// reader.readFromFile("file.input");
        ///
        /// rect.read(&reader);
        ///
        /// reader.close();
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param reader The #aRibeiro::BinaryReader instance
        ///
        void read(aRibeiro::BinaryReader *reader);
    };
    
}

#endif
