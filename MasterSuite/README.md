MasterSuite - Ready for GitHub (NO GIT required)
==============================================

This archive contains the plugin source AND a ready-to-use GitHub Actions workflow and NSIS script.
If you don't have `git` installed on your Windows machine, you can still publish this repository on GitHub
using the web interface by following these steps:

1. Unzip this archive on your PC.
2. Create a new repository on GitHub (https://github.com) named `MasterSuite`.
3. Open the new empty repo page on GitHub, click **Add file → Upload files**.
4. Drag-and-drop ALL files and folders from the unzipped folder into the upload area (including the hidden `.github` folder).
   - If your browser doesn't allow drop of hidden folders, open the unzipped folder and select all files/folders, then drag.
5. Commit the upload (bottom of the page) with a message like \"Initial commit\".
6. After the push finishes, go to the **Actions** tab in GitHub — the workflow `Build & Package MasterSuite (Windows)` will be listed.
7. Trigger the workflow manually (if desired) using **Run workflow** or push another commit to `main`.
8. After the Actions run completes successfully, open the run and download the artifact `MasterSuite-Installer-Windows` — it contains `MasterSuite_Installer.exe`.

Notes:
- The workflow builds the plugin using JUCE cloned inside the runner. It expects CMake build outputs to include `MasterSuite.vst3`.
- The NSIS installer will be built on the runner and the resulting installer `.exe` will be uploaded as an artifact.
- If you later want to use `git` CLI: install Git for Windows (https://git-scm.com/download/win) and then run `git --version` to verify.

