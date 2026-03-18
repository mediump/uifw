mkdir -p "./_generated/"

for filename in *.ttf; do
    if [ -f "$filename" ]; then
        ../../bin/msdf-atlas-gen/linux/msdf-atlas-gen -font "$filename" -format png -imageout \
          "./_generated/${filename/.ttf/.png}" -range 4 -json "./_generated/${filename/.ttf/.json}"
    fi
done
