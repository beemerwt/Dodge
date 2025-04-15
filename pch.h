#pragma once

// Collision from minwindef min/max and std
#define NOMINMAX 

#include "resource.h"

// Windows SDK support
#include <Unknwn.h>
#include <inspectable.h>

// Needs to come before C++/WinRT headers
#include <wil/cppwinrt.h>

// WinRT
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.Metadata.h>
#include <winrt/Windows.Graphics.Capture.h>
#include <winrt/Windows.Graphics.DirectX.h>
#include <winrt/Windows.Graphics.DirectX.Direct3d11.h>
#include <winrt/Windows.Graphics.Imaging.h>
#include <winrt/Windows.Security.Authorization.AppCapabilityAccess.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.Storage.Pickers.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Composition.Desktop.h>
#include <winrt/Windows.UI.Popups.h>

// STL
#include <atomic>
#include <memory>
#include <algorithm>
#include <unordered_set>
#include <vector>
#include <optional>
#include <functional>
#include <future>
#include <deque>
#include <mutex>
#include <thread>

// D3D
#include <d3d11_4.h>
#include <dxgi1_6.h>
#include <d2d1_3.h>
#include <wincodec.h>

// DWM
#include <dwmapi.h>

// WIL
#include <wil/resource.h>
#include <wil/cppwinrt_helpers.h>
#include <wil/coroutine.h>

// CUDA
#include <cuda_runtime.h>
#include <cuda_d3d11_interop.h>

// CV
#include <opencv2/core/cuda.hpp>
#include <opencv2/cudaimgproc.hpp>
#include <opencv2/highgui.hpp>
