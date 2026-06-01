"""
engine_sound_demo.py  ─  PPT용 가상 엔진 사운드 파일 생성
  자동으로 1→6단 시프트 시뮬레이션 후 WAV 저장

  pip install numpy scipy
  python engine_sound_demo.py
"""
import numpy as np
from scipy.io import wavfile

FS       = 44100
RPM_MIN  =  900.0
RPM_MAX  = 7000.0
W_ASD    = 0.90

# ── 기어 시프트 타임라인 ──────────────────────────────────────
# (시작초, 끝초, 시작RPM, 끝RPM)
# 가속 구간은 RPM 상승, 시프트 구간은 RPM 급락
TIMELINE = [
    ( 0.0,  0.8,  900,  900),   # 공회전
    ( 0.8,  3.0,  900, 6300),   # 1단 가속
    ( 3.0,  3.4, 6300, 3000),   # 시프트 1→2
    ( 3.4,  6.5, 3000, 6300),   # 2단 가속
    ( 6.5,  6.9, 6300, 3700),   # 시프트 2→3
    ( 6.9, 11.0, 3700, 6300),   # 3단 가속
    (11.0, 11.4, 6300, 4200),   # 시프트 3→4
    (11.4, 16.5, 4200, 6300),   # 4단 가속
    (16.5, 16.9, 6300, 4700),   # 시프트 4→5
    (16.9, 22.5, 4700, 6300),   # 5단 가속
    (22.5, 22.9, 6300, 5100),   # 시프트 5→6
    (22.9, 28.0, 5100, 6300),   # 6단 가속
    (28.0, 31.0, 6300,  900),   # 감속
]

TOTAL_SEC = TIMELINE[-1][1]


def make_rpm_curve():
    n = int(TOTAL_SEC * FS)
    rpm = np.full(n, RPM_MIN)
    for t0, t1, r0, r1 in TIMELINE:
        i0 = int(t0 * FS)
        i1 = min(int(t1 * FS), n)
        rpm[i0:i1] = np.linspace(r0, r1, i1 - i0)
    return np.clip(rpm, RPM_MIN, RPM_MAX)


def synthesize(rpm_curve):
    rpm_n = (rpm_curve - RPM_MIN) / (RPM_MAX - RPM_MIN)

    # 연속 위상 적분 (청크 경계 클릭 방지)
    f_a   = 50.0 + rpm_n * 350.0
    ph_a  = 2.0 * np.pi * np.cumsum(f_a)   / FS
    ph_m  = 2.0 * np.pi * 70.0 * np.arange(len(rpm_curve)) / FS

    asd  = np.sin(ph_a) + np.sin(2*ph_a)*0.5 + np.sin(3*ph_a)*0.3
    asd *= (0.7 + 0.3 * np.sin(ph_m))
    asd *= (0.8 + 0.2 * rpm_n)

    return np.tanh(asd * W_ASD)


if __name__ == '__main__':
    print(f"생성 중...  ({TOTAL_SEC:.0f}초 / 1→6단 자동 시프트)")

    rpm_curve = make_rpm_curve()
    audio     = synthesize(rpm_curve)

    audio_int16 = (audio * 32767).astype(np.int16)
    wavfile.write('engine_sound_demo.wav', FS, audio_int16)

    print("저장 완료: engine_sound_demo.wav")
