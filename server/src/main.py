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

    elif not isinstance(event["machine_id"], str):
        raise HTTPException(
            status_code=400,
            detail="Invalid data type for machine_id"
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
            detail="Invalid timestamp format, expected ISO 8601 format (YYYY-MM-DDTHH:MM:SSZ)"
        )

    if timestamp.tzinfo is None:
        raise HTTPException(
            status_code=400,
            detail="Timestamp must include timezone information"
        )

    timestamp = timestamp.astimezone(timezone.utc).replace(tzinfo=None) #timezone UTC sem +00:00
    received_at = datetime.now(timezone.utc).replace(tzinfo=None)

    return {
        "message": "Startup received",
        "machine_id": event["machine_id"],
        "timestamp": timestamp,
        "received_at": received_at
    }