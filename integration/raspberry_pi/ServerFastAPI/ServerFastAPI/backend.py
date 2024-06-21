from fastapi import FastAPI, HTTPException
from pydantic import BaseModel, Field, validator
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
    event: str = Field(..., example="data")
    programType: str = Field(..., example="Fermentation")
    rateOrSpeed: int = Field(..., example=0)
    duration: int = Field(..., example=0)
    tempSetpoint: float = Field(..., example=25.0)
    phSetpoint: float = Field(..., example=7.2)
    doSetpoint: float = Field(..., example=5.5)
    nutrientConc: float = Field(..., example=10.0)
    baseConc: float = Field(..., example=1.0)
    experimentName: str = Field(..., example="TestExperiment")
    comment: str = Field(..., example="This is a test comment")
    currentProgram: str = Field(..., example="Fermentation")
    programStatus: str = Field(..., example="Running")
    airPumpStatus: int = Field(..., example=1)
    drainPumpStatus: int = Field(..., example=0)
    nutrientPumpStatus: int = Field(..., example=0)
    basePumpStatus: int = Field(..., example=0)
    stirringMotorStatus: int = Field(..., example=1)
    heatingPlateStatus: int = Field(..., example=1)
    ledGrowLightStatus: int = Field(..., example=0)
    waterTemp: float = Field(..., example=24.5)
    airTemp: float = Field(..., example=22.0)
    ph: float = Field(..., example=6.8)
    turbidity: float = Field(..., example=1.0)
    oxygen: float = Field(..., example=7.5)
    airFlow: float = Field(..., example=0.5)
    timestamp: str = Field(..., example="2023-05-06T12:00:00Z")

    @validator("waterTemp", "airTemp")
    def check_temp(cls, value):
        if not (-50.0 <= value <= 150.0):
            raise ValueError("Temperature must be between -50 and 150 degrees Celsius")
        return value

    @validator("ph")
    def check_ph(cls, value):
        if not (0.0 <= value <= 14.0):
            raise ValueError("pH must be between 0 and 14")
        return value

    @validator("turbidity", "oxygen", "airFlow")
    def check_positive(cls, value):
        if value < 0:
            raise ValueError("Value must be positive")
        return value

@app.post("/sensor_data")
async def receive_data(data: SensorData):
    ensure_csv_header()  # Ensure the header is present

    backend_time = datetime.now().strftime("%Y-%m-%d %H:%M:%S")

    try:
        with open(filename, 'a', newline='') as file:
            writer = csv.writer(file)
            writer.writerow([
                backend_time, data.timestamp, data.event, data.programType, data.rateOrSpeed, data.duration,
                data.tempSetpoint, data.phSetpoint, data.doSetpoint, data.nutrientConc, data.baseConc,
                data.experimentName, data.comment, data.currentProgram, data.programStatus,
                data.airPumpStatus, data.drainPumpStatus, data.nutrientPumpStatus, data.basePumpStatus,
                data.stirringMotorStatus, data.heatingPlateStatus, data.ledGrowLightStatus,
                data.waterTemp, data.airTemp, data.ph, data.turbidity, data.oxygen, data.airFlow
            ])

        return {"status": "success", "message": "Data received"}

    except Exception as e:
        raise HTTPException(status_code=400, detail=f"Error processing data: {e}")
