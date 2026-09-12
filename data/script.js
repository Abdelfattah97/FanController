// =====================================================
// Fan Controller
// ESP8266 Web Interface
// =====================================================


// =====================================================
// DOM elements
// =====================================================

let fanStateElement;
let statusDescriptionElement;
let connectionStatusElement;
let connectionTextElement;
let lastUpdateElement;
let controlButtons;
let muteButton;
let muteIcon;
let timerButton;
let timerValueElement;
let timerPanel;
let timerPanelDescription;
let timerActiveText;
let timerMinutesInput;
let timerSetButton;
let timerCancelButton;

function cacheDomElements() {

    fanStateElement =
        document.getElementById("fanState");

    statusDescriptionElement =
        document.getElementById("statusDescription");

    connectionStatusElement =
        document.getElementById("connectionStatus");

    connectionTextElement =
        document.getElementById("connectionText");

    lastUpdateElement =
        document.getElementById("lastUpdate");

    controlButtons =
        document.querySelectorAll(
            ".control-button[data-state]"
        );

    muteButton =
        document.getElementById("muteButton");

    muteIcon =
        document.getElementById("muteIcon");

    timerButton =
        document.getElementById("timerButton");

    timerValueElement =
        document.getElementById("timerValue");

    timerPanel =
        document.getElementById("timerPanel");

    timerPanelDescription =
        document.getElementById(
            "timerPanelDescription"
        );

    timerActiveText =
        document.getElementById(
            "timerActiveText"
        );

    timerMinutesInput =
        document.getElementById(
            "timerMinutes"
        );

    timerSetButton =
        document.querySelector(
            ".timer-set-button"
        );

    timerCancelButton =
        document.querySelector(
            ".timer-cancel-button"
        );
}



// =====================================================
// Fetch with timeout
// =====================================================
// Without this, a slow/broken mDNS resolution or a dead
// ESP8266 can leave fetch() hanging far longer than the
// UI should ever wait before falling back to "Offline".

async function fetchWithTimeout(url, options = {}, timeoutMs = 4000) {

    const controller = new AbortController();

    const timeoutId = setTimeout(
        () => controller.abort(),
        timeoutMs
    );

    try {

        return await fetch(url, {
            ...options,
            signal: controller.signal
        });

    } finally {

        clearTimeout(timeoutId);

    }
}


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
// Map a speed API segment to its state name
// =====================================================
// Used for optimistic UI updates right after a
// successful POST, before the WebSocket confirms it.

function speedToStateName(speed) {

    switch (speed) {

        case "speed1":
            return "SPEED1";

        case "speed2":
            return "SPEED2";

        case "speed3":
            return "SPEED3";

        case "off":
        default:
            return "OFF";
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

let reconnectAttempts = 0;

let reconnectTimeoutId = null;

const MAX_RECONNECT_DELAY_MS = 10000;


function getReconnectDelay() {

    // Simple capped backoff: 2s, 4s, 6s, 8s, 10s, 10s...

    const delay =
        2000 * (reconnectAttempts + 1);

    return Math.min(
        delay,
        MAX_RECONNECT_DELAY_MS
    );
}


function connectStatusSocket() {

    // Avoid piling up duplicate sockets if a
    // reconnect is triggered while one is still open.

    if (
        statusSocket &&
        (
            statusSocket.readyState === WebSocket.OPEN ||
            statusSocket.readyState === WebSocket.CONNECTING
        )
    ) {

        return;
    }

    if (reconnectTimeoutId) {

        clearTimeout(reconnectTimeoutId);

        reconnectTimeoutId = null;
    }


    statusSocket =
        new WebSocket(getBaseWsUrl());


    statusSocket.onopen =
        function () {

            reconnectAttempts = 0;

            setOnline();

        };


    statusSocket.onmessage =
        function (event) {

            try {

                const message =
                    JSON.parse(event.data);


                if (
                    message.type === "status"
                ) {

                    applyStatus(
                        message.data
                    );


                    setOnline();

                }

            } catch (error) {

                console.error(
                    "Invalid status message:",
                    error
                );

            }

        };


    statusSocket.onclose =
        function () {

            setOffline();

            const delay = getReconnectDelay();

            reconnectAttempts++;

            reconnectTimeoutId = setTimeout(
                connectStatusSocket,
                delay
            );

        };


    statusSocket.onerror =
        function () {

            statusSocket.close();

        };
}


// =====================================================
// Get current status
// =====================================================

async function refreshStatus() {

    try {

        const response =
            await fetchWithTimeout(
                getBaseUrl() +
                "/api/status", {
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

        applyStatus(data);


        setOnline();

    } catch (error) {

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
            await fetchWithTimeout(
                getBaseUrl() +
                "/api/fan/" +
                speed, {
                method: "POST"
            }
            );


        if (!response.ok) {

            throw new Error(
                "HTTP " + response.status
            );

        }


        // Optimistic update: reflect the change
        // immediately instead of waiting for the
        // WebSocket broadcast to round-trip back.

        updateFanState(
            speedToStateName(speed)
        );


        setOnline();

    } catch (error) {

        console.error(
            "Fan command failed:",
            error
        );


        setOffline();

    } finally {

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
        isMuted ?
            getBaseUrl() +
            "/api/beep/unmute" :
            getBaseUrl() +
            "/api/beep/mute";


    // Disable mute button

    muteButton.disabled = true;


    try {

        const response =
            await fetchWithTimeout(
                endpoint, {
                method: "POST"
            }
            );


        if (!response.ok) {

            throw new Error(
                "HTTP " + response.status
            );

        }


        // Optimistic update: flip immediately rather
        // than waiting on the WebSocket to confirm.

        updateMuteState(!isMuted);


        setOnline();

    } catch (error) {

        console.error(
            "Mute command failed:",
            error
        );


        setOffline();

    } finally {

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

const MAX_TIMER_MINUTES = 1440; // 24 hours, sanity cap


function setCustomTimer() {

    const minutes =
        Number(
            timerMinutesInput.value
        );


    if (
        !Number.isInteger(minutes) ||
        minutes <= 0 ||
        minutes > MAX_TIMER_MINUTES
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
            await fetchWithTimeout(
                getBaseUrl() +
                "/api/fan/timer/set?minutes=" +
                minutes, {
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


        // Optimistic update so the panel reflects the
        // new timer immediately rather than waiting on
        // the next WebSocket broadcast.

        updateTimerState(minutes);


        setOnline();


        // Keep panel open so the
        // user can see the active timer

    } catch (error) {

        console.error(
            "Timer command failed:",
            error
        );


        setOffline();

    } finally {

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
            await fetchWithTimeout(
                getBaseUrl() +
                "/api/fan/timer/cancel", {
                method: "POST"
            }
            );


        if (!response.ok) {

            throw new Error(
                "HTTP " + response.status
            );

        }


        // Optimistic update

        updateTimerState(null);


        setOnline();

    } catch (error) {

        console.error(
            "Timer cancel failed:",
            error
        );


        setOffline();

    } finally {

        timerCancelButton.disabled = false;

    }
}


// =====================================================
// Reconnect / refresh when the WebView becomes visible
// again (e.g. app resumed from background). Android may
// have throttled or killed the socket while backgrounded.
// =====================================================

function handleVisibilityChange() {

    if (document.visibilityState !== "visible") {

        return;
    }


    if (
        !statusSocket ||
        statusSocket.readyState === WebSocket.CLOSED ||
        statusSocket.readyState === WebSocket.CLOSING
    ) {

        connectStatusSocket();
    }

    refreshStatus();
}


// =====================================================
// Startup
// =====================================================

document.addEventListener("DOMContentLoaded", () => {

    cacheDomElements();


    const fanUrlElement =
        document.getElementById("fanUrl");

    if (fanUrlElement) {

        fanUrlElement.textContent =
            `(${getBaseUrl()})`;
    }


    document.addEventListener(
        "visibilitychange",
        handleVisibilityChange
    );


    // Start the WebSocket first; refreshStatus() only
    // applies its result if the socket hasn't already
    // delivered fresher state (see initialStateReceived).

    connectStatusSocket();

    refreshStatus();

});

function getBaseUrl() {
    return window.getFanBaseUrl
        ? window.getFanBaseUrl()
        : window.location.origin;
}
function getBaseWsUrl() {
    return `ws://${getBaseUrl().replace(/^http:\/\//, "")}:81/`;
}
