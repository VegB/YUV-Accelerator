//
//  image_conversion.hpp
//  YUV-Accelerator
//
//  Created by 朱芄蓉 on 17/12/2017.
//  Copyright © 2017 朱芄蓉. All rights reserved.
//

#ifndef image_conversion_hpp
#define image_conversion_hpp

#include <iostream>
#include <string>
#include "rgb.hpp"
#include "yuv.hpp"

double clip(int upper_bound, int lower_bound, double ori_num);

/* No optimazation */
void yuv2rgb_ori(YUVImage& yuv, RGBImage& rgb);
void rgb2yuv_ori(RGBImage& rgb, YUVImage& yuv);

#endif /* image_conversion_hpp */
