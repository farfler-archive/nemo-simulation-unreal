#include "NetworkUtils.h"

uint32_t NetworkUtils::ToNetworkOrder(uint32_t value)
{
	uint32_t result;
	uint8_t* resultBytes = reinterpret_cast<uint8_t*>(&result);
	uint8_t* valueBytes = reinterpret_cast<uint8_t*>(&value);

	resultBytes[0] = valueBytes[3];
	resultBytes[1] = valueBytes[2];
	resultBytes[2] = valueBytes[1];
	resultBytes[3] = valueBytes[0];

	return result;
}

std::vector<uint8_t> NetworkUtils::CreateDataPacketWithHeader(const std::vector<uint8_t>& Data)
{
	std::vector<uint8_t> DataPacket;
	uint32_t DataSize = static_cast<uint32_t>(Data.size());
	*reinterpret_cast<uint32_t*>(DataPacket.data()) = NetworkUtils::ToNetworkOrder(DataSize);
	DataPacket.insert(DataPacket.end(), Data.begin(), Data.end());
	return DataPacket;
}
