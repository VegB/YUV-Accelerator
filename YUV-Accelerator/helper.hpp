//
//  helper.hpp
//  YUV-Accelerator
//
//  Created by 朱芄蓉 on 17/12/2017.
//  Copyright © 2017 朱芄蓉. All rights reserved.
//

#ifndef helper_hpp
#define helper_hpp

#include <stdio.h>
// Header for MMX
#include <mmintrin.h>
// Header for SSE2
#include <emmintrin.h>
// Header for AVX
#include <immintrin.h>
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

double clip(int upper_bound, int lower_bound, double ori_num);
void print_64(__m64 num);
void print_128(__m128 num);
void write_back_from_64(__m64 num, uint8_t* dst, int pos);
void write_back_from_128(__m128 num, uint8_t* dst, int pos);
void write_back_from_128_not_reverse(__m128 num, uint8_t* dst, int pos);
void write_back_from_256(__m256 num, uint8_t* dst, int pos);
#endif /* helper_hpp */
