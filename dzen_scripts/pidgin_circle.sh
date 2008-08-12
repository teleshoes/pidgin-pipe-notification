#!/bin/sh
# Shell script to to show the pidgin status as a colored circle in the dzenbar
#
# This is a free shell script under GNU GPL version 3.0 or above
#
# Copyright (C) 2008 Armin Preiml

#you'll need a icon called circle.xbm
ICONPATH=$HOME/.icons/dzen

#pidgin
PIPEFILE=$HOME/.purple/plugins/pipe

#Sets blinking on new message on or off
BLINK=0

fpidgin() {
        STATUS=`cat $PIPEFILE`; 

        if [ "$STATUS" != "Off" ]; then
                if [ "$STATUS" == "On" -o "$STATUS" == "Available" -o "$STATUS" == "Chatty" ]; then 
                        echo -n "^fg(green)";
                elif [ "$STATUS" == "Away" ]; then
                        echo -n "^fg(blue)";
                elif [ "$STATUS" == "Extended away" ]; then
                        echo -n "^fg(brown)";
                elif [ "$STATUS" == "Do Not Disturb" ]; then
                        echo -n "^fg(red)";
                elif [ "$STATUS" == "Offline" ]; then
                        echo -n "^fg(gray70)";
                elif [ "$STATUS" == "New Message" ]; then
                        if [ $BLINK -le 1 ]; then
                                echo -n "^fg(orange)";
                        elif [ $BLINK -eq 2 ]; then
                                echo -n "^fg($BG)";
                        fi
                fi

                echo -n "^i($ICONPATH/circle.xbm)";

                echo "^fg()";
        fi
 
}

while true; do
	if [ $BLINK -ge 3 ]; then
		BLINK=1
	fi

	echo $(fpidgin)

	if [ $BLINK -ge 1 ]; then
		BLINK=$((BLINK+1))
	fi

	sleep 1;
done
