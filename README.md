# Hardware-in-the-Loop (HIL) Framework
## Using NASA Trick and NASA cFS

This project demonstrates a **basic educational HIL architecture**
integrating:

- Trick Simulation Environment
- NASA Core Flight System (cFS)
- A simple socket-based HIL interface layer

---

## 📚 Educational Objective

This scaffold helps you learn:

1. How Trick schedules simulation time and models.
2. How cFS runs flight software as message-driven applications.
3. How to connect both through a transport interface (socket middleware).
4. How HIL architecture works conceptually.

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

This framework is intentionally minimal and heavily commented.
