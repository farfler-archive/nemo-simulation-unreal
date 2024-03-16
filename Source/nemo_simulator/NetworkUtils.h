#pragma once

#include "CoreMinimal.h"

namespace NetworkUtils
{
	uint32_t ToNetworkOrder(uint32_t value);
	std::vector<uint8_t> CreateDataPacketWithHeader(const std::vector<uint8_t>& Data);
}
