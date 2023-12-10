#include <iostream>
#include <string>
#include <fstream>
#include <nlohmann/json.hpp>
#include <vector>
using namespace std;

using json = nlohmann::json;



class Song {
private:
    string name;
    string artist;
    string videoId;

public:
    Song() {}
    Song(string name, string artist, string videoId) : name(name), artist(artist), videoId(videoId) {}

    string getName() { return name; }
    string getArtist() { return artist; }
    string getVideoId() { return videoId; }

    void setName(string name) { this->name = name; }
    void setArtist(string artist) { this->artist = artist; }
    void setVideoId(string videoId) { this->videoId = videoId; }

    void playSong(bool playVideo) {
        string playCommand;
        if (playVideo) {
            playCommand = "mpv https://www.youtube.com/watch?v=" + videoId;
        } else {
            playCommand = "mpv --no-video https://www.youtube.com/watch?v=" + videoId;
        }

        cout << endl << endl;
        system(playCommand.c_str());
        cout << endl << endl;
    }
};

class Playlist {
private:
    vector<Song> songs;

public:
    vector<Song> getSongs() { return songs; }

    void setSongs(vector<Song> songs) { this->songs = songs; }

    void addToPlaylist(Song song) {
        songs.push_back(song);
    }

    void playPlaylist(bool playVideo) {
        for (Song song : songs) {
            song.playSong(playVideo);
        }
    }
};




string user;
vector<string> fav_list;
Playlist fav;



string addPlus(string input)
{
    string result;
    for (int i = 0; i < input.length(); ++i)
    {
        if (input[i] == ' ')
        {
            result += '+';
        }
        else
        {
            result += input[i];
        }
    }
    return result;
}

string getYouTubeVideoId(string song, string artist, string apiKey)
{
    string query = addPlus(song) + '+' + addPlus(artist);
    string command = "curl -X GET 'https://www.googleapis.com/youtube/v3/search?key=" + apiKey + "&q=" + query + "&type=video&videoCategoryId=10' > output.json"; // 10 is the video category id for music
    cout << endl
         << endl;
    int exitCode = system(command.c_str());
    cout << endl
         << endl;
    if (exitCode == 0)
    {
        ifstream inputFile("output.json");
        if (inputFile.is_open())
        {
            json root;
            inputFile >> root;

            json items = root["items"];
            if (!items.empty())
            {
                string videoId = items[0]["id"]["videoId"];
                inputFile.close();
                return videoId;
            }
            else
            {
                cout << "No videos found!" << endl;
            }

            inputFile.close();
        }
        else
        {
            cout << "Failed to open output.json" << endl;
        }
    }
    else
    {
        cout << "Failed to execute cURL command." << endl;
    }

    return "";
}

string getSpotifyAccessToken(string clientID, string clientSecret)
{
    string command = "curl -X POST \"https://accounts.spotify.com/api/token\" -H \"Content-Type: application/x-www-form-urlencoded\" -d \"grant_type=client_credentials&client_id=" + clientID + "&client_secret=" + clientSecret + "\" > output.json";
    cout << endl
         << endl; // because of the download logs
    int exitCode = system(command.c_str());
    cout << endl
         << endl;
    if (exitCode == 0)
    {
        ifstream inputFile("output.json");
        if (inputFile.is_open())
        {
            json jsonData;
            inputFile >> jsonData;
            inputFile.close();

            if (jsonData.contains("access_token"))
            {
                return jsonData["access_token"];
            }
        }
    }
    else
    {
        cout << "Failed to execute cURL command." << endl;
    }

    return "";
}
string credentialsFile = "credentials3.json";
json credentials;

void loadCredentials()
{
    ifstream file(credentialsFile);
    if (file.is_open())
    {
        file >> credentials;
        file.close();
    }
}

void saveCredentials()
{
    ofstream file(credentialsFile);
    if (file.is_open())
    {
        file << credentials;
        file.close();
    }
}

void createAccount()
{
    string username, password;
    cout << "Enter new username: ";
    cin >> username;
    cout << "Enter password: ";
    cin >> password;

    credentials[username]["pass"] = password;
    vector<string> fav_new;
    credentials[username]["fav"] = fav_new;
    saveCredentials();

    cout << "Account created successfully!" << endl;
}

bool login()
{
    string username, password;
    cout << "Enter username: ";
    cin >> username;
    cout << "Enter password: ";
    cin >> password;

    if (credentials.contains(username) && credentials[username]["pass"] == password)
    {
        cout << "Login successful!" << endl;
        fav_list = credentials[username]["fav"];
        vector<Song> song_list;
        for (int i=0;i<fav_list.size();i++){
              Song temp;
              temp.setVideoId(fav_list[i]);
              song_list.push_back(temp);
        }
        fav.setSongs(song_list);
        user = username;
        return true;
    }
    else
    {
        cout << "Invalid username or password. Please try again." << endl;
        return false;
    }
}

bool askForVideoPreference()
{
    char choice;
    cout << "Do you want to play the video? (y/n): ";
    cin >> choice;
    return (choice == 'y' || choice == 'Y');
}






void search(string access_token, string apiKey)
{
    string songName;
    cout << "Enter the song name: ";
    cin.ignore(); // Clear the buffer GPT
    getline(cin, songName);

    string formattedSongName = addPlus(songName);

    string command = "curl -X GET 'https://api.spotify.com/v1/search?q=" + formattedSongName + "&type=track&limit=5&access_token=" + access_token + "' > output.json";
    cout << endl
         << endl;
    int exitCode = system(command.c_str());
    cout << endl
         << endl;
    if (exitCode == 0)
    {
        ifstream inputFile("output.json");
        if (inputFile.is_open())
        {
            json root;
            inputFile >> root;

            json tracks = root["tracks"]["items"];
            if (tracks.empty())
            {
                cout << "No tracks found!" << endl;
            }
            else
            {
                cout << "Top 5 tracks:" << endl;
                for (int i = 0; i < tracks.size(); i++)
                {
                    json track = tracks[i];
                    string track_name = track["name"];
                    string artist_name = track["artists"][0]["name"];
                    cout << i + 1 << ". Track: " << track_name << "\n   Artist: " << artist_name << "\n\n";
                }

                cout << "Enter the number of the track you want to play (1-5): ";
                int choice;
                cin >> choice;

                if (choice >= 1 && choice <= 5)
                {
                    json chosen_track = tracks[choice - 1];
                    string chosen_track_name = chosen_track["name"];
                    string chosen_artist_name = chosen_track["artists"][0]["name"];
                    cout << "Playing track: " << chosen_track_name << " by " << chosen_artist_name << "\n";

                    string videoId = getYouTubeVideoId(chosen_track_name, chosen_artist_name, apiKey);
                    if (!videoId.empty())
                    {
                       Song s1(chosen_track_name,chosen_artist_name,videoId);
                        bool playVideo = askForVideoPreference();
                        s1.playSong(playVideo);
                        

                        cout << "Add to Favorites (y/n)" << endl;
                        char choice;
                        cin >> choice;
                        if (choice == 'y')
                        {
                            fav.addToPlaylist(s1);
                            fav_list.push_back(videoId);
                            credentials[user]["fav"] = fav_list;
                            saveCredentials();
                        }

                        else
                        {
                            cout << "Could not find the song on YouTube." << endl;
                        }
                    }
                    else
                    {
                        cout << "Invalid choice! Please select a number between 1 and 5." << endl;
                    }
                }
            }

            inputFile.close();
        }
        else
        {
            cout << "Internal Server Error! 500 Error!" << endl;
        }
    }
    else
    {
        cout << "Failed to execute cURL command." << endl;
    }
}

int main()
{
    loadCredentials();

    bool loggedIn = false;
    while (!loggedIn)
    {
        cout << "1. Create Account\n2. Login\nEnter option: " << endl;
        int choice;
        while (!(cin >> choice))
        {
            cout << "Invalid choice. Try again." << endl;
            cin.clear();             // clear the error flag on cin so that future I/O operations will work correctly after invalid input has been entered and ignored using cin.ignore() below (cin will not allow further I/O operations if its error flag is set)
            cin.ignore(10000, '\n'); // ignore bad input up to 10000 characters or the newline character '\n', whichever comes first
        }

        if (choice == 1)
        {
            createAccount();
        }
        else if (choice == 2)
        {
            loggedIn = login();
        }
        else
        {
            cout << "Invalid choice. Try again." << endl;
        }
    }

    string clientID = "13d3ebbdb54b4f2d9c7223f4655048dc";                // Spotify
    string clientSecret = "1fc57941ab3441d2b0f09472b6925d50";            // Spotify
    string access_token = getSpotifyAccessToken(clientID, clientSecret); // Spotify
    string apiKey = "AIzaSyAZLtq61NE2gHMtBDsDR29Ojid4wuRWFEs";           // YouTube

    string choices = "1. Search \n2. Play Favorites\n3. Logout\n";
    int ch = 0;
    
    while (ch != 3)
    {
        cout << choices;
          while (!(cin >> ch))
        {
            cout << "Invalid choice. Try again." << endl;
            cin.clear();             // clear the error flag on cin so that future I/O operations will work correctly after invalid input has been entered and ignored using cin.ignore() below (cin will not allow further I/O operations if its error flag is set)
            cin.ignore(10000, '\n'); // ignore bad input up to 10000 characters or the newline character '\n', whichever comes first
        }
        if (ch == 1)
        {
            search(access_token, apiKey);
        }
        else if (ch == 2)
        {  
            bool playvideo = askForVideoPreference();
            fav.playPlaylist(playvideo);
        }
        else if (ch == 3)
        {
            cout << "Logging out\n";
        }
        else
        {
            cout << "Invalid choice. Try again\n";
        }
    }

    return 0;
}
