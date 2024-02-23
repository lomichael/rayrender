# rayrender 
[![MIT License](https://img.shields.io/badge/license-MIT-blue)](https://github.com/lomichael/rayrender/LICENSE)
[![justforfunnoreally.dev badge](https://img.shields.io/badge/justforfunnoreally-dev-9ff)](https://justforfunnoreally.dev)

A raycaster and raytracer written in C.

> **DISCLAIMER**: This is a hobby project and a work-in-progress.

## Build
rayrender has been tested on Debian Linux.

Prerequisites: 
- git
- gcc
- GLFW3

To clone the repository and build the app:
```sh
git clone https://codeberg.org/lomichael/rayrender
gcc main.c -o demo -lglfw -lGL -lm
```

## Usage
**Run the demo raycaster**:
```sh
./demo
```

## Resources
- [Lode's Computer Graphics Tutorial](https://lodev.org/cgtutor/raycasting.html)

## License
rayrender is licensed under the MIT License. See `LICENSE` in the project for details.
