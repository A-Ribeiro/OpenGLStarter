
#ifndef BresenhamIterator_h
#define BresenhamIterator_h

#include <aribeiro/common.h>

namespace aRibeiro {

/// \brief Classical line iterator algorithm
///
/// Generate a raster of a line specified by two points.
///
/// Example:
///
/// \code
/// #include <aribeiro/aribeiro.h>
/// using namespace aRibeiro;
///
/// bool matrix[height][width];
/// int x, y, xd yd;
/// 
/// BresenhamIterator it(x, y, xd, yd);
/// 
/// it.getXY(&x, &y);
/// matrix[y][x] = true;
/// while (it.next(&x, &y)) {
///     matrix[y][x] = true;
/// }
/// \endcode
///
/// \author Alessandro Ribeiro
///
class BresenhamIterator{
    int deltaCol ,
        deltaRow ,
        fraction,
        nextCol ,
        nextRow ,
        endCol ,
        endRow ;
    int stepRow, stepCol;
    int iterateCol;//if not, is to iterate ow
    public:
    /// \brief Initialize the Bresenham algorithm state
    ///
    /// It is needed to specify the source and target point as integers.
    ///
    /// There are several algorithms to rasterize lines, but the Bresenham
    /// is eficient and it dont uses floating points to compute the intermediate
    /// points to make the line.
    ///
    /// \author Alessandro Ribeiro
    /// \param x origin x
    /// \param y origin y
    /// \param xd target x
    /// \param yd target y
    ///
    BresenhamIterator(const int x,const int y,const int xd,const int yd);
    /// \brief Returns true until the algorithm reachs the target point
    /// \author Alessandro Ribeiro
    /// \return True if the target point is reached by the iteration
    ///
    bool next();
    /// \brief Reads the current point of the rasterization state
    /// \author Alessandro Ribeiro
    /// \param x Returns the current x of the iteration
    /// \param y Returns the current y of the iteration
    ///
    void getXY(int *x,int *y);

    /// \brief Returns true until the algorithm reachs the target point.
    /// Reads the current point of the rasterization state
    /// \author Alessandro Ribeiro
    /// \param x Returns the current x of the iteration
    /// \param y Returns the current y of the iteration
    /// \return True if the target point is reached by the iteration
    ///
    bool next(int *x,int *y);

};

}

#endif
