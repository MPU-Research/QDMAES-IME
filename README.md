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
