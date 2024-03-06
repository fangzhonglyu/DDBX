# DDBX (Distribute Database System for Online Multi-player Games)

A system that aims to provide a low-cost, lightweight backend for online multiplayer games. 
The DDBX system is divided into two separate subsystems: Transient (Latency-sensitive, real-time connection) and Persistent (Security-sensitive, permanent data).

## The Persistent Subsystem
The Persistent subsystem is designed to be secure and scalable with Strong acidity and transaction guarantees with the Google Cloud Spanner and Functions services.

The Google Cloud Function is implemented using Python and currently supports secure login and signup operations. Player Authentication is performed with Bcrypt and JWT.

> Planned for development (In order of priority)
- Inventory System 
- Player Tags/Traits 
- Protections against brute-force attacks
- Inter-player item transactions
- Global player statistics

## The Transient subsystem (PLANNED) 
The Transient subsystem is to be low-latency and lightweight, with dynamic lobbying and support for complex physics simulations with inter-player interactions. It would be based on WebRTC and provide builtin support for Box2d.

A lobby-based (Not centralized) Transient system based on WebRTC is available in the [CUGL-Networking](https://github.com/fangzhonglyu/cugl-networking) repository.

