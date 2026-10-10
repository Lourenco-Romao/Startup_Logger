async function loadEvents(urlParams="") {
    let url = "/api/v1/events";

    if (urlParams) url += "?" + urlParams;

    console.log(url)

    try {
        const response = await fetch(url);

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

const hostnameFilter = document.getElementById("hostname-filter");
const osFilter = document.getElementById("os-filter");
const eventFilter = document.getElementById("event-filter");
const fromFilter = document.getElementById("from-filter");
const toFilter = document.getElementById("to-filter");

function getFilters() {
    const params = new URLSearchParams();

    if (hostnameFilter.value) {
        params.append("hostname", hostnameFilter.value);
    }

    if (osFilter.value) {
        params.append("os", osFilter.value);
    }

    if (eventFilter.value) {
        params.append("eventType", eventFilter.value);
    }

    if (fromFilter.value) {
        const timestamp = Math.floor(
            new Date(fromFilter.value).getTime() / 1000
        );

        params.append("from", timestamp);
    }

    if (toFilter.value) {
        const timestamp = Math.floor(
            new Date(toFilter.value).getTime() / 1000
        );

        params.append("to", timestamp);
    }

    return params.toString();
}

let hostnameTimeout;

hostnameFilter.addEventListener("input", () => {
    clearTimeout(hostnameTimeout);

    hostnameTimeout = setTimeout(() => {
        loadEvents(getFilters());
    }, 300);
});


osFilter.addEventListener("change", () => {
    loadEvents(getFilters());
});

eventFilter.addEventListener("change", () => {
    loadEvents(getFilters());
});

fromFilter.addEventListener("change", () => {
    loadEvents(getFilters());
});

toFilter.addEventListener("change", () => {
    loadEvents(getFilters());
});

window.addEventListener("DOMContentLoaded", () => {
    loadEvents(getFilters());
});

function clearFilters() {
    hostnameFilter.value = "";
    osFilter.value = "";
    eventFilter.value = "";
    fromFilter.value = "";
    toFilter.value = "";

    loadEvents();
}

document.getElementById("clear-filters").addEventListener("click", clearFilters);