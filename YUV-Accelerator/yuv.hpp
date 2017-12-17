//
//  yuv.hpp
//  YUV-Accelerator
//
//  Created by 朱芄蓉 on 17/12/2017.
//  Copyright © 2017 朱芄蓉. All rights reserved.
//

#ifndef yuv_hpp
#define yuv_hpp

#include "helper.hpp"
using namespace std;

/* stores a single YUV Image */
class YUVImage{
public:
    string name;
    int width;
    int height;
    uint8_t *y, *u, *v;
    //int16_t *ys, *us, *vs;
    float *yf, *uf, *vf;
    
    YUVImage(int w, int l, string name_){
        width = w;
        height = l;
        name = name_;
        y = new uint8_t[width * height];
        u = new uint8_t[width * height / 4];
        v = new uint8_t[width * height / 4];
        cout << "YUV Image created!" << endl;
    };
    
    ~YUVImage(){
        delete []y;
        delete []u;
        delete []v;
        cout << "YUV Imaged destroyed!" << endl;
    };
    
    void read_in_image(string filename);
    void init_image();
    void write_to_file(string filename);
    void get_uv(uint8_t* u_, uint8_t* v_);
};

#endif /* yuv_hpp */
