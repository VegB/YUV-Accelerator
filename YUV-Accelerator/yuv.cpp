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
    
    ifstream fin;
    fin.open(filename);
    /* read in Y */
    for(int i = 0; i < width * height; ++i){
        fin >> y[i];
        // cout << (int)y[i] << " " << endl;
    }
    /* read in U */
    for(int i = 0; i < width * height / 4; ++i){
        fin >> u[i];
    }
    /* read in V */
    for(int i = 0; i < width * height / 4; ++i){
        fin >> v[i];
    }
    fin.close();
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
