MasterSuite - Source bundle (minimal)
=================================

What this archive contains:
- A compact JUCE + CMake plugin source ready to compile into a VST3.
- Simplified DSP implementations (EQ, simple compressor, saturation, delay, JUCE reverb, mid/side stereo width, basic limiter).
- A basic UI that exposes the main parameters via APVTS attachments.

Important limitations & notes:
- This environment cannot compile JUCE or produce binaries. You MUST compile on your machine that has JUCE and a C++ toolchain installed (Visual Studio on Windows or Xcode on macOS).
- The code is intentionally compact to be easy to understand and extend — it's a starting point, not a final polished mastering suite.
- I recommend cloning JUCE next to this folder: `git clone https://github.com/juce-framework/JUCE.git` so that the path `../JUCE` points to the JUCE repo.

Build instructions (Windows example):
1. Clone JUCE next to this project folder so you have a structure like:
   - Projects/
     - JUCE/
     - MasterSuite/
2. Open an admin x64 Native Tools command prompt (or regular, depending on your toolchain).
3. Run:
   ```
   cd MasterSuite
   cmake -B build -G "Visual Studio 17 2022" -A x64
   cmake --build build --config Release
   ```
4. After a successful build the VST3 will typically be in `build/Release/` or inside the Visual Studio products directory.
5. Copy the generated `.vst3` to `C:\\Program Files\\Common Files\\VST3` or another VST3 path and scan in FL Studio 21 (Options → Manage plugins → Find plugins).

Optional: Create an installer (Windows - NSIS)
---------------------------------------------
You can make an NSIS script to copy the VST3 to the system VST3 folder. Example NSIS script (fill with proper values):
```
!define PRODUCT_NAME \"MasterSuite\"\n!define PRODUCT_VERSION \"0.1\"\n!define VST3_NAME \"MasterSuite.vst3\"\n\nOutFile \"MasterSuite-Installer.exe\"\nInstallDir \"$PROGRAMFILES\\Common Files\\VST3\"\n\nSection \"Install\"\n  SetOutPath \"$INSTDIR\"\n  File \"path\\to\\built\\${VST3_NAME}\"\nSectionEnd\n```
Use NSIS to compile the installer.

If you want, I can also:
- Generate a GitHub Actions workflow file to build the plugin automatically on push (Windows + macOS), and upload artifacts (VST3) so you can download them without building locally.
- Create a pre-filled NSIS script or Inno Setup script with the correct paths and metadata.

