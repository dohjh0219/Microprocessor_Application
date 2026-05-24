"""
gauge image generator  ─  속도계 + RPM 계기 동시 생성
  pip install matplotlib numpy
  python make_gauge.py
"""
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches

# ═══════════════════════════════════════════════════════════
#  공통 설정
# ═══════════════════════════════════════════════════════════
PRINT_MM   = 100     # 인쇄 지름 (mm)
DPI        = 300

# 바늘 각도  (12시=0°, 시계방향+)
# 서보 실측 후 이 값 조정 → 재실행
ANGLE_ZERO = 225.0   # 최솟값 위치 (7:30 방향)
ANGLE_MAX  =  45.0   # 최댓값 위치 (1:30 방향)  → 180° 스윕

# ═══════════════════════════════════════════════════════════
#  속도계 설정
# ═══════════════════════════════════════════════════════════
SPD_MAX         = 220
SPD_MAJOR_STEP  =  20
SPD_MINOR_STEP  =   5
SPD_LABELS      = [0, 40, 80, 120, 160, 200]
SPD_ZONE_YELLOW = 160
SPD_ZONE_RED    = 200

# ═══════════════════════════════════════════════════════════
#  RPM 계기 설정
# ═══════════════════════════════════════════════════════════
RPM_MAX         = 8000     # 표시 최대 RPM
RPM_MAJOR_STEP  = 1000
RPM_MINOR_STEP  =  200
RPM_LABELS      = [0, 1, 2, 3, 4, 5, 6, 7, 8]   # ×1000
RPM_ZONE_YELLOW = 6000     # 파워밴드 끝 (노랑 시작)
RPM_ZONE_RED    = 6800     # 레드라인
# ═══════════════════════════════════════════════════════════


def val_to_clock(val, val_max):
    sweep = (ANGLE_MAX - ANGLE_ZERO) % 360
    return ANGLE_ZERO + (val / val_max) * sweep


def clock_to_rad(deg):
    return np.radians(90.0 - deg)


def draw_arc(ax, r, v_s, v_e, v_max, color, lw):
    m1 = (90 - val_to_clock(v_e, v_max)) % 360
    m2 = (90 - val_to_clock(v_s, v_max)) % 360
    ax.add_patch(mpatches.Arc(
        (0, 0), 2 * r, 2 * r, angle=0,
        theta1=m1, theta2=m2,
        color=color, lw=lw, zorder=2
    ))


def make_gauge(title, val_max, major_step, minor_step, labels,
               zone_yellow, zone_red, unit_label, label_scale=1,
               filename='gauge.png'):
    inch = PRINT_MM / 25.4
    fig, ax = plt.subplots(figsize=(inch, inch))
    ax.set_aspect('equal')
    ax.set_xlim(-1.2, 1.2)
    ax.set_ylim(-1.2, 1.2)
    ax.axis('off')
    fig.patch.set_facecolor('#0d0d0d')

    # 배경 원
    ax.add_patch(plt.Circle((0, 0), 1.12, color='#141414', zorder=0))
    ax.add_patch(plt.Circle((0, 0), 1.12, color='#3a3a3a', fill=False, lw=1.5, zorder=1))

    # 색 구역 호
    draw_arc(ax, 0.97, 0,          zone_yellow, val_max, '#22cc44', lw=7)
    draw_arc(ax, 0.97, zone_yellow, zone_red,   val_max, '#ffaa00', lw=7)
    draw_arc(ax, 0.97, zone_red,    val_max,    val_max, '#ee3333', lw=7)

    # 눈금
    for v in range(0, val_max + 1, minor_step):
        ang = clock_to_rad(val_to_clock(v, val_max))
        if v % major_step == 0:
            r_in, lw_t, col = 0.76, 2.5, 'white'
        elif v % (major_step // 2) == 0:
            r_in, lw_t, col = 0.84, 1.5, '#aaaaaa'
        else:
            r_in, lw_t, col = 0.89, 1.0, '#555555'
        ax.plot(
            [r_in * np.cos(ang), 0.94 * np.cos(ang)],
            [r_in * np.sin(ang), 0.94 * np.sin(ang)],
            color=col, lw=lw_t, zorder=3, solid_capstyle='round'
        )

    # 숫자
    fs = PRINT_MM * 0.14
    for lv in labels:
        v = lv * label_scale
        ang = clock_to_rad(val_to_clock(v, val_max))
        # 안쪽으로 살짝 당겨서 눈금선과 겹치지 않게
        r_txt = 0.57
        ax.text(
            r_txt * np.cos(ang), r_txt * np.sin(ang),
            str(lv), color='white', fontsize=fs,
            fontweight='bold', ha='center', va='center', zorder=4,
            fontfamily='monospace'
        )

    # 단위 레이블
    ax.text(0, -0.30, unit_label, color='#888888',
            fontsize=fs * 0.68, ha='center', va='center', zorder=4)

    # 중심
    ax.add_patch(plt.Circle((0, 0), 0.07, color='#cccccc', zorder=6))
    ax.add_patch(plt.Circle((0, 0), 0.035, color='#333333', zorder=7))

    plt.savefig(filename, dpi=DPI, bbox_inches='tight',
                facecolor='#0d0d0d', edgecolor='none')
    plt.close()
    print(f"저장: {filename}  ({PRINT_MM}mm × {PRINT_MM}mm @ {DPI}dpi)")


# ── 속도계 생성 ──────────────────────────────────────────────
make_gauge(
    title='speedometer',
    val_max=SPD_MAX,
    major_step=SPD_MAJOR_STEP,
    minor_step=SPD_MINOR_STEP,
    labels=SPD_LABELS,
    zone_yellow=SPD_ZONE_YELLOW,
    zone_red=SPD_ZONE_RED,
    unit_label='km/h',
    label_scale=1,
    filename='gauge_speed.png'
)

# ── RPM 계기 생성 ────────────────────────────────────────────
make_gauge(
    title='rpm',
    val_max=RPM_MAX,
    major_step=RPM_MAJOR_STEP,
    minor_step=RPM_MINOR_STEP,
    labels=RPM_LABELS,
    zone_yellow=RPM_ZONE_YELLOW,
    zone_red=RPM_ZONE_RED,
    unit_label='×1000  rpm',
    label_scale=1000,      # 표시는 0~8, 실제값은 ×1000
    filename='gauge_rpm.png'
)

print()
print("서보 보정 후 ANGLE_ZERO / ANGLE_MAX 수정 → 재실행")
