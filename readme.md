LeoEngine
=========

Description
-----------

A playground C++ project centered around physicaly based rendering.
It features a CPU path tracer integrator, and both BSDF and light importance sampling (both area lights and image-based environment lighting). I also added a couple of PBR reflection models (Torrance Sparrow, Oren Nayar). Intersection computations were originally done with a BVH using axis-aligned slabs, but are now done with Embree.

### Important notice ###

This is an evergoing work in progress, as it is mostly used for self-teaching. I did not take the time to clean it up and I am in the middle of bugfixes. However it shoud be buildable and runable if you want to take a look!

### Dependencies ###

Everything is embedded in the external folder. You should not have to add anything and can build the project as it is.
However, **if you experience opengl-related problems**, you might want to replace the glad library and include in the external folder with a version that suits your setup. I use opengl for the viewer.

In general, if any library is giving you a hard time, you can specify a path to your own installation in the *.env.cmake* file, or modify *CMakeLists.txt*. By default all libraries, headers and DLLs are fetched from the *external* folder.

The following (lib and headers) are included in the external folder:
* [GLFW](https://github.com/glfw/glfw)
* [Assimp](https://github.com/assimp/assimp)
* [Embree](https://github.com/embree/embree)
* [GLAD](https://glad.dav1d.de/)

The following (headers only) are also included in the external folder:
* [STB image](https://github.com/nothings/stb/blob/master/stb_image.h)
* [GLM](https://github.com/g-truc/glm)

### Building the project ###

To configure the project, run

> cd AurelRayTracer && cmake .

CMake might ask you to specify a generator, in the case please use "Visual Studio 16 2019".

You can also use CMake-GUI of course.

Then open the .sln file with *Visual Studio (2019 or later)* and build the project. This should create the *AurelRayTracer.exe* file in the project source directory.


How to use
----------

**Launch *AurelRayTracer.exe* from the project root**. This will render the many test scenes I use and produce ppm images in the root folder.

Acknowledgments and nice resources
----------------------------------

Thanks to all these invaluable resources:
* The bible: [PBRT](https://www.pbr-book.org/)
* A very good introductory series: [Ray Tracing In Series](https://raytracing.github.io/)
* And a very nice course on ray tracing: [TU Wien - Ray Tracing](https://www.youtube.com/playlist?list=PLujxSBD-JXgnGmsn7gEyN28P1DnRZG7qi)