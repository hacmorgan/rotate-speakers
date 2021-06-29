#!/usr/bin/env python3
from distutils.core import setup

setup(
    name = "rotate-speakers-receiver",
    version = "0.0.1",
    author = "Hamish Morgan",
    author_email = "ham430@gmail.com",
    description = "Receive messages over TCP and send commands to arduino over serial",
    scripts = ["rotate-speakers-receiver"],
    install_requires = ["pyserial"]
)
