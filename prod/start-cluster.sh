#!/bin/sh

new_redis_port="${REDIS_PORT:-6379}"

attestation_server_port="${REDIS_ATTESTATION_PORT:-8541}"

sed -i "s/tls-port 6357/tls-port $new_redis_port/g" /usr/local/etc/redis/redis.conf

# Extract attestation server MRENCLAVE to file
# SCONE_HASH=1 /home/attestation_server/attestation-server > /home/attestation_server/mrenclave

# Make MRENCLAVE file readonly to all
# chmod 444 /home/attestation_server/mrenclave

# Run attestation server in background
SCONE_VERSION=1 SCONE_LOG=7 SCONE_CONFIG_ID=secure-redis-container-cluster/attestation_server_$attestation_server_port /home/attestation_server/attestation-server &

# Extract redis server MRENCLAVE to file
# SCONE_HASH=1 redis-server > /home/redis/mrenclave

# Make MRENCLAVE file readonly to all
# chmod 444 /home/redis/mrenclave

# Run redis with provided config file
SCONE_VERSION=1 SCONE_LOG=7 SCONE_CONFIG_ID=secure-redis-container-cluster/redis redis-server /usr/local/etc/redis/redis.conf