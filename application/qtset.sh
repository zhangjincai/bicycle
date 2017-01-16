#!/bin/bash

export set LDPATH=/usr/lib
export set LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$LDPATH
export set QT_QWS_FONTDIR=$LDPATH/fonts
export set QT_PLUGIN_PATH=$LDPATH/qt/plugins

export set QWS_DISPLAY="LinuxFB:/dev/fb0"
export set QWS_KEYBOARD=ndev:/dev/ttyO5

echo $QWS_KEYBOARD


#./fb_set /dev/fb0


















