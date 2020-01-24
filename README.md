# Socket Server with Character Driver
This repo contains source code and scripts for AESD assignment 9

- The socket server accepts strings from clients and writes it to a file/character driver depending on the macro.
- The server can handle multiple clients which are managed in threads. Dynamic creation of threads is managed by linked list.
- The server can be started as a deamon (after ensuring that it can bind to the given address). The script aesdsocket-start-stop.sh will start the server as a daemon.
- The data written to the character device is handled by the character driver. The driver supports open, close, read, write, ioctl and llseek system calls.
- The data sent to the character device is managed in a circular buffer whose size can be set using a macro.
- A client can send an ioctl command to the socket server which in turn verifies the command and is executed by the character driver. The values passed into the ioctl command indicates the offset from which the stored data is required by the client. The driver processes the ioctl command and seeks to that offset and returns the data from that offset to the server upon read call.
- Appropriate locking mechanisms are implemented in socket server and character driver to protect shared resources.

Verified communication with Ubuntu client and cross-compiled server running on QEMU image built using Buildroot.

The socket server code is present in [server/aesdsocket.c](https://github.com/MohitRane8/ECEN-5013-Advanced-Embedded-Software-Development-Assignments/blob/MohitRane8_assignment9_submission/server/aesdsocket.c).

The character drive code is present in [aesd-char-driver/aesdchar.c](https://github.com/MohitRane8/ECEN-5013-Advanced-Embedded-Software-Development-Assignments/blob/MohitRane8_assignment9_submission/aesd-char-driver/aesdchar.c)
