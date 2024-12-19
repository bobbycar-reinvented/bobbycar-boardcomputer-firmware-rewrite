#!/bin/bash

# check if command rgb565-converter is available
  if ! command -v rgb565-converter &> /dev/null
  then
      echo "rgb565-converter is not installed"
      # check if system is archlinux
      if command -v pacman &> /dev/null
      then
          echo "You can install it from the AUR like this: yay -S python-rgb565-converter"
      else
          echo "please install rgb565-converter manually. You can find it here: https://github.com/CommanderRedYT/rgb565-converter"
      fi

      exit 1
  fi


for i in icons/*
do
    OUTPUT_FILE="main/icons/$(basename "$i" .png).cpp"

    rgb565-converter -i "$i" -o "$OUTPUT_FILE" --namespace "bobbyicons" -s --cpp-template-file "rgb565_cpp_template.txt" --h-template-file "rgb565_h_template.txt"

    ICON_NAME="$(basename "$i" .png)"
    case "$ICON_NAME" in
        bobbycar|shortcircuit|logo)
            echo "no grey version needed for $ICON_NAME"
            ;;
        *)
            OUTPUT_FILE="main/icons/${ICON_NAME}_grey.cpp"
            rgb565-converter -i "$i" -o "$OUTPUT_FILE" --namespace "bobbyicons" -s --background "#5c5c5c" --cpp-template-file "rgb565_cpp_template.txt" --h-template-file "rgb565_h_template.txt"
            ;;
    esac
done
