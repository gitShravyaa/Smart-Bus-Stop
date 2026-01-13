// ---------------- CONFIG ----------------

// UPDATE ONLY THIS LINE WHEN ESP32 IP CHANGES:
const ESP32_BASE = "http://10.239.29.173";  

const THINGSPEAK_CHANNEL = 3167997;
const THINGSPEAK_RESULTS = 30;

const THING_URL = 
  `https://api.thingspeak.com/channels/${THINGSPEAK_CHANNEL}/feeds.json?results=${THINGSPEAK_RESULTS}`;

// Chart objects
let chartDistance, chartPassenger, chartLight;

// Utility JSON fetch
async function fetchJSON(url) {
  const res = await fetch(url, { cache: "no-store" });
  if (!res.ok) throw new Error("HTTP " + res.status);
  return res.json();
}

// ---------------- CHART SETUP ----------------
function initCharts() {
  const ctxD = document.getElementById("chart-distance").getContext("2d");
  chartDistance = new Chart(ctxD, {
    type: "line",
    data: { labels: [], datasets: [{ label: "Distance (cm)", data: [], borderColor: "#00b894" }] },
    options: { animation: false }
  });

  const ctxP = document.getElementById("chart-passenger").getContext("2d");
  chartPassenger = new Chart(ctxP, {
    type: "bar",
    data: { labels: [], datasets: [{ label: "Passenger (0/1)", data: [], backgroundColor: "#0984e3" }] },
    options: { animation: false, scales: { y: { max: 1.2 } } }
  });

  const ctxL = document.getElementById("chart-light").getContext("2d");
  chartLight = new Chart(ctxL, {
    type: "line",
    data: { labels: [], datasets: [{ label: "Light", data: [], backgroundColor: "rgba(253,203,110,0.2)", borderColor: "#fdcb6e" }] },
    options: { animation: false }
  });
}

// ---------------- CARD UPDATE ----------------
function updateCards(feed) {
  const d = Number(feed.field1) || 999;
  const p = Number(feed.field2) || 0;
  const l = Number(feed.field3) || 500;

  document.getElementById("bus-status").textContent =
    d < 25 ? "🚍 Bus Approaching" : "❌ No Bus Nearby";

  document.getElementById("passenger-status").textContent =
    p ? "🧍 Passenger Waiting" : "— No Passenger";

  document.getElementById("light-status").textContent =
    l === 100 ? "🌙 Dark" : "☀️ Bright";
}

// ---------------- CHART UPDATE ----------------
function updateCharts(feeds) {
  const labels = feeds.map(f => new Date(f.created_at).toLocaleTimeString());
  const dist = feeds.map(f => Number(f.field1) || null);
  const pass = feeds.map(f => Number(f.field2) || 0);
  const light = feeds.map(f => Number(f.field3) || 0);

  chartDistance.data.labels = labels;
  chartDistance.data.datasets[0].data = dist;
  chartDistance.update();

  chartPassenger.data.labels = labels;
  chartPassenger.data.datasets[0].data = pass;
  chartPassenger.update();

  chartLight.data.labels = labels;
  chartLight.data.datasets[0].data = light;
  chartLight.update();
}

// ---------------- FETCH THINGSPEAK ----------------
async function refreshFromThingSpeak() {
  try {
    const data = await fetchJSON(THING_URL);
    const feeds = data.feeds;
    if (!feeds || feeds.length === 0) return;

    updateCards(feeds.at(-1));
    updateCharts(feeds);
  } catch (e) {
    console.error("TS Error:", e);
  }
}

// ---------------- ESP32 CONTROL ----------------
async function sendControl(path) {
  const log = document.getElementById("control-log");

  try {
    const res = await fetch(`${ESP32_BASE}/${path}`, { method: "GET", cache: "no-store" });
    log.textContent = `Sent: ${path} → ${res.status}`;
  } catch (e) {
    log.textContent = "ESP32 Offline or IP Wrong";
    console.error(e);
  }
}

function toggleLight(state) { sendControl(`light?state=${state}`); }
function toggleBuzzer(state) { sendControl(`buzzer?state=${state}`); }

// ---------------- INIT ----------------
initCharts();
refreshFromThingSpeak();
setInterval(refreshFromThingSpeak, 15000);   // updates every 15 sec
