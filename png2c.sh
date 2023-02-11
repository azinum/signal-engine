#!/usr/bin/env bash

FILE=include/assets.h

echo "// assets.h" > ${FILE}
echo "#ifndef _ASSETS_H" >> ${FILE}
echo "#define _ASSETS_H" >> ${FILE}

for PNG_PATH in assets/*.png; do
	NAME=${PNG_PATH%.png}
	NAME=${NAME##*/}
	./png2c ${PNG_PATH} ${NAME} >> ${FILE}
done

echo "#endif // _ASSETS_H" >> ${FILE}
