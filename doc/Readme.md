# RingBufferCommunication

## Project Description
This project implements a simple communication mechanism between a sender and a receiver using a ring buffer. The project is split into three main components:
- `sender.c`: Handles the sending of data to the ring buffer.
- `receiver.c`: Handles the receiving of data from the ring buffer.
- `common.c` and `common.h`: Contain common definitions and functions used by both the sender and receiver.

## File Structure
- `sender.c`: Contains the implementation for the sender.
- `receiver.c`: Contains the implementation for the receiver.
- `common.c`: Contains shared functionality between the sender and receiver.
- `common.h`: Header file for `common.c`.
- `CMakeLists.txt`: CMake build configuration file.

## Prerequisites
- CMake (version 3.10 or higher)
- A C compiler (supporting C99 standard)

## Installation

1. **Clone the repository:**
   ```bash
   git clone <repository_url>
   cd <repository_name>
   ```

2. **Build the project using CMake:**
    ```bash
    mkdir build
    cd build
    cmake ..
    make
    ```

## Usage
After building the project, you will have two executables: sender and receiver.

```bash
./sender -m NumberofRingbufferElements < input.txt
```

```bash
./receiver -m NumberofRingbufferElements > output.txt
```

## Credits
Developed by Ilija Kostic