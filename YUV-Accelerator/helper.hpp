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

void print_64(__m64 num);
void print_128(__m128 num);
double clip(int upper_bound, int lower_bound, double ori_num);
void write_back_from_64(__m64 num, uint8_t* dst, int pos);
void write_back_from_128(__m128 num, uint8_t* dst, int pos);
void write_back_from_128_not_reverse(__m128 num, uint8_t* dst, int pos);
void write_back_from_256(__m256 num, uint8_t* dst, int pos);

/* clip num to [lower_bound, upper_bound] */
inline double clip(int lower_bound, int upper_bound, double num){
    if(num > upper_bound){
        num = upper_bound;
    }
    if(num < lower_bound){
        num = lower_bound;
    }
    return num;
}

inline int16_t getbit(__m64 num, int s,int e){
    long long inst = (long long)num;
    unsigned long long mask = 0xffffffffffffffff;
    mask = (mask >> s) << s;
    mask = (mask << (63 - e)) >> (63 - e);
    int16_t rst = (int16_t)((inst & mask) >> s);
    rst = rst;
    return rst;
}

inline void write_back_mmx(__m64 rst, uint8_t* dst, int pos){
    for(int i = 0; i < 4; ++i){
        dst[pos + i] = getbit(rst, i << 4, ((i + 1) << 4) - 1);
    }
}

inline void write_back_sse2(__m128 rst, uint8_t* dst, int pos){
    float tmp[4];
    _mm_store_ps(tmp, rst);
    for(int i = 0; i < 4; ++i){
        dst[pos + i] = (uint8_t)tmp[3 - i];
    }
}

inline void write_back_avx(__m256 rst, uint8_t* dst, int pos){
    float tmp[8];
    _mm256_store_ps(tmp, rst);
    for(int i = 0; i < 8; ++i){
        dst[pos + i] = (uint8_t)tmp[7 - i];
    }
}
#endif /* helper_hpp */
