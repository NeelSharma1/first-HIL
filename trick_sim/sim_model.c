/*
 * sim_model.c
 *
 * Simple 1D mass dynamics model for HIL demonstration.
 * This file illustrates:
 *   - State propagation in Trick
 *   - Placeholder for socket communication
 */

#include <stdio.h>
#include <math.h>

// State variables
double position = 0.0;
double velocity = 0.0;
double force_cmd = 0.0;

// Simple physics parameters
double mass = 1.0;

// Derivative function
void model_derivatives() {
    // a = F/m
    double acceleration = force_cmd / mass;

    // Euler integration (educational simplicity)
    velocity += acceleration * 0.01;
    position += velocity * 0.01;
}

// Placeholder function for socket send
void send_sensor_data() {
    // Here you would serialize position and send via TCP
    printf("Sending position: %f\n", position);
}

// Placeholder function for receiving command
void receive_command() {
    // In real HIL, force_cmd would be updated via socket
    // For now, simulate small command
    force_cmd = 1.0;
}
