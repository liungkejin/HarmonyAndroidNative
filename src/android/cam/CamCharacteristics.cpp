//
// Created by LiangKeJin on 2024/12/7.
//

#include "CamCharacteristics.h"

NAMESPACE_DEFAULT

void CamCharacteristics::dump() {
    _INFO("dump camera characteristics");
    auto tags = getAllTags();
    for (auto tag : tags) {
        _INFO("tag: %s", CamUtils::tagString(tag));
    }
}

NAMESPACE_END