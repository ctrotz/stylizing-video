# Stylizing Video (Better) by Example
## Installation
### Requirements
* Qt 5.2.0 or higher
* [OpenCV 4.3.0](https://github.com/opencv/opencv/tree/4.3.0) + [opencv_contrib](https://github.com/opencv/opencv_contrib)

### Mac Installation
Install both OpenCV 4.3.0 and opencv_contrib from source, making sure to link them at build time by passing in
```
cmake -DOPENCV_EXTRA_MODULES_PATH=<opencv_contrib>/modules <opencv_source_directory>
```
These should be installed to `/usr/local` to link properly with this project.

We recommend the following workflow:
1. Clone both repositories to the `deps/` directory.
2. Create a `build` directory in the cloned `opencv` repo. 
3. Run 
```
cmake -DOPENCV_EXTRA_MODULES_PATH=../../opencv_contrib/modules ..
make -j4
make install
```
4. All set!

## Citations
Ondřej Jamriška, Šárka Sochorová, Ondřej Texler, Michal Lukáč, Jakub Fišer, Jingwan Lu, Eli Shechtman, and Daniel Sýkora. 2019.
Stylizing Video by Example.
*ACM Trans. Graph.* 38, 4, Article 107 (July 2019), 11 pages. https://doi.org/10.1145/3306346.3323006
