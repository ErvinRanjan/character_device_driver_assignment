
# Character Device Driver Assignment

## Author: Ervin Ranjan
## Roll Number: B220027CS

### Overview
This assignment features a character device driver that implements the following functionalities:
1. Kernel version checking during module insertion.
2. Device read/write operations using command line tools and a user program.

### Problem Statement
The driver must:
- Accept an array parameter called `kernel_version` to verify compatibility with the currently running kernel version.
- Print the assigned major and minor numbers upon successful insertion.
- Allow writing the string `<FIRSTNAME>_<ROLLNO>` to the device using the `echo` command and reading it back using the `cat` command.
- Print appropriate messages in the kernel log when the device is opened, closed, read from, and written to.

### Methodology 
Detailed function signatures can be found in pdf
1. **Module Parameters**: The `kernel_version` parameter is handled using:
   `module_param_array`

2. **Driver Insertion**: The insertion function `chr_driver_init` performs the following operations:
   - Checks the kernel version using `LINUX_VERSION_CODE` and `KERNEL_VERSION_CODE(major,minor,patch)`.
   - Allocates memory for the device buffer with `kmalloc`.
   - Registers character device numbers using `alloc_chrdev_region`.
   - Initializes the character device structure and adds it with `cdev_add`.
   - Creates a device class and a device file node in `/dev`.

3. **Driver Exit**: The `chr_driver_exit` function cleans up resources by:
   - Freeing the allocated buffer using `kfree`.
   - Destroying the device and class using `device_destroy` and `class_destroy`.
   - Unregistering character device numbers with `unregister_chrdev_region`.

4. **File Operations**:
   - **Open**: Prints messages upon opening the device.
   - **Release**: Prints messages upon closing the device.
   - **Read/Write**: Uses `copy_to_user` and `copy_from_user` for data transfer, while updating offsets and maintaining buffer size.

### Execution Process

#### 1. Compilation
To compile the driver code, navigate to the directory containing the source code and run:
```bash
make
```

#### 2. Inserting the Module
Insert the module with the command:
```bash
insmod chr_driver.ko kernel_version=6,5,13
```
- If the kernel version does not match, an error message will be printed in the kernel message buffer, viewable using:
```bash
dmesg | tail
```

#### 3. Checking Device Registration
After successful insertion, check the major number assigned to the device:
```bash
cat /proc/devices
```
- The device will be listed as `character_device` with its major number.

#### 4. Device Interaction
The device file will be created in `/dev/` as `character_device_1`. Change the ownership to allow read/write access:
```bash
sudo chown ervin /dev/character_device_1
```

- **Writing to the Device**:
Use the `echo` command to write data:
```bash
echo "ERVIN_B220007CS" > /dev/character_device_1
```

- **Reading from the Device**:
Use the `cat` command to read data:
```bash
cat /dev/character_device_1
```

#### 5. User Program
A sample user program (`read_write.c`) is included to demonstrate file operations. Compile and execute the user program:
```bash
gcc read_write.c -o read_write
./read_write
```

### Conclusion
This assignment demonstrates the creation and functionality of a character device driver, including kernel version checks, device read/write operations, and interaction with user programs. The code is structured to ensure efficient handling of device operations while providing clear kernel log messages for debugging and verification.
