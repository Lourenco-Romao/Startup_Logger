from fastapi import FastAPI, HTTPException
import time

app = FastAPI()

valid_event_types = {"startup"}

APPLICATION_CREATED_AT = 1791471878 # This is a Unix timestamp that is the limit for the timestamp received in the request. It represents the date the application was created. Any timestamp before this date will be considered invalid.

@app.post("/api/v1/startups", status_code=201)
def create_startup(event: dict):
    if "hostname" not in event or "timestamp" not in event or "eventType" not in event:
        raise HTTPException(
            status_code=400,
            detail="Missing required fields: hostname or timestamp or eventType"
        )

    if not isinstance(event["eventType"], str):
        raise HTTPException(
            status_code=400,
            detail="Invalid data type for eventType"
        )

    event["eventType"] = event["eventType"].lower()

    if event["eventType"] not in valid_event_types:
        raise HTTPException(
            status_code=400,
            detail=f"Invalid eventType. Must be one of: {', '.join(valid_event_types)}"
        )

    if not isinstance(event["hostname"], str):
        raise HTTPException(
            status_code=400,
            detail="Invalid data type for hostname"
        )

    event["hostname"] = event["hostname"].strip()

    if not event["hostname"]:
        raise HTTPException(
            status_code=400,
            detail="hostname cannot be empty"
        )

    if len(event["hostname"]) > 255:
        raise HTTPException(
            status_code=400,
            detail="hostname is too long"
        )

    if not isinstance(event["timestamp"], int):
        raise HTTPException(
            status_code=400,
            detail="Invalid data type for timestamp"
        )
    
    received_at = int(time.time())

    if event["timestamp"] > received_at + 300:
        raise HTTPException(
            status_code=400,
            detail="timestamp can not be in the future"
        )
    
    if event["timestamp"] < APPLICATION_CREATED_AT:
        raise HTTPException(
            status_code=400,
            detail="timestamp predates application creation"
        )

    return {
        "message": "Startup received",
        "hostname": event["hostname"],
        "timestamp": event["timestamp"],
        "received_at": received_at,
        "eventType": event["eventType"]
    }