# OpenGLStarter

[Back to HOME](../../index)

## Platform Time and Sleep

This is a high precision, cross-platform time function implementation.

The PlatformTime object computes the time deltas and pass it to the application to write their interactive code.

To use the PlatformTime class you need to call the update method (once per frame), to compute the delta.

To use the PlatformSleep, you need to call the sleepMillis, sleepSec or busySleepMicro.

Example:

```cpp
PlatformTime time;
time.timeScale = 0.5f;
time.update();
PlatformSleep::sleepMillis(2000);
time.update();
	
printf("time.deltaTime (2 secs): %f\n", time.deltaTime);
printf("time.unscaledDeltaTime (2 secs): %f\n", time.unscaledDeltaTime);
```
