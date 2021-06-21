#ifndef Property__H
#define Property__H

#include <aribeiro/common.h>
#include <aribeiro/Delegate.h>

/*
 property declaration:


 Property<vec2> Size;

 Size = vec2(1.0f);

 // you can listen to property changes

 void MyListeningFunction(Property<vec2> *prop) {
    vec2 vOld = prop->oldValue;
    vec2 vNew = prop->value;
    //you can rollback the modification.
    //  The next listeners will receive the old value as new to set...
    prop->rollback();
 }

 Size.OnChange.add(MyListeningFunction);


 */

namespace aRibeiro {

    /// \brief Define a custom property with a callback event list to manage the changes.
    ///
    /// The property class store the value you set through the template pattern.
    ///
    /// When the class detects it changed, it will call the #Property::OnChange event.
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// Property<vec2> Size;
    ///
    /// // you can listen to property changes
    /// 
    /// void MyListeningFunction(Property<vec2> *prop) {
    ///     vec2 vOld = prop->oldValue;
    ///     vec2 vNew = prop->value;
    ///     //you can rollback the modification.
    ///     //  The next listeners will receive the old value as new to set...
    ///     prop->rollback();
    /// }
    /// 
    /// Size.OnChange.add(MyListeningFunction);
    /// 
    /// // trigger the property change
    /// Size = vec2(1.0f);
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    ///
    template <typename T>
    class _SSE2_ALIGN_PRE Property {

        // avoid copy, using copy constructors
        Property(const Property&) {}
        void operator=(const Property&) {}

    public:

        BEGIN_DECLARE_DELEGATE_INSIDE_TEMPLATE(PropertyBaseEvent, Property<T> *prop) CALL_PATTERN(prop) END_DECLARE_DELEGATE;

        T oldValue;///< The property last value, before the modification
        T value;///< The property current value

        PropertyBaseEvent OnChange;///< Called when a modification occurs

        /// \brief Construct this property with an initial value.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// Property<vec2> Size = Property<vec2>( vec2(1.0f) );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        Property(const T &defaultValue) {
            oldValue = defaultValue;
            value = defaultValue;
        }

        Property() {
            oldValue = T();
            value = T();
        }

        /// \brief Undo the last modification
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// Property<vec2> Size;
        ///
        /// // you can listen to property changes
        /// 
        /// void MyListeningFunction(Property<vec2> *prop) {
        ///     vec2 vOld = prop->oldValue;
        ///     vec2 vNew = prop->value;
        ///     //you can rollback the modification.
        ///     //  The next listeners will receive the old value as new to set...
        ///     prop->rollback();
        /// }
        /// 
        /// Size.OnChange.add(MyListeningFunction);
        /// 
        /// // trigger the property change
        /// Size = vec2(1.0f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        void rollback() {
            value = oldValue;
        }

        /// \brief Set the property value from a parameter of the type that is defined in the property creation
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// Property<vec2> Size;
        ///
        /// ...
        ///
        /// // Set the property value
        /// Size = vec2(1.0f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        void operator=(const T &v) {
            if (
                value != v
                //memcmp(&v, &value, sizeof(T)) != 0
                ) {
                oldValue = value;
                value = v;
                OnChange(this);
            }
        }

        /// \brief Cast the property to the template parameter class
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// Property<vec2> Size;
        ///
        /// ...
        ///
        /// vec2 vec_content = (vec2)Size;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        operator T() const {
            return value;
        }

        SSE2_CLASS_NEW_OPERATOR
    }_SSE2_ALIGN_POS;

}

#endif
