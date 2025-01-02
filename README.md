# AI Chatbot

#### Video Demo: <https://youtu.be/96rdAvVNIuY>

## Project Overview

This project is a **Command-Line Interface (CLI) AI Chatbot** implemented in C++. It integrates the Gemini API for natural language generation and supports user authentication with registration and login features. Each user's chat history is stored and retrieved during subsequent logins, providing a personalized experience.

---

## Features

- **User Authentication**:
  - Register with a unique username and password (passwords are hashed using SHA256 for security).
  - Log in with your credentials to access your chat history.
- **Chat History**:
  - User-specific chat histories are stored in text files (e.g., `history_<username>.txt`) and persist across sessions.
  - View previous conversations upon login.
- **AI-Powered Conversations**:
  - Uses the Gemini API to generate intelligent and context-aware responses.
- **Robust Error Handling**:
  - Handles invalid inputs, API errors, and empty requests gracefully.

---

## Prerequisites

- C++ Compiler with support for C++11 or later
- OpenSSL Library (for password hashing)
- `libcurl` Library (for HTTP requests)
- `nlohmann/json` Library (for JSON processing)
- Gemini API Key

---

## Setup Instructions

### 1. Clone the Repository

```bash
git clone https://github.com/HtetAungKhant23/AIChatBot.git
cd AIChatBot
```

### 2. Install Dependencies

Make sure the following libraries are installed on your system:

- OpenSSL
- `libcurl`
- `nlohmann/json`

For Debian-based systems:

```bash
sudo apt-get install libssl-dev libcurl4-openssl-dev nlohmann-json3-dev
```

### 3. Set Up Your API Key

Replace the placeholder API key in the code with your actual Gemini API key:

```bash
string apiKey = "YOUR_API_KEY_HERE";
```

### 4. Compile the Project

Use g++ to compile the source code:

```bash
g++ aichatbot.cpp -o chatbot -lcurl -std=c++17 -lssl -lcrypto -I/usr/include/openssl
```

### 5. Run the Chatbot

Execute the program:

```bash
./chatbot
```

---

## How to Use

### Register

1. Start the program and choose the **Register** option.
2. Enter a unique username and password.
3. Your credentials will be securely stored.

### Login

1. Choose the **Login** option and enter your username and password.
2. View your previous chat history (if any).

### Start Chatting

- Enter your messages, and the chatbot will respond using the Gemini API.
- Type `exit` to end the session.

---

## Error Handling

- **Invalid Credentials**: Displays a message for incorrect username or password.
- **API Errors**: Prints detailed error messages if the API fails to respond.
