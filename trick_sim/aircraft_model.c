/*
 * aircraft_model.c
 *
 * 6DOF aircraft dynamics model for HIL demonstration.
 * This file implements:
 *   - 6DOF rigid body dynamics
 *   - Simple aerodynamics model
 *   - Control surface effectiveness
 *   - Socket communication for HIL
 */

#include <stdio.h>
#include <math.h>

// Physical constants
#define GRAVITY 9.81
#define AIR_DENSITY 1.225

// Aircraft state variables
typedef struct {
    // Position (NED frame)
    double x, y, z;
    
    // Velocity (body frame)
    double u, v, w;
    
    // Euler angles
    double phi, theta, psi;  // roll, pitch, yaw
    
    // Angular rates (body frame)
    double p, q, r;
    
    // Control surface deflections (radians)
    double elevator;    // pitch control
    double aileron;     // roll control
    double rudder;      // yaw control
    double throttle;    // thrust control (0-1)
} AircraftState;

AircraftState aircraft = {0};

// Aircraft parameters
typedef struct {
    double mass;                    // kg
    double Ixx, Iyy, Izz;           // moments of inertia
    double Ixz;                     // cross moment of inertia
    
    // Aerodynamic coefficients
    double CL0, CLalpha;            // lift coefficients
    double CD0, CDalpha;            // drag coefficients
    double CYbeta;                  // side force coefficient
    
    // Moment coefficients
    double Clbeta, Clp, Clr;        // roll moment
    double Cm0, Cmalpha, Cmq;       // pitch moment
    double Cnbeta, Cnp, Cnr, Cndr;  // yaw moment
    
    // Control effectiveness
    double Cde, Cda, Cdr;           // elevator, aileron, rudder
    double thrust_max;              // maximum thrust
} AircraftParams;

AircraftParams params = {
    .mass = 1200.0,
    .Ixx = 1285.0, .Iyy = 1825.0, .Izz = 2666.0, .Ixz = 0.0,
    .CL0 = 0.28, .CLalpha = 4.5,
    .CD0 = 0.03, .CDalpha = 0.30,
    .CYbeta = -0.98,
    .Clbeta = -0.12, .Clp = -0.45, .Clr = 0.15,
    .Cm0 = 0.0, .Cmalpha = -1.2, .Cmq = -12.0,
    .Cnbeta = 0.25, .Cnp = -0.03, .Cnr = -0.15, .Cndr = -0.08,
    .Cde = -1.2, .Cda = 0.15, .Cdr = -0.08,
    .thrust_max = 5000.0
};

// Simulation parameters
double dt = 0.01;  // 100 Hz update rate

// Helper functions
double rad2deg(double rad) { return rad * 180.0 / M_PI; }
double deg2rad(double deg) { return deg * M_PI / 180.0; }

// Calculate aerodynamic forces and moments
void calculate_aero_forces(double *Fx, double *Fy, double *Fz,
                          double *L, double *M, double *N) {
    
    // Calculate airspeed and angle of attack
    double V = sqrt(aircraft.u*aircraft.u + aircraft.v*aircraft.v + aircraft.w*aircraft.w);
    double alpha = atan2(aircraft.w, aircraft.u);
    double beta = asin(aircraft.v / V);
    
    // Dynamic pressure
    double q_bar = 0.5 * AIR_DENSITY * V * V;
    
    // Lift and drag coefficients
    double CL = params.CL0 + params.CLalpha * alpha;
    double CD = params.CD0 + params.CDalpha * alpha * alpha;
    
    // Forces in wind frame
    double lift = q_bar * CL;
    double drag = q_bar * CD;
    double side_force = q_bar * params.CYbeta * beta;
    
    // Transform to body frame
    *Fx = drag - q_bar * params.Cde * aircraft.elevator;
    *Fy = side_force + q_bar * params.Cda * aircraft.aileron + q_bar * params.Cdr * aircraft.rudder;
    *Fz = -lift;
    
    // Moments
    *L = q_bar * (params.Clbeta * beta + params.Clp * aircraft.p + params.Clr * aircraft.r + 
                  params.Cda * aircraft.aileron + params.Cdr * aircraft.rudder);
    *M = q_bar * (params.Cm0 + params.Cmalpha * alpha + params.Cmq * aircraft.q + 
                  params.Cde * aircraft.elevator);
    *N = q_bar * (params.Cnbeta * beta + params.Cnp * aircraft.p + params.Cnr * aircraft.r + 
                  params.Cdr * aircraft.rudder);
}

// 6DOF equations of motion
void aircraft_derivatives() {
    double Fx, Fy, Fz, L, M, N;
    
    // Calculate aerodynamic forces and moments
    calculate_aero_forces(&Fx, &Fy, &Fz, &L, &M, &N);
    
    // Add thrust
    Fx += params.thrust_max * aircraft.throttle;
    
    // Add gravity (transform to body frame)
    Fx -= params.mass * GRAVITY * sin(aircraft.theta);
    Fy += params.mass * GRAVITY * cos(aircraft.theta) * sin(aircraft.phi);
    Fz += params.mass * GRAVITY * cos(aircraft.theta) * cos(aircraft.phi);
    
    // Linear accelerations (body frame)
    double u_dot = Fx/params.mass - aircraft.q*aircraft.w + aircraft.r*aircraft.v;
    double v_dot = Fy/params.mass - aircraft.r*aircraft.u + aircraft.p*aircraft.w;
    double w_dot = Fz/params.mass - aircraft.p*aircraft.v + aircraft.q*aircraft.u;
    
    // Angular accelerations (body frame)
    double p_dot = (L + (params.Iyy - params.Izz)*aircraft.q*aircraft.r + params.Ixz*aircraft.p*aircraft.q) / params.Ixx;
    double q_dot = (M + (params.Izz - params.Ixx)*aircraft.p*aircraft.r - params.Ixz*(aircraft.p*aircraft.p - aircraft.r*aircraft.r)) / params.Iyy;
    double r_dot = (N + (params.Ixx - params.Iyy)*aircraft.p*aircraft.q + params.Ixz*aircraft.q*aircraft.r) / params.Izz;
    
    // Integrate using Euler method
    aircraft.u += u_dot * dt;
    aircraft.v += v_dot * dt;
    aircraft.w += w_dot * dt;
    
    aircraft.p += p_dot * dt;
    aircraft.q += q_dot * dt;
    aircraft.r += r_dot * dt;
    
    // Update Euler angles
    aircraft.phi += (aircraft.p + aircraft.sin(aircraft.theta)*aircraft.q + 
                     aircraft.cos(aircraft.theta)*aircraft.r) * dt;
    aircraft.theta += (aircraft.cos(aircraft.phi)*aircraft.q - 
                       aircraft.sin(aircraft.phi)*aircraft.r) * dt;
    aircraft.psi += (aircraft.sin(aircraft.phi)/aircraft.cos(aircraft.theta)*aircraft.q + 
                     aircraft.cos(aircraft.phi)/aircraft.cos(aircraft.theta)*aircraft.r) * dt;
    
    // Update position (simplified - assuming small angles for position integration)
    aircraft.x += aircraft.u * dt;
    aircraft.y += aircraft.v * dt;
    aircraft.z += aircraft.w * dt;
}

// Send sensor data to cFS
void send_sensor_data() {
    // Create sensor data structure
    typedef struct {
        double timestamp;
        double x, y, z;
        double u, v, w;
        double phi, theta, psi;
        double p, q, r;
    } SensorData;
    
    SensorData sensors = {
        .timestamp = 0.0,  // Would be actual simulation time
        .x = aircraft.x, .y = aircraft.y, .z = aircraft.z,
        .u = aircraft.u, .v = aircraft.v, .w = aircraft.w,
        .phi = aircraft.phi, .theta = aircraft.theta, .psi = aircraft.psi,
        .p = aircraft.p, .q = aircraft.q, .r = aircraft.r
    };
    
    printf("Sending aircraft state: pos(%.2f,%.2f,%.2f) vel(%.2f,%.2f,%.2f) euler(%.1f,%.1f,%.1f)\n",
           aircraft.x, aircraft.y, aircraft.z, aircraft.u, aircraft.v, aircraft.w,
           rad2deg(aircraft.phi), rad2deg(aircraft.theta), rad2deg(aircraft.psi));
    
    // TODO: Send via TCP socket to HIL interface
}

// Receive control commands from cFS
void receive_command() {
    // TODO: Receive control surface commands via TCP socket
    // For now, simulate simple control inputs
    
    // Simple trim condition
    aircraft.elevator = deg2rad(0.0);
    aircraft.aileron = deg2rad(0.0);
    aircraft.rudder = deg2rad(0.0);
    aircraft.throttle = 0.5;  // 50% throttle
    
    // Could add simple test maneuvers here
    static double time = 0.0;
    time += dt;
    
    // Example: small pitch oscillation
    if (time > 5.0 && time < 10.0) {
        aircraft.elevator = deg2rad(2.0 * sin(2.0 * time));
    }
}

// Initialize aircraft state
void init_aircraft() {
    aircraft.x = 0.0; aircraft.y = 0.0; aircraft.z = -1000.0;  // 1km altitude
    aircraft.u = 50.0; aircraft.v = 0.0; aircraft.w = 0.0;      // 50 m/s forward
    aircraft.phi = 0.0; aircraft.theta = deg2rad(5.0); aircraft.psi = 0.0;  // 5 deg pitch up
    aircraft.p = 0.0; aircraft.q = 0.0; aircraft.r = 0.0;
    
    aircraft.elevator = 0.0;
    aircraft.aileron = 0.0;
    aircraft.rudder = 0.0;
    aircraft.throttle = 0.6;
}