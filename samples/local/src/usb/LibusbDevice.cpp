//
// Created by LiangKeJin on 2025/3/21.
//

#include "LibusbDevice.h"

NAMESPACE_DEFAULT

std::list<LibusbConfig> LibusbDevice::getConfigs() const {
    std::list<LibusbConfig> configs;
    for (int i = 0; i < m_desc.bNumConfigurations; ++i) {
        libusb_config_descriptor* config = nullptr;
        const int ret = libusb_get_config_descriptor(m_dev, i, &config);
        if (ret != 0) {
            _ERROR("Failed to get config descriptor[%d]: %s", i, LibusbUtils::errString(ret));
            continue;
        }
        configs.push_back(LibusbConfig(*config));
        libusb_free_config_descriptor(config);
    }
    return configs;
}

int LibusbDevice::open() {
    if (m_handle) {
        return 0;
    }

    int ret = libusb_open(m_dev, &m_handle);
    if (ret) {
        _ERROR("Failed to open device: %s", LibusbUtils::errString(ret));
    }
    else {
        _INFO("Device opened: %s", this->toString());
    }

    return ret;
}

int LibusbDevice::active(const int interfaceNumber) const {
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

int LibusbDevice::claimInterface(const LibusbInterfaceSetting& setting, libusb_endpoint_transfer_type transType) {
    _FATAL_IF(!m_handle, "Device is not open");
    int ret = libusb_claim_interface(m_handle, setting.number());
    if (ret != 0) {
        _ERROR("Failed to claim interface: %s", LibusbUtils::errString(ret));
    } else {
        m_active_interface = setting;
        m_transfer_type = transType;
        m_active_in_endpoint = setting.getInEndpoint(transType);
        m_active_out_endpoint = setting.getOutEndpoint(transType);
        _INFO("Interface claimed: %s", setting.toString());
    }
    return ret;
}

int LibusbDevice::transCtrl(uint8_t reqType, uint8_t req,
                           uint16_t val, uint16_t index, uint8_t* data, uint16_t wLength, uint32_t timeout) const {
    _FATAL_IF(!m_handle, "Device is not open");

    int ret = libusb_control_transfer(m_handle, reqType, req, val, index, data, wLength, timeout);
    _ERROR_IF(ret != 0, "Failed to control transfer(type: %d, request: %d, value: %d, index: %d): %s",
              reqType, req, val, index, LibusbUtils::errString(ret));
    return ret;
}

int LibusbDevice::sendBulk(unsigned char* data, int length, int* actual_length, unsigned int timeout) {
    _FATAL_IF(!m_handle, "Device is not open");
    _FATAL_IF(!m_active_interface.valid(), "Interface is not claimed");

    uint8_t endpoint = m_active_out_endpoint.address();
    int ret = libusb_bulk_transfer(m_handle, endpoint, data, length, actual_length, timeout);
    _ERROR_IF(ret != 0, "Failed to send bulk transfer(endpoint: %d): %s", endpoint, LibusbUtils::errString(ret));
    return ret;
}

int LibusbDevice::recvBulk(unsigned char* data, int length, int* actual_length, unsigned int timeout) {
    _FATAL_IF(!m_handle, "Device is not open");
    _FATAL_IF(!m_active_interface.valid(), "Interface is not claimed");

    uint8_t endpoint = m_active_in_endpoint.address();
    int ret = libusb_bulk_transfer(m_handle, endpoint, data, length, actual_length, timeout);
    _ERROR_IF(ret != 0, "Failed to recv bulk transfer(endpoint: %d): %s", endpoint, LibusbUtils::errString(ret));
    return ret;
}

int LibusbDevice::sendInterrupt(unsigned char* data, int length, int* actual_length, unsigned int timeout) {
    _FATAL_IF(!m_handle, "Device is not open");
    _FATAL_IF(!m_active_interface.valid(), "Interface is not claimed");
    uint8_t endpoint = m_active_out_endpoint.address();
    int ret = libusb_interrupt_transfer(m_handle, endpoint, data, length, actual_length, timeout);
    _ERROR_IF(ret!= 0, "Failed to send interrupt transfer(endpoint: %d): %s", endpoint, LibusbUtils::errString(ret));
    return ret;
}

int LibusbDevice::recvInterrupt(unsigned char* data, int length, int* actual_length, unsigned int timeout) {
    _FATAL_IF(!m_handle, "Device is not open");
    _FATAL_IF(!m_active_interface.valid(), "Interface is not claimed");
    uint8_t endpoint = m_active_in_endpoint.address();
    int ret = libusb_interrupt_transfer(m_handle, endpoint, data, length, actual_length, timeout);
    _ERROR_IF(ret!= 0, "Failed to recv interrupt transfer(endpoint: %d): %s", endpoint, LibusbUtils::errString(ret));
    return ret;
}

void LibusbDevice::close() {
    if (m_active_interface.valid()) {
        libusb_release_interface(m_handle, m_active_interface.number());
    }
    if (m_handle) {
        libusb_close(m_handle);
        m_handle = nullptr;
    }
}

NAMESPACE_END
