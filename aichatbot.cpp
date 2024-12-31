#include <curl/curl.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <nlohmann/json.hpp>
#include <openssl/sha.h>
#include <sstream>
#include <string>
#include <unordered_map>

using namespace std;
using json = nlohmann::json;

string hashPassword(const string &password) {
  unsigned char hash[SHA256_DIGEST_LENGTH];
  SHA256((unsigned char *)password.c_str(), password.size(), hash);
  stringstream ss;
  for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
    ss << hex << setw(2) << setfill('0') << (int)hash[i];
  }
  return ss.str();
}

unordered_map<string, string> loadCredentials() {
  unordered_map<string, string> credentials;
  ifstream file("users.txt");
  string username, password;
  while (file >> username >> password) {
    credentials[username] = password;
  }
  return credentials;
}

void saveCredentials(const unordered_map<string, string> &credentials) {
  ofstream file("users.txt");
  for (const auto &pair : credentials) {
    file << pair.first << " " << pair.second << "\n";
  }
}

bool authenticateUser(const string &username, const string &password,
                      const unordered_map<string, string> &credentials) {
  auto it = credentials.find(username);
  if (it != credentials.end() && it->second == hashPassword(password)) {
    return true;
  }
  return false;
}

size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
  ((string *)userp)->append((char *)contents, size * nmemb);
  return size * nmemb;
}

bool SendRequest(const string &prompt, string &response, const string &apiKey) {
  CURL *curl;
  CURLcode res;

  curl = curl_easy_init();
  if (curl) {
    string url = "https://generativelanguage.googleapis.com/v1beta/models/"
                 "gemini-1.5-flash:generateContent?key=" +
                 apiKey;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 1L);

    curl_easy_setopt(
        curl, CURLOPT_HTTPHEADER,
        curl_slist_append(nullptr, "Content-Type: application/json"));

    json request_body = {
        {"contents",
         json::array({{{"parts", json::array({{{"text", prompt}}})}}})}};

    string request_str = request_body.dump();

    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request_str.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, request_str.length());

    response.clear();
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
      cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << endl;
      return false;
    }

    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

    if (http_code != 200) {
      cerr << "HTTP Error: " << http_code << endl;
      cout << "Response Body: " << response << endl;
      return false;
    }

    curl_easy_cleanup(curl);
    return true;
  } else {
    cerr << "curl_easy_init() failed" << endl;
    return false;
  }
}

int main() {
  unordered_map<string, string> credentials = loadCredentials();

  cout << "\nWelcome to AI Chatbot!\n";
  cout << "1. Login\n2. Register\nChoose an option: ";
  int choice;
  string username, password;
  cin >> choice;
  cin.ignore();

  bool authenticated = false;

  if (choice == 1) {
    cout << "Enter username: ";
    cin >> username;
    cout << "Enter password: ";
    cin >> password;
    cout << endl;

    if (authenticateUser(username, password, credentials)) {
      authenticated = true;
      cout << "Login successful!\n";
    } else {
      cout << "Invalid username or password.\n";
    }
  } else if (choice == 2) {
    cout << "Choose a username: ";
    cin >> username;
    cout << "Choose a password: ";
    cin >> password;
    cout << endl;

    if (credentials.find(username) == credentials.end()) {
      credentials[username] = hashPassword(password);
      saveCredentials(credentials);
      authenticated = true;
      cout << "Registration successful!\n";
    } else {
      cout << "Username already exists.\n";
    }
  } else {
    cout << "Invalid option.\n";
  }

  if (!authenticated)
    return 0;

  // Load chat history
  string history_file = "history_" + username + ".txt";
  ifstream history_in(history_file);
  string line;
  cout << "\nYour previous conversations:\n" << endl;
  while (getline(history_in, line)) {
    cout << line << endl;
  }
  history_in.close();

  string apiKey = "YOUR_API_KEY_HERE";
  string user_input;

  cout << "Start chatting! Type 'exit' to quit.\n";
  ofstream history_out(history_file, ios::app);

  string temp;

  while (true) {
    cout << "You: ";
    cin >> temp;

    if (temp == "exit") {
      break;
    }

    getline(cin, user_input);

    if (user_input.empty()) {
      cout << endl;
      continue;
    }

    history_out << "You: " << user_input << endl;

    string chatbot_response;

    if (SendRequest(user_input, chatbot_response, apiKey)) {
      try {
        json response_json = json::parse(chatbot_response);

        if (response_json.contains("candidates") &&
            !response_json["candidates"].empty()) {
          string generated_text =
              response_json["candidates"][0]["content"]["parts"][0]["text"];
          if (!generated_text.empty()) {
            cout << "Bot: " << generated_text << endl;
            history_out << "Bot: " << generated_text << endl;
          } else {
            cerr << "Error: Generated text is empty or null" << endl;
          }
        } else {
          cerr << "Error: No candidates or invalid response structure" << endl;
        }
      } catch (const exception &e) {
        cerr << "Error parsing JSON response: " << e.what() << endl;
      }
    }
  }

  cout << "Bot: Have a nice day, GoodBye🥳!\n" << endl;

  return 0;
}
