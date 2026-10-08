from flask import Flask, request, jsonify
import time

from validation import validate_event
from database import insert_event

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

if __name__ == "__main__":
    app.run(debug=True)