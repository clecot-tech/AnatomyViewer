# AnatomyViewer

A desktop 3D viewer for anatomical models (STL), written in C++20 with Qt 6 and OpenGL.

Personal learning project, built with an emphasis on testability, traceability and
safety-oriented engineering practices (inspired by IEC 62304).

## Status

Work in progress (see roadmap below).

## Architecture

- `src/core`: application logic, no Qt dependency, unit-tested with GoogleTest
- `src/app`: Qt / OpenGL user interface
- `tests`: unit tests for `core`

## Build

Requirements: CMake 3.24 or later, a C++20 compiler (MSVC or GCC), Qt 6.

    cmake -S . -B build -DCMAKE_PREFIX_PATH=<path to your Qt kit>
    cmake --build build --config Debug
    ctest --test-dir build --build-config Debug --output-on-failure

## Roadmap

- [ ] Milestone 1: project skeleton, unit tests, continuous integration (in progress)
- [ ] Milestone 2: STL loading and display
- [ ] Milestone 3: camera interaction
- [ ] Milestone 4: measurement tool
- [ ] Milestone 5: requirements traceability (requirements to tests)