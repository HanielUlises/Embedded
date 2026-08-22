#!/usr/bin/env python3
import argparse
import collections

import serial
import matplotlib.pyplot as plt
import matplotlib.animation as animation


def parse_args():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("--port", default="/dev/ttyACM0",
                    help="Serial device for the ST-Link VCP (default: /dev/ttyACM0)")
    p.add_argument("--baud", type=int, default=115200)
    p.add_argument("--window", type=int, default=320,
                    help="Number of samples kept on screen (default: 320, one full signal period)")
    return p.parse_args()


def main():
    args = parse_args()
    ser = serial.Serial(args.port, args.baud, timeout=1)

    data = collections.deque(maxlen=args.window)

    fig, ax = plt.subplots()
    line, = ax.plot([], [])
    ax.set_xlim(0, args.window)
    ax.set_ylim(-1.5, 1.5)
    ax.set_xlabel("sample")
    ax.set_ylabel("amplitude")
    ax.set_title(f"Live signal — {args.port} @ {args.baud} baud")
    ax.grid(True)

    def update(_frame):
        while ser.in_waiting:
            raw = ser.readline().decode(errors="ignore").strip()
            if not raw:
                continue
            try:
                data.append(float(raw))
            except ValueError:
                continue 
        line.set_data(range(len(data)), data)
        if data:
            ax.set_ylim(min(data) - 0.2, max(data) + 0.2)
        return (line,)

    ani = animation.FuncAnimation(fig, update, interval=30, cache_frame_data=False)
    plt.tight_layout()
    plt.show()

    ser.close()


if __name__ == "__main__":
    main()
