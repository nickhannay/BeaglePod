# Beaglepod

The Beaglepod is a C project that enables mp3 playback, with local and remote control options. It utilizes the ZEN cape's components such as joystick, and potentiometer in addition to the LCD screen for user interaction.

## Features

- Music Playback: Beaglepod supports playing music in various formats, providing high-quality audio output.
- Local and Remote Control: Users can control Beaglepod locally using the joystick and potentiometer, as well as remotely through a web interface.
- LCD Menu: Beaglepod displays a menu on the LCD for user interaction, allowing selection of songs, changing settings, connecting to Bluetooth devices, and other functionalities.
- Song Information Display: The LCD shows the name, artist, and runtime of the current song playing.
- Volume Control: Users can adjust the volume using the potentiometer.
- Web Interface: Beaglepod includes a web interface for adding and deleting songs to interact with the music player through a user-friendly interface.

## Building the Project

To build the Beaglepod project, follow these steps:

1. Clone the repository to your Beaglebone Green board.
2. Navigate to the project directory in your terminal.
3. Run the command `make all` to build the entire project and create an executable called `beaglePod` in the shared folder.

## Running the Project

To run Beaglepod, follow these steps:

1. After building the project, navigate to the `web-server` directory in your terminal.
2. Run the command `npm start dev` to start the web interface locally.
3. Access the web interface from the host device's web browser by navigating to `http://localhost:3000`.

## Cleaning the Project

To clean the Beaglepod project and remove all build artifacts and the executable, follow these steps:

1. Navigate to the project directory in your terminal.
2. Run the command `make clean` to remove all build artifacts and the executable.

## Contributing

If you would like to contribute to the Beaglepod project, please follow these steps:

1. Fork the repository on GitHub.
2. Create a new branch for your changes.
3. Make your changes and thoroughly test them.
4. Submit a pull request to the main repository, explaining the changes and their benefits to the project.

## License

Beaglepod project is open-source and released under the [MIT License](LICENSE). You are free to use, modify, and distribute the code for personal and commercial purposes, but please attribute the original authors and provide a link to the original repository in your derivative works.
