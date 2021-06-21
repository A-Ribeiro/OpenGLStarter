# OpenGLStarter

[Back to HOME](../../index)

## 3D Model Container

There is a definition of a container that is not specific for any render API.

With this container you can store:

* Animations
* Lights
* Cameras
* Materials
* Geometries
* Nodes (scene graph)

The name of the file format the __ModelContainer.h__ reads and writes is called BAMS (Binary Asilva Mesh Scene). The file format is a __zlib__ compressed binary stream.

If you take a look at the __projects-lib/data-model/model/__ you will see the structure of each element of the container.

Example of writing container content to file:

```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;
#include <data-model/data-model.h>
using namespace model;


ModelContainer *container = new ModelContainer();

// fill the container with elements
{
    Geometry geometry;
    //...
    container->geometries.push_back(geometry);

    Animation animation;
    //...
    container->animations.push_back(animation);

    // ...
}

//
// save the container to file
//
container->write("output.bams");

delete container;
```

Example of reading container content from file:

```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;
#include <data-model/data-model.h>
using namespace model;
#include <mini-gl-engine/mini-gl-engine.h>
using namespace GLEngine;

Transform* nodeTraverse(int currentIndex,const model::Node &node,const model::ModelContainer *container) {
    Transform* result = new Transform();
    aRibeiro::mat4 m = node.transform;
    
    result->LocalPosition = aRibeiro::toVec3(m * aRibeiro::vec4(0,0,0,1));    
    result->LocalScale = aRibeiro::vec3(aRibeiro::length(m[0]),
                                        aRibeiro::length(m[1]),
                                        aRibeiro::length(m[2]));
    
    m = aRibeiro::extractRotation(m);
    m[0] = aRibeiro::normalize(m[0]);
    m[1] = aRibeiro::normalize(m[1]);
    m[2] = aRibeiro::normalize(m[2]);
    result->LocalRotation = aRibeiro::extractQuat( m );
    
    result->Name = node.name;

    //here you can process geometries also
    if ( node.geometries.size() > 0 ){
        // ...
    }

    for(size_t i=0;i<node.children.size();i++){
        uint32_t chidx = node.children[i];
        result->addChild(nodeTraverse(chidx,container->nodes[chidx],container));
    }
    
    return result;
}


model::ModelContainer *container = new model::ModelContainer();
container->read("input.bams");

// now can iterate over the container elements
Transform *graph = nodeTraverse(0, container->nodes[0], container);

delete container;
```

