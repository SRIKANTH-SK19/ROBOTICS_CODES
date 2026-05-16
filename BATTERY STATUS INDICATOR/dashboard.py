# =========================================================
# FINAL INDUSTRIAL AI BATTERY DASHBOARD (STABLE VERSION)
# =========================================================

import streamlit as st
import serial
import pandas as pd
import plotly.graph_objects as go
import plotly.express as px
import time
import re
import os
import numpy as np
import random

from sklearn.linear_model import LinearRegression
from sklearn.ensemble import IsolationForest

st.set_page_config(layout="wide")
st.title("⚡ Industrial AI Battery Monitoring System")

# ---------------- SERIAL ----------------
port = st.sidebar.text_input("COM Port", "COM5")
baud = 115200

@st.cache_resource
def connect_serial():
    try:
        ser = serial.Serial(port, baud, timeout=1)
        time.sleep(2)
        ser.reset_input_buffer()
        return ser
    except:
        return None

ser = connect_serial()

if ser is None:
    st.error("❌ ESP32 Not Connected")
    st.stop()

# ---------------- SESSION ----------------
if "data" not in st.session_state:
    st.session_state.data = []

# 🔥 NEW BUFFER (ADDED)
if "health_buffer" not in st.session_state:
    st.session_state.health_buffer = []

if "vals" not in st.session_state:
    st.session_state.vals = {
        "temp": 0,
        "voltage": 0,
        "current": 0,
        "power": 0,
        "time": "Standby",
        "status": "No Load"
    }

if "health" not in st.session_state:
    st.session_state.health = 68.0

if "time_val" not in st.session_state:
    st.session_state.time_val = 2.0

vals = st.session_state.vals

# ---------------- SERIAL READ ----------------
def read_packet():
    packet = []
    started = False
    start_time = time.time()

    while time.time() - start_time < 1:
        try:
            line = ser.readline().decode(errors="ignore").strip()

            if "DATA:START" in line:
                packet = []
                started = True
                continue

            if "DATA:END" in line and started:
                return packet

            if started:
                packet.append(line)

        except:
            pass

    return None

# ---------------- PARSER ----------------
def parse_packet(packet):
    for line in packet:
        if ":" not in line:
            continue

        key, value = line.split(":", 1)
        key = key.strip().lower()
        value = value.strip()

        num = re.findall(r"[-+]?\d*\.?\d+", value)

        if key == "temperature" and num:
            vals["temp"] = float(num[0])

        elif key == "voltage" and num:
            vals["voltage"] = float(num[0])

        elif key == "current" and num:
            vals["current"] = float(num[0])

        elif key == "power" and num:
            vals["power"] = float(num[0])

        elif key == "time left":
            vals["time"] = value

        elif key == "status":
            vals["status"] = value

# ---------------- UPDATE ----------------
packet = read_packet()
if packet:
    parse_packet(packet)

# ---------------- STORE ----------------
current_time = time.time()

row = {
    "t": current_time,
    "temp": vals["temp"],
    "voltage": vals["voltage"],
    "current": vals["current"],
    "power": vals["power"]
}

st.session_state.data.append(row)

# 🔥 NEW: STORE BUFFER DATA
st.session_state.health_buffer.append({
    "time": current_time,
    "voltage": vals["voltage"]
})

# keep only last 5 seconds
st.session_state.health_buffer = [
    x for x in st.session_state.health_buffer
    if current_time - x["time"] <= 5
]

if len(st.session_state.data) > 500:
    st.session_state.data.pop(0)

df = pd.DataFrame(st.session_state.data)

# save log
df.tail(1).to_csv("battery_log.csv", mode='a', header=not os.path.exists("battery_log.csv"), index=False)

# =========================================================
# KPI
# =========================================================
c1, c2, c3, c4 = st.columns(4)
c1.metric("🌡 Temp", f"{vals['temp']} °C")
c2.metric("⚡ Voltage", f"{vals['voltage']} V")
c3.metric("🔌 Current", f"{vals['current']} A")
c4.metric("🔥 Power", f"{vals['power']} W")

# =========================================================
# ⏳ STABLE TIME CALCULATION
# =========================================================
if len(df) > 5:
    avg_current = df["current"].rolling(10).mean().iloc[-1]
else:
    avg_current = vals["current"]

if avg_current < 0.15:
    stable_time = "Standby"
else:
    hours = 2.2 / avg_current
    hours = max(0.5, min(5, hours))

    st.session_state.time_val = (
        0.9 * st.session_state.time_val +
        0.1 * hours
    )

    hr = int(st.session_state.time_val)
    mn = int((st.session_state.time_val - hr) * 60)

    stable_time = f"{hr}h {mn}min"

# =========================================================
# BIG TIME DISPLAY
# =========================================================
st.markdown("## ⏳ Remaining Runtime")

st.markdown(f"""
<div style="
    font-size:48px;
    font-weight:bold;
    color:#22c55e;
    text-align:center;
    padding:15px;
    border-radius:10px;
    background:rgba(34,197,94,0.1);
">
{stable_time}
</div>
""", unsafe_allow_html=True)

# =========================================================
# STATUS
# =========================================================
if "no load" in vals["status"].lower():
    st.warning("🔌 No Load Connected")
else:
    st.success("⚡ Load Connected")

# =========================================================
# SPLIT PANEL
# =========================================================
st.markdown("## 📊 Advanced Analytics")
col_left, col_main = st.columns([1,3])

# ---------------- LEFT PANEL ----------------
with col_left:
    st.markdown("### 📋 Last 5 Readings")
    last5 = df.tail(5)

    st.dataframe(last5[["voltage","current","power"]])

    fig_bar = px.bar(last5, y="current", title="Current (Last 5)")
    fig_bar.update_layout(height=250, template="plotly_dark")
    st.plotly_chart(fig_bar, use_container_width=True)

# ---------------- MAIN PANEL ----------------
with col_main:

    if len(df) > 30:

        df["v_avg"] = df["voltage"].rolling(10).mean()
        df["c_avg"] = df["current"].rolling(10).mean()

        # ML prediction
        X = np.arange(len(df)).reshape(-1, 1)
        y = df["power"].values

        model = LinearRegression()
        model.fit(X, y)

        future = model.predict([[len(df)+20]])[0]

        # anomaly
        iso = IsolationForest(contamination=0.05)
        df["anomaly"] = iso.fit_predict(df[["voltage","current","power"]])
        anomalies = df[df["anomaly"] == -1]

        # =========================================================
        # 🔋 BATTERY HEALTH (UPDATED USING BUFFER)
        # =========================================================
        if len(st.session_state.health_buffer) > 5:
            voltages = [x["voltage"] for x in st.session_state.health_buffer]
            avg_voltage = sum(voltages) / len(voltages)
        else:
            avg_voltage = df["voltage"].rolling(10).mean().iloc[-1]

        target_health = (avg_voltage - 3.2) / (4.2 - 3.2) * 100
        target_health = max(65, min(100, target_health))

        st.session_state.health = (
            0.97 * st.session_state.health +
            0.03 * target_health
        )

        health = round(st.session_state.health, 1)

        # AI PANEL
        st.markdown("## 🧠 AI Intelligence")
        a1, a2, a3 = st.columns(3)
        a1.metric("🔋 Battery Health", f"{health}%")
        a2.metric("🔮 Predicted Power", f"{future:.2f} W")
        a3.metric("⚠ Anomalies", len(anomalies))

        # GRAPHS (UNCHANGED)
        col1, col2 = st.columns(2)

        with col1:
            fig = go.Figure()
            fig.add_trace(go.Scatter(y=df["voltage"], name="Voltage"))
            fig.add_trace(go.Scatter(y=df["v_avg"], name="Avg Voltage"))
            st.plotly_chart(fig, use_container_width=True)

        with col2:
            fig2 = go.Figure()
            fig2.add_trace(go.Scatter(y=df["current"], name="Current"))
            fig2.add_trace(go.Scatter(y=df["c_avg"], name="Avg Current"))
            st.plotly_chart(fig2, use_container_width=True)

        st.markdown("## 🚨 Anomaly Detection")

        fig3 = go.Figure()
        fig3.add_trace(go.Scatter(y=df["power"], name="Power"))

        fig3.add_trace(go.Scatter(
            x=anomalies.index,
            y=anomalies["power"],
            mode='markers',
            name='Anomalies'
        ))

        st.plotly_chart(fig3, use_container_width=True)

# ---------------- REFRESH ----------------
time.sleep(0.4)
st.rerun()