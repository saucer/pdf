<hr>

<div align="center"> 
    <img src="https://raw.githubusercontent.com/saucer/saucer.github.io/master/static/img/logo.png" height="312" />
</div>

<p align="center"> 
    PDF (Printer) module for <a href="https://github.com/saucer/saucer">saucer</a>
</p>

---

## 📦 Installation

* Using [CPM](https://github.com/cpm-cmake/CPM.cmake)
  ```cmake
  CPMFindPackage(
    NAME           saucer-pdf
    VERSION        3.0.0
    GIT_REPOSITORY "https://github.com/saucer/pdf"
  )
  ```

* Using FetchContent
  ```cmake
  include(FetchContent)

  FetchContent_Declare(saucer-pdf GIT_REPOSITORY "https://github.com/saucer/pdf" GIT_TAG v3.0.0)
  FetchContent_MakeAvailable(saucer-pdf)
  ```

Finally, link against target:

```cmake
target_link_libraries(<target> saucer::pdf)
```

## 📃 Usage

For a usage example see the [pdf-example](https://github.com/saucer/saucer/blob/master/examples/pdf/main.cpp)
