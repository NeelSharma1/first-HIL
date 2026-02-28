# HIL Interface Middleware

This module demonstrates TCP communication between Trick and cFS.

In a real deployment:
- Trick acts as client or server
- cFS app reads from hardware driver
- Data serialized using a protocol (e.g., CCSDS)

For educational purposes, we use raw TCP and simple doubles.

## Aircraft Modeling (new)

This directory now integrates the new aircraft modeling code. Key files:

- Trick simulation model: [trick_sim/aircraft_model.c](../trick_sim/aircraft_model.c)
- Trick simulation input/driver: [trick_sim/aircraft_input.py](../trick_sim/aircraft_input.py)
- Additional sim sources: [trick_sim/sim_model.c](../trick_sim/sim_model.c)

HIL bridge and sockets:

- HIL socket bridge: [hil_interface/aircraft_hil_socket.py](./aircraft_hil_socket.py)
- Low-level socket utils: [hil_interface/hil_socket.py](./hil_socket.py)

cFS example consumer:

- Flight control app example: [cfs_app/aircraft_flight_control.c](../cfs_app/aircraft_flight_control.c)

Usage summary:
1. Run the Trick simulation (see the Trick model files above) to stream aircraft state over TCP.
2. Start the HIL bridge ([hil_interface/aircraft_hil_socket.py](./aircraft_hil_socket.py)) to forward model data to cFS.
3. Run the cFS app (example: [cfs_app/aircraft_flight_control.c](../cfs_app/aircraft_flight_control.c)) to consume the streamed doubles.
