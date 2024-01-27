# Stylizing Video (Better) by Example

## Installation

### Requirements

* Qt 5.2.0 or higher
* [OpenCV 4.3.0](https://github.com/opencv/opencv/tree/4.3.0) + [opencv_contrib](https://github.com/opencv/opencv_contrib)
* [FFTW 3.3.8 (or higher)](http://www.fftw.org/)
* Mac: macOS 10.15 or higher, Xcode 11 with Command Line Tools

### Mac Installation

Clone our repository and run `git submodule update --init --recursive` to set up our EbSynth submodule.

```sh
brew install opencv fftw
(cd deps/ebsynth && ./build-macos-cpu-only.sh)
qmake
make
```

## Implementation

### Guide Generation

#### Optical Flow-based Guides

We generate inter-image optical flow-based vector fields that encode the velocity of pixels
between images using SimpleFlow from opencv_contrib. We present below vector fields generated on our test set.

* Vector field from image to image encoded as HSV images:

![Cat optical flow in HSV](https://media.giphy.com/media/dxHtk041o3VlaS8NCL/giphy.gif)

* Scalar vx field encoded as grayscale:

![Cat vx in gray](https://media.giphy.com/media/mBeo1ENx26fvTFIr9H/giphy.gif)

* Scalar vy field encoded as grayscale:

![Cat vy in gray](https://media.giphy.com/media/mFl7l8vvtbO2cR3aw7/giphy.gif)

## Demo

Please [see demo of the final product](https://www.youtube.com/watch?v=77BxiVeDPSI), including an extension to neural style transfer-based keyframes.

## Citations

Ondřej Jamriška, Šárka Sochorová, Ondřej Texler, Michal Lukáč, Jakub Fišer, Jingwan Lu, Eli Shechtman, and Daniel Sýkora. 2019.
Stylizing Video by Example.
*ACM Trans. Graph.* 38, 4, Article 107 (July 2019), 11 pages. https://doi.org/10.1145/3306346.3323006

Michael W. Tao, Jiamin Bai, Pushmeet Kohli, and Sylvain Paris. "SimpleFlow: A Non-iterative, Sublinear Optical Flow Algorithm".
*Computer Graphics Forum (Eurographics 2012)*, 31(2), May 2012.
