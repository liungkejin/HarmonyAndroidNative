//
// Created by LiangKeJin on 2025/3/16.
//

#include "LibusbWindow.h"
#include <libusb.h>

#include "AOAProtocol.h"
#include "LibusbMgr.h"

using namespace znative;

LibusbMgr usbMgr;
std::list<LibusbDevice> devices;
bool needRefreshDevices = false;
AOAInfo aoaInfo = {
    "AILive Inc.",
    "AILive",
    "AILive demo",
    "1234567890",
    "1.0",
    "https://www.ailive.com"
};

void LibusbWindow::onAppInit(int width, int height) {
    usbMgr.setListener(this);
    usbMgr.initialize();
    devices = usbMgr.listDevices();
}

void LibusbWindow::onAppExit() {
    usbMgr.release();
}

void LibusbWindow::onPreRender(int width, int height) {
    if (needRefreshDevices) {
        devices = usbMgr.listDevices();
        needRefreshDevices = false;
    }
}

void LibusbWindow::onRenderImgui(int width, int height, ImGuiIO& io) {
    ImGui::Begin("LibusbWindow");
    // show devices list
    ImGui::Text("USB Device List:");
    ImGui::BeginChild("DevicesList", ImVec2(0, 100), true);
    for (auto& dev : devices) {
        ImGui::Text("%s", dev.name().c_str());
        ImGui::SameLine();
        if (dev.isOpened()) {
            if (ImGui::Button("Close")) {
                dev.close();
            }
        } else if (ImGui::Button("Open")) {
            dev.open();
        }
        ImGui::SameLine();
        if (ImGui::Button("Connect AOA")) {
            if (AOAProtocol::isAccessory(dev)) {
                AOAProtocol::openAccessory(dev);
            } else {
                if (AOAProtocol::connectAccessory(dev, aoaInfo)) {
                    needRefreshDevices = true;
                }
            }
        }
    }
    ImGui::EndChild();
    ImGui::End();
}

void LibusbWindow::onDevicePlug(LibusbDevice& dev) {
    needRefreshDevices = true;
}

void LibusbWindow::onDeviceUnplug(LibusbDevice& dev) {
    needRefreshDevices = true;
}


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "libusb.h"

#define BULK_RECV_EP    0x83
#define BULK_SEND_EP    0x02
#define INT_RECV_EP     0x81
#define INT_SEND_EP     0x01

#define VID_GOOGLE            0x18D1
#define    PID_AOA_ACC            0x2D00
#define    PID_AOA_ACC_ADB        0x2D01
#define    PID_AOA_AU            0x2D02
#define    PID_AOA_AU_ADB        0x2D03
#define    PID_AOA_ACC_AU        0x2D04
#define    PID_AOA_ACC_AU_ADB    0x2D05

static char manufacturer[] = "Lutixia";
static char modelName[] = "Demo";
static char description[] = "Android Aoa Interface";
static char version[] = "1.0";
static char uri[] = "https://www.baidu.com/";
static char serialNumber[] = "1234567890";


typedef struct
{
    unsigned int pid;
    unsigned int vid;
    unsigned char bInEndpointAddress;
    unsigned char bOutEndpointAddress;
    unsigned char bInterfaceNumber;
    libusb_device *dev;
    pthread_mutex_t stLock;
    libusb_device_handle *handle;
    libusb_hotplug_callback_handle hotplugCbh;
    int event;
    int bNumConfigurations;
    //libusb_device **devs;
} usb_dev_mngr_s;

static usb_dev_mngr_s gstUsbMngr;

void usb_error(int code, int line)
{
    fprintf(stdout,"line %d:", line);

    switch (code)
    {
        case LIBUSB_ERROR_IO:
            fprintf(stderr, "LIBUSB_ERROR_IO\n");
            break;
        case LIBUSB_ERROR_INVALID_PARAM:
            fprintf(stderr, "LIBUSB_ERROR_INVALID_PARAM\n");
            break;
        case LIBUSB_ERROR_ACCESS:
            fprintf(stderr, "Error: LIBUSB_ERROR_ACCESS\n");
            break;
        case LIBUSB_ERROR_NO_DEVICE:
            fprintf(stderr,    "LIBUSB_ERROR_NO_DEVICE\n");
            break;
        case LIBUSB_ERROR_NOT_FOUND:
            fprintf(stderr,    "LIBUSB_ERROR_NOT_FOUND\n");
            break;
        case LIBUSB_ERROR_BUSY:
            fprintf(stderr,    "LIBUSB_ERROR_BUSY\n");
            break;
        case LIBUSB_ERROR_TIMEOUT:
            fprintf(stderr,    "LIBUSB_ERROR_TIMEOUT\n");
            break;
        case LIBUSB_ERROR_OVERFLOW:
            fprintf(stderr,    "LIBUSB_ERROR_OVERFLOW\n");
            break;
        case LIBUSB_ERROR_PIPE:
            fprintf(stderr,    "LIBUSB_ERROR_PIPE\n");
            break;
        case LIBUSB_ERROR_INTERRUPTED:
            fprintf(stderr,    "LIBUSB_ERROR_INTERRUPTED\n");
            break;
        case LIBUSB_ERROR_NO_MEM:
            fprintf(stderr,    "LIBUSB_ERROR_NO_MEM\n");
            break;
        case LIBUSB_ERROR_NOT_SUPPORTED:
            fprintf(stderr,    "LIBUSB_ERROR_NOT_SUPPORTED\n");
            break;
        case LIBUSB_ERROR_OTHER:
            fprintf(stderr,    "LIBUSB_ERROR_OTHER\n");
            break;
        default:
            fprintf(stderr,"unkown error\n");
            break;
    }
}

static int usb_getEndpoint(const struct libusb_interface_descriptor * interface, usb_dev_mngr_s* user_device)
{
    int i;
    int ret = 0;
    const struct libusb_endpoint_descriptor *epdesc;

    for(i=0; i<interface->bNumEndpoints; i++)
    {
        epdesc = &interface->endpoint[i];

        if(epdesc->bmAttributes == LIBUSB_TRANSFER_TYPE_BULK) //transfer type :bulk
        {
            if(epdesc->bEndpointAddress & LIBUSB_ENDPOINT_IN) // in endpoint
            {
                printf("EP IN: %02x \n", epdesc->bEndpointAddress);
                   user_device->bInEndpointAddress = epdesc->bEndpointAddress;
            }
            //else if (epdesc->bEndpointAddress & LIBUSB_ENDPOINT_OUT)// out endpoint
            else if (!(epdesc->bEndpointAddress & LIBUSB_ENDPOINT_IN))
            {
                printf("EP OUT: %02x \n", epdesc->bEndpointAddress);
                   user_device->bOutEndpointAddress = epdesc->bEndpointAddress;
            }
        }

        if (user_device->bInEndpointAddress && user_device->bOutEndpointAddress)
        {
            ret = 1;
            break;
        }
    }

    return ret;
}


static int usb_getUSBInfo(usb_dev_mngr_s* user_device)
{
    int rv = -1;
    int i,j,k;
    struct libusb_config_descriptor *conf_desc = NULL;
    const struct libusb_interface *inter;
    const struct libusb_interface_descriptor *interdesc;

    for (i = 0; i < user_device->bNumConfigurations; i++)
    {
        if(user_device->dev != NULL)
        {
            if (LIBUSB_SUCCESS != libusb_get_config_descriptor(user_device->dev, i, &conf_desc))
            {
                continue;
            }
        }

        for (j = 0; j < conf_desc->bNumInterfaces; j++)
        {
            inter = &conf_desc->interface[j];

            for (k=0; k < inter->num_altsetting; k++)
            {
                interdesc = &inter->altsetting[k];

                if(interdesc->bInterfaceClass == LIBUSB_CLASS_VENDOR_SPEC && interdesc->bInterfaceSubClass == LIBUSB_CLASS_VENDOR_SPEC)
                {
                    if (user_device->bInEndpointAddress <= 0 || user_device->bOutEndpointAddress <= 0)
                    {
                         if(usb_getEndpoint(interdesc, user_device))
                        {
                            user_device->bInterfaceNumber = interdesc->bInterfaceNumber;
                            rv = 0;
                            break;
                        }
                    }
                }
                else
                {
                    printf("bInterfaceClass:%02x, bInterfaceSubClass:%02x\n", interdesc->bInterfaceClass, interdesc->bInterfaceSubClass);
                }
            }
        }

        libusb_free_config_descriptor(conf_desc);
        conf_desc = NULL;

        if (0 == rv)
        {
            break;
        }
    }

    return rv;
}

//检查安卓设备是否处于Accessory模式
static int usb_checkAccessory(uint16_t idVendor, uint16_t idProduct)
{
    if (idVendor == VID_GOOGLE)
    {
        switch (idProduct)
        {
            case PID_AOA_ACC:
            case PID_AOA_ACC_ADB:
            case PID_AOA_ACC_AU:
            case PID_AOA_ACC_AU_ADB:
                return 1;

            //音频
            case PID_AOA_AU:
            case PID_AOA_AU_ADB:
                break;

            default:
                break;
        }
    }

    return 0;
}

static int usb_sendCtrl(libusb_device_handle *handle, char *buff, int req, int index)
{
    int ret = 0;

    if (NULL != buff)
    {
        ret = libusb_control_transfer(handle, 0x40, req, 0, index, (uint8_t *)buff, strlen(buff) + 1, 0);
    }
    else
    {
        ret = libusb_control_transfer(handle, 0x40, req, 0, index, NULL, 0, 0);
    }

    if (ret < 0) {
        usb_error(ret, __LINE__);
    }

    return ret;
}

static int usb_setupAccessory(libusb_device *dev)
{
    unsigned char ioBuffer[2] = {0};
    int aoaVersion;
    int ret = 0;
    libusb_device_handle *handle = NULL;

    ret = libusb_open (dev, &handle);
    if (LIBUSB_SUCCESS != ret)
    {
        printf("libusb_open failed, ret:%d\n", ret);
        usb_error(ret, __LINE__);
        return -1;
    }

    //如果USB连接到内核驱动则进行 detach
    //libusb_set_auto_detach_kernel_driver(gstUsbMngr.handle, 1);
    if(libusb_kernel_driver_active(handle, 0) > 0)
    {
        printf("kernel driver active, ignoring device");
        if(libusb_detach_kernel_driver(handle, 0) != LIBUSB_SUCCESS)
        {
            printf("failed to detach kernel driver, ignoring device");
            goto EXIT;;
        }
    }

    //发送序号为51的USB报文，获取手机的AOA协议版本，目前为1或2
    ret = libusb_control_transfer(handle, 0xC0, 51, 0, 0, ioBuffer, 2, 1000);
    if (ret < 0)
    {
        usb_error(ret, __LINE__);
        goto EXIT;
    }

    aoaVersion  = ioBuffer[1] << 8 | ioBuffer[0];
    printf("AOA Verion: %d \n", aoaVersion);
    if (aoaVersion != 1 && aoaVersion != 2)
    {
        goto EXIT;
    }

    ret = -1;
    usleep(1000);

    //发送序号为52的USB报文，涉及制造商、型号、版本、设备描述、序列号、uri
    if (usb_sendCtrl(handle, manufacturer, 52, 0) < 0) {
        goto EXIT;
    }
    if (usb_sendCtrl(handle, modelName, 52, 1) < 0) {
        goto EXIT;
    }
    if (usb_sendCtrl(handle, description, 52, 2) < 0) {
        goto EXIT;
    }
    if (usb_sendCtrl(handle, version, 52, 3) < 0) {
        goto EXIT;
    }
    if (usb_sendCtrl(handle, uri, 52, 4) < 0) {
        goto EXIT;
    }
    if (usb_sendCtrl(handle, serialNumber, 52, 5) < 0) {
        goto EXIT;
    }

    printf("Accessory Identification sent\n");

    // 发送序号为53的USB报文，切换USB模式
    if (usb_sendCtrl(handle, NULL, 53, 0) < 0) {
        goto EXIT;
    }

    ret = 0;

EXIT:
    libusb_close(handle);
    handle = NULL;
    return ret;
}

static int usb_openUSB(void)
{
    int rc = 0;

    rc = libusb_open(gstUsbMngr.dev, &gstUsbMngr.handle);
    if (LIBUSB_SUCCESS != rc)
    {
        return -1;
    }

    //声明读写接口
    rc = libusb_claim_interface(gstUsbMngr.handle, gstUsbMngr.bInterfaceNumber);
    if (LIBUSB_SUCCESS != rc)
    {
        libusb_close(gstUsbMngr.handle);
        gstUsbMngr.handle = NULL;
        return -1;
    }

    return 0;
}

/*
    不能在usb_hotplugCallback调用usb_handleHotplugArrived和usb_handleHotplugLeft
    否则会出现LIBUSB_ERROR_BUSY
*/
int usb_handleHotplugArrived(void)
{
    int rc = 0;

    printf("===>> usb_handleHotplugArrived \n");
    pthread_mutex_lock(&gstUsbMngr.stLock);

    if (!usb_checkAccessory(gstUsbMngr.vid, gstUsbMngr.pid))//不是AOA模式
    {
        rc = usb_setupAccessory(gstUsbMngr.dev);
        if (rc)
        {
            fprintf (stderr, "usb_setupAccessory Error, rc:%d\n", rc);
            pthread_mutex_unlock(&gstUsbMngr.stLock);
            return -1;
        }
    }
    else    //是AOA模式
    {
        rc = usb_getUSBInfo(&gstUsbMngr);
        if (rc)
        {
            fprintf (stderr, "usb_getUSBInfo Error, rc:%d\n", rc);
            pthread_mutex_unlock(&gstUsbMngr.stLock);
            return -1;
        }

        rc = usb_openUSB();
        if (rc)
        {
            fprintf (stderr, "usb_openUSB Error, rc:%d\n", rc);
            pthread_mutex_unlock(&gstUsbMngr.stLock);
            return -1;
        }
    }

    pthread_mutex_unlock(&gstUsbMngr.stLock);
    return 0;
}

int usb_handleHotplugLeft(void)
{
    if (gstUsbMngr.handle)
    {
        libusb_release_interface (gstUsbMngr.handle, gstUsbMngr.bInterfaceNumber);
        libusb_close (gstUsbMngr.handle);
        gstUsbMngr.handle = NULL;
    }

    return 0;
}

static int LIBUSB_CALL usb_hotplugCallback(libusb_context *ctx, libusb_device *dev, libusb_hotplug_event event, void *user_data)
{
    int rc;
    struct libusb_device_descriptor desc;

    pthread_mutex_lock(&gstUsbMngr.stLock);

    rc = libusb_get_device_descriptor(dev, &desc);
    if (LIBUSB_SUCCESS != rc) {
        fprintf (stderr, "Error getting device descriptor\n");
        pthread_mutex_unlock(&gstUsbMngr.stLock);
        return -1;
    }

    if (event == LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED)//插入
    {
        if (0x1d6b == desc.idVendor && 0x0002 == desc.idProduct) //过滤设备本身存在的usb设备
        {
            pthread_mutex_unlock(&gstUsbMngr.stLock);
            return 0;
        }

        printf ("Device attached: %04x:%04x\n", desc.idVendor, desc.idProduct);

        gstUsbMngr.vid = desc.idVendor;
        gstUsbMngr.pid = desc.idProduct;
        gstUsbMngr.bInEndpointAddress = 0;
        gstUsbMngr.bOutEndpointAddress = 0;
        gstUsbMngr.bNumConfigurations = desc.bNumConfigurations;
        gstUsbMngr.dev = dev;
        gstUsbMngr.event = event;

    }
    else if (event == LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT)//拔出
    {
        printf ("Device detached: %04x:%04x\n", desc.idVendor, desc.idProduct);
        gstUsbMngr.event = event;
    }
    else
    {
        printf("event wrong, event: %d \n", event);
        gstUsbMngr.event = 0;
    }

    pthread_mutex_unlock(&gstUsbMngr.stLock);
    return 0;
}

//监听事件
void *usb_eventMonitorProcess(void *args)
{
    int rc = 0;

    while(1)
    {
        //处理事件
        if (gstUsbMngr.event == LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED)
        {
            usb_handleHotplugArrived();
            gstUsbMngr.event = 0;
        }
        else if (gstUsbMngr.event == LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT)
        {
            usb_handleHotplugLeft();
            gstUsbMngr.event = 0;
        }

        //事件循环（阻塞），读写USB会触发
        rc = libusb_handle_events(NULL);
        if (rc < 0)
        {
            printf("libusb_handle_events() failed: %s\n", libusb_error_name(rc));
        }
    }

    return NULL;
}

int usb_write(char *buf, int bufLen)
{
    int rc  = 0;
    int nActualBytes = 0;

    pthread_mutex_lock(&gstUsbMngr.stLock);

    if (NULL == gstUsbMngr.handle || 0 != gstUsbMngr.event)
    {
        pthread_mutex_unlock(&gstUsbMngr.stLock);
        return -1;
    }

    rc = libusb_bulk_transfer(gstUsbMngr.handle, gstUsbMngr.bOutEndpointAddress, (unsigned char *)buf, bufLen, &nActualBytes, 1000);
    if (rc < 0)
    {
        printf("libusb_bulk_transfer(0x01) write failed:[%s] \n", libusb_strerror(rc));
        pthread_mutex_unlock(&gstUsbMngr.stLock);
        return rc;
    }

    pthread_mutex_unlock(&gstUsbMngr.stLock);
    return nActualBytes;
}

int usb_read(char *buf, int bufLen)
{
    int rc  = 0;
    int nActualBytes = 0;

    pthread_mutex_lock(&gstUsbMngr.stLock);

    if (NULL == gstUsbMngr.handle || 0 != gstUsbMngr.event)
    {
        pthread_mutex_unlock(&gstUsbMngr.stLock);
        return -1;
    }

    rc = libusb_bulk_transfer(gstUsbMngr.handle, gstUsbMngr.bInEndpointAddress, (unsigned char *)buf, bufLen, &nActualBytes, 1000);
    if (rc < 0)
    {
        //printf("libusb_bulk_transfer(0x81) read failed:[%s] \n", libusb_strerror(rc));
        pthread_mutex_unlock(&gstUsbMngr.stLock);
        return rc;
    }

    pthread_mutex_unlock(&gstUsbMngr.stLock);
    return nActualBytes;
}

//测试读写
void *usb_readProcess(void *args)
{
    int rc = 0;
    char readBuf[4*1024] = {0};
    int nActualBytes = 0;

    while (1)
    {
        if (NULL == gstUsbMngr.handle)
        {
            usleep(100*1000);
            continue;
        }

        rc = usb_read(readBuf, sizeof(readBuf));
        if (rc > 0)
        {
            usb_write(readBuf, rc);
        }
    }
}

int usb_init(void)
{
    int product_id, vendor_id, class_id;
    int rc;
    pthread_t tid;

    vendor_id = LIBUSB_HOTPLUG_MATCH_ANY;
    product_id = LIBUSB_HOTPLUG_MATCH_ANY;
    class_id = LIBUSB_HOTPLUG_MATCH_ANY;

    memset(&gstUsbMngr, 0x0, sizeof(usb_dev_mngr_s));

    rc = pthread_mutex_init(&gstUsbMngr.stLock, NULL);
    if (rc)
    {
        printf("pthread_mutex_init failed \n");
        return EXIT_FAILURE;
    }

    rc = libusb_init (NULL);
    if (rc < 0)
    {
        printf("failed to initialise libusb: %s\n", libusb_error_name(rc));
        return EXIT_FAILURE;
    }

    //获取当前库是否支持热插拔
    if (!libusb_has_capability (LIBUSB_CAP_HAS_HOTPLUG)) {
        printf("Hotplug capabilities are not supported on this platform\n");
        libusb_exit (NULL);
        return EXIT_FAILURE;
    }

    //注册热插拔事件回调
    rc = libusb_hotplug_register_callback (NULL, LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED | LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT, LIBUSB_HOTPLUG_ENUMERATE, vendor_id,
        product_id, class_id, usb_hotplugCallback, NULL, &gstUsbMngr.hotplugCbh);
    if (LIBUSB_SUCCESS != rc) {
        printf("libusb_hotplug_register_callback failed \n");
        libusb_exit (NULL);
        return EXIT_FAILURE;
    }

    //开启事件处理线程
    pthread_create(&tid, NULL, usb_eventMonitorProcess, NULL);

    return LIBUSB_SUCCESS;
}

int usb_deinit(void)
{
    if (gstUsbMngr.handle) {
        libusb_release_interface(gstUsbMngr.handle, gstUsbMngr.bInterfaceNumber);
        libusb_close (gstUsbMngr.handle);
        gstUsbMngr.handle = NULL;
    }

    if (0 < gstUsbMngr.hotplugCbh) {
        libusb_hotplug_deregister_callback(NULL, gstUsbMngr.hotplugCbh);
    }

    pthread_mutex_destroy(&gstUsbMngr.stLock);
    libusb_exit (NULL);
    return LIBUSB_SUCCESS;
}
