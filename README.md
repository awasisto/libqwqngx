libqwqngx
=========

A wrapper for the Quantum World Corporation (QWC) / ComScire quantum random
number generator (QRNG) driver with extra features including:

- Unbounded random byte generation.
- Efficient uniform 32-bit integer random number generation using an improved 
  Lemire's nearly divisionless random integer generation algorithm.
- Efficient normal 64-bit floating-point random number generation using an 
  improved Ziggurat algorithm.

Installation (for Debian and derivatives)
-----------------------------------------

```bash
# Install g++, cmake, libusb-1.0-0-dev, and libconfuse-dev

sudo apt update
sudo apt -y install g++ cmake libusb-1.0-0-dev libconfuse-dev
sudo ln -s $(find /usr/lib -name libusb-1.0.a -print -quit) /usr/local/lib/libusb-1.0.a
sudo ln -s $(find /usr/lib -name libusb-1.0.so -print -quit) /usr/local/lib/libusb-1.0.so

# Install libftdi1

cd 3rdparty/libftdi1
mkdir build
cd build
cmake ..
make
sudo make install
sudo ln -s /usr/local/include/libftdi1/ftdi.h /usr/local/include/ftdi.h
cd ../..

# Install libqwqng

cd libqwqng
mkdir build
cd build
cmake ..
make
sudo make install
cd ../../..

# Install libqwqngx

mkdir build
cd build
cmake ..
make
sudo make install
sudo ldconfig
```

License
-------

    MIT License

    Copyright (c) 2025 Andika Wasisto

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.