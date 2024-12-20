# Path Tracer

<p align="middle">
  <img src="images/1.png" width="150" />
  <img src="images/4.png" width="150" /> 
  <img src="images/7.png" width="150" />
</p>

## Compilation

To compile, you need GLFW and CMake installed on your system.
<br>
If above requirements are fulfilled, just run the following.
```
git clone --recursive https://github.com/RaphaelAsla/PathTracer.git
mkdir build && cd build
CXX=clang++ cmake -DCMAKE_BUILD_TYPE=Release ..
make
./raytracer
```
## This project is discontinued in favor of [Nexavey](https://github.com/RaphaelAsla/Nexavey) which will include it's own ray tracer.
