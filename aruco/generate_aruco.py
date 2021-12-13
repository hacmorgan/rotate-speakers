#!/usr/bin/env python3


"""
@file

Generate aruco codes for the two speakers.

@author Hamish Morgan
@date   13/12/2021
"""


import cv2
import numpy as np


aruco_dict = cv2.aruco.Dictionary_get(cv2.aruco.DICT_4X4_50)

left = np.zeros((300, 300, 1), dtype="uint8")
right = left.copy()

# print(f"Aruco dict: {aruco_dict}")

cv2.aruco.drawMarker(dictionary=aruco_dict, id=0, sidePixels=300, img=left, borderBits=1)
cv2.aruco.drawMarker(dictionary=aruco_dict, id=1, sidePixels=300, img=right, borderBits=1)

cv2.imwrite("left_aruco.png", left)
cv2.imwrite("right_aruco.png", right)
