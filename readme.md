Oberin is an online fantasy role-playing game with a friendly virtual community. Created by David M. Bourg and Glenn Seemann.

# Instructions
In progress. I'm current analyzing the requirements to build and run the Oberin server and client. It uses CodeWarrior with a PowerPC Mac OS. First the goal is to get it to build and work. Ariki uses Snow Leopard on VMWare with CodeWarrior 10.

# Goal
Transition Oberin from CodeWarrior to more recent build tools. Upgrade the platform from older Mac Power PC to the latest platforms.

# Planned Roadmap
1. Get Oberin built and running in SheepShaver or QEMU using CodeWarrior.
2. Document how to run and the code and assets.
3. Experiment with game asset conversions. Organize and preserve data.
4. Choose between incrementally upgrading the build process from CodeWarrior to XCode and up the platforms or attempting straight transitioning the code to a completely new up-to-date build toolchain.
5. Execute choice.

# Setup
Folders required in the main server folder: Accounts, AKA IP, AKA Mac, CC Logs, Creature Settings, GM Logs, Oberin Containers, Oberin Halls, Oberin Logs, Overin Signs.

# Future Improvements
 - Hash Character Passwords
 - Client version check
 - Enforce all client limits on server
 - Crypto for all sent packets with a key set by the server admin
