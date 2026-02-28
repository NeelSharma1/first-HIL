### In this framework:

- Trick → Simulates physics
- cFS → Runs flight software
- HIL Interface → Bridges timing & data exchange

---

## Key Learning Concepts

### 1. Trick
- Variable integration
- Time stepping
- Model scheduling

### 2. cFS
- Executive Services (ES)
- Software Bus (SB)
- Event Services (EVS)

### 3. Integration
- Transport protocol
- Deterministic timing
- Synchronization

---

## How to Expand This

- Replace TCP with UDP or shared memory
- Add real sensor hardware
- Use CCSDS telemetry packets
- Add real-time OS (RTEMS)
