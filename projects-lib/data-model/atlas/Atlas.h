#ifndef atlas__h__
#define atlas__h__

#include <aribeiro/aribeiro.h>
#include <vector>

#include "AtlasRect.h"
#include "AtlasElement.h"

namespace DataModel {
    
    /// \brief Memory Bitmap Atlas Creator
    ///
    /// This class allow the creation of an atlas from raw RGBA buffers.
    /// 
    /// It can compute the distribution of all the small RGBA images in a single <br />
    /// power of two RGBA image.
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// #include <data-model/data-model.h>
    /// using namespace aRibeiro;
    /// using namespace DataModel;
    ///
    /// // Create an atlas with the space between elements of 8 pixels
    /// Atlas atlas( 8, 8 );
    ///
    /// // load RGBA buffers and create an element in the atlas
    /// // you can do this for all elements you want to add to this atlas
    /// {
    ///     char* rgbaElement;
    ///     int width, height;
    ///     AtlasElement* atlasElement = atlas.addElement("element",width,height);
    ///     atlasElement->copyFromRGBABuffer(rgbaElement, width * 4);
    /// }
    ///
    /// // after add all elements
    /// // compute the final atlas image
    /// atlas.organizePositions(true); // fast mode: true
    ///
    /// // export the atlas image
    /// atlas.savePNG("atlas.png");
    ///
    /// // export the atlas internal sprites definitions
    /// atlas.writeTable("atlas.bin");
    ///
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    ///
    class Atlas{
        
        //private copy constructores, to avoid copy...
        Atlas(const Atlas& v);
        void operator=(const Atlas& v);
        
        void resetPositions();

        bool colideWithAnyObject(AtlasElement* element, int maxIterate);

        std::vector<AtlasRect> computeInsertPositionArray(const AtlasRect &screen, int currY, int maxIterate);
        
        AtlasElement* findMinY(const AtlasRect &screen,
                               int currY,
                               int maxIterate);
        
        bool repositionAllElements(const AtlasRect &screen, bool fastMode);
        
        std::vector<AtlasElement*> elements;
        
        void clearElements();
    public:
        
        AtlasRect textureResolution;///< Will hold the image resolution that is needed to store the whole Atlas
        int xspacing;///< Horizontal in-between sprite space
        int yspacing;///< Vertical in-between sprite space
        
        /// \brief Atlas Creator Constructor
        ///
        /// Create an Atlas by defining the space between sprites you want <br />
        /// thie atlas to compute.
        ///
        /// Example:
        ///
        /// \author Alessandro Ribeiro
        /// \param xspacing Horizontal pixel spacing between sprites
        /// \param yspacing Vertical pixel spacing between sprites
        ///
        Atlas(int xspacing, int yspacing);
        virtual ~Atlas();
        
        /// \brief Add new atlas element internal reference into this Atlas
        ///
        /// \author Alessandro Ribeiro
        /// \param name Name of the sprite
        /// \param w Width
        /// \param h Height
        /// \return Pointer to a created AtlasElement
        ///
        AtlasElement* addElement(const std::string &name, int w, int h);
        
        /// \brief Organize the positions of the sprites in a single image.
        ///
        /// The RGBA buffers are not modified.
        ///
        /// To create the final RGBA image you need to call: #createRGBA or #createA.
        ///
        /// \author Alessandro Ribeiro
        /// \param fastMode Use the fast mode algorithm to find the best location do the registered sprites.
        ///
        void organizePositions(bool fastMode);
        
        /// \brief Create an image with all sprite elements inside an Atlas
        ///
        /// The RGBA buffers are not modified.
        ///
        /// You need to call this method after the #organizePositions.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// #include <data-model/data-model.h>
        /// using namespace aRibeiro;
        /// using namespace DataModel;
        ///
        /// ...
        ///
        /// atlas.organizePositions(true);
        ///
        /// uint8_t* image_RGBA = atlas.createRGBA();
        ///
        /// // use the atlas.textureResolution as image_RGBA resolution
        /// ...
        ///
        /// atlas.releaseRGBA(&image_RGBA);
        ///
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \return A pointer to an allocated image in RGBA format
        ///
        uint8_t * createRGBA()const;
        
        /// \brief Release the memory allocated with #createRGBA method
        ///
        /// \author Alessandro Ribeiro
        /// \param[out] data Pointer to the data you need to release
        ///
        void releaseRGBA(uint8_t **data)const;

        /// \brief Create an image with all sprite elements inside an Atlas (Just Alpha Channel)
        ///
        /// The RGBA buffers are not modified.
        ///
        /// You need to call this method after the #organizePositions.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// #include <data-model/data-model.h>
        /// using namespace aRibeiro;
        /// using namespace DataModel;
        ///
        /// ...
        ///
        /// atlas.organizePositions(true);
        ///
        /// uint8_t* image_A = atlas.createA();
        ///
        /// // use the atlas.textureResolution as image_A resolution
        /// ...
        ///
        /// atlas.releaseA(&image_A);
        ///
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \return A pointer to an allocated image in Alpha format
        ///
        uint8_t * createA()const;

        /// \brief Release the memory allocated with #createA method
        ///
        /// \author Alessandro Ribeiro
        /// \param[out] data Pointer to the data you need to release
        ///
        void releaseA(uint8_t **data)const;
        
        /// \brief Write the internal bitmap references table of this Atlas
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// #include <data-model/data-model.h>
        /// using namespace aRibeiro;
        /// using namespace DataModel;
        ///
        /// Atlas atlas( 8, 8 );
        ///
        /// ...
        ///
        /// BinaryWriter writer;
        /// writer.writeToFile("file.output");
        ///
        /// atlas.write(&writer);
        ///
        /// writer.close();
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param writer The #aRibeiro::BinaryWriter instance
        ///
        void write(aRibeiro::BinaryWriter *writer)const;

        /// \brief Read the internal bitmap references table of an Atlas saved with #write.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// #include <data-model/data-model.h>
        /// using namespace aRibeiro;
        /// using namespace DataModel;
        ///
        /// Atlas atlas( 8, 8 );
        ///
        /// ...
        ///
        /// BinaryReader reader;
        /// reader.readFromFile("file.input");
        ///
        /// atlas.read(&reader);
        ///
        /// reader.close();
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param reader The #aRibeiro::BinaryReader instance
        ///
        void read(aRibeiro::BinaryReader *reader);
        

        /// \brief Write the RGBA PNG image file of the Atlas
        ///
        /// \author Alessandro Ribeiro
        /// \param filename The filename you want to save the image of the Atlas
        ///
        void savePNG(const std::string &filename)const;

        /// \brief Write the Alpha(GrayScale) PNG image file of the Atlas
        ///
        /// \author Alessandro Ribeiro
        /// \param filename The filename you want to save the image of the Atlas
        ///
        void savePNG_Alpha(const std::string &filename)const;

        /// \brief Write the reference table of this Atlas
        ///
        /// \author Alessandro Ribeiro
        /// \param filename The filename you want to save the reference table of the Atlas
        ///
        void writeTable(const std::string &filename)const;
        
    };
    
}

#endif
