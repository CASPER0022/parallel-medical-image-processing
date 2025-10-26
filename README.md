# 🧠 Parallel Medical Image Processing (C++ + OpenMP)

This small project shows how to process medical images in C++ — both **serially** and **in parallel** using **OpenMP**.
It uses the **STB image libraries** to load and save PNG files, and runs easily on Windows using **MinGW-w64 (GCC)**.

---

## What’s Inside

| File                               | Description                                                                                         |
| ---------------------------------- | --------------------------------------------------------------------------------------------------- |
| `medical_serial.cpp`               | Regular (non-parallel) version — can process single or multiple images                              |
| `medical_parallel.cpp`             | Parallel version using OpenMP — processes a batch of images faster                                  |
| `stb_image.h`, `stb_image_write.h` | Single-file image libraries (you can get them from [nothings/stb](https://github.com/nothings/stb)) |

---

## ⚙️ Requirements

* **MinGW-w64 (GCC)** — make sure it supports OpenMP (`libgomp` is included in most versions).
* **C++17** support (needed for `std::filesystem`)
* **PowerShell** (for the commands below)
* Some `.png` image files to test (e.g., `image1.png`, `image2.png`)

---

## How to Build & Run

Open **PowerShell** in the project folder (where your `.cpp` files and images are located).

### 1️. Compile

```powershell
# Parallel version
g++ medical_parallel.cpp -o medical_parallel -std=c++17 -fopenmp

# Serial version
g++ medical_serial.cpp -o medical_serial -std=c++17 -fopenmp
```

Notes:

* Use `-std=c++17` for filesystem support.
* Use `-fopenmp` for OpenMP — don’t forget the dash! (`fopenmp` without `-` causes an error)

---

### 2️. Run

```powershell
# Run parallel version
./medical_parallel.exe

# Or the serial version
./medical_serial.exe
```

After running, you should see two folders created automatically:

* `negative/` → contains the negative versions of your images
* `edge/` → contains edge-detected versions

Each image will have the same name as your original one.

---

## Customizing Your Images

Open **`medical_parallel.cpp`**, and near the top you’ll find:

```cpp
vector<string> inputFilenames = {"image1.png", "image2.png"};
```

Just change or add filenames there — make sure they exist in your folder.

---

## ⚡ How the Parallel Version Works

* Uses `#pragma omp parallel for` to process multiple images at once
* Inner loops (like pixel transformations) also use OpenMP for faster processing
* A few places use `#pragma omp critical` to safely handle console output

---

## 🛠 Common Issues
Missing STB headers | Download them from [https://github.com/nothings/stb](https://github.com/nothings/stb) |
