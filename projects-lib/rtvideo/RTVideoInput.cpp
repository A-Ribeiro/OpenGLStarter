#include "RTVideoInput.h"

#include <limits.h>
#include <float.h>

#ifdef __APPLE__
    #include <rtvideo/avfoundation.h>
#endif

#ifdef WIN32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <dshow.h>

//Sample Grabber Method
interface ISampleGrabberCB : public IUnknown
{
    virtual STDMETHODIMP SampleCB(double SampleTime, IMediaSample *pSample) = 0;
    virtual STDMETHODIMP BufferCB(double SampleTime, BYTE *pBuffer, long BufferLen) = 0;
};

interface ISampleGrabber : public IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE SetOneShot(BOOL OneShot) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetMediaType(const AM_MEDIA_TYPE *pType) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetConnectedMediaType(AM_MEDIA_TYPE *pType) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetBufferSamples(BOOL BufferThem) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCurrentBuffer(long *pBufferSize, long *pBuffer) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCurrentSample(IMediaSample **ppSample) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetCallback(ISampleGrabberCB *pCallback, long WhichMethodToCallback) = 0;
};

static const IID IID_ISampleGrabber = 
{ 0x6B652FFF, 0x11FE, 0x4fce, { 0x92, 0xAD, 0x02, 0x66, 0xB5, 0xD7, 0xC7, 0x8F } };
static const IID IID_ISampleGrabberCB = 
{ 0x0579154A, 0x2B53, 0x4994, { 0xB0, 0xD0, 0xE7, 0x73, 0x14, 0x8E, 0xFF, 0x85 } };
static const CLSID CLSID_SampleGrabber = 
{ 0xC1F400A0, 0x3F08, 0x11d3, { 0x9F, 0x0B, 0x00, 0x60, 0x08, 0x03, 0x9E, 0x37 } };
static const CLSID CLSID_NullRenderer =
{ 0xc1f400a4, 0x3f08, 0x11d3, {0x9f, 0x0b, 0x00, 0x60, 0x08, 0x03, 0x9e, 0x37} };



class SamplerCallbackImplementation :public ISampleGrabberCB {
public:
    aRibeiro::aribeiro_OnDataMethodPtrType OnData;

    virtual STDMETHODIMP SampleCB(double SampleTime, IMediaSample *pSample) {
        //printf("s");

        BYTE* data;
        pSample->GetPointer(&data);
        LONG size = pSample->GetActualDataLength();

        OnData((uint8_t*)data, size);

        return S_OK;
    }
    virtual STDMETHODIMP BufferCB(double SampleTime, BYTE *pBuffer, long BufferLen) {
        //printf("b");
        return S_OK;
    }

    void SetOnData(const aRibeiro::aribeiro_OnDataMethodPtrType &OnData) {
        this->OnData = OnData;
    }

    STDMETHODIMP_(ULONG) AddRef() { 
        return 1; 
    }

    STDMETHODIMP_(ULONG) Release() { 
        return 2; 
    }
    
    STDMETHODIMP QueryInterface(REFIID type, void **ppvObject) {
        if (IsEqualIID(IID_ISampleGrabberCB, type)) {
            *ppvObject = static_cast<ISampleGrabberCB*>(this);
            return S_OK;
        }
        return VFW_E_NO_INTERFACE;
    }
};



HRESULT EnumerateDevices(REFGUID category, IEnumMoniker **ppEnum)
{
    // Create the System Device Enumerator.
    ICreateDevEnum *pDevEnum;
    HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL,
        CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (LPVOID *)&pDevEnum);

    if (SUCCEEDED(hr))
    {
        // Create an enumerator for the category.
        hr = pDevEnum->CreateClassEnumerator(category, ppEnum, 0);
        if (hr == S_FALSE)
        {
            hr = VFW_E_NOT_FOUND;  // The category is empty. Treat as an error.
        }
        pDevEnum->Release();
    }
    return hr;
}

IMoniker *queryMoniker(std::wstring _devicePath) {
    IMoniker *aux = NULL;

    IEnumMoniker *pEnum;
    HRESULT hr = EnumerateDevices(CLSID_VideoInputDeviceCategory, &pEnum);
    if (SUCCEEDED(hr)) {

        IMoniker *pMoniker = NULL;
        while (pEnum->Next(1, &pMoniker, NULL) == S_OK)
        {
            IPropertyBag *pPropBag;
            hr = pMoniker->BindToStorage(0, 0, IID_PPV_ARGS(&pPropBag));
            if (FAILED(hr))
            {
                pMoniker->Release();
                continue;
            }
            VARIANT var;
            VariantInit(&var);

            std::wstring originalDeviceName;
            std::wstring devicePath;

            hr = pPropBag->Read(L"FriendlyName", &var, 0);
            if (SUCCEEDED(hr)) {
                originalDeviceName = var.bstrVal;
                VariantClear(&var);
            }

            hr = pPropBag->Read(L"DevicePath", &var, 0);
            if (SUCCEEDED(hr))
            {
                // The device path is not intended for display.
                devicePath = var.bstrVal;
                VariantClear(&var);
            }

            pPropBag->Release();

            if (devicePath.compare(_devicePath) == 0) {
                aux = pMoniker;
                break;
            }
            pMoniker->Release();
        }
        pEnum->Release();
    }

    return aux;
}

void _FreeMediaType(AM_MEDIA_TYPE& mt)
{
    if (mt.cbFormat != 0)
    {
        CoTaskMemFree((PVOID)mt.pbFormat);
        mt.cbFormat = 0;
        mt.pbFormat = NULL;
    }
    if (mt.pUnk != NULL)
    {
        // pUnk should not be used.
        mt.pUnk->Release();
        mt.pUnk = NULL;
    }
}

HRESULT queryPinAndMediaType(IBaseFilter *pFilter, 
                            const GUID &subType,
                            const LONG &width, const LONG &height,
                            const REFERENCE_TIME &AvgTimePerFrame,
                            IPin **pin_result, AM_MEDIA_TYPE **mediaType_result ) {

    *mediaType_result = NULL;
    *pin_result = NULL;

    IEnumPins *EnumPins;

    HRESULT hr = pFilter->EnumPins(&EnumPins);
    if (SUCCEEDED(hr)) {
        IPin *pin;
        while (EnumPins->Next(1, &pin, NULL) == S_OK) {

            PIN_INFO pInfo;
            hr = pin->QueryPinInfo(&pInfo);
            if (SUCCEEDED(hr)) {
                if (pInfo.dir == PINDIR_OUTPUT) {

                    IEnumMediaTypes *emt = NULL;
                    hr = pin->EnumMediaTypes(&emt);
                    if (SUCCEEDED(hr)) {
                        AM_MEDIA_TYPE* mediaType = NULL;
                        while (emt->Next(1, &mediaType, NULL) == S_OK) {

                            if ((mediaType->formattype == FORMAT_VideoInfo) &&
                                // filter just devices that writes H264
                                (mediaType->subtype == subType) &&
                                (mediaType->cbFormat >= sizeof(VIDEOINFOHEADER)) &&
                                (mediaType->pbFormat != NULL)) {

                                VIDEOINFOHEADER *pVIH = (VIDEOINFOHEADER*)mediaType->pbFormat;

                                if (pVIH->bmiHeader.biWidth == width &&
                                    pVIH->bmiHeader.biHeight == height &&
                                    pVIH->AvgTimePerFrame == AvgTimePerFrame) {

                                    *mediaType_result = mediaType;
                                    *pin_result = pin;
                                    break;
                                }

                            }
                            _FreeMediaType(*mediaType);
                        }
                        emt->Release();
                    }
                }
            }
            //release if does not found PIN with a MediaType...
            if (*pin_result == NULL)
                pin->Release();
        }
        EnumPins->Release();
    }

    return pin_result == NULL;
}

void Double2Fract_inverse(double f, uint32_t *numerator, uint32_t *denominator) {
    uint32_t lUpperPart = 1;
    uint32_t lLowerPart = 1;

    double df = (double)lUpperPart / (double)lLowerPart;

    while ( aRibeiro::absv(df - f) > 2e-8 ) {
        if (df < f) {
            lUpperPart = lUpperPart + 1;
        } else {
            lLowerPart = lLowerPart + 1;
            lUpperPart = (uint32_t)(f * (double)lLowerPart);
        }
        df = (double)lUpperPart / (double)lLowerPart;
    }
    
    //inverse, because the dshow supply FPS
    *numerator = lLowerPart;
    *denominator = lUpperPart;
}



void ListResolutions(IEnumMediaTypes *mediaTypesEnumerator) {
    AM_MEDIA_TYPE* mediaType = NULL;
    VIDEOINFOHEADER* videoInfoHeader = NULL;
    while (S_OK == mediaTypesEnumerator->Next(1, &mediaType, NULL))
    {
        if ((mediaType->formattype == FORMAT_VideoInfo) &&
            (mediaType->cbFormat >= sizeof(VIDEOINFOHEADER)) &&
            (mediaType->pbFormat != NULL))
        {
            videoInfoHeader = (VIDEOINFOHEADER*)mediaType->pbFormat;
            videoInfoHeader->bmiHeader.biWidth;  // Supported width
            videoInfoHeader->bmiHeader.biHeight; // Supported height
        }
        _FreeMediaType(*mediaType);
    }
}

#endif


void RTVideoInput::printDevices() {
    RTVideoInput aux;
    std::vector<RTVideoInputDevice> &devices = aux.listDevices();

    printf("number of devices: %lu\n\n", devices.size());

    for (size_t i=0;i<devices.size();i++){
        printf("Name: %s\n", devices[i].name.c_str());
#ifdef __linux__
        printf("LinuxPath: %s\n", devices[i].device.path.c_str());
#endif
        printf("Supported Resolutions:\n");
        for (size_t j=0;j<devices[i].resolutions.size();j++){
#ifdef __APPLE__
            printf("   %ux%u (%s) :", devices[i].resolutions[j].width, devices[i].resolutions[j].height, devices[i].resolutions[j].codec_code.c_str());
#else
            printf("   %ux%u :", devices[i].resolutions[j].width, devices[i].resolutions[j].height);
#endif
            for (size_t k=0;k<devices[i].resolutions[j].framerates.size();k++){
                printf("@%f ", devices[i].resolutions[j].framerates[k].fps);
            }
            printf("\n");
        }
        printf("\n");
    }
}

std::vector<RTVideoInputDevice> &RTVideoInput::listDevices() {
    static std::vector<RTVideoInputDevice> result;

    if (result.size() > 0)
        return result;

#if defined(WIN32)

    //HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    HRESULT hr = 0;
    CoInitializeEx(NULL, COINIT_MULTITHREADED);
    //CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

    /*
    ICreateDevEnum *pSysDevEnum = NULL;
    hr = CoCreateInstance(CLSID_SystemDeviceEnum, 
        NULL, 
        CLSCTX_INPROC_SERVER, 
        IID_ICreateDevEnum, 
        (void **)&pSysDevEnum);
    */
    if (SUCCEEDED(hr))
    {
        IEnumMoniker *pEnum;
        hr = EnumerateDevices(CLSID_VideoInputDeviceCategory, &pEnum);
        if (SUCCEEDED(hr)) {

            IMoniker *pMoniker = NULL;

            while (pEnum->Next(1, &pMoniker, NULL) == S_OK)
            {
                IPropertyBag *pPropBag;
                hr = pMoniker->BindToStorage(0, 0, IID_PPV_ARGS(&pPropBag));
                if (FAILED(hr))
                {
                    pMoniker->Release();
                    continue;
                }
                VARIANT var;
                VariantInit(&var);

                std::string deviceName;
                std::wstring originalDeviceName;
                std::wstring devicePath;

                hr = pPropBag->Read(L"FriendlyName", &var, 0);
                if (SUCCEEDED(hr)) {
                    deviceName = aRibeiro::StringUtil::toString(var.bstrVal);
                    originalDeviceName = var.bstrVal;
                    VariantClear(&var);
                }
                hr = pPropBag->Read(L"DevicePath", &var, 0);
                if (SUCCEEDED(hr))
                {
                    devicePath = var.bstrVal;
                    VariantClear(&var);
                }

                RTVideoInputDevice newDevice;
                newDevice.name = deviceName;
                newDevice.originalDeviceName = originalDeviceName;
                newDevice.devicePath = devicePath;

                //bind filter to moniker
                IBaseFilter *pFilter;
                hr = pMoniker->BindToObject(NULL, NULL, IID_IBaseFilter, (void**)&pFilter);
                //process filter to get the resolutions
                {
                    IEnumPins *EnumPins;
                    hr = pFilter->EnumPins(&EnumPins);
                    if (SUCCEEDED(hr)) {
                        IPin *pin;
                        while (EnumPins->Next(1, &pin, NULL) == S_OK) {

                            PIN_INFO pInfo;
                            hr = pin->QueryPinInfo(&pInfo);
                            if (SUCCEEDED(hr)) {
                                if (pInfo.dir == PINDIR_OUTPUT) {

                                    IEnumMediaTypes *emt = NULL;
                                    hr = pin->EnumMediaTypes(&emt);
                                    if (SUCCEEDED(hr)) {
                                        AM_MEDIA_TYPE* mediaType = NULL;
                                        while (emt->Next(1, &mediaType, NULL) == S_OK) {

                                            if ((mediaType->formattype == FORMAT_VideoInfo) &&
                                                // filter just devices that writes H264
                                                (mediaType->subtype == MEDIASUBTYPE_H264) &&
                                                (mediaType->cbFormat >= sizeof(VIDEOINFOHEADER)) &&
                                                (mediaType->pbFormat != NULL)) {

                                                VIDEOINFOHEADER *pVIH = (VIDEOINFOHEADER*)mediaType->pbFormat;

                                                RTVideoInputDeviceResolution res;
                                                res.width = pVIH->bmiHeader.biWidth;
                                                res.height = pVIH->bmiHeader.biHeight;

                                                double FPS =
                                                    floor(10000000.0 / 
                                                        static_cast<double>(pVIH->AvgTimePerFrame));

                                                RTVideoInputDeviceFramerate framerate;

                                                framerate.AvgTimePerFrame = pVIH->AvgTimePerFrame;
                                                //framerate.numerator = (uint32_t)FPS;
                                                //framerate.denominator = 1.0f;
                                                Double2Fract_inverse(FPS, &framerate.numerator, &framerate.denominator);
                                                framerate.fps = FPS;


                                                res.framerates.push_back(framerate);

                                                newDevice.resolutions.push_back(res);
                                            }

                                            _FreeMediaType(*mediaType);
                                        }
                                        emt->Release();
                                    }
                                }
                            }
                            pin->Release();
                        }

                        EnumPins->Release();
                    }
                }

                result.push_back(newDevice);

                pPropBag->Release();
                pMoniker->Release();
            }

            pEnum->Release();
        }
        //pSysDevEnum->Release();
    }


    //post process the video resolutions
    for (int i = (int)result.size() - 1; i >= 0; i--) {
        if (result[i].resolutions.size() == 0) {
            result.erase(result.begin() + i);
            continue;
        }

        std::vector<RTVideoInputDeviceResolution> resolutions = result[i].resolutions;

        result[i].resolutions.clear();

        //check all duplicate resolutions
        for (int j = 0; j < resolutions.size(); j++) {
            RTVideoInputDeviceResolution &toInsert = resolutions[j];
            bool foundResolution = false;
            for (int k = 0; k < result[i].resolutions.size(); k++) {
                if (result[i].resolutions[k].width == toInsert.width && 
                    result[i].resolutions[k].height == toInsert.height) {
                    foundResolution = true;
                    result[i].resolutions[k].framerates.insert(
                        result[i].resolutions[k].framerates.end(), 
                        toInsert.framerates.begin(), toInsert.framerates.end());
                }
            }
            if (!foundResolution) {
                result[i].resolutions.push_back(toInsert);
            }
        }
    }



#elif defined(__linux__)

    std::vector<Device> v4l2Devices = v4l2::listDevices();

    for (size_t i=0;i<v4l2Devices.size();i++){
        //check for input devices with capture and streaming flags
        uint32_t bitFlag = V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_STREAMING;

        if ( ( v4l2Devices[i].capability.capabilities & bitFlag ) == bitFlag )
        {

            //check H264 reading format
            v4l2_fmtdesc h264_v4l2_fmtdesc;
            if (v4l2Devices[i].queryPixelFormat(V4L2_PIX_FMT_H264, &h264_v4l2_fmtdesc) ){

                RTVideoInputDevice newDevice;
                newDevice.device = v4l2Devices[i];
                newDevice.name = (char*)newDevice.device.capability.card;
                newDevice.h264_v4l2_fmtdesc = h264_v4l2_fmtdesc;

                std::vector<v4l2_frmsizeenum> allresolutions = v4l2::getFramesizes( v4l2Devices[i].path, h264_v4l2_fmtdesc );
                for (size_t j=0; j<allresolutions.size();j++){
                    RTVideoInputDeviceResolution res;
                    res._v4l2_frmsizeenum = allresolutions[j];
                    res.width = allresolutions[j].discrete.width;
                    res.height = allresolutions[j].discrete.height;


                    std::vector<v4l2_frmivalenum> frameIntervals = v4l2::getFrameIntervals(v4l2Devices[i].path, allresolutions[j]);

                    for (size_t k=0;k<frameIntervals.size();k++){
                        RTVideoInputDeviceFramerate framerate;

                        framerate._v4l2_frmivalenum = frameIntervals[k];
                        framerate.numerator = frameIntervals[k].discrete.numerator;
                        framerate.denominator = frameIntervals[k].discrete.denominator;
                        framerate.fps = (1.0 * framerate.denominator) / framerate.numerator;

                        res.framerates.push_back(framerate);
                    }

                    newDevice.resolutions.push_back(res);
                }
                result.push_back(newDevice);
            }
        }
    }

#elif defined(__APPLE__)
    
    std::vector<AF_Device> avFoundationDevices = AF_Device::listDevices();
    
    for (size_t i=0;i<avFoundationDevices.size();i++) {
        AF_Device &af_device = avFoundationDevices[i];
        RTVideoInputDevice newDevice;
        newDevice.name = af_device.name;
        
        for (size_t j=0;j<af_device.resolutions.size();j++){
            AF_DeviceResolution &af_resolution = af_device.resolutions[j];
            RTVideoInputDeviceResolution res;
            res.width = af_resolution.width;
            res.height = af_resolution.height;
            res.codec_code = af_resolution.codec_code;
            
            for(size_t k=0;k<af_resolution.framerates.size();k++){
                AF_DeviceFramerate &af_framerate = af_resolution.framerates[k];
                RTVideoInputDeviceFramerate framerate;
                
                framerate.fps = af_framerate.fps;
                framerate.numerator = af_framerate.numerator;
                framerate.denominator = af_framerate.denominator;
                
                res.framerates.push_back(framerate);
                
            }
            
            newDevice.resolutions.push_back(res);
        }
        
        result.push_back(newDevice);
    }

#else
        //Not implemented OS
    printf("ERROR: Not Implemented OS ListDevices\n");
#endif

    return result;
}


RTVideoInput::RTVideoInput(): streamingThread(this, &RTVideoInput::streamingThreadRun) {
    selectedDevice = NULL;
    selectedResolution = NULL;
    selectedFramerate = NULL;
}

RTVideoInput::~RTVideoInput() {
    close();
}


void RTVideoInput::close() {

    streamingThread.interrupt();
    streamingThread.wait();

    if (selectedDevice != NULL){
        selectedDevice = NULL;
        selectedResolution = NULL;
        selectedFramerate = NULL;
    }
}


RTVideoInputDeviceResolution *RTVideoInput::queryNearResolution(RTVideoInputDevice *device,
                                                                uint32_t width,
                                                                uint32_t height) {
    RTVideoInputDeviceResolution *result = NULL;
    int dst = INT_MAX;
    
    for (size_t i=0;i<device->resolutions.size();i++) {
        RTVideoInputDeviceResolution *resolution = &device->resolutions[i];
        int w2 = (int)resolution->width - (int)width;
        int h2 = (int)resolution->height - (int)height;
        int auxdst = w2 * w2 + h2 * h2;
        if (auxdst < dst) {
            result = resolution;
            dst = auxdst;
        }
    }
    
    return result;
}


RTVideoInputDeviceFramerate *RTVideoInput::queryNearInterval(RTVideoInputDeviceResolution *resolution,
                                                             float fps) {
    RTVideoInputDeviceFramerate *result = NULL;
    float dst = FLT_MAX;
    
    for (size_t i=0;i<resolution->framerates.size();i++) {
        RTVideoInputDeviceFramerate *framerate = &resolution->framerates[i];
        float dstaux = framerate->fps - fps;
        dstaux = dstaux * dstaux;
        if ( dstaux < dst ){
            result = framerate;
            dst = dstaux;
        }
    }
    
    return result;
}


void RTVideoInput::open(const std::string deviceName, uint32_t width, uint32_t height, float FPS,
    aRibeiro::aribeiro_OnDataMethodPtrType OnData
) {

    if (selectedDevice != NULL)
        return;

    this->OnData = OnData;

    std::vector<RTVideoInputDevice> &devices = listDevices();

    for (size_t i=0; i<devices.size();i++){
        if ( devices[i].name.compare(deviceName) == 0 ){
            selectedDevice = &devices[i];
            break;
        }
    }

    ARIBEIRO_ABORT( selectedDevice == NULL, "Device not found: %s", deviceName.c_str() );
    
    selectedResolution = queryNearResolution(selectedDevice, width, height );

    ARIBEIRO_ABORT( selectedResolution == NULL, "Resolution not found w:%u h:%u", width, height );

    selectedFramerate = queryNearInterval( selectedResolution, FPS );
    
    ARIBEIRO_ABORT( selectedFramerate == NULL, "Framerate not found %f FPS", FPS );

#ifdef __linux__
    fprintf(stdout,"VIDEO INPUT OPEN: %s (%s)\n",
            selectedDevice->name.c_str(),
            selectedDevice->device.path.c_str() );
#else
    fprintf(stdout,"VIDEO INPUT OPEN: %s\n",
            selectedDevice->name.c_str() );
#endif
    fprintf(stdout,"  %u x %u", selectedResolution->width, selectedResolution->height );
    fprintf(stdout," @ %f fps\n", selectedFramerate->fps );

    streamingThread.start();
}
#ifdef __APPLE__
void af_video_data_callback(const uint8_t *data, size_t size, void* userData){
    aRibeiro::aribeiro_OnDataMethodPtrType * OnData = (aRibeiro::aribeiro_OnDataMethodPtrType *)userData;
    OnData->operator()(data, size);
}
#endif

void RTVideoInput::streamingThreadRun() {

#if defined(__APPLE__)
    //RTVideoInputDevice *selectedDevice;
    //RTVideoInputDeviceResolution *selectedResolution;
    //RTVideoInputDeviceFramerate *selectedFramerate;
    
    AF_startStream( selectedDevice->name,
                   selectedResolution->width,
                   selectedResolution->height,
                   selectedResolution->codec_code,
                   selectedFramerate->fps,
                   &af_video_data_callback, &OnData );
    
    
#elif defined(WIN32)
    CoInitializeEx(NULL, COINIT_MULTITHREADED);

    //Create the Filter Graph Manager
    IGraphBuilder *pGraph = NULL;
    //ICaptureGraphBuilder2 *pBuild = NULL;
    IMediaControl *pControl = NULL;
    IMediaEventEx *pEvent = NULL;
    IMoniker *pMoniker = NULL;
    IBaseFilter *pCameraSourceFilter = NULL;
    IPin *pCameraOutputPin = NULL;
    AM_MEDIA_TYPE *pCameraOutputMediaType = NULL;
    IAMStreamConfig *pCameraStreamConfig = NULL;


    SamplerCallbackImplementation callbackSampler;

    ISampleGrabber *pSampleGrabber = NULL;
    IBaseFilter *pSampleGrabberFilter = NULL;
    IPin *pSampleGrabberInputPin = NULL;
    IPin *pSampleGrabberOutputPin = NULL;

    IBaseFilter *pNullRendererFilter = NULL;
    IPin *pNullRendererInputPin = NULL;

    HRESULT hr;

    hr = CoCreateInstance(CLSID_FilterGraph, NULL,
        CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (LPVOID *)&pGraph);
    ARIBEIRO_ABORT(FAILED(hr), "Error to create IGraphBuilder");

    //hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, 
    //    CLSCTX_INPROC_SERVER, IID_ICaptureGraphBuilder2, (LPVOID *)&pBuild);
    //ARIBEIRO_ABORT(FAILED(hr), "Error to create CLSID_CaptureGraphBuilder2");

    //pBuild->SetFiltergraph(pGraph);
    
    hr = pGraph->QueryInterface(IID_IMediaControl, (LPVOID *)&pControl);
    ARIBEIRO_ABORT(FAILED(hr), "Error to get IMediaControl");
    
    hr = pGraph->QueryInterface(IID_IMediaEventEx, (LPVOID *)&pEvent);
    ARIBEIRO_ABORT(FAILED(hr), "Error to get IMediaEventEx");

    pMoniker = queryMoniker(selectedDevice->devicePath);
    ARIBEIRO_ABORT(pMoniker == NULL, "Failed to get moniker");

    hr = pMoniker->BindToObject(0, 0, IID_IBaseFilter, (LPVOID *)&pCameraSourceFilter);
    ARIBEIRO_ABORT(FAILED(hr), "Error to bind base filter");

    hr = queryPinAndMediaType(pCameraSourceFilter, MEDIASUBTYPE_H264,
        selectedResolution->width, selectedResolution->height,
        selectedFramerate->AvgTimePerFrame,
        &pCameraOutputPin, &pCameraOutputMediaType);
    ARIBEIRO_ABORT(FAILED(hr), "queryPinAndMediaType()");

    hr = pCameraOutputPin->QueryInterface(&pCameraStreamConfig);
    ARIBEIRO_ABORT(FAILED(hr), "pCameraOutputPin->QueryInterface(&pCameraStreamConfig)");

    pCameraStreamConfig->SetFormat(pCameraOutputMediaType);
    
    hr = pGraph->AddFilter(pCameraSourceFilter, L"Capture Filter");
    ARIBEIRO_ABORT(FAILED(hr), "pGraph->AddFilter Capture Filter failed");

    //
    // Custom Renderer filter
    //

    
    hr = CoCreateInstance(CLSID_SampleGrabber, NULL,
        CLSCTX_INPROC_SERVER, IID_IBaseFilter, (LPVOID *)&pSampleGrabberFilter);
    ARIBEIRO_ABORT(FAILED(hr), "create pSampleGrabberFilter failed");

    hr = pGraph->AddFilter(pSampleGrabberFilter, L"Process Filter");
    ARIBEIRO_ABORT(FAILED(hr), "pGraph->AddFilter Process Filter failed");

    hr = pSampleGrabberFilter->FindPin(L"In", &pSampleGrabberInputPin);
    ARIBEIRO_ABORT(FAILED(hr), "pSampleGrabberInputPin failed");

    hr = pSampleGrabberFilter->FindPin(L"Out", &pSampleGrabberOutputPin);
    ARIBEIRO_ABORT(FAILED(hr), "pSampleGrabberOutputPin failed");


    hr = pSampleGrabberFilter->QueryInterface(IID_ISampleGrabber, (LPVOID *)&pSampleGrabber);
    ARIBEIRO_ABORT(FAILED(hr), "Query pSampleGrabber failed");

    pSampleGrabber->SetOneShot(FALSE);
    pSampleGrabber->SetBufferSamples(FALSE);
    //Tell the grabber to use our callback function - 0 is for SampleCB and 1 for BufferCB
    pSampleGrabber->SetCallback( &callbackSampler, 0 );
    callbackSampler.SetOnData(OnData);

    pSampleGrabber->SetMediaType(pCameraOutputMediaType);

    hr = CoCreateInstance(CLSID_NullRenderer, 
        NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (LPVOID *)&pNullRendererFilter);
    ARIBEIRO_ABORT(FAILED(hr), "CLSID_NullRenderer failed");
    
    hr = pGraph->AddFilter(pNullRendererFilter, L"NullRenderer");
    ARIBEIRO_ABORT(FAILED(hr), "pGraph->AddFilter NullRenderer failed");

    hr = pNullRendererFilter->FindPin(L"In", &pNullRendererInputPin);
    ARIBEIRO_ABORT(FAILED(hr), "pNullRendererFilter->FindPin Input failed");

    hr = pGraph->Connect(pCameraOutputPin, pSampleGrabberInputPin);
    ARIBEIRO_ABORT(FAILED(hr), "pGraph->Connect(pCameraOutputPin,pSampleGrabberInputPin) failed");

    hr = pGraph->Connect(pSampleGrabberOutputPin, pNullRendererInputPin);
    ARIBEIRO_ABORT(FAILED(hr), "pGraph->Connect(pSampleGrabberOutputPin,pNullRendererInputPin) failed");

    //construct the whole filter chain... including any necessary decompression
    hr = pGraph->Render(pCameraOutputPin);
    ARIBEIRO_ABORT(FAILED(hr), "pGraph->Render(pCameraOutputPin) failed");

    hr = pControl->Run();
    ARIBEIRO_ABORT(FAILED(hr), "pControl->Run() failed");

    while (!aRibeiro::PlatformThread::isCurrentThreadInterrupted()) {
        aRibeiro::PlatformSleep::sleepMillis(1);
    }

    hr = pControl->Stop();
    ARIBEIRO_ABORT(FAILED(hr), "pControl->Stop() failed");

    //long evCode;
    //hr = pEvent->WaitForCompletion(INFINITE, &evCode);
    //ARIBEIRO_ABORT(FAILED(hr), "pEvent->WaitForCompletion(INFINITE, &evCode) failed");


    //pGraph->Disconnect(pSamplerInputPin);
    //pGraph->RemoveFilter(sampler);


    pNullRendererInputPin->Release();
    pNullRendererFilter->Release();

    pSampleGrabberOutputPin->Release();
    pSampleGrabberInputPin->Release();
    pSampleGrabberFilter->Release();
    pSampleGrabber->Release();

    pCameraStreamConfig->Release();
    _FreeMediaType(*pCameraOutputMediaType);
    pCameraOutputPin->Release();
    pCameraSourceFilter->Release();
    pMoniker->Release();
    pEvent->Release();
    pControl->Release();
    //pBuild->Release();
    pGraph->Release();


#elif defined(__linux__)

    Device *device = &selectedDevice->device;

    device->open();

    device->setFormat(selectedDevice->h264_v4l2_fmtdesc, selectedResolution->_v4l2_frmsizeenum, selectedFramerate->_v4l2_frmivalenum );

    // buffer allocation and information retrieve
    const int BUFFER_COUNT = 3; // max 32

    device->setNumberOfInputBuffers(BUFFER_COUNT);

    v4l2_buffer bufferInfo[BUFFER_COUNT];
    void* bufferPtr[BUFFER_COUNT];

    for(int i=0;i<BUFFER_COUNT;i++) {
        bufferInfo[i] = device->getBufferInformationFromDevice(i);
        bufferPtr[i] = device->getBufferPointer(bufferInfo[i]);
        memset(bufferPtr[i], 0, bufferInfo[i].length);
        device->queueBuffer(i, &bufferInfo[i]);
    }

    //main loop
    device->streamON();

    v4l2_buffer bufferQueue;
    while ( !aRibeiro::PlatformThread::isCurrentThreadInterrupted() ){
        device->dequeueBuffer(&bufferQueue);

        OnData( (const uint8_t*)bufferPtr[bufferQueue.index], (uint32_t)bufferQueue.bytesused );

        device->queueBuffer(bufferQueue.index, &bufferQueue);
        //aRibeiro::PlatformSleep::sleepMillis(1);
    }

    device->streamOFF();
    device->close();

#else

    while ( !aRibeiro::PlatformThread::isCurrentThreadInterrupted() ){
        aRibeiro::PlatformSleep::sleepMillis(1);
    }

#endif

}
