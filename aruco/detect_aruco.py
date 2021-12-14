#!/usr/bin/env python3


"""
@file

Detect aruco codes on speakers

Borrows heavily from Adrian Rosebrock's excellent PyImageSearch blog post:
https://www.pyimagesearch.com/2020/12/21/detecting-aruco-markers-with-opencv-and-python/

@author Hamish Morgan
@date   13/12/2021
"""


import sys
import time

import cv2
import imutils
import numpy as np

from imutils.video import VideoStream


"""
Type aliases
"""
Image = np.ndarray


def adjust_brightness_contrast(image: Image, alpha: float, beta: float) -> Image:
    """
    Adjust the brightness and contrast of an image.

    This operation is defined pixel-wise as:

        value_after = uint8(abs(alpha * value_before + beta))

    @param[in] image Image to be modified, as numpy array.
    @param[in] alpha Contrast
    @param[in] beta Brightness
    """
    return cv2.convertScaleAbs(src=image, alpha=alpha, beta=beta)


def enhance_image(image: Image) -> Image:
    """
    Apply enhancements to an image to make it easier to detect aruco codes.

    @param[in] image Image to be enhanced
    @return Image with enhancements applied
    """
    return adjust_brightness_contrast(image, alpha=1.8, beta=1.8)


def main(view: bool = False) -> int:
    # Tells opencv which aruco codes we are using
    aruco_dict = cv2.aruco.Dictionary_get(cv2.aruco.DICT_4X4_50)
    aruco_params = cv2.aruco.DetectorParameters_create()

    # Open the webcam for video stream and give it a sec
    vs = VideoStream(src=0).start()
    time.sleep(2.0)

    # loop over the frames from the video stream
    while True:
        # grab the frame from the threaded video stream and resize it
        # to have a maximum width of 1000 pixels
        frame = vs.read()
        frame = imutils.resize(frame, width=1000)
        frame = enhance_image(frame)

        # detect ArUco markers in the input frame
        corners, ids, rejected = cv2.aruco.detectMarkers(
            image=frame, dictionary=aruco_dict, parameters=aruco_params
        )

        # verify *at least* one ArUco marker was detected
        if len(corners) > 0:
            # flatten the ArUco IDs list
            ids = ids.flatten()
            # loop over the detected ArUCo corners
            for markerCorner, markerID in zip(corners, ids):
                # extract the marker corners (which are always returned
                # in top-left, top-right, bottom-right, and bottom-left
                # order)
                corners = markerCorner.reshape((4, 2))
                top_left, top_right, bottom_right, bottom_left = corners
                # convert each of the (x, y)-coordinate pairs to integers
                top_right, bottom_right, bottom_left, top_left = (
                    (int(x), int(y))
                    for x, y in (top_right, bottom_right, bottom_left, top_left)
                )

                # draw the bounding box of the ArUCo detection
                cv2.line(frame, top_left, top_right, (0, 255, 0), 2)
                cv2.line(frame, top_right, bottom_right, (0, 255, 0), 2)
                cv2.line(frame, bottom_right, bottom_left, (0, 255, 0), 2)
                cv2.line(frame, bottom_left, top_left, (0, 255, 0), 2)

                # compute and draw the center (x, y)-coordinates of the
                # ArUco marker
                cX = int((top_left[0] + bottom_right[0]) / 2.0)
                cY = int((top_left[1] + bottom_right[1]) / 2.0)
                cv2.circle(frame, (cX, cY), 4, (0, 0, 255), -1)

                # draw the ArUco marker ID on the frame
                cv2.putText(
                    frame,
                    str(markerID),
                    (top_left[0], top_left[1] - 15),
                    cv2.FONT_HERSHEY_SIMPLEX,
                    0.5,
                    (0, 255, 0),
                    2,
                )

        # show the output frame
        cv2.imshow("Frame", frame)
        key = cv2.waitKey(1) & 0xFF

        # if the `q` key was pressed, break from the loop
        if key == ord("q"):
            break

    # do a bit of cleanup
    cv2.destroyAllWindows()
    vs.stop()


if __name__ == "__main__":
    sys.exit(main())
