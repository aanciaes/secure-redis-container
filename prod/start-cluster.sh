#!/bin/sh

new_redis_port="${REDIS_PORT:-6379}"

sed -i "s/tls-port 6357/tls-port $new_redis_port/g" /usr/local/etc/redis/redis.conf

# Extract attestation server MRENCLAVE to file
SCONE_HASH=1 /home/attestation_server/attestation-server > /home/attestation_server/mrenclave

# Make MRENCLAVE file readonly to all
chmod 444 /home/attestation_server/mrenclave

# Run attestation server in background
SCONE_VERSION=1 /home/attestation_server/attestation-server &

# Extract redis server MRENCLAVE to file
SCONE_HASH=1 redis-server > /home/redis/mrenclave

# Make MRENCLAVE file readonly to all
chmod 444 /home/redis/mrenclave

# Run redis with provided config file
SCONE_VERSION=1 redis-server /usr/local/etc/redis/redis.conf