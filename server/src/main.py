from fastapi import FastAPI, HTTPException
from datetime import datetime, timezone

app = FastAPI()


@app.post("/api/v1/startups", status_code=201)
def create_startup(event: dict):
    if "machine_id" not in event or "timestamp" not in event:
        raise HTTPException(
            status_code=400,
            detail="Missing required fields: machine_id or timestamp"
        )

    if not isinstance(event["machine_id"], str):
        raise HTTPException(
            status_code=400,
            detail="Invalid data type for machine_id"
        )

    if not event["machine_id"].strip():
        raise HTTPException(
            status_code=400,
            detail="machine_id cannot be empty"
        )

    if len(event["machine_id"]) > 255:
        raise HTTPException(
            status_code=400,
            detail="machine_id is too long"
        )

    if not isinstance(event["timestamp"], str):
        raise HTTPException(
            status_code=400,
            detail="Invalid data type for timestamp"
        )

    try:
        timestamp = datetime.fromisoformat(
            event["timestamp"].replace("Z", "+00:00")
        )

    except ValueError:
        raise HTTPException(
            status_code=400,
            detail="Invalid ISO 8601 timestamp"
        )

    if timestamp.tzinfo is None:
        raise HTTPException(
            status_code=400,
            detail="Timestamp must include timezone information"
        )

    timestamp = int(timestamp.timestamp())
    received_at = int(datetime.now(timezone.utc).timestamp())

    return {
        "message": "Startup received",
        "machine_id": event["machine_id"],
        "timestamp": timestamp,
        "received_at": received_at,
    }