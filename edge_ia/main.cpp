#include "edge-impulse-sdk/classifier/ei_run_classifier.h"
#include "config.h"

#include <hardware/gpio.h>
#include <hardware/i2c.h>
#include <hardware/uart.h>
#include <pico/stdio_usb.h>
#include <pico/stdlib.h>
#include <stdio.h>
#include <string.h>

// --- MPU6500 Definitions and Implementation ---

// MPU6500 I2C Address and Registers
#define MPU6500_ADDR     0x68 // I2C address when AD0 pin is grounded
#define MPU6500_PWR_MGMT 0x6B // Power Management register
#define MPU6500_ACCEL_X  0x3B // First accelerometer data register

// Data structure for MPU6500 readings
typedef struct {
    int16_t accel[3]; // Acceleration X, Y, Z
    int16_t gyro[3];  // Gyroscope X, Y, Z
    int16_t temp;     // Temperature
} mpu6500_data_t;

// Initialize MPU6500 sensor
void mpu6500_init(i2c_inst_t* i2c) {
    // Wake up MPU6500 by writing 0x00 to PWR_MGMT_1 register
    uint8_t buf[] = {MPU6500_PWR_MGMT, 0x00};
    i2c_write_blocking(i2c, MPU6500_ADDR, buf, 2, false);
}

// Read raw data from MPU6500
void mpu6500_read_raw(i2c_inst_t* i2c, mpu6500_data_t* data) {
    uint8_t buffer[14];

    // Point to first data register (ACCEL_XOUT_H at 0x3B)
    uint8_t reg = MPU6500_ACCEL_X;
    i2c_write_blocking(i2c, MPU6500_ADDR, &reg, 1, true);
    i2c_read_blocking(i2c, MPU6500_ADDR, buffer, 14, false);

    // Convert byte pairs to 16-bit values (big-endian)
    data->accel[0] = (buffer[0] << 8) | buffer[1];
    data->accel[1] = (buffer[2] << 8) | buffer[3];
    data->accel[2] = (buffer[4] << 8) | buffer[5];
    data->temp     = (buffer[6] << 8) | buffer[7];
    data->gyro[0]  = (buffer[8] << 8) | buffer[9];
    data->gyro[1]  = (buffer[10] << 8) | buffer[11];
    data->gyro[2]  = (buffer[12] << 8) | buffer[13];
}

// --- Main Application Code ---

// Buffer to store sensor data for inference
static float features[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE];
static int feature_ix = 0;

// Callback function to provide data to the classifier
int raw_feature_get_data(size_t offset, size_t length, float *out_ptr) {
    memcpy(out_ptr, features + offset, length * sizeof(float));
    return 0;
}

// Convert raw sensor data to physical units
void convert_sensor_data(mpu6500_data_t* raw, float* accel_g, float* gyro_dps) {
    // Convert to g (gravity) - default scale ±2g
    const float accel_scale = 16384.0f; // LSB/g for ±2g
    accel_g[0] = raw->accel[0] / accel_scale;
    accel_g[1] = raw->accel[1] / accel_scale;
    accel_g[2] = raw->accel[2] / accel_scale;
    
    // Convert to degrees/second - default scale ±250°/s
    const float gyro_scale = 131.0f; // LSB/(°/s) for ±250°/s
    gyro_dps[0] = raw->gyro[0] / gyro_scale;
    gyro_dps[1] = raw->gyro[1] / gyro_scale;
    gyro_dps[2] = raw->gyro[2] / gyro_scale;
}

// Collect sensor data for inference
bool collect_sensor_data() {
    printf("Collecting sensor data...\n");
    
    mpu6500_data_t raw_data;
    float accel_g[3], gyro_dps[3];
    
    feature_ix = 0;
    uint32_t start_time = to_ms_since_boot(get_absolute_time());
    
    while (feature_ix < EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE) {
        // Read sensor data
        mpu6500_read_raw(I2C_PORT, &raw_data);
        
        // Convert to physical units
        convert_sensor_data(&raw_data, accel_g, gyro_dps);
        
        // Store in feature buffer (6 values per sample)
        if (feature_ix + 6 <= EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE) {
            features[feature_ix++] = accel_g[0];
            features[feature_ix++] = accel_g[1];
            features[feature_ix++] = accel_g[2];
            features[feature_ix++] = gyro_dps[0];
            features[feature_ix++] = gyro_dps[1];
            features[feature_ix++] = gyro_dps[2];
            
            #if DEBUG_SENSOR_DATA
            printf("A: %.2f,%.2f,%.2f G: %.2f,%.2f,%.2f\n",
                   accel_g[0], accel_g[1], accel_g[2],
                   gyro_dps[0], gyro_dps[1], gyro_dps[2]);
            #endif
        }
        
        // Maintain sampling rate
        while (to_ms_since_boot(get_absolute_time()) < start_time + (feature_ix/6) * (1000/SAMPLE_RATE_HZ)) {
            tight_loop_contents();
        }
    }
    
    printf("Data collection complete. Samples: %d\n", feature_ix/6);
    return true;
}

// Control LEDs based on prediction
void update_leds(const char* prediction, float confidence) {
    if (strcmp(prediction, "mar_agitado") == 0) {
        gpio_put(LED_RED_PIN, 1);   // Red LED on
        gpio_put(LED_GREEN_PIN, 0); // Green LED off
        printf("🔴 MAR AGITADO detected! (confidence: %.2f%%)\n", confidence * 100);
    } else if (strcmp(prediction, "mar_calmo") == 0) {
        gpio_put(LED_RED_PIN, 0);   // Red LED off
        gpio_put(LED_GREEN_PIN, 1); // Green LED on
        printf("🟢 MAR CALMO detected! (confidence: %.2f%%)\n", confidence * 100);
    } else {
        // Unknown state - turn off both LEDs
        gpio_put(LED_RED_PIN, 0);
        gpio_put(LED_GREEN_PIN, 0);
        printf("⚪ Unknown state\n");
    }
}

// Print inference results
void print_inference_result(ei_impulse_result_t result) {
    printf("\nInference completed successfully!\n");
    printf("Timing: DSP %d ms, inference %d ms\n",
           result.timing.dsp,
           result.timing.classification);
    
    printf("Predictions:\n");
    
    float max_confidence = 0.0f;
    const char* best_prediction = "";
    
    for (uint16_t i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++) {
        printf("  %s: %.3f\n", 
               ei_classifier_inferencing_categories[i],
               result.classification[i].value);
        
        if (result.classification[i].value > max_confidence) {
            max_confidence = result.classification[i].value;
            best_prediction = ei_classifier_inferencing_categories[i];
        }
    }
    
    printf("\nBest prediction: %s (%.3f)\n", best_prediction, max_confidence);
    
    // Update LEDs only if confidence is above threshold
    if (max_confidence >= CONFIDENCE_THRESHOLD) {
        update_leds(best_prediction, max_confidence);
    } else {
        printf("Confidence below threshold (%.2f%% < %.2f%%)\n", 
               max_confidence * 100, CONFIDENCE_THRESHOLD * 100);
        gpio_put(LED_RED_PIN, 0);
        gpio_put(LED_GREEN_PIN, 0);
    }
}

// Main function
int main() {
    // Initialize USB for serial communication
    stdio_usb_init();
    
    // Wait for USB connection (optional, remove for standalone operation)
    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }
    
    printf("\n--- Sea State Classification System ---\n");
    printf("Using MPU-6500 with Edge Impulse inference\n");
    printf("I2C: SDA=%d, SCL=%d\n", I2C_SDA_PIN, I2C_SCL_PIN);
    printf("LEDs: Red=%d (mar_agitado), Green=%d (mar_calmo)\n", 
           LED_RED_PIN, LED_GREEN_PIN);
    
    // Configure GPIO pins for LEDs
    gpio_init(LED_RED_PIN);
    gpio_set_dir(LED_RED_PIN, GPIO_OUT);
    gpio_init(LED_GREEN_PIN);
    gpio_set_dir(LED_GREEN_PIN, GPIO_OUT);
    
    // LED test
    printf("\nTesting LEDs...\n");
    gpio_put(LED_RED_PIN, 1);
    sleep_ms(500);
    gpio_put(LED_RED_PIN, 0);
    gpio_put(LED_GREEN_PIN, 1);
    sleep_ms(500);
    gpio_put(LED_GREEN_PIN, 0);
    
    // Configure I2C
    i2c_init(I2C_PORT, 400 * 1000); // 400kHz
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);
    
    // Initialize MPU-6500
    printf("\nInitializing MPU-6500...\n");
    mpu6500_init(I2C_PORT);
    sleep_ms(100); // Wait for stabilization
    printf("MPU-6500 initialized successfully\n");
    
    // Check feature buffer size
    printf("\nEdge Impulse model info:\n");
    printf("Expected input size: %d\n", EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE);
    printf("Number of labels: %d\n", EI_CLASSIFIER_LABEL_COUNT);
    printf("Sample rate: %d Hz\n", SAMPLE_RATE_HZ);
    printf("Window size: %d ms\n", WINDOW_SIZE_MS);
    
    printf("\nStarting data collection and inference...\n");
    printf("---\n");
    
    ei_impulse_result_t result = {0};
    
    while (true) {
        // Collect sensor data
        if (!collect_sensor_data()) {
            printf("ERROR: Failed to collect sensor data\n");
            sleep_ms(1000);
            continue;
        }
        
        // Prepare signal for classifier
        signal_t features_signal;
        features_signal.total_length = EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE;
        features_signal.get_data = &raw_feature_get_data;
        
        // Run inference
        printf("Running inference...\n");
        EI_IMPULSE_ERROR res = run_classifier(&features_signal, &result, DEBUG_INFERENCE);
        
        if (res != EI_IMPULSE_OK) {
            printf("ERROR: Failed to run classifier (%d)\n", res);
            // Turn off LEDs on error
            gpio_put(LED_RED_PIN, 0);
            gpio_put(LED_GREEN_PIN, 0);
        } else {
            print_inference_result(result);
        }
        
        printf("---\n");
        
        // Wait before next inference
        sleep_ms(INFERENCE_INTERVAL_MS);
    }
    
    return 0;
}