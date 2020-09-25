#!/bin/sh

/home/attestation_server/attestation-server &
SCONE_HASH=1 redis-server > /home/attestation_server/mrenclave
chmod 444 /home/attestation_server/mrenclave
SCONE_VERSION=1 redis-server