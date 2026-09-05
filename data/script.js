// =====================================================
// Fan Controller
// ESP8266 Web Interface
// =====================================================


// =====================================================
// DOM elements
// =====================================================

const fanStateElement =
    document.getElementById("fanState");


const statusDescriptionElement =
    document.getElementById("statusDescription");


const connectionStatusElement =
    document.getElementById("connectionStatus");


const connectionTextElement =
    document.getElementById("connectionText");


const lastUpdateElement =
    document.getElementById("lastUpdate");


const controlButtons =
    document.querySelectorAll(
        ".control-button[data-state]"
    );


const muteButton =
    document.getElementById("muteButton");


const muteIcon =
    document.getElementById("muteIcon");


const timerButton =
    document.getElementById("timerButton");


const timerValueElement =
    document.getElementById("timerValue");


const timerPanel =
    document.getElementById("timerPanel");


const timerPanelDescription =
    document.getElementById(
        "timerPanelDescription"
    );


const timerActiveText =
    document.getElementById(
        "timerActiveText"
    );


const timerMinutesInput =
    document.getElementById(
        "timerMinutes"
    );


const timerSetButton =
    document.querySelector(
        ".timer-set-button"
    );


const timerCancelButton =
    document.querySelector(
        ".timer-cancel-button"
    );


// =====================================================
// Constants
// =====================================================

// =====================================================
// Update fan state on screen
// =====================================================

function updateFanState(state) {

    fanStateElement.textContent =
        getDisplayName(state);


    statusDescriptionElement.textContent =
        getDescription(state);


    // Remove active state from all
    // fan control buttons

    controlButtons.forEach(button => {

        button.classList.remove("active");

    });


    // Find the button corresponding
    // to the current fan state

    const activeButton =
        document.querySelector(
            `[data-state="${state}"]`
        );


    if (activeButton) {

        activeButton.classList.add("active");

    }
}


// =====================================================
// Update mute state on screen
// =====================================================

function updateMuteState(muted) {

    if (muted) {

        muteButton.classList.add("muted");

        muteIcon.textContent = "🔇";

    } else {

        muteButton.classList.remove("muted");

        muteIcon.textContent = "🔊";

    }
}


// =====================================================
// Update timer state on screen
// =====================================================

function updateTimerState(minutes) {

    // No active timer

    if (
        minutes === null ||
        minutes === undefined
    ) {

        timerButton.classList.remove("active");

        timerValueElement.textContent =
            "--:--";


        timerPanel.classList.remove(
            "timer-running"
        );


        timerPanelDescription.textContent =
            "Set when the fan should turn off";


        return;
    }


    // Active timer

    timerButton.classList.add("active");


    timerValueElement.textContent =
        formatTimer(minutes);


    timerPanel.classList.add(
        "timer-running"
    );


    timerPanelDescription.textContent =
        "Fan will turn off automatically";


    timerActiveText.textContent =
        "Turns off in " +
        formatTimer(minutes);
}


// =====================================================
// Format timer
// =====================================================

function formatTimer(minutes) {

    if (minutes < 60) {

        return minutes + " min";
    }


    const hours =
        Math.floor(minutes / 60);


    const remainingMinutes =
        minutes % 60;


    if (remainingMinutes === 0) {

        return hours + " hr";
    }


    return (
        hours +
        " hr " +
        remainingMinutes +
        " min"
    );
}


// =====================================================
// State display name
// =====================================================

function getDisplayName(state) {

    switch (state) {

        case "SPEED1":
            return "LOW";

        case "SPEED2":
            return "MEDIUM";

        case "SPEED3":
            return "HIGH";

        case "OFF":
        default:
            return "OFF";
    }
}


// =====================================================
// State description
// =====================================================

function getDescription(state) {

    switch (state) {

        case "SPEED1":
            return "Fan is running at low speed";

        case "SPEED2":
            return "Fan is running at medium speed";

        case "SPEED3":
            return "Fan is running at high speed";

        case "OFF":
        default:
            return "Fan is turned off";
    }
}


// =====================================================
// Connection status
// =====================================================

function setOnline() {

    connectionStatusElement.classList.remove(
        "offline"
    );


    connectionStatusElement.classList.add(
        "online"
    );


    connectionTextElement.textContent =
        "Online";
}


function setOffline() {

    connectionStatusElement.classList.remove(
        "online"
    );


    connectionStatusElement.classList.add(
        "offline"
    );


    connectionTextElement.textContent =
        "Offline";
}


// =====================================================
// Last update
// =====================================================

function updateLastUpdate() {

    const now = new Date();


    lastUpdateElement.textContent =
        now.toLocaleTimeString();
}


// =====================================================
// Apply complete status
// =====================================================

function applyStatus(data) {

    updateFanState(data.state);
    updateMuteState(data.mute);
    updateTimerState(data.timer);
    updateLastUpdate();
}


// =====================================================
// Realtime status WebSocket
// =====================================================

let statusSocket;

function connectStatusSocket() {

    statusSocket = new WebSocket(
        "ws://" + location.hostname + ":81/"
    );

    statusSocket.onopen = function () {
        setOnline();
    };

    statusSocket.onmessage = function (event) {

        try {
            const message = JSON.parse(event.data);

            if (message.type === "status") {
                applyStatus(message.data);
                setOnline();
            }
        } catch (error) {
            console.error("Invalid status message:", error);
        }
    };

    statusSocket.onclose = function () {
        setOffline();
        setTimeout(connectStatusSocket, 2000);
    };

    statusSocket.onerror = function () {
        statusSocket.close();
    };
}


// =====================================================
// Get current status
// =====================================================

async function refreshStatus() {

    try {

        const response =
            await fetch(
                "/api/status",
                {
                    method: "GET",
                    cache: "no-store"
                }
            );


        if (!response.ok) {

            throw new Error(
                "HTTP " + response.status
            );
        }


        const data =
            await response.json();


        // API is the single source of truth for the initial UI state.
        applyStatus(data);


        setOnline();


        updateLastUpdate();

    }

    catch (error) {

        console.error(
            "Status update failed:",
            error
        );


        setOffline();
    }
}


// =====================================================
// Change fan speed
// =====================================================

async function setFanSpeed(speed) {

    // Disable fan buttons only

    controlButtons.forEach(button => {

        button.disabled = true;

    });


    try {

        const response =
            await fetch(
                "/api/fan/" + speed,
                {
                    method: "POST"
                }
            );


        if (!response.ok) {

            throw new Error(
                "HTTP " + response.status
            );
        }


    }

    catch (error) {

        console.error(
            "Fan command failed:",
            error
        );


        setOffline();

    }

    finally {

        // Enable fan buttons again

        controlButtons.forEach(button => {

            button.disabled = false;

        });

    }
}


// =====================================================
// Toggle mute
// =====================================================

async function toggleMute() {

    // Determine current state

    const isMuted =
        muteButton.classList.contains(
            "muted"
        );


    // Select API endpoint

    const endpoint =
        isMuted
            ? "/api/beep/unmute"
            : "/api/beep/mute";


    // Disable mute button

    muteButton.disabled = true;


    try {

        const response =
            await fetch(
                endpoint,
                {
                    method: "POST"
                }
            );


        if (!response.ok) {

            throw new Error(
                "HTTP " + response.status
            );
        }


    }

    catch (error) {

        console.error(
            "Mute command failed:",
            error
        );


        setOffline();

    }

    finally {

        // Enable mute button again

        muteButton.disabled = false;

    }
}


// =====================================================
// Toggle timer panel
// =====================================================

function toggleTimerPanel() {

    const isOpen =
        timerPanel.classList.contains(
            "open"
        );


    if (isOpen) {

        closeTimerPanel();

    } else {

        openTimerPanel();

    }
}


// =====================================================
// Open timer panel
// =====================================================

function openTimerPanel() {

    timerPanel.classList.add(
        "open"
    );


    timerButton.setAttribute(
        "aria-expanded",
        "true"
    );

}


// =====================================================
// Close timer panel
// =====================================================

function closeTimerPanel() {

    timerPanel.classList.remove(
        "open"
    );


    timerButton.setAttribute(
        "aria-expanded",
        "false"
    );

}


// =====================================================
// Set custom timer
// =====================================================

function setCustomTimer() {

    const minutes =
        Number(
            timerMinutesInput.value
        );


    if (
        !Number.isInteger(minutes) ||
        minutes <= 0
    ) {

        timerMinutesInput.focus();

        return;
    }


    setFanOffTimer(minutes);
}


// =====================================================
// Set fan off timer
// =====================================================

async function setFanOffTimer(minutes) {

    timerSetButton.disabled = true;

    timerMinutesInput.disabled = true;


    try {

        const response =
            await fetch(
                "/api/fan/timer/set?minutes=" +
                minutes,
                {
                    method: "POST"
                }
            );


        if (!response.ok) {

            throw new Error(
                "HTTP " + response.status
            );
        }


        // Clear custom input

        timerMinutesInput.value = "";


        // Keep panel open so the
        // user can see the active timer

    }

    catch (error) {

        console.error(
            "Timer command failed:",
            error
        );


        setOffline();

    }

    finally {

        timerSetButton.disabled = false;

        timerMinutesInput.disabled = false;

    }
}


// =====================================================
// Cancel fan off timer
// =====================================================

async function cancelFanOffTimer() {

    timerCancelButton.disabled = true;


    try {

        const response =
            await fetch(
                "/api/fan/timer/cancel",
                {
                    method: "POST"
                }
            );


        if (!response.ok) {

            throw new Error(
                "HTTP " + response.status
            );
        }


    }

    catch (error) {

        console.error(
            "Timer cancel failed:",
            error
        );


        setOffline();

    }

    finally {

        timerCancelButton.disabled = false;

    }
}


// =====================================================
// Initial status
// =====================================================

refreshStatus();
connectStatusSocket();