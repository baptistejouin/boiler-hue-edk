
# boiler-hue

Reactive Lighting System for Philips Hue Using the Hue Entertainment Development Kit (EDK), OpenCV, and NDI

**Objective:**
Develop a low-latency system that processes NDI video/audio inputs (e.g., from TouchDesigner) and converts them into dynamic lighting effects for Philips Hue devices, creating immersive, real-time experiences.

**Technical Stack:**
- Philips Hue EDK: Low-latency control of Hue lights.
- OpenCV: Real-time image analysis (color, motion, intensity).
- NDI: High-performance streaming over IP networks.

**Design Principles:**
- Plug-and-play integration.
- Optimized for low-latency synchronization.
- Modular and adaptable for custom applications.

## Install

### 1. Request Access to the EDK

Visit the Hue developer site and submit the access request form.  
Make sure to include your GitHub username so that repository cloning works during the build process.
Make sure you have configured correctly your GitHub SSH key on your device.

### 2. Install OpenCV

Install OpenCV through your preferred package manager.  
For macOS, an example using Homebrew:

```sh
brew install opencv
```

### 3. Install the NDI SDK

You can install the NDI SDK in one of two ways:

#### Option A — Homebrew (macOS)
```sh
brew install --cask libndi
```
after that, start the installation by opening the .pkg 

#### Option B — Direct Download
Download the SDK from the official NDI site and complete the required form.
https://ndi.video/for-developers/ndi-sdk/

-> After extracting the package, copy:

- the `Include` (headers) directory  
- the `libndi` library  

into your project's `lib` directory.

Finally, adjust your CMake configuration to point to these paths as needed.

You should now be ready to build the project.

