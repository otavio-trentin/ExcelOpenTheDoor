import sys
import serial
import time

def main():
    # Check if the correct number of arguments were passed
    # if len(sys.argv) != 4:
    #     print("Usage: python send_to_arduino.py <COM_PORT> <VALUE1> <VALUE2>")
    #     sys.exit(1)

    # Extract arguments
    # com_port = "COM10" #sys.argv[1]
    # value1 = 3 #sys.argv[2]
    # value2 = 10 #sys.argv[3]
    com_port =  sys.argv[1]
    value1 = sys.argv[2]
    value2 = sys.argv[3]

    # Create the string to send
    data_to_send = f"{value1}, {value2}\n"

    
     # Open serial port
    ser = serial.Serial(com_port, 9600, timeout=1)
    time.sleep(2)  # Wait for Arduino to reset

    # Send data
    ser.write(data_to_send.encode())
    print(f"Data sent to {com_port}: {data_to_send.strip()}")

    # Optional: Read response from Arduino
    # while True:
    #     response = ser.readline().decode().strip()
    #     print(f"Response from Arduino: {response}")
    #     time.sleep(1)

    ser.close()
    # except serial.SerialException as e:
    #     print(f"Error opening or communicating through serial port: {e}")
    #     sys.exit(1)


main()
