//
//  yuv.cpp
//  YUV-Accelerator
//
//  Created by 朱芄蓉 on 17/12/2017.
//  Copyright © 2017 朱芄蓉. All rights reserved.
//

#include <fstream>
#include "yuv.hpp"

void YUVImage::read_in_image(string filename){
    cout << "[" << name << "]: Read in image from " << filename << endl;
    
    FILE * fp;
    if ((fp = fopen(filename.c_str(), "rb")) == NULL){
        cerr << "[" << name << "]: Can not open " << filename << endl;
    }
    char * buf = new char[(unsigned int)(width * height * 1.5)];
    fread(buf, sizeof(char), width * height * 1.5, fp);
    fclose(fp);
    memcpy(y, buf, width * height * sizeof(char));
    memcpy(u, buf + width * height, width * height * sizeof(char) / 4);
    memcpy(v, buf + (width * height * 5)/4, width * height * sizeof(char) / 4);
    delete[] buf;
}

void YUVImage::init_image(){
    memset(y, 0, width * height);
    memset(u, 0, width * height / 4);
    memset(v, 0, width * height / 4);
}

void YUVImage::write_to_file(string filename){
    cout << "[" << name << "]: writing image to " << filename << endl;
    
    ofstream fout;
    fout.open(filename, ofstream::out | ofstream::app);
    for(int i = 0; i < width * height; ++i){
        fout << y[i];
    }
    for(int i = 0; i < width * height / 4; ++i){
        fout << u[i];
    }
    for(int i = 0; i < width * height / 4; ++i){
        fout << v[i];
    }
    fout.close();
}
