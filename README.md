# server-client

Transfer files using TCP sockets in C

### Assumptions 
1. The folder in which server looks for files is the same folder where server.c is located
2. Once the file is transferred, client disconnects while server wait for anotherconnection
3. The max packet size for transferring file is chosen 256 bytes

### Usage
* Change the IP & port in the files
* Make binaries using gcc
* Run server binary & then client binary, & enter the name of the file


### Code flow
* The client accepts filename from the user & sends it to already running server
* The server looks for the file in it's current folder
* If the file doesn't exist, send a **NF** signal to the client for _file not found_
* Else read the file & send it in chunks of 256 bytes,<br>
when the file is finished reading, send a **END** signal to client & disconnect
* Client saves the data until an **END** signal is received
