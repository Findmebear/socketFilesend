### Run 

- Start by opening the Ubuntu or other Linux terminal and run "make" command in it.
This would generate the executables, object files and place them in the build folder. 

- Run the `fileserver` executable using the command `./build/run/fileserver --hostname 127.0.0.1:5009` where address can be any local address (starting from 127) for the simplicity purposes, and port can be any 4 digit number. The fileserver now listens on socket for any messages that may pop up. 

- Then, choose the file to send over the socket in the folder `files`. Open another terminal without closing the previous one and run the command `./build/run/client --hostname 127.0.0.1:5009 --send files/logo.svg` where the port has to match the one chosen for fileserver in order to establish the connection. The connection is reusable, the fileserver never stops unless you send the stop signal.

- The client executable handles two types of commands: `-request` and `-send`. After the `-send` command the path of the desired file has to be written in order to locate it and send over to the socket. In order to `-request` the file, you need to send it to the socket first, so that the fileserver stores it into the HashMap with `std::string` as key and `File` struct as value. While you need to specify the full path of the file you are sending, you only need to put the file name when you request it. Once it is received, the file is stored in the folder `received`.
Example: `./build/run/client --hostname 127.0.0.1:5009 --request logo.svg`


### Serialization, Encryption and HashMap 

- Every time the file is send to the socket, its name and bytes are stored into the File struct. When you request the file, the filename is stored into Request struct `Request.name`. 

- Once the struct is created, it is serialized into the format described below, the form of bytes. The bytes are then encrypted using the XOR method with the key 42. The functions for serialization and encryption are located in src/libs/lib.cpp. The serialized vector of bytes with an argument unsigned char is sent to the socket. 

- The fileserver accepts the connection, encrypts and deserializes the vector of bytes into either Request or File structs. If it is a File struct - it is going to be stored in the HashMap. If it is a Request, the Request.name would be found in the HashMap keys and the matching value File would be serialized, encrypted and sent over the socket to client. 

- HashMap is built without any external libraries. I created the Node library for each node in the linkedlist. I as well created the linkedlist in linkedlistlib.cpp as a backbone of the HashMap. Then, I created the hashlib that stands for the HashMap functionality, it also handles the cases when the Node already exists in the HashMap. 

### Serialized File

If you have a file called `file.txt` with the contents `Hello`, then the serialzied, unencrypted message should look like this:

```
┌───────────────────────────────────────────────────────────────────────────────┐
│0xae     // map tag                                                            │
│0x01     // 1 kv pair                                                          │
├───────────────────────────────────────────────────────┬───────────┬───────────┤
│0xaa     // string8 tag                                │           │           │
│0x04     // 4 characters                               │ key       │           │
│File     // the string "File"                          │           │           │
├───────────────────────────────────────────────────────┼───────────┤  pair 1   │
│0xae     // the value associated with the key is a map │           │           │
│0x02     // 2 kv pairs                                 │           │           │
├────────────────────────────────┬───────────┬──────────┤           │           │
│0xaa     // string8 tag         │           │          │           │           │
│0x04     // 4 characters        │ key       │          │           │           │
│name     // the string "name"   │           │          │           │           │
├────────────────────────────────┼───────────┤ pair 1   │           │           │
│0xaa     // string8 tag         │           │          │           │           │
│0x08     // 8 characters        │ value     │          │           │           │
│file.txt // the filename        │           │          │           │           │
├────────────────────────────────┼───────────┼──────────┤           │           │
│0xaa     // string8 tag         │           │          │ value     │           │
│0x05     // 5 characters        │ key       │          │           │           │
│bytes    // the string "bytes"  │           │          │           │           │
├────────────────────────────────┼───────────┤ pair 2   │           │           │
│0xac     // array8 tag          │ value     │          │           │           │
│0x05     // 5 elements          │           │          │           │           │
|0xa2     // unsigned byte tag   │           │          │           │           │
│H        // a byte              │           │          │           │           │
|0xa2     // unsigned byte tag   │           │          │           │           │
│e        // a byte              │           │          │           │           │
|0xa2     // unsigned byte tag   │           │          │           │           │
│l        // a byte              │           │          │           │           │
|0xa2     // unsigned byte tag   │           │          │           │           │
│l        // a byte              │           │          │           │           │
|0xa2     // unsigned byte tag   │           │          │           │           │
│o        // a byte              │           │          │           │           │
└────────────────────────────────┴───────────┴──────────┴───────────┴───────────┘           
```

In decimal:
```
[174, 1, 170, 4, 70, 105, 108, 101, 174, 2, 170, 4, 110, 97, 109, 101, 170, 8, 102, 105, 108, 101, 46, 116, 120, 116, 170, 5, 98, 121, 116, 101, 115, 172, 5, 162, 72, 162, 101, 162, 108, 162, 108, 162, 111]
```


### Serialized Request

If you have a file called `file.txt` that you are requesting, then the unencrypted message should look like this:

```
┌───────────────────────────────────────────────────────────────────────────────┐
│0xae     // map tag                                                            │
│0x01     // 1 kv pair                                                          │
├───────────────────────────────────────────────────────┬───────────┬───────────┤
│0xaa     // string8 tag                                │           │           │
│0x07     // 7 characters                               │ key       │           │
│Request  // the string "Request"                       │           │           │
├───────────────────────────────────────────────────────┼───────────┤  pair 1   │
│0xae     // the value associated with the key is a map │           │           │
│0x01     // 1 kv pair                                  │           │           │
├──────────────────────────────┬───────────┬────────────┤           │           │
│0xaa     // string8 tag       │           │            │           │           │
│0x04     // 4 characters      │ key       │            │ value     │           │
│name     // the string "name" │           │            │           │           │
├──────────────────────────────┼───────────┤ pair 1     │           │           │
│0xaa     // string8 tag       │           │            │           │           │
│0x08     // 8 characters      │ value     │            │           │           │
│file.txt // the filename      │           │            │           │           │
└──────────────────────────────┴───────────┴────────────┴───────────┴───────────┘           
```

In decimal:
```
[174, 1, 170, 7, 82, 101, 113, 117, 101, 115, 116, 174, 1, 170, 4, 110, 97, 109, 101, 170, 8, 102, 105, 108, 101, 46, 116, 120, 116]
```

## Works Cited

[0]: https://studentaffairs.lehigh.edu/content/code-conduct
[1]: https://www.markdownguide.org/basic-syntax#reference-style-links
[2]: http://crasseux.com/books/ctutorial/Building-a-library.html#Building%20a%20library
[3]: https://support.zoom.us/hc/en-us/articles/201362473-Local-recording
[4]: http://gitlab.cse.lehigh.edu/arm320-cse109/homeworks/homework-8
[5]: https://www.tutorialspoint.com/unix_sockets/socket_quick_guide.htm
[6]: https://www.technical-recipes.com/2014/getting-started-with-client-server-applications-in-c/
[7]: http://gitlab.cse.lehigh.edu/cse109/spring-2022/assignments/homework-5
[8]: https://www.csl.mtu.edu/cs4411.ck/www/NOTES/process/shm/example-1.html
