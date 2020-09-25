#!/bin/sh

# Run attestation server in background
/home/attestation_server/attestation-server &

# Extract redis server MRENCLAVE to file
SCONE_HASH=1 redis-server > /home/attestation_server/mrenclave

# Make MRENCLAVE file readonly to all
chmod 444 /home/attestation_server/mrenclave

# Run redis with provided config file
SCONE_VERSION=1 redis-server /usr/local/etc/redis/redis.conf