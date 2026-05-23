"""
Gear Shift Race – 실시간 모니터
UART 포맷: "S:XXX,R:XXXX,G:X\n"  (50ms 주기)

사용법:
  1. PORT 변수를 실제 시리얼 포트로 변경
     macOS 예시: /dev/cu.usbserial-XXXX  또는  /dev/cu.SLAB_USBtoUART
     Windows  예시: COM3
  2. pip install pyserial matplotlib
  3. python realtime_plot.py
"""

import re
import serial
import serial.tools.list_ports
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from collections import deque

# ── 설정 ───────────────────────────────────────────────────────────
PORT  = '/dev/cu.usbmodem11401'   # None 이면 자동 탐색, 직접 지정 시: '/dev/cu.SLAB_USBtoUART'
BAUD  = 9600
WIN   = 300    # 화면에 표시할 데이터 포인트 수 (300 × 50ms = 15초)
DT_MS = 50     # UART 전송 주기 [ms]

# ── 자동 포트 탐색 ─────────────────────────────────────────────────
if PORT is None:
    candidates = list(serial.tools.list_ports.comports())
    for p in candidates:
        if any(k in p.description.lower() for k in ('cp210', 'ch340', 'ftdi', 'uart', 'usb')):
            PORT = p.device
            print(f"[자동 탐색] 포트 발견: {PORT}  ({p.description})")
            break
    if PORT is None and candidates:
        PORT = candidates[0].device
        print(f"[자동 탐색] 첫 번째 포트 사용: {PORT}")
    if PORT is None:
        raise RuntimeError("시리얼 포트를 찾을 수 없습니다. PORT 변수를 직접 설정하세요.")

# ── 버퍼 ───────────────────────────────────────────────────────────
t_buf    = deque(maxlen=WIN)
spd_buf  = deque(maxlen=WIN)
rpm_buf  = deque(maxlen=WIN)
gear_buf = deque(maxlen=WIN)
tick     = 0

# ── 시리얼 열기 ────────────────────────────────────────────────────
ser = serial.Serial(PORT, BAUD, timeout=0.05)
print(f"연결됨: {PORT} @ {BAUD} baud")

# ── 그래프 설정 ────────────────────────────────────────────────────
fig, axes = plt.subplots(3, 1, figsize=(11, 7), sharex=True)
fig.suptitle("Gear Shift Race – Real-time Monitor", fontsize=13, fontweight='bold')
fig.patch.set_facecolor('#1e1e1e')

for ax in axes:
    ax.set_facecolor('#2d2d2d')
    ax.tick_params(colors='#cccccc')
    ax.spines[:].set_color('#555555')
    ax.yaxis.label.set_color('#cccccc')
    ax.grid(True, color='#444444', linewidth=0.5)

ax_spd, ax_rpm, ax_gear = axes

# Speed
ln_spd, = ax_spd.plot([], [], color='#4fc3f7', linewidth=1.8)
ax_spd.set_ylabel("Speed (km/h)")
ax_spd.set_ylim(0, 170)
ax_spd.axhline(150, color='#ef5350', linestyle='--', linewidth=0.8, alpha=0.6, label='Max 150')
ax_spd.legend(loc='upper left', fontsize=8, facecolor='#333333', labelcolor='#cccccc')

# RPM
ln_rpm, = ax_rpm.plot([], [], color='#ff8a65', linewidth=1.8)
ax_rpm.set_ylabel("RPM")
ax_rpm.set_ylim(0, 7500)
ax_rpm.axhline(6800, color='#ef5350', linestyle='--', linewidth=1.0, alpha=0.7, label='Redline 6800')
ax_rpm.axhspan(3000, 5500, alpha=0.08, color='#a5d6a7', label='Power band')
ax_rpm.legend(loc='upper left', fontsize=8, facecolor='#333333', labelcolor='#cccccc')

# Gear
ln_gear, = ax_gear.plot([], [], color='#ce93d8', linewidth=1.8, drawstyle='steps-post')
ax_gear.set_ylabel("Gear")
ax_gear.set_xlabel("Time (ms)")
ax_gear.set_ylim(0, 5)
ax_gear.set_yticks([1, 2, 3, 4])

# 실시간 텍스트
txt_spd  = ax_spd.text(0.99, 0.88, '', transform=ax_spd.transAxes,
                       ha='right', color='#4fc3f7', fontsize=10, fontweight='bold')
txt_rpm  = ax_rpm.text(0.99, 0.88, '', transform=ax_rpm.transAxes,
                       ha='right', color='#ff8a65', fontsize=10, fontweight='bold')
txt_gear = ax_gear.text(0.99, 0.75, '', transform=ax_gear.transAxes,
                        ha='right', color='#ce93d8', fontsize=10, fontweight='bold')

plt.tight_layout(rect=[0, 0, 1, 0.96])

# ── 데이터 파싱 및 애니메이션 ──────────────────────────────────────
PATTERN = re.compile(r'S:(\d+),R:(\d+),G:(\d+)')

def update(_frame):
    global tick
    while ser.in_waiting:
        try:
            raw = ser.readline().decode('utf-8', errors='ignore').strip()
            m = PATTERN.search(raw)
            if m:
                spd  = int(m.group(1))
                rpm  = int(m.group(2))
                gear = int(m.group(3))
                t_buf.append(tick * DT_MS)
                spd_buf.append(spd)
                rpm_buf.append(rpm)
                gear_buf.append(gear)
                tick += 1
        except Exception:
            pass

    if len(t_buf) < 2:
        return ln_spd, ln_rpm, ln_gear

    ts = list(t_buf)
    xs = [ts[0], ts[-1]]

    ln_spd.set_data(ts, list(spd_buf))
    ln_rpm.set_data(ts, list(rpm_buf))
    ln_gear.set_data(ts, list(gear_buf))

    for ax in axes:
        ax.set_xlim(xs[0], max(xs[1], xs[0] + WIN * DT_MS))

    # 현재값 텍스트 갱신
    txt_spd.set_text(f"{spd_buf[-1]} km/h")
    txt_rpm.set_text(f"{rpm_buf[-1]} RPM")
    txt_gear.set_text(f"Gear {gear_buf[-1]}")

    return ln_spd, ln_rpm, ln_gear

ani = animation.FuncAnimation(
    fig, update, interval=50, blit=False, cache_frame_data=False
)

try:
    plt.show()
finally:
    ser.close()
    print("시리얼 포트 닫힘")
