# Hardware-in-the-Loop (HIL) Framework
## Using NASA Trick and NASA cFS

This project is meant to demonstrate a **basic HIL architecture**
integrating:

- Trick Simulation Environment
- NASA Core Flight System (cFS)
- A simple socket-based HIL interface layer

The goal is to hopefully build this out into a general aircraft model for a typical RC plane, using model predictive control (MPC) instead of the currently attempted PID.

---

## 🏗 Architecture Overview

      +-------------------+
      |     Trick Sim     |
      |  (Dynamics Model) |
      +-------------------+
                |
                | TCP Socket
                |
      +-------------------+
      |   HIL Interface   |
      |  (Middleware)     |
      +-------------------+
                |
                | Software Bus Message
                |
      +-------------------+
      |       cFS         |
      |  (Flight SW App)  |
      +-------------------+

---

## 📂 Directory Structure

docs/          → Setup instructions and architecture explanation  
trick_sim/     → Trick simulation scaffold  
cfs_app/       → Minimal cFS application skeleton  
hil_interface/ → TCP transport middleware  

---

## 🚀 Quick Start (Conceptual)

1. Build Trick simulation in trick_sim/
2. Integrate HIL socket client into Trick model
3. Build cFS app in cfs_app/
4. Run cFS with custom app
5. Start Trick simulation
6. Observe message exchange

---

This framework is intentionally minimal and heavily commented. Docs are primarily AI generated.
