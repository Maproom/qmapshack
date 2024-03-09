#!/bin/sh
# checking arguments: intested in -x, -g, -b
while getopts ":bgx" opt; do
  case $opt in
    b)
      echo "-b was triggered!"
      ;;
    g)
      echo "-g was triggered!" >&2
      ;;
    x)
        echo "-x was triggered!" >&2
      ;;
    \?)
      echo "Invalid option: -$OPTARG" >&2
      exit
      ;;
  esac
done