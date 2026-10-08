from flask import Flask, request, jsonify
import time

from validation import validate_event
from database import insert_event, get_events

app = Flask(__name__)

@app.route("/api/v1/events", methods=["POST"])
def create_event():
    event = request.get_json(silent=True)

    received_at = int(time.time())

    error = validate_event(event, received_at)

    if error is not None:
        return jsonify({
            "error": error
        }), 400

    if not insert_event(event, received_at):
        return jsonify({
            "error": "Failed to insert event into database"
        }), 500

    return jsonify({
        "message": "Event received and stored successfully",
        "received_at": received_at
    }), 201


@app.route("/api/v1/events", methods=["GET"])
def list_events():
    events = get_events()

    if events is None:
        return jsonify({
            "error": "Failed to retrieve events from database"
        }), 500
    if not events:
        return jsonify({
            "message": "No events found"
        }), 200
    
    return jsonify(events), 200


if __name__ == "__main__":
    app.run(debug=True)