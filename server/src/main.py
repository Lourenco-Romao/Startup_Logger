from fastapi import FastAPI, HTTPException
from datetime import datetime

app = FastAPI()

@app.post("/api/v1/startups", status_code=201)
def create_startup(event: dict):
    if "machine_id" not in event or "timestamp" not in event:
        raise HTTPException(
            status_code=400,
            detail="Missing required fields: machine_id or timestamp"
        )

    elif not isinstance(event["machine_id"], str):
        raise HTTPException(
            status_code=400,
            detail="Invalid data type for machine_id"
        )

    try:
        datetime.fromisoformat(
            event["timestamp"].replace("Z", "+00:00")
        )
    except ValueError:
        raise HTTPException(
            status_code=400,
            detail="Invalid timestamp format, expected ISO 8601 format (YYYY-MM-DDTHH:MM:SSZ)"
        )

    return {
        "message": "Startup received",
        "machine_id": event["machine_id"],
        "timestamp": event["timestamp"],
    }