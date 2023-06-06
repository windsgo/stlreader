# Usage

- 对于Ubuntu低于20.04，CMAKE版本低于3.15的系统，该CMAKE构建方法可能出错
- 可以进入src目录进行make构建，查看相关make的Readme说明
- [src/Readme.md](src/Readme.md)
## How to build (and clean)

### Way 1

```bash
# clean
rm -rf build
rm src/*.so
# build
cmake -B build
cmake --build build -j8
# run test
./run_test.sh
```

### Way 2 (Recommanded)

```bash
./clean.sh # clean
./build.sh # build
./run_test.sh # run test
```

## Export to Use

- Copy the `libstlread.so` under `build/lib` to where you want to use it
- Then in python:(see `test.py`)
  ```python
  from libstlread import stlread
  a = stlread(yourfilename)
  # ...
  ```