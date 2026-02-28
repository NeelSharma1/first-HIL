# Trick Simulation Module

This folder contains a minimal Trick-based simulation model.

## What This Model Does

- Simulates a 1D mass system
- Sends simulated sensor data over TCP
- Receives actuator commands from cFS

## Files

sim_model.c     → Simple dynamics model
input.py        → Trick run configuration

## Learning Goals

- Understand Trick scheduling
- See how Trick model variables are integrated
- Understand where networking hooks in
