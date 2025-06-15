# Project 2 - Bring up Gordian placer in Linux

## Introduction and Background

This project implements a Gordian placement algorithm, a fundamental step in the physical design process of Very Large Scale Integration (VLSI) circuits. Placement determines the precise location of each component (like logic gates and memory blocks) on a silicon chip. The Gordian algorithm is a well-known quadratic placement technique that uses mathematical optimization to find an initial placement, which is then refined by subsequent stages.

This project is designed to be built and executed on a remote Linux environment, specifically CentOS 7, utilizing standard command-line tools like `make` and external libraries such as Armadillo, BLAS, and LAPACK.

### Prerequisites

To build and run this project, the following environment is required on your CentOS 7 machine (`cosr101`):

* **Operating System:** CentOS 7.
* **Compiler:** GCC 9.3.1 or newer, available via the `devtoolset-9` Software Collection.
* **Build Tools:** `make`, `wget`, `tar`, `cmake`.
* **Libraries:**
    * OpenBLAS (or another BLAS/LAPACK implementation) compiled and installed locally.
    * Armadillo C++ library linked against your local BLAS/LAPACK installation, compiled and installed locally.

*Note: This project operates under the assumption that you do not have root/sudo privileges on the remote machine. Therefore, external libraries must be compiled and installed within a user-accessible directory, such as `~/Assignment2/third_party` as used in the examples below.*

## Environment Setup

This section details necessary code changes for Linux compatibility and steps to set up the required build environment and libraries.

### Code Changes for Linux Compatibility

Some functions or practices common in Windows development environments (like Visual Studio) might need adjustments for standard C++ and GCC compatibility on Linux.

* **`snprintf` vs `sprintf_s`:**
    The `sprintf_s` function is specific to Microsoft's implementation and is not available in standard C++. Use `snprintf` for safer, standard-compliant string formatting with buffer size checks.

    ```c++
    // Original (Windows/MSVC specific):
    // sprintf_s(filename, "Partition_plot_%d.bmp", *fileID);

    // New (Standard C++):
    snprintf(filename, sizeof(filename), "Partition_plot_%d.bmp", *fileID);
    ```
    *Explanation:* `snprintf` is a standard C library function that formats a string and stores it in a buffer, taking the buffer size as an argument to prevent buffer overflows.

* **Function Return Type:**
    Ensure functions have appropriate return types and return values where expected. The original `Gordian::netlist_read()` might have been intended to return a status.

    ```c++
    // Original:
    char Gordian::netlist_read()
    {
        printf("input the name of netlist file.(include file name extension)\n");
        // ... file reading logic ...
        netlist_file.close();
        // Missing return statement or intended as void?
    }

    // New (assuming a return value is needed/expected):
    char Gordian::netlist_read()
    {
        printf("input the name of netlist file.(include file name extension)\n");
        // ... file reading logic ...
        netlist_file.close();
        return 1; // Added a return value
    }
    ```
    *Explanation:* Adding `return 1;` ensures the function explicitly returns a `char` value, which might be necessary depending on how the calling code uses this function, or to satisfy compiler requirements.

### Sourcing GCC 9.3 (devtoolset-9)

CentOS 7's default GCC is older. `devtoolset-9` provides GCC 9.3.1. To use this newer compiler, you need to enable its environment in your current shell session.

```bash
source scl_source enable devtoolset-9
```
Explanation: The `scl_source` enable command is part of the Software Collections (SCL) utility. It modifies the current shell's environment variables (like `PATH`, `LD_LIBRARY_PATH`) to include the paths for `devtoolset-9`, making its binaries (like `g++`) and libraries the default for commands executed in this shell. You need to run this command in every new terminal session before building or running the project.

### Downloading and Installing OpenBLAS
OpenBLAS is a high-performance implementation of the BLAS and LAPACK libraries, which Armadillo uses for linear algebra operations. You need to download, compile, and install it in your local directory.

```Bash
# Download the source code for version 0.3.27
wget [https://github.com/OpenMathLib/OpenBLAS/archive/refs/tags/v0.3.27.tar.gz](https://github.com/OpenMathLib/OpenBLAS/archive/refs/tags/v0.3.27.tar.gz)

# Extract the downloaded tarball
tar -xzf v0.3.27.tar.gz

# Change into the extracted source directory
cd OpenBLAS-0.3.27

# Compile OpenBLAS
make

# Install OpenBLAS to a specific location
make PREFIX=../third_party/openblas install
```
### Downloading and Installing Armadillo
Armadillo is a C++ library for linear algebra, designed to be easy to use while integrating with high-performance BLAS/LAPACK libraries like OpenBLAS.

1. **Download Armadillo**: Download the source code from the official website: https://arma.sourceforge.net/download.html
For example, download version 14.4.1: armadillo-14.4.1.tar.xz

2. **Compile and Install Armadillo**:

```Bash
# Assuming you are in the project root directory and the armadillo tarball is here
tar -xvf armadillo-14.4.1.tar.xz

cd armadillo-14.4.1/

mkdir build

cd build/

# Configure the build using CMake
cmake .. \
    -DCMAKE_INSTALL_PREFIX=/disk1/2025DA1/group4/projects/p2/Gordian_Placement/Assignment2/third_party/armadillo \
    -DBLAS_LIBRARY=/disk1/2025DA1/group4/projects/p2/Gordian_Placement/Assignment2/third_party/openblas/lib/libopenblas.so \
    -DLAPACK_LIBRARY=/disk1/2025DA1/group4/projects/p2/Gordian_Placement/Assignment2/third_party/openblas/lib/libopenblas.so

# Compile Armadillo
make

# Install Armadillo
make install
```
Note: Adjust the installation paths (`CMAKE_INSTALL_PREFIX`, `BLAS_LIBRARY`, `LAPACK_LIBRARY`) in the `cmake` command to match the actual locations on your cosr101 account.


## Building the Project
Follow these steps to build the `GordianPlacement` executable using the Makefile:

1. Connect to `cosr101` via SSH using your terminal.

2. Navigate to the project root directory (e.g., `~/Assignment2`) where the Makefile is located.

3. Enable the devtoolset-9 environment for your current shell session. This is crucial for using the correct GCC version and compatible libraries.

```Bash
source scl_source enable devtoolset-9
```

4. Clean any previous build artifacts (object files, executable). This is optional but recommended for a clean build.

```Bash
make clean
```


5. Run the make command to build the project.

```Bash
make
```
Upon successful build, an executable file named `GordianPlacement` will be created in the current directory (as per the Makefile configuration).

### Makefile Explanation
The Makefile controls the build process of your project. It defines compiler settings, source files, dependencies, and build steps.

Makefile
```bash
#---------------------------------------------------
# Makefile for Gordian_Placement (含 EasyBMP)
#---------------------------------------------------

# --- Compiler Settings ---
CXX        := g++ # The C++ compiler command to use. Set to g++.
CXXFLAGS   := -std=c++14 -O2 -Wall -g \ # Flags for the C++ compiler.
              -Ithird_party/armadillo/include \ # Add Armadillo include path.
              -I/usr/include/armadillo # Add standard system Armadillo include path (optional, might not be needed if using local install exclusively).
# -std=c++14: Use C++14 standard.
# -O2: Optimization level 2.
# -Wall: Enable all standard warnings.
# -g: Include debugging information.
# -I<path>: Add <path> to the list of directories to search for header files.

# LDFLAGS needs to include search paths (-L) and runtime paths (-rpath) for libraries.
LDFLAGS := -Lthird_party/armadillo/lib64 -Lthird_party/openblas/lib \ # Add library search paths.
           -Wl,-rpath,<span class="math-inline">\(PWD\)/third\_party/armadillo/lib64 \\ \# Add runtime library search path for armadillo\.
\-Wl,\-rpath,</span>(PWD)/third_party/openblas/lib # Add runtime library search path for openblas.
           # Note: $(PWD) expands to the absolute path of the current directory where make is executed. This helps the executable find shared libraries at runtime without needing to set LD_LIBRARY_PATH explicitly every time.
# -L<path>: Add <path> to the list of directories to search for libraries during linking.
# -Wl,<options>: Pass comma-separated options to the linker.
# -rpath,<path>: Add <path> to the runtime search path for shared libraries.

# LDLIBS := $(LDFLAGS) -larmadillo -lopenblas -lgfortran -lm # Libraries to link against.
# LDLIBS := $(LDFLAGS) -llapack -lblas -lgfortran -lm # Alternative linking if using static lapack/blas or different names.
LDLIBS := -larmadillo -lopenblas -lgfortran -lm # Libraries to link against. LDFLAGS are often applied separately by the linker rule.
# -l<library_name>: Link against library lib<library_name>.so or lib<library_name>.a.
# -larmadillo: Link Armadillo library.
# -lopenblas: Link OpenBLAS library (provides BLAS and LAPACK).
# -lgfortran: Link the GNU Fortran runtime library (often needed by BLAS/LAPACK).
# -lm: Link the math library.

# --- Project Source Files ---
SRCS       := main.cpp \
              EasyBMP.cpp \
              EasyBMP_Geometry.cpp \
              Gordian.cpp
# List of all .cpp source files in the project.

# --- Automatically Generate Object File List (.o) ---
OBJS       := $(SRCS:.cpp=.o)
# Generates corresponding .o file names from the source file list.

# --- Final Executable Name ---
TARGET     := GordianPlacement
# The name of the final executable file.

#---------------------------------------------------
# 1) Default Target: Build everything
#---------------------------------------------------
all: $(TARGET)
# The default target, executed when you just type 'make'. Depends on the executable.

#---------------------------------------------------
# 2) Link: Combine all .o files
#---------------------------------------------------
$(TARGET): $(OBJS)
    $(CXX) $^ -o $@ $(LDLIBS) <span class="math-inline">\(LDFLAGS\) \# Add LDFLAGS here during linking
\# Rule to build the target executable\. Depends on all object files \(</span>^).
# Compiles/links using CXX, inputs are object files (<span class="math-inline">^\), output is the target \(</span>@), links libraries (LDLIBS) and applies linker flags (LDFLAGS).

#---------------------------------------------------
# 3) Compile Rule: .cpp -> .o
#---------------------------------------------------
%.o: %.cpp
    $(CXX) $(CXXFLAGS) -c $< -o <span class="math-inline">@
\# Generic rule to compile any \.cpp file \(</span><) into a .o file ($@).
# Uses CXX with CXXFLAGS and the -c flag (compile only, do not link).

#---------------------------------------------------
# 4) Clean intermediate files and executable
#---------------------------------------------------
.PHONY: clean
clean:
    rm -f $(OBJS) $(TARGET)
# A phony target (not a real file). Removes all object files and the executable.

#---------------------------------------------------
# 5) Check Variables (for debugging Makefile)
#---------------------------------------------------
.PHONY: info
info:
    @echo "CXX        = $(CXX)"
    @echo "CXXFLAGS = $(CXXFLAGS)"
    @echo "LDFLAGS    = $(LDFLAGS)" # LDFLAGS might be complex with Wl,-rpath, check with specific tools if needed
    @echo "LDLIBS     = $(LDLIBS)"
    @echo "SRCS       = $(SRCS)"
    @echo "OBJS       = $(OBJS)"
    @echo "TARGET     = <span class="math-inline">\(TARGET\)"
\# A phony target to print the values of key Makefile variables\.
\# \# Optional\: How to run the program after setting LD\_LIBRARY\_PATH
\# \# Get the absolute path of the current directory
\# PROJECT\_DIR\=</span>(pwd)

# # Set LD_LIBRARY_PATH, adding your library directories to the search path
# # Note: You need to add both armadillo and openblas lib directories
# export LD_LIBRARY_PATH=$PROJECT_DIR/third_party/armadillo/lib64:$PROJECT_DIR/third_party/openblas/lib:$LD_LIBRARY_PATH

# # Now execute your program
# # ./GordianPlacement
```

## Running the Project


### To execute the `GordianPlacement` program

1.  Connect to `cosr101` via SSH.
2.  Navigate to the directory containing the `GordianPlacement` executable.
3.  Enable the devtoolset-9 environment (necessary for runtime library compatibility):
    ```bash
    source scl_source enable devtoolset-9
    ```
4.  Run the executable:
    ```bash
    ./GordianPlacement
    ```
5.  The program will prompt you for the netlist file name:
    ```text
    input the name of netlist file.(include file name extension)
    ```
6.  Enter the full name of your netlist file (e.g., `netlist_L.txt`) and press Enter.

The program will process the input and output results similar to this:

```text
partition finish!
Overall iterations: 10
Overall wire length is 20539.959(include fixed cells)
Calculation end!
Time for Gordian iterations: 27.09
```

The program will also output the result graph (.bmp) of each partition.

### Organizing Output Files

To help manage the generated .bmp files and results from different runs, a shell script `organize_bmp.sh` is provided. This script collects all .bmp files from the current directory and moves them into a user-specified subdirectory within the Result folder.

Ensure the Result directory exists in the same location as the GordianPlacement executable and the organize_bmp.sh script. If it doesn't exist, create it:

```Bash
mkdir Result
```
Ensure the organize_bmp.sh script has execute permissions:

```Bash
chmod +x organize_bmp.sh
```
Run the script, providing the desired name for the result subfolder as a command-line argument:

```Bash
./organize_bmp.sh <folder_name>
```
Replace <folder_name> with the name you want for the subdirectory (e.g., run_netlist_L, case1, experiment_A).

Example:

```Bash
./organize_bmp.sh netlist_L_results
```
The script will create the directory `Result/<folder_name>` (if it doesn't already exist) and move all `.bmp` files from the current directory into this new folder.



### Deleting Output (.bmp files)
If you need to clean up the generated `.bmp` files from the current directory, you can use the find command.

```Bash
# Optional: Confirm the files to be deleted
find . -maxdepth 1 -type f -name "*.bmp" -ls

# Delete the files
find . -maxdepth 1 -type f -name "*.bmp" -delete
```

Caution: The -delete action is permanent. Use the `-ls` command first to ensure you are deleting the correct files.

## Execution process
![alt text](image.png)
![alt text](image-2.png)
