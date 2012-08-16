//
//  main.cpp
//  LAB_histogram
//
//  Created by Zhipeng Wu on 8/16/12.
//  Copyright (c) 2012 Zhipeng Wu. All rights reserved.
//

#include <iostream>
#include "lab_histogram.h"

int main(int argc, const char * argv[])
{

  std::string img_path(argv[1]);
  LabHistogram my_hists(img_path.c_str());
  int row_block, col_block;
  std::cout << "The expect number of blocks in row: ";
  std::cin >> row_block;
  std::cout << "The expect number of blocks in column: ";
  std::cin >> col_block;
  
  bool success = my_hists.ExtractHist(row_block, col_block);
  if (!success) {
    std::cout << "Histogram extraction failure...";
    return -1;
  }
  
  int img_width = my_hists.width();
  int img_height = my_hists.height();
  
  std::cout << "Image row number: " << img_height << std::endl;
  std::cout << "Image column number: " << img_width << std::endl;
  
  int row_block_real = my_hists.row_block();
  int col_block_real = my_hists.col_block();
  
  std::cout << "The real number of blocks in row: " << row_block_real << std::endl;
  std::cout << "The real number of blocks in column: " << col_block_real << std::endl;

  std::vector<BlockHist> block_hists = my_hists.histograms();
  for (int i = 0; i < row_block_real; ++i) {
    for (int j = 0; j < col_block_real; ++j) {
      std::cout << "Histogram on row[" << i << "] col[" << j <<"] :" << std::endl;
      for (int c = 0; c < 256; ++c) {
        std::cout << block_hists[i * col_block_real + j].hist[c] << '\t';
      }
      std::cout << std::endl;
    }
  }
  return 0;
}

