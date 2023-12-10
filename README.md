# Spotify Clone with C++ and YouTube APIs

## Project Overview

This project is a Spotify Clone implemented in C++ with integrated YouTube APIs, providing playing capabilities. The application utilizes various libraries for seamless functionality.

## Libraries Needed

Ensure that you have the following libraries installed before running the project:

1. **CURL**
   - [CURL Documentation and Installation](https://curl.haxx.se/)
   
2. **Nlohmann JSON**
   - [Nlohmann JSON GitHub Repository](https://github.com/nlohmann/json)

3. **MPV**
   - [MPV Player](https://mpv.io/)

## Installation Instructions

Follow these steps to set up the project on your local machine:

1. Install CURL:
   - Follow the [CURL documentation](https://curl.haxx.se/docs/install.html) for installation instructions.

2. Install Nlohmann JSON:
   - Clone the [Nlohmann JSON GitHub repository](https://github.com/nlohmann/json) and follow the provided installation instructions.

3. Install MPV:
   - Download and install MPV from the [official website](https://mpv.io/).

4. Clone the Spotify Clone repository:
   ```bash
   git clone https://github.com/aahalani/spotifyclonecpp.git
   cd spotifyclonecpp
   ```

## Getting Started

1. Clone the repository. 
2. Update `clientID` and `clientSecret` with your Spotify application credentials.
3. Update the `apiKey` variable with your YouTube API key.
4. Compile the code using a C++ compiler. While compiling, incude the path for the `nlohmann/json` library.
5. Use the command `g++ -std=c++11 -o res spotify.cpp -lcurl -l/Users/username/Documents/json-develop/include` (Change the path according to the location of the `json` library on your machine and `-std=c++11` is optional)
6. Run the compiled executable.

## Features

- **Account Creation/Login**: Users can create an account and log in using a username/password.
- **Search**: Search for songs using the Spotify API and play them.
- **Play Favorites**: Play a playlist of favorite songs added by the user.
- **YouTube Integration**: Fetches YouTube videos related to searched songs.

## Usage

1. Start the application.
2. Choose between creating an account or logging in.
3. After logging in, choose options to search for songs, play favorites, or log out.

Block Diagram:

![Block Diagram](https://github.com/aahalani/spotifyclonecpp/assets/29179250/0345bc4c-7f42-4de3-b71b-d951cccdc17f.png)


## Credits

- This application utilizes the Spotify and YouTube APIs.
- The `nlohmann/json` library for JSON parsing.
- `MPV` and `CURL`
- Avval and Co.

