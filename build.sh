rm -rf ./output/*
echo "Cleaned output directory."

mkdir -p ./output

g++ -Werror ./src/main.cpp ./src/cmdL.cpp -o ./output/t.exe
# -Werror  - treat warnings as errors
if [ $? -ne 0 ]; then
    echo "Build failed."
    exit 1
fi
echo "Build succeeded."