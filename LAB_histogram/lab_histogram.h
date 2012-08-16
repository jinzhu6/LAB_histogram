//
//  lab_histogram.h
//  LAB_histogram
//
//  Created by Zhipeng Wu on 8/16/12.
//  Copyright (c) 2012 Zhipeng Wu. All rights reserved.
//

#ifndef __LAB_histogram__lab_histogram__
#define __LAB_histogram__lab_histogram__

#include <iostream>
#include <string>
#include <vector>
#include <opencv/highgui.h>

// This struct is only used for carrying data. It describes a
// pixel's color information in CIElab representation.
struct LabPixel {
  // The l value of the current pixel:
  double l_value;
  // The a value of the current pixel:
  double a_value;
  // The b_value of the current pizel:
  double b_value;
  // The x(column), y(row) positions of the current pixel:
  CvPoint pixel_position;
};

// This struct stores the extract LAB histogram for a specific block.
// We first split the image into M * N blocks. M is the number of row
// blocks and N is the number of column blocks. Then we extract LAB
// histogram for the block and save the histogram in the following
// struct.
struct BlockHist {
  int row_ind;    // row index of the current block. [0, M - 1]
  int col_ind;    // colomn index of the current block. [0, N - 1]
  int hist[256];  // LAB histogram for the current block.
};
// LAB histogram:
// L:  Luminance.
//     Ranges [0, 100].
//     0:   Black.
//     100: White.
// A:  Red<->Green chromaticity component.
//     Ranges [-128, 127] (possible).
//     -128: Green.
//     127:  Red.
// B:  Yello<->Blue chromaticity component.
//     Ranges [-128, 127] (possible).
//     -128: Blue.
//     127:  Yellow.

// Traditionally, we divide L into 4 bins, A & B into 8 bins. Then we have a
// 4x8x8 = 256 bins histogram.

class LabHistogram {
public:
  // Constructor:
  explicit LabHistogram(const std::string& image_path) {
    image_ = cvLoadImage(image_path.c_str(), CV_LOAD_IMAGE_COLOR);
    width_ = image_->width;
    height_ = image_->height;
  }
  // Destructor:
  ~LabHistogram() {
    if (image_) {
      cvReleaseImage(&image_);
      image_ = NULL;
    }
  }
  
  // Histogram extraction.
  // row_block is the number of blocks in row.
  // col_block is the number of blocks in col.
  bool ExtractHist(int row_block, int col_block);
  
  // Accessor:
  const std::vector<BlockHist>& histograms() const {
    return histograms_;
  }
  // The actual block number may be slightly different from the expected
  // one given by the user. Use the following functions to get actual number.
  const int row_block() const {
    return row_block_;
  }
  const int col_block() const {
    return col_block_;
  }
  const int width() const {
    return width_;
  }
  const int height() const {
    return height_;
  }
  
private:
  // Useful functions.
  
  // Our approach use CIELAB color space. We first need to
  // Convert the image color space from BGR(OpenCv) to CIELAB.
  // Meanwhile, the 2-D image is vectorized into 1-D float arrays.
  void ImageBGR2LAB();
  // Set of pixel-wise functions for color space converting.
  // To convert a RGB image to LAB. We shoulf first convert it
  // to XYZ space. Following functions provides the way to convert
  // from/to XYZ & LAB.
  
  // Gamma adjust function.
  double Gamma(double x);
  // Reverse gamma.
  double ReverseGamma(double x);
  // Convert RGB to XYZ color space.
  void RGB2XYZ(uchar R, uchar G, uchar B, double* X, double* Y, double* Z);
  // Convert XYZ to RGB color space.
  void XYZ2RGB(double X, double Y, double Z, uchar* R, uchar* G, uchar* B);
  // Adjust function for XYZ2LAB
  double Revise(double x);
  // Reverse adjust function for LAB2XYZ
  double ReverseRevise(double x);
  // Convert XYZ to LAB color space.
  void XYZ2LAB(double X, double Y, double Z, double* L, double* A, double* B);
  // Convert LAB to XYZ color space.
  void LAB2XYZ(double L, double A, double B, double* X, double* Y, double* Z);
  
  // Image for histogram generation.
  IplImage* image_;
  // Image_'s width(col).
  int width_;
  // Image_'s height(row).
  int height_;
  // We turn the RGB image into LAB color space and store the l, a, b values.
  // Besides, the original 2-d image data is transformed into 1-d array.
  // We store all the color-position information in the following array.
  std::vector<LabPixel> pixels_;
  // The actual number of blocks in row.
  int row_block_;
  // The number of blocks in colomn.
  int col_block_;
  // The actual calculate LAB histogram.
  std::vector<BlockHist> histograms_;
};
  


#endif /* defined(__LAB_histogram__lab_histogram__) */
