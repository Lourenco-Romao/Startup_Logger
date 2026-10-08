async function loadEvents() {
    try {
        const response = await fetch("/api/v1/events");

        if (!response.ok) {
            throw new Error(`HTTP error: ${response.status}`);
        }

        const events = await response.json();

        renderEvents(events);

    } catch (error) {
        console.error("Failed to load events:", error);
    }
}

function osColor(os) {
    switch(os) {
        case ("windows"):
            return "#0078D4"
        case ("linux"):
            return "#F5A623"
        case ("macos"):
            return "#A2AAAD"
        case ("android"):
            return "#3DDC84"
        case ("ios"):
            return "#FF375F"
        default:
            return "white"
    }
}

function eventTypeColor(eventType) {
    switch (eventType) {
        case ("startup"):
            return "#3DDC84"
        default:
            return "white"
    }
}

function renderEvents(events) {
    const tableBody = document.getElementById("events-table-body");
    const emptyState = document.getElementById("empty-state");

    tableBody.innerHTML = "";

    if (events.length === 0) {
        emptyState.hidden = false;
        return;
    }

    emptyState.hidden = true;

    events.forEach(event => {
        const row = document.createElement("tr");

        timestamp = new Date(event.timestamp * 1000).toLocaleString();
        receivedAt = new Date(event.receivedAt * 1000).toLocaleString();

        row.innerHTML = `
            <td>${event.hostname}</td>
            <td style="color: ${eventTypeColor(event.eventType)}">${event.eventType}</td>
            <td style="color: ${osColor(event.os)};">${event.os}</td>
            <td>${timestamp}</td>
            <td>${receivedAt}</td>
        `;

        tableBody.appendChild(row);
    });
}

window.addEventListener("DOMContentLoaded", loadEvents);