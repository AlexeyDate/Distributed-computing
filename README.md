# Parallelization results
**Information about the processor on which the time estimation was performed:**
* AMD FX-8350
* 8 Cores, 8 threads
* Base frequency: 4000 mHZ
* Max frequency: 4200 mHZ
___
**Сalculation of time from changing the number of points for 1, 2, 4, 8 threads. The range of points was from 50 to 2000 in increments of 50 points.**
![TimeUp](https://github.com/AlexeyDate/DistributedComputing/assets/86290623/c8334f03-9db7-487f-8506-32ab99be206e)

___
<p align="center"><b>Calculation of speed up</p></b>

```math
S = Tserial / Tparallel
```
![SpeedUp](https://github.com/AlexeyDate/DistributedComputing/assets/86290623/efc2bd88-57fe-4f83-9466-9ea0c4a8d60d)

___
<p align="center"><b>Calculation of Efficiency (p - number of threads)</p></b>
  
```math
E = S / p = Tserial / (p * Tparallel)
```
![Efficiency](https://github.com/AlexeyDate/DistributedComputing/assets/86290623/066b13f1-5655-426c-8ef6-28391371fddb)
