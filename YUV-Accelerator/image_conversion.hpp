//
//  image_conversion.hpp
//  YUV-Accelerator
//
//  Created by 朱芄蓉 on 17/12/2017.
//  Copyright © 2017 朱芄蓉. All rights reserved.
//

#ifndef image_conversion_hpp
#define image_conversion_hpp

#include "helper.hpp"
#include "rgb.hpp"
#include "yuv.hpp"

/* No optimazation */
void yuv2rgb_ori(YUVImage& yuv, RGBImage& rgb);
void rgb2yuv_ori(RGBImage& rgb, YUVImage& yuv);

/* MMX */
void yuv2rgb_mmx(YUVImage& yuv, RGBImage& rgb);
void rgb2yuv_mmx(RGBImage& rgb, YUVImage& yuv);

/* SSE2 */
void yuv2rgb_sse2(YUVImage& yuv, RGBImage& rgb);
void rgb2yuv_sse2(RGBImage& rgb, YUVImage& yuv);

/* AVX */
void yuv2rgb_avx(YUVImage& yuv, RGBImage& rgb);
void rgb2yuv_avx(RGBImage& rgb, YUVImage& yuv);

#endif /* image_conversion_hpp */
