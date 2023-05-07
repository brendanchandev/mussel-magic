
CURRENT_DIR=`readlink -f .`

echo "CURRENT_DIR = " $CURRENT_DIR

gcc -L../uugear/RaspberryPi/bin -Wall MusselPiv11.cpp -lUUGear -lrt