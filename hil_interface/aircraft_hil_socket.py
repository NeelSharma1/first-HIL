# aircraft_hil_socket.py
# Enhanced TCP server for aircraft HIL data exchange

import socket
import struct
import time
import threading

HOST = "localhost"
PORT = 5005

# Data structure definitions
SENSOR_DATA_FORMAT = "d13d"  # timestamp + 12 doubles + timestamp
SENSOR_DATA_SIZE = struct.calcsize(SENSOR_DATA_FORMAT)

CONTROL_CMD_FORMAT = "d5d"   # timestamp + 4 doubles + timestamp
CONTROL_CMD_SIZE = struct.calcsize(CONTROL_CMD_FORMAT)

class AircraftHILServer:
    def __init__(self):
        self.sensor_data = {
            'timestamp': 0.0,
            'x': 0.0, 'y': 0.0, 'z': -1000.0,
            'u': 50.0, 'v': 0.0, 'w': 0.0,
            'phi': 0.0, 'theta': 0.087, 'psi': 0.0,  # 5 degrees pitch
            'p': 0.0, 'q': 0.0, 'r': 0.0
        }
        
        self.control_commands = {
            'timestamp': 0.0,
            'elevator': 0.0,
            'aileron': 0.0,
            'rudder': 0.0,
            'throttle': 0.6
        }
        
        self.running = False
        self.client_conn = None
        
    def pack_sensor_data(self):
        """Pack sensor data into binary format for transmission"""
        data = (
            self.sensor_data['timestamp'],
            self.sensor_data['x'], self.sensor_data['y'], self.sensor_data['z'],
            self.sensor_data['u'], self.sensor_data['v'], self.sensor_data['w'],
            self.sensor_data['phi'], self.sensor_data['theta'], self.sensor_data['psi'],
            self.sensor_data['p'], self.sensor_data['q'], self.sensor_data['r'],
            self.sensor_data['timestamp']
        )
        return struct.pack(SENSOR_DATA_FORMAT, *data)
    
    def unpack_control_command(self, data):
        """Unpack control command from binary format"""
        if len(data) != CONTROL_CMD_SIZE:
            print(f"Warning: Expected {CONTROL_CMD_SIZE} bytes, got {len(data)}")
            return None
            
        unpacked = struct.unpack(CONTROL_CMD_FORMAT, data)
        self.control_commands = {
            'timestamp': unpacked[0],
            'elevator': unpacked[1],
            'aileron': unpacked[2],
            'rudder': unpacked[3],
            'throttle': unpacked[4]
        }
        return self.control_commands
    
    def handle_client(self, conn, addr):
        """Handle individual client connection"""
        print(f"Connected by {addr}")
        self.client_conn = conn
        
        try:
            while self.running:
                # Receive sensor data from Trick simulation
                data = conn.recv(SENSOR_DATA_SIZE)
                if not data:
                    break
                
                # Unpack and store sensor data
                if len(data) == SENSOR_DATA_SIZE:
                    unpacked = struct.unpack(SENSOR_DATA_FORMAT, data)
                    self.sensor_data = {
                        'timestamp': unpacked[0],
                        'x': unpacked[1], 'y': unpacked[2], 'z': unpacked[3],
                        'u': unpacked[4], 'v': unpacked[5], 'w': unpacked[6],
                        'phi': unpacked[7], 'theta': unpacked[8], 'psi': unpacked[9],
                        'p': unpacked[10], 'q': unpacked[11], 'r': unpacked[12]
                    }
                    
                    print(f"Received sensor data: pos({self.sensor_data['x']:.1f},{self.sensor_data['y']:.1f},{self.sensor_data['z']:.1f}) "
                          f"vel({self.sensor_data['u']:.1f},{self.sensor_data['v']:.1f},{self.sensor_data['w']:.1f}) "
                          f"euler({rad2deg(self.sensor_data['phi']):.1f},{rad2deg(self.sensor_data['theta']):.1f},{rad2deg(self.sensor_data['psi']):.1f})")
                    
                    # Send control commands back
                    cmd_data = struct.pack(CONTROL_CMD_FORMAT,
                        self.control_commands['timestamp'],
                        self.control_commands['elevator'],
                        self.control_commands['aileron'],
                        self.control_commands['rudder'],
                        self.control_commands['throttle'],
                        self.control_commands['timestamp']
                    )
                    conn.sendall(cmd_data)
                    
                    print(f"Sent control commands: elevator={self.control_commands['elevator']:.3f} "
                          f"aileron={self.control_commands['aileron']:.3f} "
                          f"rudder={self.control_commands['rudder']:.3f} "
                          f"throttle={self.control_commands['throttle']:.3f}")
                
        except Exception as e:
            print(f"Error handling client: {e}")
        finally:
            self.client_conn = None
            conn.close()
            print(f"Connection closed from {addr}")
    
    def start_server(self):
        """Start the HIL server"""
        self.running = True
        
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            s.bind((HOST, PORT))
            s.listen()
            print(f"Aircraft HIL Interface listening on {HOST}:{PORT}")
            
            try:
                while self.running:
                    conn, addr = s.accept()
                    # Handle each client in a separate thread
                    client_thread = threading.Thread(target=self.handle_client, args=(conn, addr))
                    client_thread.daemon = True
                    client_thread.start()
                    
            except KeyboardInterrupt:
                print("\nShutting down server...")
                self.running = False

def rad2deg(rad):
    return rad * 180.0 / 3.14159265359

if __name__ == "__main__":
    server = AircraftHILServer()
    server.start_server()