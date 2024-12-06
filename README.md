# GLRenderer
A simple renderer written with OpenGL featuring realtime graphics techniques 

![Sponza scene with 2B from Nier: Automata](/screenshots/Sponza_2B_Feather.png)
Sponza scene with 2B from NieR: Automata  

## Features 
* Phong / Blinn-Phong lighting
* Model Loading
* Simple GUI using ImGui library
* Postprocessing options
* Antialiasing: MSAA
* HDR & Bloom
* Normal mapping
* Shadow mapping
* Deferred shading
* PBR
* IBL

## Screenshots 
![Desert Gun Scene with debug UI](/screenshots/Desert_UI.png)

## Building 
### Prerequisites
* OpenGL >= 3.3
* At least 4GB VRAM recommended for smooth use
* C++20, CMake >= 3.10

 
First, clone the repo:  
```bash 
git clone https://github.com/horamz/GLRenderer
```
Then generate the build files (using Ninja for build system): 
```bash
cd path/to/repo
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release 
```
Then build and run: 
```bash
cmake --build build && ./build/GLRenderer.exe
```
 

 
