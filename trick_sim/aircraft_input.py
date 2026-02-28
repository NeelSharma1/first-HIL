# aircraft_input.py
# Trick run configuration for aircraft simulation

import trick

# Set simulation parameters
trick.exec_set_time_step(0.01)  # 100 Hz
trick.exec_set_terminate_time(60.0)  # 60 seconds

# Schedule the aircraft model
trick.add_job(trick.sim_services, "aircraft_derivatives", 
              trick.cycle = "0.01", trick.phase = 1)

# Schedule communication
trick.add_job(trick.sim_services, "send_sensor_data", 
              trick.cycle = "0.01", trick.phase = 2)
trick.add_job(trick.sim_services, "receive_command", 
              trick.cycle = "0.01", trick.phase = 3)

# Initialize aircraft
trick.sim_services.init_aircraft()

# Enable data recording
trick.dataproducts_set_cycle(0.1)  # Record at 10 Hz
trick.dataproducts_add("aircraft.x")
trick.dataproducts_add("aircraft.y")
trick.dataproducts_add("aircraft.z")
trick.dataproducts_add("aircraft.u")
trick.dataproducts_add("aircraft.v")
trick.dataproducts_add("aircraft.w")
trick.dataproducts_add("aircraft.phi")
trick.dataproducts_add("aircraft.theta")
trick.dataproducts_add("aircraft.psi")
trick.dataproducts_add("aircraft.elevator")
trick.dataproducts_add("aircraft.aileron")
trick.dataproducts_add("aircraft.rudder")
trick.dataproducts_add("aircraft.throttle")