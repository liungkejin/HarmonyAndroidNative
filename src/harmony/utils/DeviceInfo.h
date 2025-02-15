//
// Created on 2025/2/15.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".


#pragma once
#include "common/Common.h"
#include <deviceinfo.h>

NAMESPACE_DEFAULT

class DeviceInfo {
public:
    /**
     * Obtains the device type represented by a string,
     * which can be {@code phone} (or {@code default} for phones), {@code wearable}, {@code liteWearable},
     * {@code tablet}, {@code tv}, {@code car}, or {@code smartVision}.
     */
    static std::string deviceType() { return OH_GetDeviceType(); }
    /**
     * Obtains the device manufacturer represented by a string.
     */
    static std::string manufacture(void) { return OH_GetManufacture(); }

    /**
     * Obtains the device brand represented by a string.
     * @syscap SystemCapability.Startup.SystemInfo
     * @since 10
     */
    static std::string brand(void) { return OH_GetBrand(); }

    /**
     * Obtains the product name speaded in the market
     * @syscap SystemCapability.Startup.SystemInfo
     * @since 10
     */
    static std::string marketName(void) { return OH_GetMarketName(); }

    /**
     * Obtains the product series represented by a string.
     * @syscap SystemCapability.Startup.SystemInfo
     * @since 10
     */
    static std::string productSeries(void) { return OH_GetProductSeries(); }

    /**
     * Obtains the product model represented by a string.
     * @syscap SystemCapability.Startup.SystemInfo
     * @since 10
     */
    static std::string getProductModel(void) { return OH_GetProductModel(); }

    /**
     * Obtains the software model represented by a string.
     * @syscap SystemCapability.Startup.SystemInfo
     * @since 10
     */
    static std::string softwareModel(void) { return OH_GetSoftwareModel(); }

    /**
     * Obtains the hardware model represented by a string.
     * @syscap SystemCapability.Startup.SystemInfo
     * @since 10
     */
    static std::string hardwareModel(void) { return OH_GetHardwareModel(); }

    /**
     * Obtains the bootloader version number represented by a string.
     * @syscap SystemCapability.Startup.SystemInfo
     * @since 10
     */
    static std::string bootloaderVersion(void) { return OH_GetBootloaderVersion(); }

    /**
     * Obtains the application binary interface (Abi) list represented by a string.
     * @syscap SystemCapability.Startup.SystemInfo
     * @since 10
     */
    static std::string abiList(void) { return OH_GetAbiList(); }

    /**
     * Obtains the security patch tag represented by a string.
     * @syscap SystemCapability.Startup.SystemInfo
     * @since 10
     */
    static std::string securityPatchTag(void) { return OH_GetSecurityPatchTag(); }

    /**
     * Obtains the product version displayed for customer represented by a string.
     * @syscap SystemCapability.Startup.SystemInfo
     * @since 10
     */
    static std::string displayVersion(void) { return OH_GetDisplayVersion(); }

    /**
     * Obtains the incremental version represented by a string.
     * @syscap SystemCapability.Startup.SystemInfo
     * @since 10
     */
    static std::string incrementalVersion(void) { return OH_GetIncrementalVersion(); }

    /**
     * Obtains the OS release type represented by a string.
     *
     * <p>The OS release category can be {@code Release}, {@code Beta}, or {@code Canary}.
     * The specific release type may be {@code Release}, {@code Beta1}, or others alike.
     * @syscap SystemCapability.Startup.SystemInfo
     * @since 10
     */
    static std::string osReleaseType(void) { return OH_GetOsReleaseType(); }

    /**
     * Obtains the OS full version name represented by a string.
     * @syscap SystemCapability.Startup.SystemInfo
     * @since 10
     */
    static std::string osFullName(void) { return OH_GetOSFullName(); }

    /**
     * Obtains the SDK API version number.
     * @syscap SystemCapability.Startup.SystemInfo
     * @since 10
     */
    static int sdkApiVersion(void) { return OH_GetSdkApiVersion(); }

    /**
     * Obtains the first API version number.
     * @syscap SystemCapability.Startup.SystemInfo
     * @since 10
     */
    static int firstApiVersion(void) { return OH_GetFirstApiVersion(); }

    /**
     * Obtains the version ID by a string.
     * @syscap SystemCapability.Startup.SystemInfo
     * @since 10
     */
    static std::string versionId(void) { return OH_GetVersionId(); }

    /**
     * Obtains the build type of the current running OS.
     * @syscap SystemCapability.Startup.SystemInfo
     * @since 10
     */
    static std::string buildType(void) { return OH_GetBuildType(); }

    /**
     * Obtains the build user of the current running OS.
     * @syscap SystemCapability.Startup.SystemInfo
     * @since 10
     */
    static std::string buildUser(void) { return OH_GetBuildUser(); }

    /**
     * Obtains the build host of the current running OS.
     * @syscap SystemCapability.Startup.SystemInfo
     * @since 10
     */
    static std::string buildHost(void) { return OH_GetBuildHost(); }

    /**
     * Obtains the build time of the current running OS.
     * @syscap SystemCapability.Startup.SystemInfo
     * @since 10
     */
    static std::string buildTime(void) { return OH_GetBuildTime(); }

    /**
     * Obtains the version hash of the current running OS.
     * @syscap SystemCapability.Startup.SystemInfo
     * @since 10
     */
    static std::string buildRootHash(void) { return OH_GetBuildRootHash(); }

    /**
     * Obtains the Distribution OS name represented by a string.
     *
     * <p>Independent Software Vendor (ISV) may distribute OHOS with their own OS name.
     * If ISV not specified, it will return an empty string
     * @syscap SystemCapability.Startup.SystemInfo
     * @since 10
     */
    static std::string distributionOSName(void) { return OH_GetDistributionOSName(); }

    /**
     * Obtains the ISV distribution OS version represented by a string.
     * If ISV not specified, it will return the same value as OH_GetOSFullName
     * @syscap SystemCapability.Startup.SystemInfo
     * @since 10
     */
    static std::string distributionOSVersion(void) { return OH_GetDistributionOSVersion(); }

    /**
     * Obtains the ISV distribution OS api version represented by a integer.
     * If ISV not specified, it will return the same value as OH_GetSdkApiVersion
     * @syscap SystemCapability.Startup.SystemInfo
     * @since 10
     */
    static int distributionOSApiVersion(void) { return OH_GetDistributionOSApiVersion(); }

    /**
     * Obtains the ISV distribution OS release type represented by a string.
     * If ISV not specified, it will return the same value as OH_GetOsReleaseType
     * @syscap SystemCapability.Startup.SystemInfo
     * @since 10
     */
    static std::string distributionOSReleaseType(void) { return OH_GetDistributionOSReleaseType(); }

public:
    static std::string dump() {
        std::stringstream ss;
        ss << "Device Info {\n";
        ss << "  DeviceType: " << deviceType() << "\n";
        ss << "  Manufacture: " << manufacture() << "\n";
        ss << "  Brand: " << brand() << "\n";
        ss << "  MarketName: " << marketName() << "\n";
        ss << "  ProductSeries: " << productSeries() << "\n";
        ss << "  ProductModel: " << getProductModel() << "\n";
        ss << "  SoftwareModel: " << softwareModel() << "\n";
        ss << "  HardwareModel: " << hardwareModel() << "\n";
        ss << "  BootloaderVersion: " << bootloaderVersion() << "\n";
        ss << "  AbiList: " << abiList() << "\n";
        ss << "  SecurityPatchTag: " << securityPatchTag() << "\n";
        ss << "  DisplayVersion: " << displayVersion() << "\n";
        ss << "  IncrementalVersion: " << incrementalVersion() << "\n";
        ss << "  OsReleaseType: " << osReleaseType() << "\n";
        ss << "  OsFullName: " << osFullName() << "\n";
        ss << "  SdkApiVersion: " << std::to_string(sdkApiVersion()) << "\n";
        ss << "  FirstApiVersion: " << std::to_string(firstApiVersion()) << "\n";
        ss << "  VersionId: " << versionId() << "\n";
        ss << "  BuildType: " << buildType() << "\n";
        ss << "  BuildUser: " << buildUser() << "\n";
        ss << "  BuildHost: " << buildHost() << "\n";
        ss << "  BuildTime: " << buildTime() << "\n";
        ss << "  BuildRootHash: " << buildRootHash() << "\n";
        ss << "  DistributionOSName: " << distributionOSName() << "\n";
        ss << "  DistributionOSVersion: " << distributionOSVersion() << "\n";
        ss << "  DistributionOSApiVersion: " << std::to_string(distributionOSApiVersion()) << "\n";
        ss << "  DistributionOSReleaseType: " << distributionOSReleaseType() << "\n";
        ss << "}\n";
        return ss.str();
    }
};

NAMESPACE_END
