#include <iostream>
#include <string>
#include <curl/curl.h>
#include <fstream>
#include <nlohmann/json.hpp>

using namespace std;

using json = nlohmann::json;

int WriteCallback(void *contents, int size, int nmemb, string *output)
{
    int total_size = size * nmemb;
    output->append(static_cast<char *>(contents), total_size);
    return total_size;
}

string addPlus(const string input)
{
    string result;
    for (size_t i = 0; i < input.length(); ++i)
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

string getYouTubeVideoId(const string song, const string artist, const string apiKey)
{
    string query = addPlus(song) + '+' + addPlus(artist);
    string api_url = "https://www.googleapis.com/youtube/v3/search?key=" + apiKey + "&q=" + query + "&type=video&videoCategoryId=10";
    cout << query << endl;
    CURL *curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, api_url.c_str());

        string response;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        CURLcode res = curl_easy_perform(curl);

        try
        {
            json root = json::parse(response);

            const json items = root["items"];

            if (!items.empty())
            {
                const string videoId = items[0]["id"]["videoId"];
                return videoId;
            }
        }
        catch (const json::parse_error e)
        {
            cerr << "JSON parse error: " << e.what() << endl;
        }

        curl_easy_cleanup(curl);
    }
    return "";
}

string getSpotifyAccessToken(const string clientID, const string clientSecret)
{
    string token_url = "https://accounts.spotify.com/api/token";
    string postFields = "grant_type=client_credentials";
    postFields += "&client_id=" + clientID;
    postFields += "&client_secret=" + clientSecret;

    CURL *curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, token_url.c_str());
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postFields.c_str());

        string response;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK)
        {
            cout << "Failed to get access token from Spotify." << endl;
            return "";
        }

        try
        {
            json root = json::parse(response);
            const string accessToken = root["access_token"];
            return accessToken;
        }
        catch (const json::parse_error e)
        {
            cerr << "JSON parse error: " << endl;
        }
    }

    return ""; // Return empty string on failure
}

const string credentialsFile = "credentials.json";
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

    credentials[username] = password;

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

    if (credentials.contains(username) && credentials[username] == password)
    {
        cout << "Login successful!" << endl;
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

int main()
{
    loadCredentials();

    bool loggedIn = false;
    while (!loggedIn)
    {
        cout << "1. Create Account\n2. Login\nEnter option: ";
        int choice;
        cin >> choice;

        switch (choice)
        {
        case 1:
            createAccount();
            break;
        case 2:
            loggedIn = login();
            break;
        default:
            cout << "Invalid choice. Try again." << endl;
        }
    }

    if (!loggedIn)
    {
        cout << "Login failed. Exiting..." << endl;
        return 1;
    }

    const string clientID = "13d3ebbdb54b4f2d9c7223f4655048dc";
    const string clientSecret = "1fc57941ab3441d2b0f09472b6925d50";
    string access_token = getSpotifyAccessToken(clientID, clientSecret);
    const string apiKey = "AIzaSyAZLtq61NE2gHMtBDsDR29Ojid4wuRWFEs";

    string songName;
    cout << "Enter the song name: ";
    cin.ignore(); // Clear the buffer
    getline(cin, songName);

    string formattedSongName = addPlus(songName);

    string api_url = "https://api.spotify.com/v1/search?q=" + formattedSongName + "&type=track&limit=5";
    api_url += "&access_token=" + access_token;

    CURL *curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, api_url.c_str());

        string response;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        CURLcode res = curl_easy_perform(curl);

        try
        {
            json root = json::parse(response);

            const json tracks = root["tracks"]["items"];
            if (tracks.empty())
            {
                cout << "No tracks found!" << endl;
            }
            else
            {
                cout << "Top 5 tracks:" << endl;
                for (int i = 0; i < tracks.size(); i++)
                {
                    const json track = tracks[i];
                    const string track_name = track["name"];
                    const string artist_name = track["artists"][0]["name"];
                    cout << i + 1 << ". Track: " << track_name << "\n   Artist: " << artist_name << "\n\n";
                }

                cout << "Enter the number of the track you want to play (1-5): ";
                int choice;
                cin >> choice;

                if (choice >= 1 && choice <= 5)
                {
                    const json chosen_track = tracks[choice - 1];
                    const string chosen_track_name = chosen_track["name"];
                    const string chosen_artist_name = chosen_track["artists"][0]["name"];

                    cout << "Playing track: " << chosen_track_name << " by " << chosen_artist_name << "\n";

                    string videoId = getYouTubeVideoId(chosen_track_name, chosen_artist_name, apiKey);
                    if (!videoId.empty())
                    {
                        bool playVideo = askForVideoPreference();

                        string playCommand;
                        if (playVideo)
                        {
                            playCommand = "mpv https://www.youtube.com/watch?v=" + videoId;
                        }
                        else
                        {
                            playCommand = "mpv --no-video https://www.youtube.com/watch?v=" + videoId;
                        }

                        system(playCommand.c_str());
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
        catch (const json::parse_error e)
        {
            cerr << "JSON parse error: " << endl;
        }

        curl_easy_cleanup(curl);
    }

    return 0;
}
