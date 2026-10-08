import time

VALID_EVENT_TYPES = {"startup"}
VALID_OS = {"windows", "linux", "macos", "android", "ios"}

APPLICATION_CREATED_AT = 1791471878


def validate_event(event, received_at):
    if event is None:
        return "Request body must contain valid JSON"

    if ("hostname" not in event or "timestamp" not in event or "eventType" not in event or "os" not in event):
        return "Missing required fields: hostname or timestamp or eventType or OS"

    if not isinstance(event["os"], str):
        return "Invalid data type for OS"

    event["os"] = event["os"].strip().lower()

    if event["os"] not in VALID_OS:
        return (
            "Invalid OS. "
            f"Must be one of: {', '.join(VALID_OS)}"
        )

    if not isinstance(event["eventType"], str):
        return "Invalid data type for eventType"

    event["eventType"] = event["eventType"].strip().lower()

    if event["eventType"] not in VALID_EVENT_TYPES:
        return (
            "Invalid eventType. "
            f"Must be one of: {', '.join(VALID_EVENT_TYPES)}"
        )

    if not isinstance(event["hostname"], str):
        return "Invalid data type for hostname"

    event["hostname"] = event["hostname"].strip()

    if not event["hostname"]:
        return "hostname cannot be empty"

    if len(event["hostname"]) > 255:
        return "hostname is too long"

    if type(event["timestamp"]) is not int:
        return "Invalid data type for timestamp"


    if event["timestamp"] > received_at + 300:
        return "timestamp can not be in the future"

    if event["timestamp"] < APPLICATION_CREATED_AT:
        return "timestamp predates application creation"

    return None