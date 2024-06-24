from fastapi import FastAPI, HTTPException
from pydantic import BaseModel, Field, ValidationError
import csv
from datetime import datetime
import os

app = FastAPI()

# Define the directory and file path
data_dir = "/Raspberry/Bioreactor/ServerFastAPI/data"
filename = os.path.join(data_dir, "data.csv")

# Ensure the data directory exists
os.makedirs(data_dir, exist_ok=True)

def ensure_csv_header():
    """Ensure that the CSV file has the correct header"""
    file_exists = os.path.isfile(filename)
    if not file_exists:
        with open(filename, 'w', newline='') as file:
            writer = csv.writer(file)
            writer.writerow([
                "Backend_Time", "ESP_Time", "event", "programType", "rateOrSpeed", "duration",
                "tempSetpoint", "phSetpoint", "doSetpoint", "nutrientConc", "baseConc", 
                "experimentName", "comment", "currentProgram", "programStatus",
                "airPumpStatus", "drainPumpStatus", "nutrientPumpStatus", "basePumpStatus",
                "stirringMotorStatus", "heatingPlateStatus", "ledGrowLightStatus",
                "waterTemp", "airTemp", "ph", "turbidity", "oxygen", "airFlow"
            ])

class SensorData(BaseModel):
    sensor_value: dict = Field(..., example={
        "prog": "None", "stat": "Idle", "ap": 0, "dp": 0, "np": 0, "bp": 0,
        "sm": 0, "hp": 0, "lg": 0, "wT": 0.0, "aT": 0.0, "pH": 0.0, "tb": 0.0,
        "ox": 0.0, "af": 0.0,
        "event": "data", "programType": "", "rateOrSpeed": 0, "duration": 0,
        "tempSetpoint": 0.0, "phSetpoint": 0.0, "doSetpoint": 0.0, "nutrientConc": 0.0, 
        "baseConc": 0.0, "experimentName": "", "comment": "", 
        "currentProgram": "None", "programStatus": "", "airPumpStatus": 0,
        "drainPumpStatus": 0, "nutrientPumpStatus": 0, "basePumpStatus": 0,
        "stirringMotorStatus": 0, "heatingPlateStatus": 0, "ledGrowLightStatus": 0,
        "waterTemp": 0.0, "airTemp": 0.0, "ph": 0.0, "turbidity": 0.0, "oxygen": 0.0, "airFlow": 0.0
    })
    timestamp: str = Field(..., example="2023-05-06T12:00:00Z")

@app.post("/sensor_data")
async def receive_data(data: SensorData):
    ensure_csv_header()  # Ensure the header is present

    backend_time = datetime.now().strftime("%Y-%m-%d %H:%M:%S")

    try:
        sensor_data = data.sensor_value
        # Ensure all required fields are present
        required_fields = [
            "event", "programType", "rateOrSpeed", "duration",
            "tempSetpoint", "phSetpoint", "doSetpoint", "nutrientConc", "baseConc", 
            "experimentName", "comment", "currentProgram", "programStatus",
            "airPumpStatus", "drainPumpStatus", "nutrientPumpStatus", "basePumpStatus",
            "stirringMotorStatus", "heatingPlateStatus", "ledGrowLightStatus",
            "waterTemp", "airTemp", "ph", "turbidity", "oxygen", "airFlow"
        ]
        
        for field in required_fields:
            if field not in sensor_data:
                raise HTTPException(status_code=400, detail=f"Missing field: {field}")

        with open(filename, 'a', newline='') as file:
            writer = csv.writer(file)
            writer.writerow([
                backend_time, data.timestamp,
                sensor_data.get("event", ""),
                sensor_data.get("programType", ""), sensor_data.get("rateOrSpeed", 0),
                sensor_data.get("duration", 0), sensor_data.get("tempSetpoint", 0.0), 
                sensor_data.get("phSetpoint", 0.0), sensor_data.get("doSetpoint", 0.0),
                sensor_data.get("nutrientConc", 0.0), sensor_data.get("baseConc", 0.0),
                sensor_data.get("experimentName", ""), sensor_data.get("comment", ""),
                sensor_data.get("currentProgram", ""), sensor_data.get("programStatus", ""),
                sensor_data.get("airPumpStatus", 0), sensor_data.get("drainPumpStatus", 0), 
                sensor_data.get("nutrientPumpStatus", 0), sensor_data.get("basePumpStatus", 0),
                sensor_data.get("stirringMotorStatus", 0), sensor_data.get("heatingPlateStatus", 0), 
                sensor_data.get("ledGrowLightStatus", 0), sensor_data.get("waterTemp", 0.0), 
                sensor_data.get("airTemp", 0.0), sensor_data.get("ph", 0.0), 
                sensor_data.get("turbidity", 0.0), sensor_data.get("oxygen", 0.0), 
                sensor_data.get("airFlow", 0.0)
            ])

        return {"status": "success", "message": "Data received"}

    except ValidationError as e:
        raise HTTPException(status_code=422, detail=str(e))
    except Exception as e:
        raise HTTPException(status_code=400, detail=f"Error processing data: {e}")

