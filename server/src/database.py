import sqlite3
import os

from pathlib import Path
from dotenv import load_dotenv

ROOT_DIR = Path(__file__).resolve().parents[2]

load_dotenv(ROOT_DIR / ".env")

DATABASE = ROOT_DIR / os.getenv("DATABASE_PATH")


def get_connection():
    cnx = sqlite3.connect(DATABASE)
    cnx.execute("PRAGMA foreign_keys = ON")
    cnx.row_factory = sqlite3.Row
    return cnx

def init_database():
    cnx = get_connection()

    cnx.execute("""
        CREATE TABLE IF NOT EXISTS os (
            id_os INTEGER PRIMARY KEY,
            name TEXT NOT NULL UNIQUE
        )
    """)

    cnx.execute("""
        CREATE TABLE IF NOT EXISTS events (
            id_events INTEGER PRIMARY KEY,
            hostname TEXT NOT NULL,
            event_type TEXT NOT NULL,
            timestamp INTEGER NOT NULL,
            received_at INTEGER NOT NULL,
            os_id INTEGER NOT NULL,

            FOREIGN KEY (os_id)
                REFERENCES os(id_os)
        )
    """)

    cnx.executemany(
        """
        INSERT OR IGNORE INTO os (name)
        VALUES (?)
        """,
        [
            ("windows",),
            ("linux",),
            ("macos",),
            ("android",),
            ("ios",)
        ]
    )

    cnx.commit()
    cnx.close()

def insert_event(event, received_at):
    cnx = get_connection()

    os_name = event["os"]
    os_id = cnx.execute(
        "SELECT id_os FROM os WHERE name = ?",
        (os_name,)
    ).fetchone()

    if os_id is None:
        cnx.close()
        return False

    os_id = os_id[0]

    cnx.execute(
        """
        INSERT INTO events (hostname, event_type, timestamp, received_at, os_id)
        VALUES (?, ?, ?, ?, ?)
        """,
        (
            event["hostname"],
            event["eventType"],
            event["timestamp"],
            received_at,
            os_id
        )
    )

    cnx.commit()
    cnx.close()
    return True

def get_events(os_name=None, hostname=None, event_type=None, from_timestamp=None, to_timestamp=None, limit=100):
    cnx = get_connection()

    query = """
        SELECT
            events.id_events AS idEvents,
            events.hostname AS hostname,
            events.event_type AS eventType,
            events.timestamp AS timestamp,
            events.received_at AS receivedAt,
            os.name AS os
        FROM events
        JOIN os ON events.os_id = os.id_os
        WHERE 1=1
        """
    params = []
    

    if os_name is not None:
        query += " AND os.name = ?"
        params.append(os_name)
    if hostname is not None:
        query += " AND events.hostname LIKE ?"
        params.append(f"%{hostname}%")
    if event_type is not None:
        query += " AND events.event_type = ?"
        params.append(event_type)
    if from_timestamp is not None:
        query += " AND events.timestamp >= ?"
        params.append(from_timestamp)
    if to_timestamp is not None:
        query += " AND events.timestamp <= ?"
        params.append(to_timestamp)

    query += " ORDER BY events.timestamp DESC"
    query += " LIMIT ?"
    params.append(limit)

    events = cnx.execute(query, params).fetchall()

    cnx.close()

    return [dict(row) for row in events]