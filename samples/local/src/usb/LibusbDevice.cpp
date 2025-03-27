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

bool LibusbDevice::prepare() {
    if (m_info.is_prepared) {
        return true;
    }
    if (this->open() == LIBUSB_SUCCESS) {
        this->m_info.manufacturer = this->getDescString(m_desc.iManufacturer);
        this->m_info.product = this->getDescString(m_desc.iProduct);
        this->m_info.serial_number = this->getDescString(m_desc.iSerialNumber);
        this->close();
        this->m_info.is_prepared = true;
        return true;
    }
    _ERROR("Failed to prepare device: %s", this->toString());
    return false;
}


int LibusbDevice::open() {
    if (m_handle) {
        return LIBUSB_SUCCESS;
    }

    int ret = libusb_open(m_dev, &m_handle);
    _ERROR_RETURN_IF(ret != 0, ret, "Failed to open device: %s", LibusbUtils::errString(ret))

    m_info.is_opened = true;
    if (this->m_info.product.empty()) {
        this->m_info.manufacturer = this->getDescString(m_desc.iManufacturer);
        this->m_info.product = this->getDescString(m_desc.iProduct);
        this->m_info.serial_number = this->getDescString(m_desc.iSerialNumber);
    }

    return ret;
}

int LibusbDevice::active(const int interfaceNumber) const {
    _FATAL_IF(!m_handle, "Device is not open");

    int ret = libusb_kernel_driver_active(m_handle, interfaceNumber);
    if (ret != 0) {
        if (libusb_has_capability(LIBUSB_CAP_SUPPORTS_DETACH_KERNEL_DRIVER)) {
            ret = libusb_detach_kernel_driver(m_handle, interfaceNumber);
            if (ret != 0) {
                _ERROR("Failed to detach kernel driver: %s", LibusbUtils::errString(ret));
            }
        }
    }
    return ret;
}

int LibusbDevice::claimInterface(const LibusbInterfaceSetting& setting, libusb_endpoint_transfer_type transType) {
    _FATAL_IF(!m_handle, "Device is not open");
    LibusbInterfaceSetting activeSetting = setting;
    if (!activeSetting.activeInEndpoint(transType)) {
        libusb_release_interface(m_handle, setting.number());
        _ERROR("Interface[%d] does not have active in endpoint of transfer type: %s",
            setting.number(), LibusbUtils::endpointTransferType(transType));
        return -1;
    }
    if (!activeSetting.activeOutEndpoint(transType)) {
        libusb_release_interface(m_handle, setting.number());
        _ERROR("Interface[%d] does not have active out endpoint of transfer type: %s",
            setting.number(), LibusbUtils::endpointTransferType(transType));
        return -1;
    }
    int ret = libusb_claim_interface(m_handle, activeSetting.number());
    if (ret != 0) {
        _ERROR("Failed to claim interface: %s", LibusbUtils::errString(ret));
        return ret;
    }
    m_active_interface = activeSetting;
    _INFO("Interface claimed: %s", m_active_interface.toString());
    return LIBUSB_SUCCESS;
}

int LibusbDevice::transCtrl(uint8_t reqType, uint8_t req,
                           uint16_t val, uint16_t index, uint8_t* data, uint16_t wLength, uint32_t timeout) const {
    _FATAL_IF(!m_handle, "Device is not open");

    // 返回的是传输的字节数
    int ret = libusb_control_transfer(m_handle, reqType, req, val, index, data, wLength, timeout);
    _ERROR_IF(ret < 0, "Failed to control transfer(type: %d, request: %d, value: %d, index: %d): %s",
              reqType, req, val, index, LibusbUtils::errString(ret));
    return ret;
}

int LibusbDevice::sendBulk(unsigned char* data, int length, int* actual_length, unsigned int timeout) {
    _FATAL_IF(!m_handle, "Device is not open");
    _FATAL_IF(!m_active_interface.valid(), "Interface is not claimed");

    uint8_t endpoint = m_active_interface.getActiveOutEndpoint().address();
    int ret = libusb_bulk_transfer(m_handle, endpoint, data, length, actual_length, timeout);
    _ERROR_IF(ret != 0 && ret != LIBUSB_ERROR_TIMEOUT, "Failed to send bulk transfer(endpoint: %d): %s", endpoint, LibusbUtils::errString(ret));
    return ret;
}

int LibusbDevice::recvBulk(unsigned char* data, int length, int* actual_length, unsigned int timeout) {
    _FATAL_IF(!m_handle, "Device is not open");
    _FATAL_IF(!m_active_interface.valid(), "Interface is not claimed");

    uint8_t endpoint = m_active_interface.getActiveInEndpoint().address();
    int ret = libusb_bulk_transfer(m_handle, endpoint, data, length, actual_length, timeout);
    _ERROR_IF(ret != 0 && ret != LIBUSB_ERROR_TIMEOUT, "Failed to recv bulk transfer(endpoint: %d): %s", endpoint, LibusbUtils::errString(ret));
    return ret;
}

int LibusbDevice::sendInterrupt(unsigned char* data, int length, int* actual_length, unsigned int timeout) {
    _FATAL_IF(!m_handle, "Device is not open");
    _FATAL_IF(!m_active_interface.valid(), "Interface is not claimed");

    uint8_t endpoint = m_active_interface.getActiveOutEndpoint().address();
    int ret = libusb_interrupt_transfer(m_handle, endpoint, data, length, actual_length, timeout);
    _ERROR_IF(ret != 0 && ret != LIBUSB_ERROR_TIMEOUT, "Failed to send interrupt transfer(endpoint: %d): %s", endpoint, LibusbUtils::errString(ret));
    return ret;
}

int LibusbDevice::recvInterrupt(unsigned char* data, int length, int* actual_length, unsigned int timeout) {
    _FATAL_IF(!m_handle, "Device is not open");
    _FATAL_IF(!m_active_interface.valid(), "Interface is not claimed");

    uint8_t endpoint = m_active_interface.getActiveInEndpoint().address();
    int ret = libusb_interrupt_transfer(m_handle, endpoint, data, length, actual_length, timeout);
    _ERROR_IF(ret != 0 && ret != LIBUSB_ERROR_TIMEOUT, "Failed to recv interrupt transfer(endpoint: %d): %s", endpoint, LibusbUtils::errString(ret));
    return ret;
}

void LibusbDevice::close() {
    if (m_active_interface.valid()) {
        libusb_release_interface(m_handle, m_active_interface.number());
    }
    if (m_handle) {
        libusb_close(m_handle);
        m_handle = nullptr;
        m_info.is_opened = false;
    }
}

NAMESPACE_END
