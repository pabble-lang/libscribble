#!/bin/sh

set x

SCRIBBLE_VERSION=$(dpkg-parsechangelog --show-field Version)
sed -i "s/^#define SCRIBBLE_TOOL_VERSION.*$/#define SCRIBBLE_TOOL_VERSION \"$SCRIBBLE_VERSION\"/" src/scribble-tool.c
