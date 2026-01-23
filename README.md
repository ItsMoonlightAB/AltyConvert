<div align="center">

# 🔄 AltyConvert

**A modern, fast, and offline multimedia converter for Windows.** Built with **C** and **GTK4**, powered by **FFmpeg**.

[![Platform](https://img.shields.io/badge/platform-Windows-0078D6?style=flat-square&logo=windows)](https://github.com/ItsMoonlightAB/AltyConvert)
[![Language](https://img.shields.io/badge/language-C-A8B9CC?style=flat-square&logo=c)](https://en.wikipedia.org/wiki/C_(programming_language))
[![GUI](https://img.shields.io/badge/GUI-GTK4-green?style=flat-square&logo=gtk)](https://www.gtk.org/)
[![License](https://img.shields.io/badge/license-MIT-orange?style=flat-square)](LICENSE)

[Download Latest Release](https://github.com/ItsMoonlightAB/AltyConvert/releases/latest)

</div>

---

## 📖 About The Project

**AltyConvert** is a lightweight desktop application designed to make file conversion simple and accessible.</br>
Unlike online converters, AltyConvert processes files **locally on your machine**, ensuring privacy and speed.</br>
It features a modern Dark Mode interface with drag-and-drop support.

### ✨ Key Features
* 🎨 **Modern UI:** Clean, dark-themed interface using CSS styling in GTK4.
* 🖱️ **Drag & Drop:** Simply drag your files into the window to start.
* ⚡ **Fast Conversion:** Utilizes the power of FFmpeg for efficient encoding.
* 🔒 **100% Offline:** No file upload required. Everything happens on your PC.
* 🧠 **Smart Detection:** Automatically detects file types (Image, Audio, Video) and suggests relevant formats.

---

## 📂 Supported Formats

| Type | Input & Output Formats |
| :--- | :--- |
| **🎬 Video** | `.mp4`, `.mkv`, `.avi`, `.mov`, `.wmv`, `.webm`, `.ogg` |
| **🎵 Audio** | `.mp3`, `.wav`, `.ogg` |
| **🖼️ Image** | `.jpg`, `.jpeg`, `.png`, `.webp`, `.tiff`, `.ico`, `.gif` |

---

## 🐛 Known Issues & Feedback

Currently, there is one known limitation :
* **Video to `.ogg` conversion** will fail.

I am actively working on improving the tool.</br>
If you encounter any bugs or have suggestions, please feel free to **[open an issue](https://github.com/ItsMoonlightAB/AltyConvert/issues)** on GitHub.

Your feedback is highly appreciated and helps me improve AltyConvert!

---

## 🚀 Download & Installation

**For regular users :**

1.  Go to the **[Releases Page](https://github.com/ItsMoonlightAB/AltyConvert/releases)** (on the right side of GitHub).
2.  Download the latest `.zip` file (e.g., `AltyConvert_1.0.zip`).
3.  **Extract** the ZIP file content into a folder.
4.  Double-click **`AltyConvert.exe`** to launch the app.

> **⚠️ Important :** Keep `AltyConvert.exe`, `ffmpeg.exe`, and all `.dll` files in the same folder.</br>
> Do not move the executable alone, or it will not start.

---

## 🛠️ Build yourself

For **developers** who want to modify or compile the code themselves.

### Prerequisites
You need a Windows environment with **MSYS2 (UCRT64)** installed.

### 1. Install Dependencies
Open your MSYS2 UCRT64 terminal and run :

```bash
pacman -S --needed base-devel mingw-w64-ucrt64-x86_64-toolchain mingw-w64-ucrt64-x86_64-gtk4 mingw-w64-ucrt64-x86_64-ffmpeg
```

### 2. Clone the Repository
```bash
git clone https://github.com/ItsMoonlightAB/AltyConvert.git
cd AltyConvert
```

### 3. Compile
Use the provided Makefile to compile the project :
```bash
make
```

### 4. Run
```bash
./build/AltyConvert.exe
```
*(Note : To run the compiled version, ensure `ffmpeg.exe` and `gspawn-win64-helper.exe` are accessible).*

---

## 🏗️ Project Structure

```text
AltyConvert/
├── assets/          # Icon (icon.ico)
├── include/         # Header files (.h)
├── src/             # Source code (.c)
│   ├── main.c       # Entry point & UI logic
│   ├── converter.c  # Threading & Process logic
│   ├── utils.c      # Helper functions
│   └── conv_*.c     # FFmpeg wrappers for Audio/Video/Image
├── Makefile         # Compilation script
└── resource.rc      # Windows resource file
```

---

## 🔧 Technologies Used

* **C (C11):** Core logic and performance.
* **GTK4:** Graphical User Interface (GUI).
* **CSS:** Styling the interface (Material You).
* **FFmpeg:** Backend engine for multimedia processing.
* **Win32 API:** Used for native folder selection dialogs (`ShlObj`).

---

## 📝 License

Distributed under the MIT License.

---

<div align="center">
  <sub>Built by ItsMoonlight</sub>
</div>
