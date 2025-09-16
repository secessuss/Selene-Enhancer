# Set to true if you don’t want to mount the system folder
SKIPMOUNT=false

# Set to true to enable debug logging during installation
DEBUG=false

ui_print "- Extracting module files"
unzip -o "$ZIPFILE" 'service.sh' 'enhancer_binary' -d "$MODPATH" >&2

set_permissions() {
  set_perm "$MODPATH/service.sh" 0 0 0755
  set_perm "$MODPATH/enhancer_binary" 0 0 0755
}

SKIPUNZIP=1
unzip -qjo "$ZIPFILE" 'common/functions.sh' -d "$TMPDIR" >&2
. "$TMPDIR/functions.sh"