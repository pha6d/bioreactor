import serial
import time
import csv
from datetime import datetime
import os

# Replace 'COM4' with the correct serial port for your Arduino
ser = serial.Serial('COM4', 115200, timeout=1)
time.sleep(2)  # Wait for the connection to be established

# Name of the CSV file where the data will be saved
filename = "data.csv"

# Check if the file exists to decide whether to add the header or not
file_exists = os.path.isfile(filename)

# Create a CSV file if it doesn't exist and write column headers
if not file_exists:
    with open(filename, 'w', newline='') as file:
        writer = csv.writer(file)
        writer.writerow(["Time", "Temperature", "pH", "Turbidity_Voltage", "O2"])  # Add "DO (mg/L)" as a column header

try:
    while True:
        if ser.in_waiting > 0:
            line = ser.readline().decode('utf-8').rstrip()
            # Extracting temperature, pH, turbidity voltage, and DO from the line
            try:
                temperature = line.split('temperature: ')[1].split('°C')[0].strip()
                ph = line.split('pH: ')[1].split(' ')[0].strip()
                turbidity_voltage = line.split('Turbidity Voltage: ')[1].split(' ')[0].strip()
                do_raw_binary = line.split('DO: ')[1].split(' ')[0].strip()  # Extract the binary string for DO
                do_value = int(do_raw_binary, 2) / 100.0  # Convert binary string to integer, then divide by 100 to get the DO value in mg/L
                
                # Get the current timestamp
                now = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
                
                # Print formatted data
                print(f"{now} - Temperature: {temperature}, pH: {ph}, Turbidity Voltage: {turbidity_voltage}, O2: {do_value} mg/L")
                
                # Write to the CSV file in append mode
                with open(filename, 'a', newline='') as file:
                    writer = csv.writer(file)
                    writer.writerow([now, temperature, ph, turbidity_voltage, do_value])
            except Exception as e:
                print(f"Error processing line: {line}. Error: {e}")
except KeyboardInterrupt:
    print("Program stopped.")

ser.close()


