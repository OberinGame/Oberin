Oberin is an online fantasy role-playing game with a friendly virtual community. Created by David M. Bourg and Glenn Seemann.

# Instructions
Follow the documentation in the documentation repository for setting up and compiling Oberin.

# Goal
Transition Oberin from CodeWarrior to more recent build tools. Upgrade the platform from older Mac Power PC to the latest platforms.

# Planned Roadmap
1. ~~Get Oberin built and running in SheepShaver or QEMU or VMWare using CodeWarrior.~~ Completed
2. ~~Document how to run and the code and assets.~~ Completed
3. Experiment with game asset conversions. Organize and preserve data.
4. Make QoL and maintenance improvements.
5. Choose between incrementally upgrading the build process from CodeWarrior to XCode and up the platforms or attempting straight transitioning the code to a completely new up-to-date build toolchain.
6. Execute choice.

# Setup
Folders required in the main server folder: Accounts, AKA IP, AKA Mac, CC Logs, Creature Settings, GM Logs, Oberin Containers, Oberin Halls, Oberin Logs, Overin Signs.

Check the Oberin documentation repository for information on set up.

# Future Improvements
 - Hash Character Passwords
 - Client version check
 - Enforce all client limits on server
 - Crypto for all sent packets with a key set by the server admin
