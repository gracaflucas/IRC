# 💬 IRC Server (C++98)

A fully functional **IRC (Internet Relay Chat) server** built in **C++98**.  
This project follows the RFC 1459 specification and implements the core IRC commands, client handling, and channel management.

---

## 🚀 Overview

This IRC server allows multiple clients to connect and communicate in real time using the IRC protocol.  
It supports user authentication, channel creation, messaging between users or channels, and basic moderation commands.

You can test it using an IRC client (HexChat) or `nc` (netcat).

---

## 🧠 How It Works

At its core, the server:
1. **Creates a TCP socket** bound to a specific port (IPv4 only).
2. **Listens for incoming connections** and accepts new clients.
3. Uses **`poll()`** to handle multiple clients **concurrently without threads**.
4. **Parses incoming messages** according to the IRC protocol.
5. **Sends appropriate responses** (JOIN, PART, PRIVMSG, etc.) to other clients or channels.
6. Cleans up properly on disconnect or Ctrl+C.

### 🧩 Simplified architecture
