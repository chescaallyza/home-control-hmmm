import requests
import time
import serial
import json

class HomeControl:
    def __init__(self, com_port, baud_rate):
        self.ser = serial.Serial(com_port, baud_rate)

    def get_light_states(self):
        try:
            response = requests.get('http://localhost/home-control/dB/states.php')
            response.raise_for_status()
            return response.json()
        except requests.exceptions.RequestException as e:
            print(f"HTTP Request failed: {e}")
            return []
        except ValueError as e:
            print(f"JSON decoding failed: {e}")
            return []

    def update_slide_switch_states(self, room):
        url = 'http://localhost/home-control/control/hardware_control.php'
        try:
            response = requests.post(url, json={"room": room, "toggle": True})
            response.raise_for_status()
            print(f"Switch state toggled for room: {room}")
        except requests.exceptions.RequestException as e:
            print(f"HTTP Request failed: {e}")

    def read_slide_switch(self):
        while self.ser.in_waiting:
            line = self.ser.readline().decode().strip()
            if line.startswith("{") and line.endswith("}"):
                try:
                    states = json.loads(line)
                    print(f"Read from Arduino: {states}")
                    room = states.get("room")
                    if room is not None:
                        self.update_slide_switch_states(room)
                except json.JSONDecodeError as e:
                    print(f"JSON decoding failed: {e}")

    def update_database(self, room, state):
        url = 'http://localhost/home-control/dB/states.php'
        data = {'room': room, 'state': state}
        try:
            response = requests.post(url, data=data)
            response.raise_for_status()
            print(f"Successfully updated database for room {room} with state {state}")
        except requests.exceptions.RequestException as e:
            print(f"Failed to update database for room {room}: {e}")

    def control_arduino(self, room, state):
        room_to_pin = {
            1: 5,
            2: 6,
            4: 10,
            5: 11
        }
        if room in room_to_pin:
            pin = room_to_pin[room]
            self.ser.write(f'{pin}:{state}\n'.encode())
            print(f"Sent to Arduino: pin {pin}, state {state}")

    def read_sensor_data(self):
        try:
            if self.ser.in_waiting > 0:
                line = self.ser.readline().decode('utf-8').strip()
                if line.startswith("{") and line.endswith("}"):
                    try:
                        data = json.loads(line)
                        temperature = data.get("temperature")
                        ir_detected = data.get("motionDetected")
                        light = data.get("light")
                        light_status = data.get("lightStatus")

                        print(f"Temperature: {temperature} °C, IR Sensor: {'Someone is detected' if ir_detected else 'No motion detected'}, Light Level: {light}, Light Status: {'ON' if light_status else 'OFF'}")

                        with open('./sensor_data.json', 'w') as f:
                            json.dump(data, f)

                    except json.JSONDecodeError:
                        pass
        except Exception:
            pass

    def main_loop(self):
        while True:
            self.read_sensor_data()
            light_states = self.get_light_states()
            for state_info in light_states:
                room = state_info.get('room')
                state = state_info.get('state')
                if room is not None and state is not None:
                    self.control_arduino(room, state)
            self.read_slide_switch()
            time.sleep(0.1)
