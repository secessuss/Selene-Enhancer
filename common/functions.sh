if $DEBUG; then
  set -x
else
  set +x
fi

if ! $BOOTMODE; then
  abort "- Installation not supported!"
fi

ui_print "- Extracting module files"
unzip -o "$ZIPFILE" -x 'META-INF/*' 'common/functions.sh' -d "$MODPATH" >&2

if $SKIPMOUNT; then
  touch "$MODPATH/skip_mount"
fi

ui_print "- Setting permissions"
set_perm_recursive "$MODPATH" 0 0 0755 0644
set_permissions

ui_print "- Cleaning up"
rm -rf \
  "$MODPATH/common" \
  "$MODPATH/customize.sh" \
  "$MODPATH/.git*"

ui_print "- Installation complete!"
