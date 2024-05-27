mkdir build
cd build
cmake .. -A WIN32

set CONFIG_OPTION=Debug
#set CONFIG_OPTION=Release

cmake --build . --config %CONFIG_OPTION%
cmake --install . --config %CONFIG_OPTION%
