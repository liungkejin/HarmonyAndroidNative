//
// Created by LiangKeJin on 2025/3/21.
//

#pragma once

#include <list>

#include "LibusbUtils.h"
#include <sstream>
#include <common/Log.h>

#include "ZNamespace.h"

NAMESPACE_DEFAULT

class LibusbEndpoint {
public:
    LibusbEndpoint() {
        m_desc.bLength = 0;
    }

    LibusbEndpoint(const libusb_endpoint_descriptor& desc) : m_desc(desc) {
    }

    LibusbEndpoint(const LibusbEndpoint& o) : m_desc(o.m_desc) {
    }

    LibusbEndpoint& operator =(const LibusbEndpoint& o) {
        m_desc = o.m_desc;
        return *this;
    }

public:
    bool valid() const {
        return m_desc.bLength != 0;
    }

    uint8_t address() const {
        return m_desc.bEndpointAddress;
    }

    uint8_t type() const {
        return m_desc.bmAttributes;
    }

    bool isBulkType() const {
        return type() == LIBUSB_TRANSFER_TYPE_BULK;
    }

    bool isInEndpoint() const {
        return m_desc.bEndpointAddress & LIBUSB_ENDPOINT_IN;
    }

    bool isOutEndpoint() const {
        return !isInEndpoint();
    }

    uint16_t maxPacketSize() const {
        return m_desc.wMaxPacketSize;
    }

    std::string toString() const {
        std::stringstream ss;
        ss << "{address: " << LibusbUtils::toHexString(address())
            << ", type: " << LibusbUtils::toHexString(type())
            << ", in: " << isInEndpoint() << ", out: " << isOutEndpoint()
            << ", maxPacketSize: " << maxPacketSize() << "}";
        return ss.str();
    }

private:
    libusb_endpoint_descriptor m_desc;
};

class LibusbInterfaceSetting {
public:
    LibusbInterfaceSetting() {
        m_desc.bLength = 0;
    }

    LibusbInterfaceSetting(const libusb_interface_descriptor& desc) : m_desc(desc) {
        for (int i = 0; i < desc.bNumEndpoints; ++i) {
            m_endpoints.push_back(LibusbEndpoint(desc.endpoint[i]));
        }
    }

    LibusbInterfaceSetting(const LibusbInterfaceSetting& o) : m_desc(o.m_desc) {
        for (const auto& endpoint : o.m_endpoints) {
            m_endpoints.push_back(endpoint);
        }
    }

    LibusbInterfaceSetting& operator =(const LibusbInterfaceSetting& o) {
        m_desc = o.m_desc;
        m_endpoints.clear();
        for (const auto& endpoint : o.m_endpoints) {
            m_endpoints.push_back(endpoint);
        }
        return *this;
    }

public:
    bool valid() const {
        return m_desc.bLength != 0;
    }

    uint8_t number() const {
        return m_desc.bInterfaceNumber;
    }

    uint8_t classId() const {
        return m_desc.bInterfaceClass;
    }

    uint8_t subclassId() const {
        return m_desc.bInterfaceSubClass;
    }

    uint8_t protocolId() const {
        return m_desc.bInterfaceProtocol;
    }

    std::list<LibusbEndpoint> endpoints() const {
        return m_endpoints;
    }

    LibusbEndpoint getInEndpoint(libusb_endpoint_transfer_type type) const {
        for (const auto& endpoint : m_endpoints) {
            if (endpoint.type() == type && endpoint.isInEndpoint()) {
                return endpoint;
            }
        }
        return LibusbEndpoint();
    }

    LibusbEndpoint getOutEndpoint(libusb_endpoint_transfer_type type) const {
        for (const auto& endpoint : m_endpoints) {
            if (endpoint.type() == type && endpoint.isOutEndpoint()) {
                return endpoint;
            }
        }
        return LibusbEndpoint();
    }

    bool hasInAndOutEndpoints(libusb_endpoint_transfer_type type) const {
        bool has_in = false;
        bool has_out = false;
        for (const auto& endpoint : m_endpoints) {
            if (endpoint.type() != type) {
                continue;
            }
            if (endpoint.isOutEndpoint()) {
                has_out = true;
            }
            else if (endpoint.isInEndpoint()) {
                has_in = true;
            }
        }
        return has_in && has_out;
    }

    std::string toString(int indent = 0) const {
        std::string indent_str;
        for (int i = 0; i < indent; ++i) {
            indent_str += " ";
        }
        std::stringstream ss;
        ss << "{\n";
        ss << indent_str << "number: " << LibusbUtils::toHexString(number()) << '\n'
           << indent_str << "class: " << LibusbUtils::toHexString(classId()) << '\n'
           << indent_str << "subclass: " << LibusbUtils::toHexString(subclassId()) << '\n'
           << indent_str << "protocol: " << LibusbUtils::toHexString(protocolId()) << '\n'
           << indent_str << "endpoints: [\n";
        for (const auto& endpoint : m_endpoints) {
            ss << indent_str << "  " << endpoint.toString() << ", \n";
        }
        ss << indent_str << "]}";
        return ss.str();
    }

private:
    libusb_interface_descriptor m_desc;
    std::list<LibusbEndpoint> m_endpoints;
};

class LibusbInterface {
public:
    LibusbInterface(const libusb_interface& desc) : m_desc(desc) {
        for (int i = 0; i < desc.num_altsetting; ++i) {
            m_settings.push_back(LibusbInterfaceSetting(desc.altsetting[i]));
        }
    }

    LibusbInterface(const LibusbInterface& o) : m_desc(o.m_desc) {
        for (const auto& setting : o.m_settings) {
            m_settings.push_back(setting);
        }
    }

    LibusbInterface& operator =(const LibusbInterface& o) {
        m_desc = o.m_desc;
        m_settings.clear();
        for (const auto& setting : o.m_settings) {
            m_settings.push_back(setting);
        }
        return *this;
    }

public:
    const std::list<LibusbInterfaceSetting>& settings() const {
        return m_settings;
    }

    uint8_t settingSize() const {
        return m_desc.num_altsetting;
    }

    LibusbInterfaceSetting findInterface(uint8_t classId, uint8_t subclassId,
        libusb_endpoint_transfer_type transType, bool hasIn, bool hasOut) const {
        for (const auto& setting : m_settings) {
            if (setting.classId() == classId && setting.subclassId() == subclassId) {
                if (hasIn && hasOut) {
                    if (setting.hasInAndOutEndpoints(transType)) {
                        return setting;
                    }
                }
                else if (hasIn) {
                    if (setting.getInEndpoint(transType).valid()) {
                        return setting;
                    }
                }
                else if (hasOut) {
                    if (setting.getOutEndpoint(transType).valid()) {
                        return setting;
                    }
                }
            }
        }
        return LibusbInterfaceSetting();
    }

    std::string toString(int indent = 0) const {
        std::string indent_str;
        for (int i = 0; i < indent; ++i) {
            indent_str += " ";
        }
        std::stringstream ss;
        ss << "{\n";
        ss << indent_str << "settingSize: " << (int) settingSize() << '\n';
        ss << indent_str << "settings: [\n";
        int i = 0;
        for (const auto& setting : m_settings) {
            ss << indent_str << "  [" << i++ << "]: " << setting.toString(indent + 4) << ", \n";
        }
        ss << indent_str << "]}";
        return ss.str();
    }

private:
    libusb_interface m_desc;
    std::list<LibusbInterfaceSetting> m_settings;
};

class LibusbConfig {
public:
    LibusbConfig(const libusb_config_descriptor& desc) : m_desc(desc) {
        for (int i = 0; i < desc.bNumInterfaces; ++i) {
            m_interfaces.push_back(LibusbInterface(desc.interface[i]));
        }
    }

    LibusbConfig(const LibusbConfig& o) : m_desc(o.m_desc) {
        for (const auto& interface : o.m_interfaces) {
            m_interfaces.push_back(interface);
        }
    }

    LibusbConfig& operator =(const LibusbConfig& o) {
        m_desc = o.m_desc;
        m_interfaces.clear();
        for (const auto& interface : o.m_interfaces) {
            m_interfaces.push_back(interface);
        }
        return *this;
    }

public:
    std::list<LibusbInterface> interfaces() const {
        return m_interfaces;
    }

    uint8_t interfaceSize() const {
        return m_desc.bNumInterfaces;
    }

    LibusbInterfaceSetting findInterface(uint8_t classId, uint8_t subclassId,
        libusb_endpoint_transfer_type transType, bool hasIn, bool hasOut) const {
        for (const auto& interface : m_interfaces) {
            auto setting = interface.findInterface(classId, subclassId, transType, hasIn, hasOut);
            if (setting.valid()) {
                return setting;
            }
        }
        return LibusbInterfaceSetting();
    }

    std::string toString() const {
        std::stringstream ss;
        ss << "{\n";
        ss << "interfaceSize: " << (int) interfaceSize() << "\n";
        ss << "interfaces: [\n";
        int i = 0;
        for (const auto& interface : m_interfaces) {
            ss << "  [" << i << "]: " << interface.toString(4) << ", \n";
            i++;
        }
        ss << "]}";
        return ss.str();
    }

private:
    // 注意：不能使用结构体里面的指针
    libusb_config_descriptor m_desc;
    std::list<LibusbInterface> m_interfaces;
};

class LibusbDevice {
public:
    LibusbDevice() {
        m_desc.bLength = 0;
    }

    LibusbDevice(libusb_device* dev, const libusb_device_descriptor& desc) {
        libusb_ref_device(dev);
        m_dev = dev;
        m_desc = desc;
    }

    LibusbDevice(const LibusbDevice& o) : m_dev(o.m_dev), m_desc(o.m_desc), m_handle(o.m_handle),
                                          m_active_interface(o.m_active_interface) {
        libusb_ref_device(m_dev);
        m_active_in_endpoint = o.m_active_in_endpoint;
        m_active_out_endpoint = o.m_active_out_endpoint;
    }

    LibusbDevice& operator =(const LibusbDevice& o) {
        if (m_dev) {
            libusb_unref_device(m_dev);
        }
        m_dev = o.m_dev;
        libusb_ref_device(m_dev);
        m_desc = o.m_desc;
        m_handle = o.m_handle;
        m_active_interface = o.m_active_interface;
        m_active_in_endpoint = o.m_active_in_endpoint;
        m_active_out_endpoint = o.m_active_out_endpoint;
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
    bool valid() const {
        return m_dev != nullptr;
    }

    int open();

    bool isOpened() const {
        return m_handle != nullptr;
    }

    /**
     * 如果USB连接到内核驱动则进行 detach
     */
    int active(const int interfaceNumber = 0) const;

    /**
     * 声明读写接口
     */
    int claimInterface(const LibusbInterfaceSetting& setting, libusb_endpoint_transfer_type transType);

    /**
     * 发送和接收控制报文
     */
    int transCtrl(uint8_t reqType, uint8_t req, uint16_t val,
                 uint16_t index, uint8_t* data, uint16_t wLength, uint32_t timeout = 0) const;

    /**
     * 批量发送数据
     * 同步传输时效性不高
     * @param actual_length 实际传输长度
     */
    int sendBulk(unsigned char* data, int length,
                 int* actual_length, unsigned int timeout);

    /**
     * 批量接收数据
     * 同步传输时效性不高
     * @param actual_length 实际传输长度
     */
    int recvBulk(unsigned char* data, int length,
                 int* actual_length, unsigned int timeout);

    /**
     * 发送中断数据
     * 同步传输时效性不高
     * @param actual_length 实际传输长度
     */
    int sendInterrupt(unsigned char* data, int length,
                      int* actual_length, unsigned int timeout);

    /**
     * 接收中断数据
     * 同步传输时效性不高
     * @param actual_length 实际传输长度
     */
    int recvInterrupt(unsigned char* data, int length,
                      int* actual_length, unsigned int timeout);

    void close();

public:
    const libusb_device* device() const { return m_dev; }

    std::string name() const {
        char buf[32] = {0};
        std::string ps;
        if (isOpened()) {
            ps = productString();
        }
        if (ps.empty()) {
            snprintf(buf, 32, "v:0x%x-p:0x%x", productId(), vendorId());
            ps = buf;
        }
        return ps;
    }

    std::list<LibusbConfig> getConfigs() const;

    uint8_t numConfigurations() const { return m_desc.bNumConfigurations; }

    uint32_t productId() const { return m_desc.idProduct; }

    uint32_t vendorId() const { return m_desc.idVendor; }

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
        ss << "LibusbDevice(" << LibusbUtils::toHexString((uint64_t)m_dev) << ") {\n";
        // ss << "  desc type : " << LibusbUtils::descTypeToString(m_desc.bDescriptorType) << "\n";
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
        }
        else {
            ss << "  manufacturer : " << (int)m_desc.iManufacturer << "\n";
            ss << "  product      : " << (int)m_desc.iProduct << "\n";
            ss << "  serial number: " << (int)m_desc.iSerialNumber << "\n";
        }
        ss << "  num configurations : " << (int)m_desc.bNumConfigurations << "\n";
        if (m_active_interface.valid()) {
            ss << "  active interface: " << m_active_interface.toString() << "\n";
        }
        ss << "}\n";
        return ss.str();
    }

private:
    libusb_device* m_dev = nullptr;
    libusb_device_descriptor m_desc;

    libusb_device_handle* m_handle = nullptr;

    LibusbInterfaceSetting m_active_interface;
    libusb_endpoint_transfer_type m_transfer_type = LIBUSB_ENDPOINT_TRANSFER_TYPE_BULK;
    LibusbEndpoint m_active_in_endpoint;
    LibusbEndpoint m_active_out_endpoint;
};

NAMESPACE_END
