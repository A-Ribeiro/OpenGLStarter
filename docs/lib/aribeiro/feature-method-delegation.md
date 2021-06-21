# OpenGLStarter

[Back to HOME](../../index)

## Method and Function Delegation (Events)

Yes you can use method delegation with this framework.

Method delegation is a way to pass a method from a class as parameter to another.

It can be used to implement __events__.

See the example below:

```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;

// first you need to declare the delegate type
BEGIN_DECLARE_DELEGATE(DelegateWithIntParameter, int v) CALL_PATTERN (v) END_DECLARE_DELEGATE;

//functions you want to call
void DelegateFunctionOutside(int v) {
  printf("  executed DelegateFunctionOutside: %i\n", v);
}

//class definition
class DelegateTest{
public:
  void int1(int v) {
    printf("  executed method int1: %i\n", v);
  }
  void int2(int v) {
    printf("  executed method int2: %i\n", v);
  }
};

void main(int argc, char* argv[]) {

  //create instance of DelegateTest
  DelegateTest delegateTestObj;

  //now you can declare an object of the type DelegateWithIntParameter
  DelegateWithIntParameter OnInt;

  //add the calls you want to do
  OnInt.add(&delegateTestObj, &DelegateTest::int1);
  OnInt.add(&delegateTestObj, &DelegateTest::int2);
  OnInt.add(&DelegateFunctionOutside);
  
  //call the event with parameter 10
  OnInt(10);
}
```

## Method And Function Pointer

Another case, is where you want to pass a method or a function as parameter, but you need to use the return value from them.

In this case you can use the method/function pointer.

See the example below:

```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;

//define the method pointer type MethodSumPtr_returnInt with a return
//   The type returns an int and have two parameters
//   Pay attention on the ReturnMethodCall with the two parameters call
DefineMethodPointer(MethodSumPtr_returnInt, int, int a, int b) ReturnMethodCall(a, b)

//define the method pointer type MethodSumPtr_returnVoid without a return
//   The type doesn't return anything(void) and have two parameters
//   Pay attention on the VoidMethodCall with the two parameters call
DefineMethodPointer(MethodSumPtr_returnVoid, void, int a, int b) VoidMethodCall(a, b)

int FunctionExample_returnInt(int a, int b) {
    return a + b;
}

void FunctionExample_returnVoid(int a, int b) {
    ...
}

class ExampleClass {
public:
    int MethodExample_returnInt(int a, int b) {
        return a + b;
    }
    void MethodExample_returnVoid(int a, int b) {
        ...
    }
};

ExampleClass obj;

//
// Return int Examples
//

// using functor example
MethodSumPtr_returnInt methodPtr_returnInt = &FunctionExample_returnInt;

int c = 0;

if (methodPtr_returnInt != NULL)
    c = methodPtr_returnInt(1, 2);

// using method reference
methodPtr_returnInt = MethodSumPtr_returnInt( &obj, &ExampleClass::MethodExample_returnInt );

if (methodPtr_returnInt != NULL)
    c = methodPtr_returnInt(1, 2);

//
// Return Void Examples
//

// using functor example
MethodSumPtr_returnVoid methodPtr_returnVoid = &FunctionExample_returnVoid;

if (methodPtr_returnVoid != NULL)
    methodPtr_returnVoid(1, 2);

// using method reference
methodPtr_returnVoid = MethodSumPtr_returnVoid( &obj, &ExampleClass::MethodExample_returnVoid );
if (methodPtr_returnVoid != NULL)
    methodPtr_returnVoid(1, 2);
```
