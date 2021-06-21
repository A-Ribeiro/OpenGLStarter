#ifndef Press_Release__H
#define Press_Release__H

#include "Delegate.h"

namespace aRibeiro {

    /// \class EventPressReleaseDetector
    /// \brief Callback pattern without parameters
    ///
    /// Used as the main event type for the #aRibeiro::PressReleaseDetector up state and down state.
    ///
    /// Example of use with functions:
    ///
    /// \code
    ///    void callbackFunction() {
    ///        ...
    ///    }
    ///
    ///    EventPressReleaseDetector OnData;
    ///
    ///    OnData.add( &callbackFunction );
    ///
    ///    OnData.call();
    /// \endcode
    ///
    /// Example of use with method:
    ///
    /// \code
    ///    class ExampleClass {
    ///    public:
    ///        void callbackFunction(){
    ///            ...
    ///        }
    ///    };
    ///
    ///    ExampleClass obj;
    ///
    ///    EventPressReleaseDetector OnData;
    ///
    ///    OnData.add( &obj, &ExampleClass::callbackFunction );
    ///
    ///    OnData.call();
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    ///
    BEGIN_DECLARE_DELEGATE(EventPressReleaseDetector) CALL_PATTERN() END_DECLARE_DELEGATE;


    /// \brief Detects up and down events from any boolean input.
    ///
    /// \author Alessandro Ribeiro
    ///
    class PressReleaseDetector {

    public:

        //
        // public state variables
        //
        bool down;///< true when occurs a transition from !pressed to pressed
        bool up;///< true when occurs a transition from pressed to !pressed
        bool pressed;///< current pressed state

        //
        // events
        //
        /// \brief Called when a down event occurs
        ///
        /// Example of use with functions:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// void callbackFunction() {
        ///     ...
        /// }
        ///
        /// PressReleaseDetector right;
        /// right.OnDown.add( &callbackFunction );
        /// ...
        /// \endcode
        ///
        /// Example of use with method:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// class ExampleClass {
        /// public:
        ///     void callbackFunction(){
        ///         ...
        ///     }
        /// };
        ///
        /// ExampleClass obj;
        ///
        /// PressReleaseDetector right;
        /// right.OnDown.add( &obj, &ExampleClass::callbackFunction );
        /// ...
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        EventPressReleaseDetector OnDown;

        /// \brief Called when an up event occurs
        ///
        /// Example of use with functions:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// void callbackFunction() {
        ///     ...
        /// }
        ///
        /// PressReleaseDetector right;
        /// right.OnUp.add( &callbackFunction );
        /// ...
        /// \endcode
        ///
        /// Example of use with method:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// class ExampleClass {
        /// public:
        ///     void callbackFunction(){
        ///         ...
        ///     }
        /// };
        ///
        /// ExampleClass obj;
        ///
        /// PressReleaseDetector right;
        /// right.OnUp.add( &obj, &ExampleClass::callbackFunction );
        /// ...
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        EventPressReleaseDetector OnUp;

        PressReleaseDetector();

        /// \brief Set the current variable state
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// PressReleaseDetector right;
        ///
        /// void loop() {
        ///     right.setState( sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D) );
        ///
        ///     if ( right.down ) {
        ///         ...
        ///     }
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        void setState(bool v);

    };

}
#endif
