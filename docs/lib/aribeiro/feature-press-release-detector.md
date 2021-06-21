# OpenGLStarter

[Back to HOME](../../index)

## Press Release Detector

This class helps to implement key based state change.

The idea is to continuous setting the current value (true - with signal, false - without signal) and detect the edges (down - up to down, up - down to up).

The class have three variables that are updated according the signal state update:

* __down:__ Edge up to down
* __up:__ Edge down to up
* __pressed:__ Current variable state

Example:

```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;

PressReleaseDetector right;

void loop() {
    right.setState( sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || 
                    Keyboard::isKeyPressed(sf::Keyboard::D) );
    if ( right.down ) {
        // ...
    }
}
```

