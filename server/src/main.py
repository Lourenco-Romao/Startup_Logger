from flask import Flask, request, jsonify, render_template
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
    os_name = request.args.get("os")
    hostname = request.args.get("hostname")
    event_type = request.args.get("eventType")

    from_timestamp = request.args.get("from", type=int)
    to_timestamp = request.args.get("to", type=int)
    limit = request.args.get("limit", type=int, default=100)

    events = get_events(
        os_name=os_name,
        hostname=hostname,
        event_type=event_type,
        from_timestamp=from_timestamp,
        to_timestamp=to_timestamp,
        limit=limit
    )

    return jsonify(events), 200

@app.route("/events", methods=["GET"])
def events_page():
    return render_template("events.html")


if __name__ == "__main__":
    app.run(debug=True)