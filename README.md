# YUV-Accelerator

------

This project is about YUV image operations' acceleration with multiple instruction sets, including SSE2, MMX and AVX.

## Prerequisites

Please make sure you have the following instruction sets:

- SSE2
- MMX
- AVX

## How to run

### Select instruction set

In main.cpp, line 12~15, comment out the instruction sets that you not need.

If more than one macros are left uncommented, then the first one will be used.

### Run

You can compile and run in command line by:

```
$ sh run.sh
```

However this might not work due to instruction set missing, typically AVX. 

Personally, I enabled AVX in Xcode(ver 9.2). So I compile the project in Xcode and then run in command line. Quite twisty I know.

## Performance

### Alpha Blending

Using [dem2.yuv](https://github.com/VegB/YUV-Accelerator/blob/master/input/dem2.yuv) for demo. Changing alpha from 0 to 255 and it gives: 

![Screen Shot 2017-12-18 at 7.22.50 AM](https://github.com/VegB/YUV-Accelerator/blob/master/img/Screen%20Shot%202017-12-18%20at%207.22.50%20AM.png)

​								*alpha  = 60*

![Screen Shot 2017-12-18 at 7.22.59 AM](https://github.com/VegB/YUV-Accelerator/blob/master/img/Screen%20Shot%202017-12-18%20at%207.22.59%20AM.png)

​								*alpha  = 120*



![Screen Shot 2017-12-18 at 7.23.10 AM](https://github.com/VegB/YUV-Accelerator/blob/master/img/Screen%20Shot%202017-12-18%20at%207.23.10%20AM.png)

​								*alpha = 180*



![Screen Shot 2017-12-18 at 7.23.17 AM](https://github.com/VegB/YUV-Accelerator/blob/master/img/Screen%20Shot%202017-12-18%20at%207.23.17%20AM.png)

​								*alpha = 246*

##### Baseline: x86-64 ISA

- Time cost: 8.54665 s

##### MMX

- Time cost: 12.2618 s 

- Improved by:     -43.46% 


##### SSE2

- Time cost: 6.67817 s 

- Improved by:    21.81%

##### AVX

- Time cost: 5.07584 s

- Improved by:     40.64%




## Superimposing

Changing from [dem1.yuv](https://github.com/VegB/YUV-Accelerator/blob/master/input/dem1.yuv) to   [dem2.yuv](https://github.com/VegB/YUV-Accelerator/blob/master/input/dem2.yuv) for demo:



![Screen Shot 2017-12-18 at 7.27.46 AM](https://github.com/VegB/YUV-Accelerator/blob/master/img/Screen%20Shot%202017-12-18%20at%207.27.46%20AM.png)

​					*alpha1 = 60, alpha2 = 195*



![Screen Shot 2017-12-18 at 7.27.58 AM](https://github.com/VegB/YUV-Accelerator/blob/master/img/Screen%20Shot%202017-12-18%20at%207.27.58%20AM.png)

​					*alpha1 = 120, alpha2 = 135*



![Screen Shot 2017-12-18 at 7.28.09 AM](https://github.com/VegB/YUV-Accelerator/blob/master/img/Screen%20Shot%202017-12-18%20at%207.28.09%20AM.png)

​					*alpha1 = 180, alpha2 = 75*



![Screen Shot 2017-12-18 at 7.28.19 AM](https://github.com/VegB/YUV-Accelerator/blob/master/img/Screen%20Shot%202017-12-18%20at%207.28.19%20AM.png)

​					*alpha1 = 255, alpha2 = 0*



##### Baseline: x86-64 ISA

- Time cost: 23.8617 s

##### MMX

- Time cost: 26.559 s  
- Improved by:     -11.3% 

##### SSE2

- Time cost:  13.4358 s
- Improved by:    43.69%

##### AVX

- Time cost: 16.4382 s
- Improved by:     31.11%

