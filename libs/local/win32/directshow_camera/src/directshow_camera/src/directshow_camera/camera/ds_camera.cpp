/**
* Copy right (c) 2020-2024 Ka Chun Wong. All rights reserved.
* This is a open source project under MIT license (see LICENSE for details).
* If you find any bugs, please feel free to report under https://github.com/kcwongjoe/directshow_camera/issues
**/

#include "directshow_camera/camera/ds_camera.h"

#include "directshow_camera/utils/check_hresult_utils.h"
#include "directshow_camera/utils/win32_utils.h"
#include "directshow_camera/utils/ds_camera_utils.h"
#include "directshow_camera/utils/com_lib_utils.h"

namespace DirectShowCamera
{
    
#pragma region Life cycle

    DirectShowCamera::DirectShowCamera() :
        m_mediaControlHandler(IMediaControlHandler())
    {
        // Initialize COM Library
        const auto result = COMLibUtils::COMLibUtils::InitCOMLib(m_errorString);
    }

    DirectShowCamera::~DirectShowCamera()
    {
        Release();

        // Uninitialize COM Library
        COMLibUtils::COMLibUtils::UninitCOMLib();
    }

    void DirectShowCamera::Release()
    {
        if (this)
        {
            HRESULT hr = NOERROR;

            Stop();

            // Release video format
            m_videoFormats.Clear();
            m_currentVideoFormatIndex = -1;

            // assigning a newly created DirectShowVideoFormat will reset m_isEmpty
            m_sampleGrabberVideoFormat = DirectShowVideoFormat();

            // Release stream config
            DirectShowCameraUtils::SafeRelease(&m_amStreamConfig);

            // Release sample grabber callback
            if (m_sampleGrabber != NULL) m_sampleGrabber->SetCallback(NULL, 1);
            DirectShowCameraUtils::SafeRelease(&m_sampleGrabberCallback);

            // Release sample grabber
            DirectShowCameraUtils::SafeRelease(&m_sampleGrabber);

            // Disconnect filters from capture device
            DirectShowCameraUtils::NukeDownStream(m_filterGraphManager, m_directShowFilter);

            // Release media event
            DirectShowCameraUtils::SafeRelease(&m_mediaEvent);

            // Release media control
            m_mediaControlHandler.Release();

            // Release null renderer filter
            DirectShowCameraUtils::SafeRelease(&m_nullRendererFilter);

            // Release grabber filter
            DirectShowCameraUtils::SafeRelease(&m_grabberFilter);

            // Release DirectShow Filter
            DirectShowCameraUtils::SafeRelease(&m_directShowFilter);

            // Release capture graph builder
            DirectShowCameraUtils::DestroyGraph(m_filterGraphManager);
            DirectShowCameraUtils::SafeRelease(&m_filterGraphManager);

            // Release capture graph builder
            DirectShowCameraUtils::SafeRelease(&m_captureGraphBuilder);

            //Release Properties
            m_property.reset();

            m_grabberMediaSubType = MEDIASUBTYPE_None;

            m_isOpening = false;
        }		
    }

#pragma endregion Life cycle

#pragma region Connection

    bool DirectShowCamera::Open(
        IBaseFilter** directShowFilter,
        std::optional<const DirectShowVideoFormat> videoFormat,
        // If true, the output data will be converted to RGB24 if the format support it.
        bool convertOutputDataToRGB24IfSupported
    )
    {
        // Initialize variable
        bool result = true;

        if (!m_isOpening)
        {
            m_directShowFilter = *directShowFilter;
            m_convertOutputDataToRGB24IfSupported = convertOutputDataToRGB24IfSupported;

            m_sampleGrabberCallback = new SampleGrabberCallback();
            // Create the capture graph builder
            if (result)
            {
                const auto hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC_SERVER, IID_ICaptureGraphBuilder2, (void**)&m_captureGraphBuilder);
                result = CheckHResultUtils::CheckCoCreateInstanceResult(hr, m_errorString, "Error on creating the Capture Graph Builder");
            }

            // Create the Filter Graph Manager.
            if (result)
            {
                const auto hr = CoCreateInstance(CLSID_FilterGraph, 0, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void**)&m_filterGraphManager);
                result = CheckHResultUtils::CheckCoCreateInstanceResult(hr, m_errorString, "Error on creating the Filter Graph Manager");
            }

            // Set a media event. We can use this to check device disconnection.
            if (result)
            {
                const auto hr = m_filterGraphManager->QueryInterface(IID_IMediaEventEx, (void**)&m_mediaEvent);
                if (hr != S_OK)
                {
                    result = false;
                    m_errorString = " Could not create media event object for device disconnection(hr = " + std::to_string(hr) + ").";
                }
            }

            // Set the Filter Graph Manager into Capture Graph Builder
            if (result)
            {
                const auto hr = m_captureGraphBuilder->SetFiltergraph(m_filterGraphManager);
                result = CheckHResultUtils::CheckICGB2SetFiltergraphResult(hr, m_errorString, "Error on setting the Filter Graph Manager to Capture Graph Builder");
            }

            // Set the media control
            if (result)
            {
                result = m_mediaControlHandler.CreateMediaControl(m_filterGraphManager, m_errorString);
            }

            // Add DirectShow Filter
            if (result)
            {
                const auto hr = m_filterGraphManager->AddFilter(m_directShowFilter, NULL);
                result = CheckHResultUtils::CheckIGBAddFilterResult(hr, m_errorString, "Error on adding DirectShow Filter");
            }

            // Set capture pin
            if (result)
            {
                const auto hr = m_captureGraphBuilder->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, m_directShowFilter, IID_IAMStreamConfig, (void**)&m_amStreamConfig);
                result = CheckHResultUtils::CheckICGB2FindInterfaceResult(hr, m_errorString, "Error on setting capture pin");
            }

            // Get media type
            AM_MEDIA_TYPE* amMediaType = NULL;
            if (result)
            {
                // Set video format
                if (videoFormat != std::nullopt && videoFormat.has_value())
                {
                    // Update the video format list so that we can find the AM_Media_Type in the list.
                    UpdateVideoFormatList();

                    // Set format
                    result = setVideoFormat(videoFormat.value());

                    // errorMessage has been updated inside setVideoFormat(). Don't overwrite the error message.
                    if (!result) m_errorString = "Error on setting video format: " + m_errorString;
                }

                // Get format
                if (result)
                {
                    const auto hr = m_amStreamConfig->GetFormat(&amMediaType);
                    result = CheckHResultUtils::CheckIIAMSCGetFormatResult(hr, m_errorString, "Error on getting media type");
                }
            }

            // Create the Sample Grabber.
            if (result)
            {
                const auto hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&m_grabberFilter);
                result = CheckHResultUtils::CheckCoCreateInstanceResult(hr, m_errorString, "Error on creating the Sample Grabber");
            }

            // Add Sample Grabber
            if (result)
            {
                const auto hr = m_filterGraphManager->AddFilter(m_grabberFilter, L"Sample Grabber");
                result = CheckHResultUtils::CheckIGBAddFilterResult(hr, m_errorString, "Error on adding Sample Grabber");
            }

            // Connect Sample Grabber
            if (result)
            {
                const auto hr = m_grabberFilter->QueryInterface(IID_ISampleGrabber, (void**)&m_sampleGrabber);
                if (hr != S_OK)
                {
                    result = false;
                    m_errorString = "Could not query Sample Grabber(hr = " + std::to_string(hr) + ").";
                }
            }

            // Set Grabber callback function
            if (result)
            {
                m_sampleGrabber->SetOneShot(FALSE);
                m_sampleGrabber->SetBufferSamples(FALSE);
                const auto hr = m_sampleGrabber->SetCallback(m_sampleGrabberCallback, 0);// 0 is for SampleCB and 1 for BufferCB
                if (hr != S_OK)
                {
                    result = false;
                    m_errorString = "Could not set sample grabber callback function(hr = " + std::to_string(hr) + ").";
                }
            }

            // Set grabber filter media type
            if (result)
            {
                UpdateGrabberFilterVideoFormat();
            }

            // Create a null renderer filter to discard the samples after you are done with them.
            if (result)
            {
                const auto hr = CoCreateInstance(CLSID_NullRenderer, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)(&m_nullRendererFilter));
                result = CheckHResultUtils::CheckCoCreateInstanceResult(hr, m_errorString, "Error on creating the null renderer filter");
            }

            // Add null renderer filter to graph
            if (result)
            {
                const auto hr = m_filterGraphManager->AddFilter(m_nullRendererFilter, L"NullRenderer");
                result = CheckHResultUtils::CheckIGBAddFilterResult(hr, m_errorString, "Error on adding Null Renderer filter");
            }

            //  Connect all filter as stream : DirectShow Filter - Grabber Filter - Null Renderer Filter
            if (result)
            {
                const auto hr = m_captureGraphBuilder->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, m_directShowFilter, m_grabberFilter, m_nullRendererFilter);
                result = CheckHResultUtils::CheckICGB2RenderStreamResult(hr, m_errorString, "Error on connecting filter (DirectShow Filter - Grabber Filter - Null Renderer Filter)");
            }

            // get video format of connected grabber filter
            if (result)
            {
                AM_MEDIA_TYPE grabberMediaType;
                ZeroMemory(&grabberMediaType, sizeof(grabberMediaType));
                const auto hr = m_sampleGrabber->GetConnectedMediaType(&grabberMediaType);
                if (hr == S_OK)
                {
                    m_sampleGrabberVideoFormat = DirectShowVideoFormat::Create(&grabberMediaType);
                }
            }

            // Try setting the sync source to null - and make it run as fast as possible
            const bool syncSourceAsNull = false;
            if (result && syncSourceAsNull)
            {
                IMediaFilter* mediaFilter = 0;
                const auto hr = m_filterGraphManager->QueryInterface(IID_IMediaFilter, (void**)&mediaFilter);
                if (hr == S_OK) {
                    mediaFilter->SetSyncSource(NULL);
                    mediaFilter->Release();
                }
            }

            // ****Release****

            // Free media type
            if (amMediaType != NULL)
            {
                DirectShowCameraUtils::DeleteMediaType(&amMediaType);
            }

            if (result)
            {
                m_isOpening = true;

                // Get property
                if (m_property != nullptr) m_property.reset();
                m_property = std::make_shared<DirectShowCameraProperties>(m_directShowFilter, m_errorString);

                // Update video format
                UpdateVideoFormatList();
                UpdateVideoFormatIndex();
            }
            else
            {
                // Release everything if error
                Release();
            }
        }

        return result;
    }

    void DirectShowCamera::Close()
    {
        Release();
    }

    bool DirectShowCamera::isOpening() const
    {
        return m_isOpening;
    }

    bool DirectShowCamera::isDisconnecting()
    {
        // Device doesn't started
        if (!m_isOpening) return true;

        long eventCode;
        LONG_PTR eventParameter1, eventParameter2;
        bool disconnected = false;

        // Get all event code
        // See https://docs.microsoft.com/en-us/windows/win32/directshow/event-notification-codes
        while (m_mediaEvent->GetEvent(&eventCode, &eventParameter1, &eventParameter2, 0) == S_OK)
        {
            //std::cout << "Event: Code: " + std::to_string(eventCode) + " Params: " + std::to_string(eventParameter1) + ", "  + std::to_string(eventParameter2) + "\n";

            // Free event parameters
            m_mediaEvent->FreeEventParams(eventCode, eventParameter1, eventParameter2);

            // Check device lost
            if (eventCode == EC_DEVICE_LOST)
            {
                EC_BANDWIDTHCHANGE;
                disconnected = true;
            }
        }
        return disconnected;
    }

    void DirectShowCamera::setDisconnectionProcess(std::function<void()> func)
    {
        m_disconnectionProcess = func;

        StartCheckConnectionThread();
    }

    void DirectShowCamera::StartCheckConnectionThread()
    {
        if (m_isCapturing && m_disconnectionProcess)
        {
            if (!m_isRunningCheckConnectionThread)
            {
                m_stopCheckConnectionThread = false;

                m_checkConnectionThread = std::thread(
                    [this]()
                    {
                        // Parameter
                        int step = 1000;

                        // Thread start running
                        m_isRunningCheckConnectionThread = true;

                        while (!m_stopCheckConnectionThread)
                        {
                            // Sleep
                            std::this_thread::sleep_for(std::chrono::milliseconds(step));
                            
                            // Check last frame time
                            auto lastFrameTime = m_sampleGrabberCallback->getLastFrameCaptureTime();
                            auto timeDiff = (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - lastFrameTime)).count(); // in ms
                            double fps = m_sampleGrabberCallback->getFPS();
                            double fpsInTime = 1.0 / fps * 1000;

                            // Check disconnection
                            if (timeDiff > 10000 || fps == 0 || timeDiff >= fpsInTime * 2)
                            {
                                bool disconnected = isDisconnecting();

                                if (disconnected)
                                {
                                    // Do something
                                    m_disconnectionProcess();

                                    // Stop capture
                                    Stop();

                                    m_stopCheckConnectionThread = true;
                                }
                            }
                        }

                        // Thread stoped
                        m_isRunningCheckConnectionThread = false;
                    }
                );
                m_checkConnectionThread.detach();
            }
        }	
    }

#pragma endregion Connection

#pragma region start/stop

    bool DirectShowCamera::Start()
    {
        bool result = true;

        if (m_isOpening)
        {
            // Set callback buffer
            UpdateGrabberFilterVideoFormat();

            // Run
            result = m_mediaControlHandler.Run(m_errorString);

            m_isCapturing = true;

            // Start check disconnection thread
            StartCheckConnectionThread();
        }
        else
        {
            result = false;
            m_errorString = "Graph is not initialized, please call initialize().";
        }

        return result;
    }

    bool DirectShowCamera::Stop()
    {
        HRESULT hr = NOERROR;
        bool result = true;

        if (this)
        {
            if (m_isOpening)
            {
                if (m_isCapturing)
                {
                    // Pause
                    if (result)
                    {
                        result = m_mediaControlHandler.Pause(m_errorString);
                    }

                    // Stop
                    if (result)
                    {
                        result = m_mediaControlHandler.Stop(m_errorString);
                    }

                    // Reset isCapturing
                    m_isCapturing = false;

                    // Stop the check disconnection thread
                    m_stopCheckConnectionThread = true;
                }
            }
            else
            {
                result = false;
                m_errorString = "Graph is not initialized, please call initialize().";
            }
        }

        return result;
    }

    bool DirectShowCamera::isCapturing() const
    {
        return m_isCapturing;
    }

#pragma endregion start/stop

#pragma region Frame


    bool DirectShowCamera::getFrame(Frame &frame, bool onlyGetNewFrame, int lastFrameIndex)
    {
        // Check
        if (!m_isCapturing) return false;

        frame.setInfo(m_sampleGrabberVideoFormat.getWidth(),
            m_sampleGrabberVideoFormat.getHeight(), m_grabberMediaSubType);
        // Get frame
        return m_sampleGrabberCallback->getFrame(frame, onlyGetNewFrame, lastFrameIndex);
    }

    unsigned long DirectShowCamera::getLastFrameIndex() const
    {
        // Check
        if (!m_isCapturing) return 0;

        return m_sampleGrabberCallback->getLastFrameIndex();
    }

    void DirectShowCamera::setMinimumFPS(const double minimumFPS)
    {
        if (m_sampleGrabberCallback)
        {
            if (minimumFPS < 0)
            {
                m_sampleGrabberCallback->setMinimumFPS(0);
            }
            else
            {
                m_sampleGrabberCallback->setMinimumFPS(minimumFPS);
            }
        }
    }

    double DirectShowCamera::getFPS() const
    {
        if (isOpening())
        {
            return m_sampleGrabberCallback->getFPS();
        }
        else
        {
            return 0;
        }
    }

    long DirectShowCamera::getFrameTotalSize() const
    {

        if (m_isCapturing)
        {
            int result = m_sampleGrabberCallback->getBufferSize();
            return result;
        }
        else
        {
            return 0;
        }
    }

    GUID DirectShowCamera::getFrameType() const
    {
        return m_grabberMediaSubType;
    }

#pragma endregion Frame

#pragma region Video Format

    void DirectShowCamera::UpdateGrabberFilterVideoFormat()
    {
        if (m_sampleGrabber != NULL)
        {
            // Get frame size
            int frameTotalSize = 0;
            GUID mediaSubType;
            DirectShowCameraUtils::AmMediaTypeDecorator(m_amStreamConfig,
                [this, &frameTotalSize, &mediaSubType](AM_MEDIA_TYPE* mediaType)
                {
                    VIDEOINFOHEADER* videoInfoHeader = reinterpret_cast<VIDEOINFOHEADER*>(mediaType->pbFormat);
                    int width = videoInfoHeader->bmiHeader.biWidth;
                    int height = videoInfoHeader->bmiHeader.biHeight;

                    if (m_convertOutputDataToRGB24IfSupported && DirectShowVideoFormatUtils::isSupportRGBConvertion(mediaType->subtype))
                    {
                        // Transform to RGB in the grabber filter
                        frameTotalSize = width * height * 3;
                        mediaSubType = MEDIASUBTYPE_RGB24;
                    }
                    else
                    {
                        // Do not convert
                        frameTotalSize = mediaType->lSampleSize;
                        mediaSubType = mediaType->subtype;
                    }
                },
                m_errorString
            );

            // Set grabber media type
            AM_MEDIA_TYPE grabberMediaType;
            ZeroMemory(&grabberMediaType, sizeof(grabberMediaType));

            grabberMediaType.majortype = MEDIATYPE_Video;
            grabberMediaType.formattype = FORMAT_VideoInfo;
            grabberMediaType.subtype = mediaSubType;
            grabberMediaType.lSampleSize = frameTotalSize;

            HRESULT hr = m_sampleGrabber->SetMediaType(&grabberMediaType);
            if (hr == S_OK)
            {
                m_grabberMediaSubType = mediaSubType;

                // get video format of grabber filter - this can fail if the graph is not yet connected
                hr = m_sampleGrabber->GetConnectedMediaType(&grabberMediaType);
                if (hr == S_OK)
                {
                    m_sampleGrabberVideoFormat = DirectShowVideoFormat::Create(&grabberMediaType);
                }

                // Set buffer size
                m_sampleGrabberCallback->setBufferSize(mediaSubType, frameTotalSize);
            }
            else
            {
                m_errorString = "Could not set media type to RGB24.(hr = " + std::to_string(hr) + ").";
            }

            DirectShowCameraUtils::FreeMediaType(grabberMediaType);
        }	
    }

    bool DirectShowCamera::UpdateVideoFormatList()
    {
        bool result = false;
        if (m_amStreamConfig != NULL)
        {
            result = m_videoFormats.Update(m_amStreamConfig, m_errorString);
        }
        else
        {
            result = false;
        }

        return result;
    }

    void DirectShowCamera::UpdateVideoFormatIndex()
    {
        if (m_videoFormats.Size() > 0)
        {
            DirectShowCameraUtils::AmMediaTypeDecorator(m_amStreamConfig,
                [this](AM_MEDIA_TYPE* mediaType)
                {
                    m_currentVideoFormatIndex = getVideoFormatIndex(mediaType);
                },
                m_errorString
            );
        }
        else
        {
            m_currentVideoFormatIndex = -1;
        }
    }

    int DirectShowCamera::getVideoFormatIndex(const AM_MEDIA_TYPE* mediaType) const
    {
        int result = -1;
        for (int i = 0; i < m_videoFormats.Size(); i++)
        {
            if (m_videoFormats.getAMMediaType(i) == mediaType)
            {
                result = i;
                break;
            }
        }

        return result;
    }

    int DirectShowCamera::getVideoFormatIndex(const DirectShowVideoFormat videoFormat) const
    {
        int result = -1;
        for (int i = 0; i < m_videoFormats.Size(); i++)
        {
            if (m_videoFormats.getVideoFormat(i) == videoFormat)
            {
                result = i;
                break;
            }
        }

        return result;
    }

    int DirectShowCamera::getCurrentVideoFormatIndex() const
    {
        return m_currentVideoFormatIndex;
    }

    DirectShowVideoFormat DirectShowCamera::getCurrentVideoFormat() const
    {
        if (m_currentVideoFormatIndex >= 0)
        {
            return m_videoFormats.getVideoFormat(m_currentVideoFormatIndex);
        }
        else
        {
            return DirectShowVideoFormat();
        }
        
    }

    DirectShowVideoFormat DirectShowCamera::getCurrentGrabberFormat() const
    {
        return m_sampleGrabberVideoFormat;
    }

    std::vector<DirectShowVideoFormat> DirectShowCamera::getVideoFormatList() const
    {
        return m_videoFormats.getVideoFormatList();
    }

    bool DirectShowCamera::setVideoFormat(const DirectShowVideoFormat videoFormat)
    {
        bool result = false;

        // Update video format list
        UpdateVideoFormatList();

        // Get current video format index
        int index = getVideoFormatIndex(videoFormat);

        // Set video format
        if (index >= 0)
        {
            result = setVideoFormat(index);
        }

        return result;
    }

    bool DirectShowCamera::setVideoFormat(const int videoFormatIndex)
    {
        bool result = true;
        HRESULT hr = NO_ERROR;

        if (result)
        {
            if (m_amStreamConfig == NULL)
            {
                // Error, not initialized
                result = false;
                m_errorString = "Graph is not initialized, please call initialize().";
            }
            else if (videoFormatIndex < 0 || videoFormatIndex >= m_videoFormats.Size())
            {
                // Error, index is out of range
                result = false;
                m_errorString = "Video format index("+ std::to_string(videoFormatIndex) +") is out of range(0," + std::to_string(m_videoFormats.Size()) + ").";
            }
            else
            {
                // Set
                hr = m_amStreamConfig->SetFormat(m_videoFormats.getAMMediaType(videoFormatIndex));
                result = CheckHResultUtils::CheckIIAMSCSetFormatResult(hr, m_errorString, "Error on setting camera resolution");

                if (result)
                {
                    // Update current video format
                    UpdateGrabberFilterVideoFormat();

                    // Update current video format index
                    m_currentVideoFormatIndex = videoFormatIndex;
                }
            }
        }

        return result;
    }

#pragma endregion Video Format

#pragma region Properties

    void DirectShowCamera::RefreshProperties()
    {
        if (m_isOpening && m_property != nullptr)
        {
            m_property->Refresh(m_directShowFilter, m_errorString);
        }
    }

    std::shared_ptr<DirectShowCameraProperties> DirectShowCamera::getProperties() const
    {
        return m_property;
    }

    void DirectShowCamera::ResetPropertiesToDefault(const bool asAuto)
    {
        if (m_directShowFilter != NULL && m_property != nullptr)
        {
            m_property->ResetToDefaultValue(m_directShowFilter, m_errorString, asAuto);
        }
    }

    bool DirectShowCamera::setPropertyValue(
        std::shared_ptr<DirectShowCameraProperty>& property,
        const long value,
        const bool isAuto
    )
    {
        if (m_directShowFilter != NULL)
        {
            return property->setValue(m_directShowFilter, value, isAuto, m_errorString);
        }
        else
        {
            m_errorString = "Error on setting " + property->getName() + " property: Camera is not open.";
            return false;
        }
    }

#pragma endregion Properties

#pragma region getCamera

    bool DirectShowCamera::getCameras(std::vector<DirectShowCameraDevice>& cameraDevices)
    {
        // Initialize and clear
        cameraDevices.clear();

        bool success = DirectShowCameraUtils::IPropertyDecorator(
            CLSID_VideoInputDeviceCategory,
            [this, &cameraDevices](IMoniker* moniker, IPropertyBag* propertyBag)
            {
                // Initialize variables
                HRESULT hr;
                VARIANT var;
                VariantInit(&var);

                std::string description = "";
                std::string friendlyName = "";
                std::string devicePath = "";

                // Get description
                hr = propertyBag->Read(L"Description", &var, 0);
                if (hr == S_OK)
                {
                    description = Win32Utils::BSTRToString(var.bstrVal);
                    VariantClear(&var);
                }

                // Get friendly name
                hr = propertyBag->Read(L"FriendlyName", &var, 0);
                if (hr == S_OK)
                {
                    friendlyName = Win32Utils::BSTRToString(var.bstrVal);
                    VariantClear(&var);
                }

                // Get device path
                hr = propertyBag->Read(L"DevicePath", &var, 0);
                if (hr == S_OK)
                {
                    // Get device path
                    devicePath = Win32Utils::BSTRToString(var.bstrVal);
                    VariantClear(&var);
                }

                // Get all video format
                std::vector<DirectShowVideoFormat> videoFormats;
                DirectShowCameraUtils::IPinDecorator(
                    moniker,
                    [this, &videoFormats](IPin* iPin, PIN_INFO* pinInfo)
                    {
                        DirectShowCameraUtils::AmMediaTypeDecorator(
                            iPin,
                            [this, &videoFormats](AM_MEDIA_TYPE* amMediaType)
                            {
                                videoFormats.push_back(DirectShowVideoFormat::Create(amMediaType));
                            },
                            m_errorString
                        );
                    },
                    m_errorString
                );

                // Sort and erase duplicates
                if (videoFormats.size() > 0)
                {
                    DirectShowVideoFormatList::SortAndUnique(videoFormats);
                }

                // Push into device
                cameraDevices.push_back(DirectShowCameraDevice(friendlyName, description, devicePath, videoFormats));
            },
            m_errorString
        );

        return success;
    }

    bool DirectShowCamera::getCamera(const int cameraIndex, IBaseFilter** directShowFilter)
    {
        int count = 0;
        bool found = false;
        bool success = DirectShowCameraUtils::IPropertyDecorator(
            CLSID_VideoInputDeviceCategory,
            [&directShowFilter, &cameraIndex, &count, &found](IMoniker* moniker, IPropertyBag* propertyBag)
            {
                // Found, obtain the DirectShow Filter
                if (count == cameraIndex)
                {
#pragma warning( push )
#pragma warning( disable : 6387)
                    moniker->BindToObject(NULL, NULL, IID_IBaseFilter, (void**)directShowFilter);
#pragma warning( pop )
                    found = true;
                }

                // Move to next
                count++;
            },
            m_errorString
        );

        return success && found;
    }

    bool DirectShowCamera::getCamera(const std::string devicePath, IBaseFilter** directShowFilter)
    {
        bool found = false;
        bool success = DirectShowCameraUtils::IPropertyDecorator(
            CLSID_VideoInputDeviceCategory,
            [&directShowFilter, &devicePath, &found](IMoniker* moniker, IPropertyBag* propertyBag)
            {
                // Initialize variables
                HRESULT hr;
                VARIANT var;
                VariantInit(&var);

                // Get device path
                hr = propertyBag->Read(L"DevicePath", &var, 0);
                if (hr == S_OK)
                {
                    std::string currentDevicePath = Win32Utils::BSTRToString(var.bstrVal);
                    VariantClear(&var);

                    // Found, obtain the DirectShow Filter
                    if (currentDevicePath == devicePath)
                    {
                        // Get device name
#pragma warning( push )
#pragma warning( disable : 6387)
                        moniker->BindToObject(NULL, NULL, IID_IBaseFilter, (void**)directShowFilter);
#pragma warning( pop )
                        found = true;
                    }
                }
            },
            m_errorString
        );

        return success && found;
    }

    bool DirectShowCamera::getCamera(const DirectShowCameraDevice device, IBaseFilter** directShowFilter)
    {
        // try to match via device path first, fallback to friendly name
        bool found = getCamera(device.getDevicePath(), directShowFilter);
        if (!found)
        {
            DirectShowCameraUtils::IPropertyDecorator(
                CLSID_VideoInputDeviceCategory,
                [&directShowFilter, &device, &found](IMoniker* moniker, IPropertyBag* propertyBag)
                {
                    // Initialize variables
                    HRESULT hr;
                    VARIANT var;
                    VariantInit(&var);

                    // Get friendly name
                    hr = propertyBag->Read(L"FriendlyName", &var, 0);
                    if (hr == S_OK)
                    {
                        std::string friendly_name = Win32Utils::BSTRToString(var.bstrVal);
                        VariantClear(&var);

                        if (friendly_name == device.getFriendlyName())
                        {
#pragma warning( push )
#pragma warning( disable : 6387)
                            moniker->BindToObject(NULL, NULL, IID_IBaseFilter, (void**)directShowFilter);
#pragma warning( pop )
                            found = true;
                        }
                    }
                },
                m_errorString
            );
        }
        return found;
    }

#pragma endregion getCamera

    void DirectShowCamera::ResetLastError()
    {
        m_errorString.clear();
    }

    std::string DirectShowCamera::getLastError() const
    {
        return m_errorString;
    }
}