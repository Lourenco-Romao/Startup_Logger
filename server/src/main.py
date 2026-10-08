from flask import Flask, request, jsonify
import time

app = Flask(__name__)

valid_event_types = {"startup"}

APPLICATION_CREATED_AT = 1791471878 # This is a Unix timestamp that is the limit for the timestamp received in the request. It represents the date the application was created. Any timestamp before this date will be considered invalid.

@app.route("/api/v1/startups", methods=["POST"])
def create_startup():
    event = request.get_json(silent=True)

    if event is None:
        return jsonify({
            "error": "Request body must contain valid JSON"
        }), 400
    
    if "hostname" not in event or "timestamp" not in event or "eventType" not in event:
        return jsonify({
            "error": "Missing required fields: hostname or timestamp or eventType"
        }), 400

    if not isinstance(event["eventType"], str):
        return jsonify({
            "error": "Invalid data type for eventType"
        }), 400

    event["eventType"] = event["eventType"].lower()

    if event["eventType"] not in valid_event_types:
        return jsonify({
            "error": f"Invalid eventType. Must be one of: {', '.join(valid_event_types)}"
        }), 400

    if not isinstance(event["hostname"], str):
        return jsonify({
            "error": "Invalid data type for hostname"
        }), 400

    event["hostname"] = event["hostname"].strip()

    if not event["hostname"]:
        return jsonify({
            "error": "hostname cannot be empty"
        }), 400

    if len(event["hostname"]) > 255:
        return jsonify({
            "error": "hostname is too long"
        }), 400

    if type(event["timestamp"]) is not int:
        return jsonify({
            "error": "Invalid data type for timestamp"
        }), 400
        
    received_at = int(time.time())

    if event["timestamp"] > received_at + 300:
        return jsonify({
            "error": "timestamp can not be in the future"
        }), 400
    
    if event["timestamp"] < APPLICATION_CREATED_AT:
        return jsonify({
            "error": "timestamp predates application creation"
        }), 400

    return jsonify({
        "message": "Startup received",
        "hostname": event["hostname"],
        "timestamp": event["timestamp"],
        "received_at": received_at,
        "eventType": event["eventType"]
    }), 201