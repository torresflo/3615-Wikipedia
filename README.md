![GitHub license](https://img.shields.io/github/license/torresflo/3615-Wikipedia.svg)
[![PRs Welcome](https://img.shields.io/badge/PRs-welcome-brightgreen.svg)](http://makeapullrequest.com)
![GitHub contributors](https://img.shields.io/github/contributors/torresflo/3615-Wikipedia.svg)
![GitHub issues](https://img.shields.io/github/issues/torresflo/3615-Wikipedia.svg)

<p align="center">
  <h1 align="center">3615 Wikipedia</h3>

  <p align="center">
    A minimal and simple ESP-32 application to access Wikipedia on Minitel
    <br/>
    Run on a <a href="https://www.tindie.com/products/iodeo/minitel-esp32-dongle/")>Minitel ESP32 Dongle</a>
    <br/>
    Based on the <a href="https://hackaday.io/project/180473-minitel-esp32">Minitel + ESP-32</a> project of <a href="https://github.com/iodeo">Louis H</a>.
    <br />
    <a href="https://github.com/torresflo/3615-Wikipedia/issues">Report a bug or request a feature</a>
  </p>
</p>

## Table of Contents

* [Introduction](#introduction)
* [Getting Started](#getting-started)
  * [Prerequisites and dependencies](#prerequisites-and-dependencies)
  * [Installation](#installation)
* [Usage](#usage)
* [Contributing](#contributing)
* [License](#license)

## Introduction

### What the hell is a Minitel?

The minitel is a french great-uncle of internet that comes back frequently in retro computing projects. This well documented old guy has a peripheral port from which we can access to screen display and keyboard input, so it can basically be used for any terminal application. The semi-graphic mode also makes GUI development possible with 80x72 resolution and 8 grayscale colors.

![Minitel image](https://github.com/torresflo/3615-Wikipedia/blob/main/Images/Minitel.webp)

## Getting Started

### Prerequisites and dependencies

You need a [Minitel ESP32 Dongle](https://www.tindie.com/products/iodeo/minitel-esp32-dongle/) to run this program. 

More information about this project can be found on the [Minitel + ESP-32](https://hackaday.io/project/180473-minitel-esp32) project page.

I am personally using Visual Studio Code for the development, but the simplest way to run the application is by using the Arduino IDE:
1. Install software [Arduino IDE](https://www.arduino.cc/en/software).
2. Add Arduino-ESP32 core to the IDE [as described here](https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html#installing-using-boards-manager).
3. Install the [Minitel1B library](https://github.com/eserandour/Minitel1B_Hard) as follows:
    - Download Minitel1B library as ZIP file.
    - In Arduino IDE, go to Sketch / Include Library / Add .ZIP Library and select your ZIP file.
4. Install the [MinitelToolkit library](https://github.com/torresflo/MinitelToolkit) the same way.
5. Install the [ArduinoJson library](https://arduinojson.org/) with Sketch / Include Library / Manage Libraries.
6. Connect your device using USB-C cable.
7. In Arduino IDE, select the corresponding serial port in Menu Tools/Port.
> [!TIP]
> If you have difficulties with USB driver, check [this sparkfun article](https://learn.sparkfun.com/tutorials/how-to-install-ch340-drivers/).
8. Select ESP32 Dev Module in Tools/Board type/ESP32 Arduino.

Done! You should be ready to compile and deploy the application.

### Installation

Follow the instructions above then clone the repo (`git clone https:://github.com/torresflo/3615-Wikipedia.git`).

> [!IMPORTANT]
> Copy `src/Secrets.h.example` to `src/Secrets.h` and set `WIFI_SSID` and `WIFI_PASSWORD` to configure the Wi-Fi connection. `src/Secrets.h` is gitignored, so your credentials are never committed.

You can now deploy and run `3615-Wikipedia.ino` on your development board.

## Usage

Once the Wi-Fi is connected, browse the English Wikipedia straight from your Minitel keyboard.

**Search.** Type a subject and press `ENVOI`.

**Pick an article.** Wikipedia rarely returns a single match, so you get a list of results. Move with the arrow keys and press `ENVOI` to open one. If nothing matches but a close spelling exists, a *Did you mean?* screen offers it, press `ENVOI` to search the suggestion.

![Wikipedia on Minitel image](https://github.com/torresflo/3615-Wikipedia/blob/main/Images/Wikipedia.jpg)

**Read.** The article is shown page by page, with the current section title and a `section / page` counter.
- `SUITE` - next page
- `RETOUR` - previous page
- `GUIDE` - open the section list
- `SOMMAIRE` - start a new search

**Jump to a section.** From the section list, navigate the tree with the arrow keys (the `Left` and `Right` keys allow to move inside or outside a parent section), press `ENVOI` to read the selected section, or `GUIDE` to go back to the article.

At any point, press `SOMMAIRE` to start over.

## Contributing

Contributions are what make the open source community such an amazing place to be learn, inspire, and create. Any contributions you make are **greatly appreciated**.

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

<!-- LICENSE -->
## License

Distributed under the GNU General Public License v3.0. See `LICENSE` for more information.
