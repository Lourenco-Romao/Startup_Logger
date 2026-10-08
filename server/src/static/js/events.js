async function loadEvents() {
    try {
        const response = await fetch("/api/v1/events");

        if (!response.ok) {
            throw new Error(`HTTP error: ${response.status}`);
        }

        const events = await response.json();

        const tableBody = document.getElementById("events-table-body");

        tableBody.innerHTML = "";

        for (const event of events) {
            const row = document.createElement("tr");

            row.innerHTML = `
                <td>${event.ID}</td>
                <td>${event.Hostname}</td>
                <td>${event["Event Type"]}</td>
                <td>${event.Timestamp}</td>
                <td>${event["Received At"]}</td>
                <td>${event.OS}</td>
            `;

            tableBody.appendChild(row);
        }

    } catch (error) {
        console.error("Failed to load events:", error);
    }
}

loadEvents();
window.addEventListener("DOMContentLoaded", loadEvents);