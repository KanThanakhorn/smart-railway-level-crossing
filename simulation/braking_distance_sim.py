#!/usr/bin/env python3
"""
braking_distance_sim.py
Discrete Event Simulation: Sensor Placement Distance (D_sensor)
Project: Smart Railway Level Crossing, KMUTNB 010123119

Formula:
    D_sensor = v_train * t_total_prep + d_reaction + d_braking + d_margin

Where:
    t_total_prep = t_yellow + t_all_red + t_gate_lower + t_scan
    d_reaction   = v_train * (t_sys_latency + t_driver_reaction)
    d_braking    = v_train^2 / (2 * a_brake)
    d_margin     = safety buffer
"""

# ── Parameters ─────────────────────────────────────────────────────────────
V_TRAIN_KMH         = 60.0    # Train speed (km/h)
A_BRAKE             = 1.0     # Emergency deceleration (m/s²)
T_YELLOW_S          = 3.0     # Road yellow light duration (s)
T_ALL_RED_S         = 2.0     # All-red clearance (s)
T_GATE_LOWER_S      = 6.5     # Gate closing time (s)
T_SCAN_S            = 1.5     # Area scan confirmation (s)
T_SYS_LATENCY_S     = 0.1     # System processing latency (s)
T_DRIVER_REACTION_S = 2.9     # Train driver reaction (s)
D_MARGIN_M          = 50.0    # Safety margin (m)

# ── Conversions ─────────────────────────────────────────────────────────────
v = V_TRAIN_KMH / 3.6          # m/s

# ── Calculations ────────────────────────────────────────────────────────────
t_total_prep = T_YELLOW_S + T_ALL_RED_S + T_GATE_LOWER_S + T_SCAN_S
d_prep       = v * t_total_prep
d_reaction   = v * (T_SYS_LATENCY_S + T_DRIVER_REACTION_S)
d_braking    = (v ** 2) / (2 * A_BRAKE)
D_sensor     = d_prep + d_reaction + d_braking + D_MARGIN_M

advance_warning_time = D_sensor / v   # seconds of warning before train arrives

# ── Results ─────────────────────────────────────────────────────────────────
print("=" * 55)
print(" Smart Railway — Sensor Distance Simulation")
print("=" * 55)
print(f"  Train speed           : {V_TRAIN_KMH:.1f} km/h ({v:.4f} m/s)")
print(f"  Emergency decel       : {A_BRAKE:.1f} m/s²")
print()
print(f"  t_total_prep          : {t_total_prep:.1f} s")
print(f"  d_prep (v × t_prep)   : {d_prep:.2f} m")
print(f"  d_reaction            : {d_reaction:.2f} m")
print(f"  d_braking             : {d_braking:.2f} m")
print(f"  d_margin              : {D_MARGIN_M:.1f} m")
print(f"  ─────────────────────────────────────────")
print(f"  D_sensor (total)      : {D_sensor:.2f} m  → use ≥ 460 m")
print()
print(f"  Advance warning time  : {advance_warning_time:.1f} s  (≥ 30 s required ✓)" 
      if advance_warning_time >= 30 else
      f"  Advance warning time  : {advance_warning_time:.1f} s  (< 30 s — FAIL ✗)")
print("=" * 55)

# ── Sweep over speed range ───────────────────────────────────────────────────
print("\n  Speed Sweep (50–80 km/h):")
print(f"  {'Speed':>8} | {'D_sensor':>10} | {'Warn Time':>10}")
print(f"  {'-'*8}-+-{'-'*10}-+-{'-'*10}")
for spd in [50, 55, 60, 65, 70, 80]:
    vv = spd / 3.6
    dp = vv * t_total_prep
    dr = vv * (T_SYS_LATENCY_S + T_DRIVER_REACTION_S)
    db = (vv ** 2) / (2 * A_BRAKE)
    ds = dp + dr + db + D_MARGIN_M
    wt = ds / vv
    ok = "✓" if wt >= 30 else "✗"
    print(f"  {spd:>5} km/h | {ds:>8.2f} m | {wt:>7.1f} s {ok}")
