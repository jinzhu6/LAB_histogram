//
//  lab_histogram.cpp
//  LAB_histogram
//
//  Created by Zhipeng Wu on 8/16/12.
//  Copyright (c) 2012 Zhipeng Wu. All rights reserved.
//

#include "lab_histogram.h"
#include <math.h>

  
// Matrix M for converting RGB color space to XYZ color space:
static const double kMatrixM[3][3] = {{0.436052025, 0.385081593, 0.143087414},
  {0.222491598, 0.716886060, 0.060621486},
  {0.013929122, 0.097097002, 0.714185470}};
// Matrix M's invert for converting XYZ to RGB color space:
static const double kInvMatrixM[3][3] = {{ 3.134051341, -1.617027710, -0.490652209},
  {-0.978762729, 1.916142228, 0.033449628},
  {0.0719425771, -0.228971179, 1.405218305}};
// Gamma adjust function.
double LabHistogram::Gamma(double x) {
  if (x > 0.04045)
    x = pow((x + 0.055) / 1.055, 2.4);
  else
    x = x / 12.92;
  return x;
}
// Reverse gamma.
double LabHistogram::ReverseGamma(double x) {
  if (x > pow((0.04045 + 0.055) / 1.055, 2.4))
    x = 1.055 * pow(x, 1.0 / 2.4) - 0.055;
  else
    x = 12.92 * x;
  return x;
}
// Convert RGB to XYZ color space.
void LabHistogram::RGB2XYZ(uchar R, uchar G, uchar B, double* X, double* Y, double* Z) {
  double RGB[]={R / 255.0, G / 255.0, B / 255.0};
  RGB[0] = Gamma(RGB[0]);
  RGB[1] = Gamma(RGB[1]);
  RGB[2] = Gamma(RGB[2]);
  
  *X = 100.0 * (kMatrixM[0][0] * RGB[0] + kMatrixM[0][1] * RGB[1] + kMatrixM[0][2] * RGB[2]);
  *Y = 100.0 * (kMatrixM[1][0] * RGB[0] + kMatrixM[1][1] * RGB[1] + kMatrixM[1][2] * RGB[2]);
  *Z = 100.0 * (kMatrixM[2][0] * RGB[0] + kMatrixM[2][1] * RGB[1] + kMatrixM[2][2] * RGB[2]);
}
// Convert XYZ to RGB color space.
void LabHistogram::XYZ2RGB(double X, double Y, double Z, uchar* R, uchar* G, uchar* B) {
  double XYZ[]={0.0,0.0,0.0};
  
  XYZ[0] = (kInvMatrixM[0][0] * X + kInvMatrixM[0][1] * Y + kInvMatrixM[0][2] * Z) / 100.0;
  XYZ[1] = (kInvMatrixM[1][0] * X + kInvMatrixM[1][1] * Y + kInvMatrixM[1][2] * Z) / 100.0;
  XYZ[2] = (kInvMatrixM[2][0] * X + kInvMatrixM[2][1] * Y + kInvMatrixM[2][2] * Z) / 100.0;
  
  XYZ[0] = ReverseGamma(XYZ[0]) > 0 ? ReverseGamma(XYZ[0]) * 255:0;
  XYZ[1] = ReverseGamma(XYZ[1]) > 0 ? ReverseGamma(XYZ[1]) * 255:0;
  XYZ[2] = ReverseGamma(XYZ[2]) > 0 ? ReverseGamma(XYZ[2]) * 255:0;
  
  *R = XYZ[0] < 255 ? static_cast<uchar>(XYZ[0] + 0.5) : 255;
  *G = XYZ[1] < 255 ? static_cast<uchar>(XYZ[1] + 0.5) : 255;
  *B = XYZ[2] < 255 ? static_cast<uchar>(XYZ[2] + 0.5) : 255;
}
//Color reference.
static const double ref_x = 96.4221;
static const double ref_y = 100.000;
static const double ref_z = 82.5221;
// Adjust function for XYZ2LAB
double LabHistogram::Revise(double x) {
  if ( x > pow(6.0 / 29.0, 3))
    x = pow(x, 1.0 / 3.0);
  else
    x = (1.0 / 3.0) * (29.0 / 6.0) * (29.0 / 6.0) * x + (16.0 / 116.0);
  return x;
}
// Reverse adjust function for LAB2XYZ
double LabHistogram::ReverseRevise(double x) {
  if (x > 6.0 / 29.0)
    x = pow(x, 3.0);
  else
    x = (x - 16.0 / 116.0) * 3 * pow(6.0 / 29.0, 2);
  return x;
}
// Convert XYZ to LAB color space.
void LabHistogram::XYZ2LAB(double X, double Y, double Z, double* L, double* A, double* B) {
  double x = X / ref_x;
  double y = Y / ref_y;
  double z = Z / ref_z;
  
  x = Revise(x);
  y = Revise(y);
  z = Revise(z);
  
  *L = (116.0 * y) - 16.0;
  *A = 500.0 * (x - y);
  *B = 200.0 * (y - z);
}
// Convert LAB to XYZ color space.
void LabHistogram::LAB2XYZ(double L, double A, double B, double* X, double* Y, double* Z) {
  double y = (L + 16.0) / 116.0;
  double x = y + A / 500.0;
  double z = y - B / 200.0;
  
  y = ReverseRevise(y);
  x = ReverseRevise(x);
  z = ReverseRevise(z);
  
  *X = x * ref_x;
  *Y = y * ref_y;
  *Z = z * ref_z;
}

// Convert the image color space from BGR(OpenCv) to CIELAB.
// Meanwhile, the 2-D image is vectorized into 1-D float arrays.
void LabHistogram::ImageBGR2LAB() {
  int step = image_->widthStep;
  uchar* data = reinterpret_cast<uchar*>(image_->imageData);
  for (int row = 0; row < height_; ++row) {
    for (int col = 0; col < width_; ++col) {
      // Access pixel values.
      uchar pixel_B, pixel_G, pixel_R;
      pixel_B = data[row * step + col * 3 + 0];
      pixel_G = data[row * step + col * 3 + 1];
      pixel_R = data[row * step + col * 3 + 2];
      // Step 1: RGB to XYZ conversion.
      double pixel_x = 0, pixel_y = 0, pixel_z = 0;
      RGB2XYZ(pixel_R, pixel_G, pixel_B, &pixel_x, &pixel_y, &pixel_z);
      // Step 2: XYZ to LAB conversion.
      double pixel_l = 0, pixel_a = 0, pixel_b = 0;
      XYZ2LAB(pixel_x, pixel_y, pixel_z, &pixel_l, &pixel_a, &pixel_b);
      // Add converted color to 1-D vectors.
      LabPixel new_pixel;
      new_pixel.l_value = pixel_l;
      new_pixel.a_value = pixel_a;
      new_pixel.b_value = pixel_b;
      new_pixel.pixel_position.x = col;
      new_pixel.pixel_position.y = row;
      pixels_.push_back(new_pixel);
    }
  }
}

bool LabHistogram::ExtractHist(int row_block, int col_block) {
  
  if ((row_block < 1) || (col_block < 1) ||
      (row_block > height_) || (col_block > width_)) {
    return false;
  }
  if (!image_) return false;
  
  // Step 1: convert image from BGR color space to LAB.
  ImageBGR2LAB();
  
  // Step 2: create and initialize M x N empty hitograms.
  int row_step = static_cast<int>(round(static_cast<float>(height_) / row_block));
  int col_step = static_cast<int>(round(static_cast<float>(width_) / col_block));
  row_block_ = static_cast<int>(ceil(static_cast<float>(height_) / row_step));
  col_block_ = static_cast<int>(ceil(static_cast<float>(width_) / col_step));
  int l_step = static_cast<int>(ceil(100 / 4));
  int a_step = static_cast<int>(ceil(256 / 8));
  int b_step = static_cast<int>(ceil(256 / 8));
  
  for (int i = 0; i < row_block_; ++i) {
    for (int j = 0; j < col_block_; ++j) {
      BlockHist new_hist;
      new_hist.row_ind = i;
      new_hist.col_ind = j;
      for (int c = 0; c < 256; ++c) {
        new_hist.hist[c] = 0;
      }
      histograms_.push_back(new_hist);
    }
  }
  
  // Step 3: traverse all the pixels and accumulate the histograms.
  
  for (int i = 0; i < width_ * height_; ++i) {
    int row_ind = static_cast<int>(floor(static_cast<float>(pixels_[i].pixel_position.y) / row_step));
    int col_ind = static_cast<int>(floor(static_cast<float>(pixels_[i].pixel_position.x) / col_step));
    int block_ind = row_ind * col_block_ + col_ind;
    // assert(block_ind < row_block * col_block);
    
    int l_ind = static_cast<int>(floor(pixels_[i].l_value / l_step));
    int a_ind = static_cast<int>(floor((pixels_[i].a_value + 128) / a_step));
    int b_ind = static_cast<int>(floor((pixels_[i].b_value + 128) / b_step));
    int bin_ind = l_ind * 8 * 8 + a_ind * 8 + b_ind;
    // assert(bin_ind < 256);
    
    ++histograms_[block_ind].hist[bin_ind];
  }
  return true;
}
