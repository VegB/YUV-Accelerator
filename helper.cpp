//
//  helper.cpp
//  YUV-Accelerator
//
//  Created by VegB on 17/12/2017.
//  Copyright © 2017 VegB. All rights reserved.
//

#include "helper.hpp"



void print_64(__m64 num){
    int16_t* p = (int16_t*)(&num);
    for(int i = 0; i < 4; ++i){
        cout << *(p + i) << " ";
    }
    cout << endl;
}

void print_128(__m128 num){
    float* p = (float*)(&num);
    for(int i = 0; i < 4; ++i){
        cout << *(p + i) << " ";
    }
    cout << endl;
}

void write_back_from_64(__m64 num, uint8_t* dst, int pos){
    int16_t* p = (int16_t*)(&num);
    for(int i = 0; i < 4; ++i){
        dst[pos + i] = (uint8_t)clip(0, 255, *(p + (3 - i)));
    }
}

void write_back_from_128(__m128 num, uint8_t* dst, int pos){
    float* p = (float*)(&num);
    for(int i = 0; i < 4; ++i){
        dst[pos + i] = (uint8_t)clip(0, 255, *(p + (3 - i)));
    }
}

void write_back_from_128_not_reverse(__m128 num, uint8_t* dst, int pos){
    float* p = (float*)(&num);
    for(int i = 0; i < 4; ++i){
        dst[pos + i] = (uint8_t)clip(0, 255, *(p + i));
    }
}

void write_back_from_256(__m256 num, uint8_t* dst, int pos){
    float* p = (float*)(&num);
    for(int i = 0; i < 8; ++i){
        dst[pos + i] = (uint8_t)clip(0, 255, *(p + (7 - i)));
    }
}

