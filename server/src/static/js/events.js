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

        timestamp = new Date(event.Timestamp * 1000).toLocaleString();
        receivedAt = new Date(event["Received At"] * 1000).toLocaleString();

        row.innerHTML = `
            <td>${event.ID}</td>
            <td>${event.Hostname}</td>
            <td>${event["Event Type"]}</td>
            <td>${event.OS}</td>
            <td>${timestamp}</td>
            <td>${receivedAt}</td>
        `;

        tableBody.appendChild(row);
    });
}

window.addEventListener("DOMContentLoaded", loadEvents);