import serial

import pygame
import time
from pygame.locals import *

pygame.init()
screen = pygame.display.set_mode((640, 480))
pygame.display.set_caption("serial capture utility")
screen.fill((159, 182, 205))
centerpos = (screen.get_rect().centerx, screen.get_rect().centery)


def display(str, center=centerpos, fontsize=40):
    font = pygame.font.Font(None, fontsize)
    text = font.render(str, True, (255, 255, 255), (159, 182, 205))
    textRect = text.get_rect()
    textRect.centerx = center[0]
    textRect.centery = center[1]

    screen.blit(text, textRect)
    pygame.display.update()


step = 0
done = False
lines_y = [y for y in range(125, screen.get_height() - 25, 25)]


def percent_deviation(t1, t2):
    a1, a2 = t1
    b1, b2 = t2

    r1 = abs(a1 - b1) / ((a1 + b1) / 2) * 100
    r2 = abs(a2 - b2) / ((a2 + b2) / 2) * 100
    return min(r1, r2)


def main_loop(data, filename, saved_points, n_saved):
    display("Press ESC to exit", center=(centerpos[0], 25))
    display("Press ENTER to save the current value", center=(centerpos[0], 75))

    display("Minimum percent deviation between", center=(centerpos[0] + 100, 150), fontsize=30)
    display("saved and current values:", center=(centerpos[0] + 100, 200), fontsize=30)
    if len(saved_points) > 0:
        display(
            str(round(min(percent_deviation(data, val) for val in saved_points), 2)) + "%",
            center=(centerpos[0] + 100, 250),
            fontsize=30,
        )

    display(
        str(data[0]) + " " + str(data[1]), center=(centerpos[0] - 250, lines_y[n_saved % len(lines_y)]), fontsize=30
    )

    pygame.event.pump()
    keys = pygame.key.get_pressed()

    global done
    if keys[K_ESCAPE]:
        done = True

    elif not len(saved_points) or (
        min(percent_deviation(data, val) for val in saved_points) > 10 and data not in saved_points
    ):
        with open(filename, "a") as f:
            f.write(str(data) + "\n")
        display("Saved: ", center=(centerpos[0], 300), fontsize=30)
        display(str(data[0]) + " " + str(data[1]), center=(centerpos[0] + 50, 300), fontsize=30)
        saved_points.append(data)
        n_saved += 1

    return saved_points, n_saved


with open("APPS_calib.txt", "r") as f:
    saved_points = []
    n_saved = 0
    for line in f:
        if done:
            exit()
        print(line.split("\t")[0], line.split("\t")[1], n_saved)
        data = (int(line.split("\t")[0]), int(line.split("\t")[1]))
        # try:
        #     ser = serial.Serial("/dev/ttyACM0", 9600)
        # except:
        #     print("Error: Could not open serial port")

        saved_points, n_saved = main_loop(data, "APPS_res.txt", saved_points, n_saved)
