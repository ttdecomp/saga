#!/bin/sh
# Build a contact sheet from host harness framebuffer captures.
set -eu
out=${1:-.work/capture/montage.png}
shift || true
files="$*"
if [ -z "$files" ]; then
    files=$(find .work/capture -maxdepth 1 -name 'window_*.ppm' -print | sort -V)
fi
[ -n "$files" ] || { echo "no capture PPMs found" >&2; exit 1; }

# A settled animation can revisit the same framebuffer indefinitely (the
# title prompt blink is the common example). Keep the captures themselves,
# but show each exact image only once in the diagnostic contact sheet.
unique_files=
seen_hashes=
for file in $files; do
    hash=$(sha256sum "$file" | cut -d ' ' -f 1)
    case " $seen_hashes " in
        *" $hash "*) ;;
        *)
            seen_hashes="$seen_hashes $hash"
            unique_files="$unique_files $file"
            ;;
    esac
done

montage $unique_files -thumbnail 320x180 -tile 4x -geometry 320x180+8+24 -label '%f' "$out"
echo "$out"
