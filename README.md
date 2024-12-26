# ft_irc - Internet Relay Chat Server

## Summary

The objective of this project is to reproduce the functionality of an IRC (Internet Relay Chat) server using C++98. You will be able to test your server by using a real IRC client, here irssi, to connect to it.

## Introduction

Internet Relay Chat (IRC) is a text-based communication protocol on the Internet. It is used for instant communication, mainly in the form of group discussions via chat channels, but also for direct communication between two people.

IRC clients connect to IRC servers to access channels. IRC servers can be interconnected to create larger networks.

## General Guidelines

You must develop an IRC server in C++98 with the following features:

- Handle multiple clients simultaneously without blocking.
- Use `poll()` (or equivalent) for non-blocking input/output operations.
- The server must support:
  - User authentication.
  - Setting a nickname and a username.
  - Joining a channel and sending/receiving private messages.
  - Broadcasting messages in a channel to all users in that channel.
  - Managing operators and basic users with the following IRC commands:
    - `KICK`: Eject a client from the channel.
    - `INVITE`: Invite a client to the channel.
    - `TOPIC`: Modify or display the channel topic.
    - `MODE`: Change the channel mode (invite-only, channel key, operator privilege, etc.).

## Usage

### Prerequisites

- A compiler compatible with C++98.
- An IRC client (irssi is used as the reference client).

### Compilation

To compile the project, run the following command in the terminal:

```sh
make
```

### Starting the Server

To start the IRC server, use the following command:
```sh
./ircserv <port> <password>
```
- **`<port>`**: The port number on which your server will accept incoming connections.
- **`<password>`**: The password required to authenticate with your IRC server.

### Connecting with irssi

To connect to your server with irssi, open a terminal and start irssi with the following command:
```sh
irssi
```
Once you are in the irssi client, connect to your server with the command:

```sh
/connect localhost <port> <password>
```