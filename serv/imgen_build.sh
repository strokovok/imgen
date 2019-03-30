sudo systemctl stop imgen

echo Building backend...

g++ -O3 -std=c++14 imgen/backend/main.cpp -o imgen/backend/imgen -pthread -lX11 -ljpeg -lpng -lboost_thread -lboost_system

echo Building frontend...

npm --prefix imgen/frontend install

npm --prefix imgen/frontend run build

sudo systemctl start imgen

