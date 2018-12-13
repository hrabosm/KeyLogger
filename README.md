# KeyLogger
This is basic KeyLogger that logs, time and date, focused window and keyboard input. It sends data each ~30 minutes to server client and deletes data which it already sent. If client can't connect to server it waits another ~30 minutes and then tries again. Feel free to tweak/change my program how you like!

Keylogger is optimized for Czech Keyboard so if you are using different keyboard layout edit the keys in special keys section.
## Disclaimer!
Using this program for purpose of **spying on other people without their concern is violation of law** (at least in most countries)! **I am not responsible for any legal charges caused by using my program!** This program is meant for studying purposes!
## Setup
1. (OPTIONAL) Change ip adress and port in Client and port in Server to your desire
2. Compile client and server using > ```g++ -g -o NAME.EXE main.cpp -lpthread -lws2_32```
3. Run client and server

## TODO List
- [x] ~~Sockets to transfer data from client server~~
- [ ] Reorganize my code and add more comments
- [ ] Self deleting function that uninstalls the program from PC
