#!/bin/bash

BOARD_IP="192.168.7.1"
BOARD_USER="root"
REMOTE_DIR="/root/tcp_project"

echo "Copying files to board..."
scp *.c Makefile "$BOARD_USER@$BOARD_IP:$REMOTE_DIR/"

echo "Building on board..."
ssh "$BOARD_USER@$BOARD_IP" "cd $REMOTE_DIR && make"
