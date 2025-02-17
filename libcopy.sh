#Get linked libs - Filter out non linked - grab the path - assign it to a variable inside loop

TARGET=./lib
if [ ! -d "$TARGET" ]; then
  mkdir $TARGET
fi

ldd bin/MVis | grep '=>' | awk '{print$3}' | while read lib; do
  if [ -f "$lib" ]; then
    echo "copying => ${lib}"
    cp "$lib" $TARGET/
  fi
done
