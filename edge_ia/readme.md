Sea State Classification with MPU-6500 and Edge Impulse
This project uses a Raspberry Pi Pico with an MPU-6500 sensor to classify sea states as "mar_agitado" (rough sea) or "mar_calmo" (calm sea) using an Edge Impulse machine learning model.

Hardware Setup
Required Components
Raspberry Pi Pico or compatible board
MPU-6500 accelerometer/gyroscope sensor
2 LEDs (red and green)
2 resistors (220Ω - 470Ω)
Breadboard and jumper wires
Connections
MPU-6500 Sensor (I2C1 - Left port on BitDogLab)
VCC → 3.3V
GND → GND
SDA → GPIO 2
SCL → GPIO 3
LEDs
Red LED (mar_agitado): GPIO 13 → Resistor → LED → GND
Green LED (mar_calmo): GPIO 11 → Resistor → LED → GND
Software Setup
Prerequisites
Pico SDK: Install and configure the Raspberry Pi Pico SDK
Edge Impulse SDK: Download your trained model from Edge Impulse
CMake: Version 3.13 or higher
ARM GCC Toolchain: For cross-compilation
Project Structure
my-project/
├── edge-impulse-sdk/          # Edge Impulse SDK (from your trained model)
├── model-parameters/          # Model parameters (from Edge Impulse)
├── tflite-model/             # TensorFlow Lite model (from Edge Impulse)
├── main.cpp                  # Main application code
├── mpu6500.h                 # MPU-6500 driver header
├── mpu6500.c                 # MPU-6500 driver implementation
├── config.h                  # Project configuration
├── CMakeLists.txt            # CMake build configuration
├── Makefile                  # Simple build script
└── README.md                 # This file
Setting up Edge Impulse Model
Train your model on Edge Impulse Studio:
Create a new project
Upload your IMU data (accelerometer + gyroscope)
Label data as "mar_agitado" or "mar_calmo"
Design impulse with preprocessing and classification
Train and test your model
Download the C++ library:
Go to your Edge Impulse project
Navigate to "Deployment" → "C++ library"
Download the ZIP file
Extract the contents to get:
edge-impulse-sdk/ folder
model-parameters/ folder
tflite-model/ folder
Place the Edge Impulse files in your project directory as shown in the structure above.
Building the Project
Using the Makefile (Recommended)
bash
# Build the project
make build

# Clean build files
make clean

# Build and deploy to Pico
make deploy

# Monitor serial output
make monitor
Using CMake directly
bash
# Create build directory
mkdir build
cd build

# Configure build
cmake -DCMAKE_BUILD_TYPE=Release ..

# Build
make -j4
Environment Setup
Make sure you have the following environment variables set:

bash
export PICO_SDK_PATH=/path/to/pico-sdk
export PICO_TOOLCHAIN_PATH=/path/to/arm-none-eabi-gcc
Usage
Running the Application
Connect your hardware according to the wiring diagram above
Build and flash the firmware to your Pico
Connect via USB and open a serial monitor (115200 baud)
Move the sensor to simulate different sea conditions
Expected Behavior
The system will:

Initialize the MPU-6500 sensor
Collect 2 seconds of sensor data (100 samples at 50Hz)
Run inference using your Edge Impulse model
Display predictions on the serial monitor
Light up the appropriate LED:
🔴 Red LED: "mar_agitado" (rough sea)
🟢 Green LED: "mar_calmo" (calm sea)
Serial Output Example
--- Sea State Classification System ---
Using MPU-6500 with Edge Impulse inference
I2C: SDA=2, SCL=3
LEDs: Red=13 (mar_agitado), Green=11 (mar_calmo)
MPU-6500 initialized successfully
Starting data collection and inference...
Collecting sensor data...
Data collection complete. Samples: 100
Running inference...
Inference completed successfully!
Timing: DSP 45 ms, inference 23 ms
Predictions:
  mar_agitado: 0.850
  mar_calmo: 0.150
Best prediction: mar_agitado (0.850)
🔴 MAR AGITADO detected!
---
Configuration
Sampling Parameters
You can adjust the sampling parameters in config.h:

SAMPLE_RATE_HZ: Data collection frequency (default: 50Hz)
WINDOW_SIZE_MS: Inference window size (default: 2000ms)
CONFIDENCE_THRESHOLD: Minimum confidence for classification
Hardware Pins
Modify pin assignments in config.h if you need different connections:

I2C pins (SDA/SCL)
LED pins (red/green)
Troubleshooting
Common Issues
Build errors: Make sure PICO_SDK_PATH is set correctly
I2C communication errors: Check wiring and pull-up resistors
Model size too large: Reduce model complexity in Edge Impulse
Inference errors: Verify model files are in correct directories
Debug Options
Enable debug output in config.h:

c
#define DEBUG_SENSOR_DATA 1    // Print raw sensor values
#define DEBUG_INFERENCE 1      // Print detailed inference info
Performance Notes
Sample rate: 50Hz provides good balance between accuracy and performance
Window size: 2 seconds gives enough data for reliable classification
Memory usage: ~2.4KB for sensor buffer + model memory requirements
Inference time: Typically 20-50ms depending on model complexity
License
This project is provided as-is for educational and research purposes. Make sure to comply with Edge Impulse's licensing terms for your trained model.

