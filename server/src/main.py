from fastapi import FastAPI
from fastapi.responses import JSONResponse

app = FastAPI()


@app.post("/api/v1/startups", status_code=201)
def create_startup(event: dict):
    if "machine_id" not in event or "timestamp" not in event:
        raise HTTPException(
            status_code=400,
            detail="Missing required fields: machine_id or timestamp"
        )

    if not isinstance(event["machine_id"], str) or not isinstance(event["timestamp"], str):
        raise HTTPException(
            status_code=400,
            detail="Invalid data types for machine_id or timestamp"
        )

    return {
        "message": "Startup received",
        "machine_id": event["machine_id"],
        "timestamp": event["timestamp"],
    }