#!/bin/sh

rm -f /tmp/sigen.*

mkfifo /tmp/sigen.out
mkfifo /tmp/sigen.in
