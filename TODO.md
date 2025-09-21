# TODO

## matt-daemon

- Test matt-daemon with multiple server launch [DONE]
  - Should log
  - To fix this:
    - Store the PID of the daemon in a file
    - When a new daemon tries to spawn, check that file, and send a signal to the running daemon
    - Also check if it's still alive

- matt-daemon should exit when sending "quit" """command"""

## ft_shield

- password
  - Specified in Makefile
    - Should be hashed
    - Passed as -D param
- DaemonServer
  - Should check password on connection
  - client timeout?
