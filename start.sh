#!/bin/sh

/home/attestation_server/server &
SCONE_HASH=1 redis-server > /home/mrenclave
chmod 444 /home/mrenclave
SCONE_VERSION=1 redis-server