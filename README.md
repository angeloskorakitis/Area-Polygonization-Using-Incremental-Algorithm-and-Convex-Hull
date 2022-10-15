# 2023-emiris-k23-assignment-1

Στο /programs έτρεξα:
cgal_create_CMakeLists -s main
cmake -DCGAL_DIR=/usr/local/opt/cgal/lib/cmake/CGAL -DCMAKE_BUILD_TYPE=Release -S . -B ../build

Για να κάνω compile και run τρέχω στο /build:
make
./main