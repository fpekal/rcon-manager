# Usage

Run `rcon-manager` to show the available servers.  
Run `rcon-manager [server-name]` to connect to chosen server.  

Then just run the commands in the interactive shell!

# Config

The config file has to be provided using the `RCON_SERVERS_CONFIG` environment variable.
Empty lines or lines beginning with `#` are ignored, so you can put comments in there.
Each line is a separate server entry with the following format:  
```
<server-name>:<host>:<port>:<password>
```

Example:
```
# My servers
my-server:127.0.0.1:25575:verylongandsecurepasswordfrfr
my-other-server:127.0.0.1:25576:123

# My friend's servers
shit:123.123.321.69:25575:amogus
```
