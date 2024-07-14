# QDMAES - Quick and Dirty Matrix Architecture Engine Simulator

QDMAES is a C-based implementation designed to simulate the behavior of proposed matrix instruction sets. Below are the key features and capabilities of QDMAES:
* Intrinsic-Like Functions for Instructions: The instructions within QDMAES are represented as intrinsic-like functions, providing a clear and concise way to simulate complex matrix operations.
* Support for Arbitrary Size Matrix Multiplication Routines: QDMAES supports matrix multiplication routines of any shape
* Support for Grouping Matrix Multiplication Routines: The simulator allows for the grouping of matrix multiplication routines, enabling users to execute multiple operations as a single grouped instruction. 
* Quick Prototyping: QDMAES facilitates rapid prototyping of new matrix instructions and architectures.
* Study of Potential New Performance Metrics: With QDMAES, users can investigate and develop new performance metrics tailored to matrix operations. 
* Potential Future Integration with QEMU: There are plans for future integration with QEMU.


QDMAES supports four experimental options for the IME approach:
* Option A - One matrix per vector
  * In this configuration, all registers are designed to store a single square tile of the matrix. This tile is represented as [λ] x [λ], where λ is the floor value of VLEN divided by ELEMENT_WIDTH. This approach ensures that each register holds one square tile of the matrix, facilitating straightforward matrix operations.
* Option A* - Common-Type Variant
  * This variation of Option A, proposed by José Moreira, addresses scenarios where a single square tile is insufficient to fully occupy a register. In this modified approach, registers store two square tiles, one placed on top of the other, thereby forming a rectangular block.
  * To validate the execution in this configuration, each MAC (Multiply-Accumulate) instruction consumes two registers from matrix A and one register from matrix B. Consequently, when registers store two blocks, the number of registers allocated to store data from matrix A is doubled.
* Option C* - Multiple matrices per register
  * This variation of Option C, also proposed by José Moreira, tackles situations where a single square tile does not fully occupy a register. In this approach, registers store multiple square tiles (denoted as L), stacked one on side of the other to form a rectangular block.
  * In the current simulator implementation, only cases where L assumes the values of 1 or 2 are considered, always opting for the lowest value that fills the register. 
* Option E - Variable matrix representation


# Installing QDMAES

To install QDMAES, follow these steps.

First, clone the QDMAES repository from GitHub:
```
git clone https://github.com/MPU-Research/QDMAES-IME.git
cd QDMAES-IME
```

Next, set up the specific option for your simulation. This step involves compiling and executing the code:

```
cd ExpecificOptionPATH/
make all
```

Replace ```SpecificOptionPATH``` with the appropriate directory path for the specific option you wish to set up.

After running the above commands, the simulation of the proposed matrix architecture should be complete. The system will compile the necessary files and execute the code based on the selected option. Upon successful execution, you should see a description of the configuration used in the simulation. This output provides details about the specific matrix architecture and instructions being simulated.

# QDMAES Components

Major Components of interest include:

* **high_level_model.c**: This is the top-level file responsible for coordinating the simulation. It includes calls to functions that compare the behaviors of different options of the accelerator against the naive algorithm. Additionally, this file describes the grouping operations (such as mlmul and nlmul) and specifies the matrix dimensions used in the simulation.
* **lib.h**: This file contains the simulated accelerator kernel. It includes the necessary functions and definitions required to emulate the behavior of the accelerator within the simulation environment. 
* **mpu.c**: This file implements the accelerator's functionality. It contains the core logic and algorithms that define how the accelerator processes matrix operations.
* **mpu.h**: This header file defines the parameters of the accelerator. Key parameters include the number of ```VLEN``` (Vector Length, in bits) and the ```number of vector registers```. 
* **basic_helpers.h**: This file includes helper functions for the naive algorithm. These functions support the basic operations and computations required for the naive approach to matrix multiplication. 
 

# Optimal grouping configuration

Each proposal within the QDMAES framework is characterized by specific values for mlmul and nlmul that achieve maximum computational intensity. These values are critical for optimizing the performance of matrix multiplications. However, it is important to note that the simulator only supports integer values for these parameters. Below are the maximum integer values supported for each proposal:


| VLEN | OptA | OptA* | OptC* | OptE |
| ------------- | ------------- | ------------- | ------------- | ------------- |
| 128 | 5 / 4 | 5 / 4 | 5 / 4 | 5 / 4 |
| 256 | 5 / 4 | 3 / 6 | 5 / 4 | 4 / 3 |
| 512 | 5 / 4 | 5 / 4 | 5 / 4 | 3 / 2 |
| 1024 | 5 / 4 | 3 / 6 | 5 / 4 | 3 / 1 |
| 2048 | 5 / 4 | 5 / 4 | 5 / 4 | 1 / 1 |

# Extracted information

After the simulation is completed, the user is presented with a description of the configuration used during the simulation. The extracted information includes:
* Block Size Allocated within the Vector Register: This metric indicates the size of the block that was allocated within each vector register during the simulation. Understanding the block size is crucial for analyzing how efficiently the register space was utilized and how the matrix tiles were mapped onto the hardware. 
* Computational Intensity Value for the Simulated Configuration: The computational intensity value represents the ratio of arithmetic operations to memory operations for the given configuration.
* Number of FMMAC Instructions Dispatched: This parameter reflects the total number of Floating Point Multiply Accumulate (FMMAC) instructions that were dispatched during the simulation.
* Number of MAC Operations Performed: The number of Multiply-Accumulate (MAC) operations performed during the simulation.

By gathering and analyzing this information, we aim to:

* Identify Patterns: Recognize recurring trends and behaviors in the simulation data, which can provide insights into the optimal configurations and performance characteristics of the accelerator.
* Uncover Hidden Performance Metrics: Reveal subtle and non-obvious metrics that may influence the overall performance. These hidden metrics can guide future optimizations and enhancements in matrix architecture design.
