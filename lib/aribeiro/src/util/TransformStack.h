#ifndef transform_stack__H
#define transform_stack__H

#include <vector>
#include <aribeiro/SSE2.h>

namespace aRibeiro {

    /// \brief Helper class to be used as the OpenGL stack behaviour.
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// TransformStack<mat4> model;
    /// 
    /// model.top = mat4::IdentityMatrix;
    ///
    /// model.push();
    /// model.top *= translate(10,0,0);
    /// // render code
    /// ...
    /// model.pop();
    ///
    /// model.push();
    /// model.top *= translate(0,0,10);
    /// // render code
    /// ...
    /// model.pop();
    ///
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    ///
    template <typename T>
    class _SSE2_ALIGN_PRE TransformStack {

    private:

        aligned_vector<T> _stack;

        //private copy constructores, to avoid copy...
        TransformStack(const TransformStack& v) {}
        void operator=(const TransformStack& v) {}

    public:

        T top;///< The top of the stack. Can be freely modified.

        /// \brief Get the current stack size
        ///
        /// This class do not have a limit for the amount of elements in the stack.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// TransformStack<mat4> model;
        /// 
        /// model.top = mat4::IdentityMatrix;
        ///
        /// //size here will be 0
        /// model.size();
        ///
        /// model.push();
        /// model.top *= translate(10,0,0);
        /// // render code
        /// ...
        ///
        /// //size here will be 1
        /// model.size();
        /// model.pop();
        ///
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \return amount of items in the stack
        ///
        int size() {
            return _stack.size();
        }

        TransformStack() :_stack(0) {
            top = T();// mat4::IdentityMatrix;
        }

        /// \brief Save the current #top variable to the top of the internal stack
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// TransformStack<mat4> model;
        /// 
        /// model.top = mat4::IdentityMatrix;
        ///
        /// model.push();
        /// model.top *= translate(10,0,0);
        /// // render code
        /// ...
        /// model.pop();
        ///
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        void push() {
            _stack.push_back(top);
        }

        /// \brief Set the #top variable to the stack top and remove the item in the stack
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// TransformStack<mat4> model;
        /// 
        /// model.top = mat4::IdentityMatrix;
        ///
        /// model.push();
        /// model.top *= translate(10,0,0);
        /// // render code
        /// ...
        /// model.pop();
        ///
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        void pop() {
            if (_stack.size() > 0) {
                top = _stack[_stack.size() - 1];
                _stack.pop_back();
            }
            //else
                //printf("error to pop element...\n");
        }

        /// \brief Assign operator
        ///
        /// To be possible to set the top variable from the #TransformStack instance.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// TransformStack<mat4> model;
        /// 
        /// model = mat4::IdentityMatrix;
        ///
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        void operator=(const T &v) {
            top = v;
        }

        /// \brief Read operator
        ///
        /// To be possible to get the top variable from the #TransformStack instance.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// TransformStack<mat4> model;
        /// 
        /// mat4 top_copy = (mat4)model;
        ///
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        operator T() const {
            return top;
        }

        SSE2_CLASS_NEW_OPERATOR

    } _SSE2_ALIGN_POS ;

}
#endif
