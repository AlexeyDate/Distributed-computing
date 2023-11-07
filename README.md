# Parallelization results
**Information about the processor on which the time estimation was performed:**
* AMD FX-8350
* 8 Cores, 8 threads
* Base frequency: 4000 mHZ
* Max frequency: 4200 mHZ
___
**Сalculation of time from changing the number of points for 1, 2, 4, 8 threads. The range of points was from 50 to 2000 in increments of 50 points.**
![TimeUp](https://github.com/AlexeyDate/DistributedComputing/assets/86290623/2d28b32a-48e7-43f9-a25b-a8f0e7250917)

___
<p align="center"><b>Calculation of speed up</p></b>

```math
S = Tserial / Tparallel
```
![SpeedUp](https://github.com/AlexeyDate/DistributedComputing/assets/86290623/2a311764-6cd4-4ee4-8653-3e49938e30b1)

___
<p align="center"><b>Calculation of Efficiency (p - number of threads)</p></b>
  
```math
E = S / p = Tserial / (p * Tparallel)
```
![Efficiency](https://github.com/AlexeyDate/DistributedComputing/assets/86290623/8cdcba9a-37c2-4844-89c5-6b21cc95cfd8)
