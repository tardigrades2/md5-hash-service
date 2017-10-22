# C program MD5 service

This is a C UNIX network programing that provide a server service to hash file
and send back to client

## How it work
input.txt(or filename) --(send to server)--> receive.txt --(hash with md5)--> output.txt
--(send back to client)--> final.txt

## How to run
If want run client in same computer, set SERVER_ADDR in client.c to 127.0.0.1,
otherwise set SERVER_ADDR to server address.

### With server:

Compile md5.c(use MD5 function to hash receive.txt file, result is output.txt file)
```
$ gcc -o md5.out md5.c -lcrypto -lssl -lz
```

Next, compile and run server.c

```
$ gcc -o s.out server.c && ./s.out
```

### With client:
Compile and run client.c

```
$ gcc -o c.out client.c
$ ./c.out
or
$ ./c.out filename
```
