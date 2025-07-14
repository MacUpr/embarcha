/**
 * @file config.h
 * @brief Configuration file for the Sea State Classification System
 */

#ifndef CONFIG_H
#define CONFIG_H

// --- Hardware Configuration ---

// I2C Configuration
#define I2C_PORT i2c1
#define I2C_SDA_PIN 2
#define I2C_SCL_PIN 3

// LED Pins
#define LED_RED_PIN 13    // GPIO13 - Mar Agitado (Rough Sea)
#define LED_GREEN_PIN 11  // GPIO11 - Mar Calmo (Calm Sea)

// --- Sampling Configuration ---

// Data collection parameters
#define SAMPLE_RATE_HZ 50        // Sampling frequency in Hz
#define WINDOW_SIZE_MS 2000      // Window size in milliseconds
#define SAMPLES_PER_WINDOW (SAMPLE_RATE_HZ * WINDOW_SIZE_MS / 1000)

// --- Inference Configuration ---

// Minimum confidence threshold for classification (0.0 to 1.0)
#define CONFIDENCE_THRESHOLD 0.7f

// Time between inferences (milliseconds)
#define INFERENCE_INTERVAL_MS 1000

// --- Debug Configuration ---

// Set to 1 to enable debug output
#define DEBUG_SENSOR_DATA 0      // Print raw sensor values
#define DEBUG_INFERENCE 0        // Print detailed inference info

#endif // CONFIG_H