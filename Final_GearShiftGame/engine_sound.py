"""
Gear Shift Race – 가상 엔진 사운드  (MATLAB 코드 기반 실시간 포팅)
UART 포맷: "S:XXX,R:XXXX,G:X\n"  (50ms 주기)

구성요소:
  ASD 사운드 : RPM 연동 저주파 + 배음 + 70Hz AM 변조

사용법:
  pip install pyserial sounddevice numpy scipy
  python engine_sound.py

주의: realtime_plot.py와 동시 실행 불가 (같은 시리얼 포트 사용)
"""

import re
import threading
import numpy as np
import sounddevice as sd
import serial
import serial.tools.list_ports

# ── 설정 ─────────────────────────────────────────────────────
PORT  = '/dev/cu.usbmodem11401'   # None 이면 자동 탐색
BAUD  = 9600
FS    = 44100    # 샘플링 주파수 (Hz)
CHUNK = 1024     # 오디오 버퍼 (~23 ms/callback)

W_ASD = 0.90   # 가상 엔진음 진폭

RPM_MIN = 900.0
RPM_MAX = 7000.0
SPD_MAX = 150.0

# ── 포트 자동 탐색 ────────────────────────────────────────────
if PORT is None:
    for p in serial.tools.list_ports.comports():
        if any(k in p.description.lower()
               for k in ('cp210', 'ch340', 'ftdi', 'uart', 'usb')):
            PORT = p.device
            print(f"[자동 탐색] {PORT}  ({p.description})")
            break
    if PORT is None:
        cands = list(serial.tools.list_ports.comports())
        if cands:
            PORT = cands[0].device
            print(f"[자동 탐색] 첫 번째 포트 사용: {PORT}")

# ── 공유 상태 (serial 스레드 ↔ 오디오 콜백) ──────────────────
class EngineState:
    def __init__(self):
        self.rpm   = RPM_MIN
        self.speed = 0.0
        self.gear  = 1
        self._lock = threading.Lock()

    def update(self, rpm, speed, gear):
        with self._lock:
            self.rpm   = float(np.clip(rpm,   RPM_MIN, RPM_MAX))
            self.speed = float(np.clip(speed, 0.0,     SPD_MAX))
            self.gear  = int(gear)

    def snapshot(self):
        with self._lock:
            return self.rpm, self.speed, self.gear

state = EngineState()

# ── 위상 누적 변수 (청크 경계에서 위상 연속성 유지) ──────────
_ph_asd = 0.0   # ASD 기본음 위상
_ph_mod = 0.0   # AM 변조파 위상 (70 Hz 고정)

# ── RPM 스무딩 (EMA) ──────────────────────────────────────────
# 청크 경계 주파수 점프 → 클릭 소리 방지
# alpha=0.08 → 시정수 약 200ms (낮을수록 더 부드러움)
_rpm_smooth = RPM_MIN
SMOOTH_ALPHA = 0.08

# ── 오디오 콜백 (sounddevice 백그라운드 스레드에서 호출) ──────
def audio_callback(outdata, frames, time_info, status):
    global _ph_asd, _ph_mod, _rpm_smooth

    rpm, speed, gear = state.snapshot()

    # EMA 스무딩: 청크마다 목표 RPM 쪽으로 조금씩 이동
    _rpm_smooth += SMOOTH_ALPHA * (rpm - _rpm_smooth)
    rpm_n = (_rpm_smooth - RPM_MIN) / (RPM_MAX - RPM_MIN)   # 0~1

    n = np.arange(frames, dtype=np.float64)

    # ── ASD : 50~400 Hz 기본음 + 배음 + 70Hz AM 변조 ─────────
    f_a      = 50.0 + rpm_n * 350.0
    step_a   = 2.0 * np.pi * f_a  / FS
    step_mod = 2.0 * np.pi * 70.0 / FS

    ph_a  = _ph_asd + n * step_a
    ph_m2 = _ph_mod + n * step_mod
    _ph_asd = (_ph_asd + frames * step_a)   % (2.0 * np.pi)
    _ph_mod = (_ph_mod + frames * step_mod) % (2.0 * np.pi)

    asd  = np.sin(ph_a) + np.sin(2*ph_a)*0.5 + np.sin(3*ph_a)*0.3
    asd *= (0.7 + 0.3 * np.sin(ph_m2))   # AM 변조 (러프니스)
    asd *= (0.8 + 0.2 * rpm_n)           # RPM 비례 진폭

    mixed = np.tanh(asd * W_ASD)

    outdata[:, 0] = mixed.astype(np.float32)


# ── UART 수신 스레드 ──────────────────────────────────────────
PATTERN = re.compile(r'S:(\d+),R:(\d+),G:(\d+)')

def serial_thread():
    if not PORT:
        print("[경고] 시리얼 포트 없음 – 아이들 RPM(900) 유지")
        return
    try:
        ser = serial.Serial(PORT, BAUD, timeout=0.1)
        print(f"연결됨: {PORT} @ {BAUD} baud")
        while True:
            try:
                raw = ser.readline().decode('utf-8', errors='ignore').strip()
                m = PATTERN.search(raw)
                if m:
                    state.update(rpm   = int(m.group(2)),
                                 speed = int(m.group(1)),
                                 gear  = int(m.group(3)))
            except Exception:
                pass
    except serial.SerialException as e:
        print(f"시리얼 오류: {e}")


# ── 진입점 ────────────────────────────────────────────────────
if __name__ == '__main__':
    threading.Thread(target=serial_thread, daemon=True).start()

    print("엔진 사운드 시작…  (Ctrl+C 로 종료)")
    print(f"  포트 : {PORT or '없음 (시뮬레이션 모드)'}")
    print(f"  FS   : {FS} Hz   CHUNK : {CHUNK} samples (~{CHUNK*1000//FS} ms/callback)")

    with sd.OutputStream(samplerate=FS, channels=1,
                         dtype='float32', blocksize=CHUNK,
                         callback=audio_callback):
        try:
            while True:
                sd.sleep(100)
        except KeyboardInterrupt:
            pass

    print("종료")
