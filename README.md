# Tomeo - Video Player for Sports Enthusiasts

Tomeo is a modern video player designed specifically for sports enthusiasts. It allows users to browse, play, and manage video content with a sleek and intuitive user interface. The application supports features like video playback, thumbnail previews, full-screen mode, and more.

---

## Features

- **Video Playback**: Play videos in high quality with smooth controls.
- **Thumbnail Previews**: Browse videos with thumbnail previews for quick selection.
- **Full-Screen Mode**: Watch videos in full-screen mode for an immersive experience.
- **Customizable Controls**: Control playback with buttons like play/pause, rewind, fast-forward, and volume adjustment.
- **Responsive Layout**: Automatically adjusts the layout to fit different screen sizes.
- **Interactive Buttons**: Like, favorite, and reward videos with interactive buttons.
- **Dynamic UI**: Hide or show the left panel for a cleaner interface.

---

## Prerequisites

Before running the project, ensure you have the following installed:

- Qt Framework: Version 5.15.2.
- C++ Compiler: Compatible with C++11 or later.
- Multimedia Libraries: Required for video playback (Qt Multimedia).

---

## Project Structure

user-interface/
├── icon/               # Images and icons used in the project
├── videos/             # videos
├── main.cpp            # Main entry point of the application
├── the_button.h        # Button class for video thumbnails
├── the_player.h        # Media player class
├── the_button.cpp      # Implementation of the button class
├── the_player.cpp      # Implementation of the media player class
├── resources.qrc       # Resource files (e.g., icons, thumbnails)
├── README.md           # Project documentation
└── the.pro             # configuration file

---

## Installation

1. **Clone the Repository**:
   ```bash
   git clone git@github.com:mengxin114/user-interface.git
   cd user-interface