# QDMAES - Quick and Dirty Matrix Architecture Engine Simulator

Implementation in c that simulates the behavior of the proposal's instructions.
* Instructions are represented as intrinsic-like functions
* Support for arbitrary size matrix multiplication routines
* Supports grouping matrix multiplication routines
* Allows for quick prototyping
* Potential future integration with QEMU


QDMAES supports 4 matrix instruction approach implementations:
* Option A - One matrix per vector
* Option A* - Common-Type Variant
* Option C* - Multiple matrices per register
* Option E - Variable matrix representation


# Installing QDMAES

Run these steps to install QDMAES:  
```
git clone https://github.com/MPU-Research/QDMAES-IME.git
cd QDMAES-IME
```

Run the steps below to set up the expecific option, this step will be responsible for compile and execute the code:
```
cd ExpecificOptionPATH/
make all
```

After that, is expected to have 

# QDMAES Components

Major Components of interest include:

* **high_level_model.c**: The top-level file that includes the calls the functions to compare the results of the option behaviors of the accelerator and the naive algorithm. This file also includes the description of the grouping (e.g., ```mlmul``` and ```nlmul```) and the ```matrix dimensions```.
* **lib.h**: The file that includes the simulated accelerator kernel.
* **mpu.c**: The file that includes the implementation of the accelerator.
* **mpu.h**: The file that includes the parameters of the accelerator.
* * Major parameters include the number of ```VLEN```, in bits, and the number of vector register.
* **basic_helpers.h**: The file that includes the helper functions for the naive algorithm.
 

# Optimal grouping configuration

Each proposal has a specific number for mmol and nlmul that achieves maximum computation intensity, however the simulator only supports integer values. Therefore, below are the maximum values ​​supported for each proposal:


| VLEN | OptA | OptA* | OptC* | OptE |
| ------------- | ------------- | ------------- | ------------- | ------------- |
| 128 | 5 / 4 | 5 / 4 | 5 / 4 | 5 / 4 |
| 256 | 5 / 4 | 3 / 6 | 5 / 4 | 4 / 3 |
| 512 | 5 / 4 | 5 / 4 | 5 / 4 | 3 / 2 |
| 1024 | 5 / 4 | 3 / 6 | 5 / 4 | 3 / 1 |
| 2048 | 5 / 4 | 5 / 4 | 5 / 4 | 1 / 1 |


