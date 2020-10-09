#!/bin/sh

new_redis_port="${REDIS_PORT:-6379}"

sed -i "s/tls-port 6363/tls-port $new_redis_port/g" /usr/local/etc/redis/redis.conf

# Run redis with provided config file
redis-server /usr/local/etc/redis/redis.conf