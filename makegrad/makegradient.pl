#!/bin/sh

export PATH=$PATH:.

makegrad 0 800 >grad_h0_s800.gif
makegrad 0 1024 >grad_h0_s1024.gif
makegrad 0 1152 >grad_h0_s1152.gif
makegrad 0 1280 >grad_h0_s1280.gif
makegrad 0 1600 >grad_h0_s1600.gif
#makegrad 0 2560 >grad_h0_s2560.gif
#makegrad 0 3200 >grad_h0_s3200.gif

makegrad 90 800 >grad_h90_s800.gif
makegrad 90 1024 >grad_h90_s1024.gif
makegrad 90 1152 >grad_h90_s1152.gif
makegrad 90 1280 >grad_h90_s1280.gif
makegrad 90 1600 >grad_h90_s1600.gif
#makegrad 90 2560 >grad_h90_s2560.gif
#makegrad 90 3200 >grad_h90_s3200.gif
