#!/bin/sh

new_redis_port="${REDIS_PORT:-6379}"

echo "$new_redis_port"

sed -i "s/port 6357/port $new_redis_port/g" /usr/local/etc/redis/redis.conf

SCONE_HASH=1 /home/attestation_server/server > /home/attestation_server/attst_server_mrenclave
SCONE_VERSION=1 /home/attestation_server/server &
SCONE_HASH=1 redis-server > /home/attestation_server/redis_mrenclave
#chmod 444 /home/attestation_server/mrenclave
SCONE_VERSION=1 redis-server /usr/local/etc/redis/redis.conf