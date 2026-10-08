import sqlite3

DATABASE = "startup_logger.db"


def get_connection():
    cnx = sqlite3.connect(DATABASE)
    cnx.execute("PRAGMA foreign_keys = ON")
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

def insert_event(event):
    cnx = get_connection()

    os_name = event["os"].lower()
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
            event["received_at"],
            os_id
        )
    )

    cnx.commit()
    cnx.close()
    return True

init_database()