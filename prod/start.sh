#!/bin/sh

# MRENCLAVE file will be fecthed from CAS
# Extract attestation server MRENCLAVE to file
# SCONE_HASH=1 /home/attestation_server/attestation-server

# Make MRENCLAVE file readonly to all
# chmod 444 /home/attestation_server/mrenclave

# Run attestation server in background
SCONE_VERSION=1 SCONE_LOG=7 SCONE_CONFIG_ID=secure-redis-container/attestation_server /home/attestation_server/attestation-server &

# MRENCLAVE file will be fecthed from CAS
# Extract redis server MRENCLAVE to file
# SCONE_HASH=1 redis-server > /home/redis/mrenclave

# Make MRENCLAVE file readonly to all
# chmod 444 /home/redis/mrenclave

# Run redis with provided config file
SCONE_VERSION=1 SCONE_LOG=7 SCONE_CONFIG_ID=secure-redis-container/redis redis-server /usr/local/etc/redis/redis.conf