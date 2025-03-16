/**
* Copy right (c) 2024 Ka Chun Wong. All rights reserved.
* This is a open source project under MIT license (see LICENSE for details).
* If you find any bugs, please feel free to report under https://github.com/kcwongjoe/directshow_camera/issues
**/

#include "directshow_camera/utils/win32_utils.h"

#include <sstream>

namespace Win32Utils
{
    std::string BSTRToString(const BSTR bstr, const int cp)
    {
        if (!bstr) {
            return "";
        }
        int size = WideCharToMultiByte(cp, 0, bstr, -1, nullptr, 0, nullptr, nullptr);
        if (size < 1) {
            return "";
        }
        std::string result(size - 1, '\0');
        WideCharToMultiByte(cp, 0, bstr, -1, &result[0], size, nullptr, nullptr);
        return result;
    }


    std::string ToString(const GUID guid)
    {
        std::ostringstream os;
        os.fill('0');

        os << std::uppercase;
        os.width(8);
        os << std::hex << guid.Data1;
        
        os << '-';

        os.width(4);
        os << std::hex << guid.Data2;
        
        os << '-';

        os.width(4);
        os << std::hex << guid.Data3;
        
        os << '-';

        os.width(2);
        os << std::hex << static_cast<short>(guid.Data4[0]);

        os.width(2);
        os << std::hex << static_cast<short>(guid.Data4[1]);

        os << '-';

        os.width(2);
        os << std::hex << static_cast<short>(guid.Data4[2]);

        os.width(2);
        os << std::hex << static_cast<short>(guid.Data4[3]);

        os.width(2);
        os << std::hex << static_cast<short>(guid.Data4[4]);

        os.width(2);
        os << std::hex << static_cast<short>(guid.Data4[5]);

        os.width(2);
        os << std::hex << static_cast<short>(guid.Data4[6]);

        os.width(2);
        os << std::hex << static_cast<short>(guid.Data4[7]);

        os << std::nouppercase;

        const std::string result = os.str();

         return result;
    }
}