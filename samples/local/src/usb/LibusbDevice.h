//
// Created by LiangKeJin on 2025/3/21.
//

#pragma once

#include "LibusbUtils.h"
#include <sstream>
#include <common/Log.h>

#include "ZNamespace.h"

NAMESPACE_DEFAULT
class LibusbDevice {
public:
    LibusbDevice(libusb_device* dev, const libusb_device_descriptor& desc) {
        libusb_ref_device(dev);
        m_dev = dev;
        m_desc = desc;
        m_in_endpoint_address = 0;
        m_out_endpoint_address = 0;
    }

    LibusbDevice(const LibusbDevice& o) : m_dev(o.m_dev), m_desc(o.m_desc),
                                          m_handle(o.m_handle),
                                          m_in_endpoint_address(o.m_in_endpoint_address),
                                          m_out_endpoint_address(o.m_out_endpoint_address) {
        libusb_ref_device(m_dev);
    }

    LibusbDevice& operator =(const LibusbDevice& o) {
        if (m_dev) {
            libusb_unref_device(m_dev);
        }
        m_dev = o.m_dev;
        libusb_ref_device(m_dev);
        m_desc = o.m_desc;
        m_handle = o.m_handle;
        m_in_endpoint_address = o.m_in_endpoint_address;
        m_out_endpoint_address = o.m_out_endpoint_address;
        return *this;
    }

    ~LibusbDevice() {
        if (m_dev) {
            libusb_unref_device(m_dev);
        }
    }

    bool operator ==(const LibusbDevice& o) const {
        return o.m_dev == m_dev && m_desc.idProduct == o.m_desc.idProduct && m_desc.idVendor == o.m_desc.idVendor;
    }

public:
    int open() {
        _FATAL_IF(m_handle, "Device is already open");

        const int ret = libusb_open(m_dev, &m_handle);
        if (ret) {
            _ERROR("Failed to open device: %s", LibusbUtils::errString(ret));
        }
        else {
            _INFO("Device opened: %s", this->toString());
        }
        return ret;
    }

    int active(const int interfaceNumber) {
        _FATAL_IF(!m_handle, "Device is not open");

        int ret = libusb_kernel_driver_active(m_handle, interfaceNumber);
        if (ret != 0) {
            ret = libusb_detach_kernel_driver(m_handle, interfaceNumber);
            if (ret != 0) {
                _ERROR("Failed to detach kernel driver: %s", LibusbUtils::errString(ret));
            }
        }
        return ret;
    }

    bool isOpened() const {
        return m_handle != nullptr;
    }

    int transfer(uint8_t request_type, uint8_t bRequest, uint16_t wValue, uint16_t wIndex,
                 unsigned char* data, uint16_t wLength, unsigned int timeout) {
        _FATAL_IF(!m_handle, "Device is not open");

        int ret = libusb_control_transfer(m_handle, request_type, bRequest, wValue, wIndex,
                                          data, wLength, timeout);
        _ERROR_IF(ret< 0, "Failed to control transfer: %s", LibusbUtils::errString(ret));
        return ret;
    }

    void close() {
        if (m_handle) {
            libusb_close(m_handle);
            m_handle = nullptr;
        }
    }

public:
    const libusb_device *device() const {
        return m_dev;
    }
    uint32_t productId() const { return m_desc.idProduct; }
    uint32_t vendorId() const { return m_desc.idVendor; }

    uint8_t inEndpointAddress() const { return m_in_endpoint_address; }
    uint8_t outEndpointAddress() const { return m_out_endpoint_address; }

    std::string manufacturerString() const {
        return getDescString(m_desc.iManufacturer);
    }

    std::string productString() const {
        return getDescString(m_desc.iProduct);
    }

    std::string serialNumberString() const {
        return getDescString(m_desc.iSerialNumber);
    }

    std::string getDescString(uint8_t index) const {
        _ERROR_RETURN_IF(!m_handle, "", "getDescString(%d) failed, Device is not open!", index);

        unsigned char buf[256] = {0};
        int bytes = libusb_get_string_descriptor_ascii(m_handle, index, buf, sizeof(buf));
        if (bytes > 0) {
            return std::string((char*)buf);
        }
        return std::string();
    }

    std::string toString() const {
        std::stringstream ss;
        ss << "LibusbDevice(" << LibusbUtils::toHexString((uint64_t) m_dev) << ") {\n";
        ss << "  desc type : " << LibusbUtils::descTypeToString(m_desc.bDescriptorType) << "\n";
        ss << "  product id: " << LibusbUtils::toHexString(m_desc.idProduct) << "\n";
        ss << "  vendor id : " << LibusbUtils::toHexString(m_desc.idVendor) << "\n";
        ss << "  usb type  : " << LibusbUtils::toHexString(m_desc.bcdUSB) << "\n";
        ss << "  class     : " << LibusbUtils::toHexString(m_desc.bDeviceClass) << "\n";
        ss << "  sub class : " << LibusbUtils::toHexString(m_desc.bDeviceSubClass) << "\n";
        ss << "  protocol  : " << LibusbUtils::toHexString(m_desc.bDeviceProtocol) << "\n";
        ss << "  max packet size: " << (int)m_desc.bMaxPacketSize0 << "\n";
        ss << "  bcd device: " << (int)m_desc.bcdDevice << "\n";
        if (m_handle) {
            ss << "  manufacturer : " << manufacturerString() << "\n";
            ss << "  product      : " << productString() << "\n";
            ss << "  serial number: " << serialNumberString() << "\n";
        } else {
            ss <<"  manufacturer : "  << (int)m_desc.iManufacturer << "\n";
            ss <<"  product      : " << (int)m_desc.iProduct << "\n";
            ss <<"  serial number: "  << (int)m_desc.iSerialNumber << "\n";
        }
        ss << "  num configurations : " << (int)m_desc.bNumConfigurations << "\n";
        ss << "  in  endpoint address: " << (int)m_in_endpoint_address << "\n";
        ss << "  out endpoint address: " << (int)m_out_endpoint_address << "\n";
        ss << "}\n";
        return ss.str();
    }

private:


private:
    libusb_device* m_dev = nullptr;
    libusb_device_descriptor m_desc;

    libusb_device_handle* m_handle = nullptr;

    uint8_t m_in_endpoint_address = 0;
    uint8_t m_out_endpoint_address = 0;
};

NAMESPACE_END
