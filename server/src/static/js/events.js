async function loadEvents() {
    try {
        const response = await fetch("/api/v1/events");

        if (!response.ok) {
            throw new Error(`HTTP error: ${response.status}`);
        }

        const events = await response.json();

        renderEvents(events);
        document.getElementById("loading").hidden = true;
        document.getElementById("table-container").hidden = false;

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

function formatDelay(seconds) {
    if (seconds < 0) {
        return `${seconds}s`
    }
    if (seconds < 60) {
        return `+${seconds}s`;
    }

    const minutes = Math.floor(seconds / 60);
    const remainingSeconds = seconds % 60;

    if (seconds < 3600) {
        return `+${minutes}m ${remainingSeconds}s`;
    }

    const hours = Math.floor(seconds / 3600);
    const remainingMinutes = Math.floor((seconds % 3600) / 60);

    if (seconds < 86400) {
        return `+${hours}h ${remainingMinutes}m ${remainingSeconds}s`;
    }

    const days = Math.floor(seconds / 86400);
    const remainingHours = Math.floor((seconds % 86400) / 3600);

    return `+${days}d ${remainingHours}h ${remainingMinutes}m`;
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

        const timestamp = new Date(event.timestamp * 1000).toLocaleString();

        const delay = event.receivedAt - event.timestamp;
        const receivedAt = formatDelay(delay)

        row.innerHTML = `
            <td>${event.hostname}</td>
            <td style="color: ${eventTypeColor(event.eventType)};">${event.eventType}</td>
            <td style="color: ${osColor(event.os)};">${event.os}</td>
            <td>${timestamp}</td>
            <td>${receivedAt}</td>
        `;

        tableBody.appendChild(row);
    });

}

window.addEventListener("DOMContentLoaded", loadEvents);