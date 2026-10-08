from flask import Flask, request, jsonify
import time

from validation import validate_event

app = Flask(__name__)

@app.route("/api/v1/startups", methods=["POST"])
def create_startup():
    event = request.get_json(silent=True)

    received_at = int(time.time())

    error = validate_event(event, received_at)

    if error is not None:
        return jsonify({
            "error": error
        }), 400

    return jsonify({
        "message": "Startup received",
        "hostname": event["hostname"],
        "timestamp": event["timestamp"],
        "received_at": received_at,
        "eventType": event["eventType"],
        "os": event["os"]
    }), 201

if __name__ == "__main__":
    app.run(debug=True)