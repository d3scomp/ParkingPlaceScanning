#!/bin/sh

cat log |grep "### EtE lattency ###"|cut -d ":" -f 2|cut -d " " -f 2 > end-to-end\ lattency.txt
