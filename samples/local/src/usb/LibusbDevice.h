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

    uint8_t attributes() const {
        return m_desc.bmAttributes;
    }

    libusb_endpoint_transfer_type transferType() const {
        return (libusb_endpoint_transfer_type) (m_desc.bmAttributes & LIBUSB_TRANSFER_TYPE_MASK);
    }

    libusb_endpoint_direction direction() const {
        return (libusb_endpoint_direction) (m_desc.bEndpointAddress & LIBUSB_ENDPOINT_DIR_MASK);
    }

    bool isInEndpoint() const {
        return direction() == LIBUSB_ENDPOINT_IN;
    }

    bool isOutEndpoint() const {
        return direction() == LIBUSB_ENDPOINT_OUT;
    }

    uint16_t maxPacketSize() const {
        return m_desc.wMaxPacketSize;
    }

    std::string toString() const {
        std::stringstream ss;
        ss << "{address: " << LibusbUtils::toHexString(address())
            << ", type: " << LibusbUtils::endpointTransferType(transferType())
            << ", direction: " << LibusbUtils::endpointDirection(direction())
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
        m_active_in_endpoint = o.m_active_in_endpoint;
        m_active_out_endpoint = o.m_active_out_endpoint;
        for (const auto& endpoint : o.m_endpoints) {
            m_endpoints.push_back(endpoint);
        }
    }

    LibusbInterfaceSetting& operator =(const LibusbInterfaceSetting& o) {
        m_desc = o.m_desc;
        m_active_in_endpoint = o.m_active_in_endpoint;
        m_active_out_endpoint = o.m_active_out_endpoint;
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

    bool activeInEndpoint(libusb_endpoint_transfer_type type) {
        m_active_in_endpoint = findInEndpoint(type);
        return m_active_in_endpoint.valid();
    }

    const LibusbEndpoint& getActiveInEndpoint() const {
        return m_active_in_endpoint;
    }

    bool activeOutEndpoint(libusb_endpoint_transfer_type type) {
        m_active_out_endpoint = findOutEndpoint(type);
        return m_active_out_endpoint.valid();
    }

    const LibusbEndpoint& getActiveOutEndpoint() const {
        return m_active_out_endpoint;
    }

    LibusbEndpoint findInEndpoint(libusb_endpoint_transfer_type type) const {
        for (const auto& endpoint : m_endpoints) {
            if (endpoint.transferType() == type && endpoint.isInEndpoint()) {
                return endpoint;
            }
        }
        return LibusbEndpoint();
    }

    LibusbEndpoint findOutEndpoint(libusb_endpoint_transfer_type type) const {
        for (const auto& endpoint : m_endpoints) {
            if (endpoint.transferType() == type && endpoint.isOutEndpoint()) {
                return endpoint;
            }
        }
        return LibusbEndpoint();
    }

    bool hasInAndOutEndpoints(libusb_endpoint_transfer_type type) const {
        bool has_in = false;
        bool has_out = false;
        for (const auto& endpoint : m_endpoints) {
            if (endpoint.transferType() != type) {
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
        ss << indent_str << "number    : " << LibusbUtils::toHexString(number()) << '\n'
           << indent_str << "class     : " << LibusbUtils::toHexString(classId()) << '\n'
           << indent_str << "subclass  : " << LibusbUtils::toHexString(subclassId()) << '\n'
           << indent_str << "protocol  : " << LibusbUtils::toHexString(protocolId()) << '\n';
        if (m_active_in_endpoint.valid()) {
            ss << indent_str << "active  in: " << m_active_in_endpoint.toString() << '\n';
        } else {
            ss << indent_str << "active  in: null\n";
        }
        if (m_active_out_endpoint.valid()) {
            ss << indent_str << "active out: " << m_active_out_endpoint.toString() << '\n';
        } else {
            ss << indent_str << "active out: null\n";
        }
        ss << indent_str << "endpoints: [\n";
        for (const auto& endpoint : m_endpoints) {
            ss << indent_str << "  " << endpoint.toString() << ", \n";
        }
        ss << indent_str << "]}";
        return ss.str();
    }

private:
    libusb_interface_descriptor m_desc;
    std::list<LibusbEndpoint> m_endpoints;

    LibusbEndpoint m_active_in_endpoint;
    LibusbEndpoint m_active_out_endpoint;
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
                    if (setting.findInEndpoint(transType).valid()) {
                        return setting;
                    }
                }
                else if (hasOut) {
                    if (setting.findOutEndpoint(transType).valid()) {
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

    std::string toString(int indent = 0) const {
        std::string indent_str;
        for (int i = 0; i < indent; ++i) {
            indent_str += " ";
        }
        std::stringstream ss;
        ss << "{\n";
        ss << indent_str << "interfaceSize: " << (int) interfaceSize() << "\n";
        ss << indent_str << "interfaces: [\n";
        int i = 0;
        for (const auto& interface : m_interfaces) {
            ss << indent_str << "  [" << i << "]: " << interface.toString(indent + 4) << ", \n";
            i++;
        }
        ss << indent_str << "]}";
        return ss.str();
    }

private:
    // 注意：不能使用结构体里面的指针
    libusb_config_descriptor m_desc;
    std::list<LibusbInterface> m_interfaces;
};

// 判断是否为配件通过 AOAProtocol::isAccessory(info) 判断
struct LibusbDeviceInfo {
    // 这个标记是为了解决在 plug 回调里面打开设备会导致失败等问题，出现 LIBUSB_DEVICE_BUSY 错误
    bool is_prepared = false;
    bool is_opened = false;

    uint16_t vendor_id = 0;
    uint16_t product_id = 0;
    uint8_t class_id = 0;
    uint8_t subclass_id = 0;
    uint8_t protocol_id = 0;

    std::string manufacturer;
    std::string product;
    std::string serial_number;

    std::string toString() const {
        std::stringstream ss;
        ss << "{\n"
           << "  is_opened: " << (is_opened ? "true" : "false") << "\n"
           << "  vendor_id: " << LibusbUtils::toHexString(vendor_id) << "\n"
           << "  product_id: " << LibusbUtils::toHexString(product_id) << "\n"
           << "  class_id: " << LibusbUtils::toHexString(class_id) << "\n"
           << "  subclass_id: " << LibusbUtils::toHexString(subclass_id) << "\n"
           << "  protocol_id: " << LibusbUtils::toHexString(protocol_id) << "\n"
           << "  manufacturer: " << manufacturer << "\n"
           << "  product: " << product << "\n"
           << "  serial_number: " << serial_number << "\n"
           << "}";
        return ss.str();
    }
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
        m_info.vendor_id = desc.idVendor;
        m_info.product_id = desc.idProduct;
        m_info.class_id = desc.bDeviceClass;
        m_info.subclass_id = desc.bDeviceSubClass;
        m_info.protocol_id = desc.bDeviceProtocol;
    }

    LibusbDevice(const LibusbDevice& o) : m_dev(o.m_dev), m_handle(o.m_handle),
                                          m_desc(o.m_desc), m_info(o.m_info),
                                          m_active_interface(o.m_active_interface) {
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
        m_info = o.m_info;
        m_active_interface = o.m_active_interface;
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

    bool operator ==(const LibusbDeviceInfo& o) const {
        return m_desc.idProduct == o.product_id && m_desc.idVendor == o.vendor_id;
    }

public:
    bool valid() const {
        return m_dev != nullptr && m_desc.bLength != 0;
    }

    bool isPrepared() const {
        return m_info.is_prepared;
    }

    /**
     * 在设备插入时调用，获取设备信息
     */
    bool prepare();

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
        if (m_info.product.empty()) {
            char buf[32] = {0};
            snprintf(buf, 32, "0x%x/0x%x", vendorId(), productId());
            return buf;
        }
        return m_info.product;
    }

    std::list<LibusbConfig> getConfigs() const;

    uint8_t numConfigurations() const { return m_desc.bNumConfigurations; }

    uint32_t productId() const { return m_desc.idProduct; }

    uint32_t vendorId() const { return m_desc.idVendor; }

    const libusb_device_descriptor& desc() const { return m_desc; }

    const LibusbDeviceInfo& info() const { return m_info; }

    const LibusbInterfaceSetting& activeInterface() const { return m_active_interface; }

    std::string getDescString(uint8_t index) const {
        _ERROR_RETURN_IF(!m_handle, "", "getDescString(%d) failed, Device is not open!", index);

        unsigned char buf[256] = {0};
        int bytes = libusb_get_string_descriptor_ascii(m_handle, index, buf, sizeof(buf));
        if (bytes > 0) {
            return std::string((char*)buf);
        }
        _WARN("getDescString(%d) failed, error=%s", index, LibusbUtils::errString(bytes));
        return std::string();
    }

    std::string toString() const {
        std::stringstream ss;
        ss << "LibusbDevice(" << LibusbUtils::toHexString((uint64_t)m_dev) << ") {\n";
        ss << "  product    id: " << LibusbUtils::toHexString(m_desc.idProduct) << "\n";
        ss << "  vendor    id : " << LibusbUtils::toHexString(m_desc.idVendor) << "\n";
        ss << "  usb      type: " << LibusbUtils::toHexString(m_desc.bcdUSB) << "\n";
        ss << "  class        : " << LibusbUtils::toHexString(m_desc.bDeviceClass) << "\n";
        ss << "  sub    class : " << LibusbUtils::toHexString(m_desc.bDeviceSubClass) << "\n";
        ss << "  protocol     : " << LibusbUtils::toHexString(m_desc.bDeviceProtocol) << "\n";
        ss << "  bcd    device: " << (int)m_desc.bcdDevice << "\n";
        ss << "  manufacturer : " << m_info.manufacturer << "\n";
        ss << "  product      : " << m_info.product << "\n";
        ss << "  serial number: " << m_info.serial_number << "\n";
        ss << "  max packet size: " << (int)m_desc.bMaxPacketSize0 << "\n";
        ss << "  num configurations: " << (int)m_desc.bNumConfigurations << "\n";
        auto configs = getConfigs();
        int i = 0;
        for (const auto& config : configs) {
            ss << "  config[" << i << "]: " << config.toString(4) << ",\n";
            i++;
        }
        if (m_active_interface.valid()) {
            ss << "  active interface: " << m_active_interface.toString(4) << "\n";
        } else {
            ss << "  active interface: null\n";
        }
        ss << "}\n";
        return ss.str();
    }

private:
    libusb_device* m_dev = nullptr;
    libusb_device_handle* m_handle = nullptr;

    libusb_device_descriptor m_desc;
    LibusbDeviceInfo m_info;

    LibusbInterfaceSetting m_active_interface;
};

NAMESPACE_END
