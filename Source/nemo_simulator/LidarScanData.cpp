#include "LidarScanData.h"
#include "NetworkUtils.h"

// Print LidarScanData to the console
void LidarScanData::PrintLidarScanData(const LidarScanData& lidarScanData)
{
	UE_LOG(LogTemp, Log, TEXT("01. frameId: %s"), *FString(lidarScanData.frameId.c_str()));
	UE_LOG(LogTemp, Log, TEXT("02. stampSec: %d"), lidarScanData.stampSec);
	UE_LOG(LogTemp, Log, TEXT("03. stampNanosec: %d"), lidarScanData.stampNanosec);
	UE_LOG(LogTemp, Log, TEXT("04. angleMin: %f"), lidarScanData.angleMin);
	UE_LOG(LogTemp, Log, TEXT("05. angleMax: %f"), lidarScanData.angleMax);
	UE_LOG(LogTemp, Log, TEXT("06. angleIncrement: %f"), lidarScanData.angleIncrement);
	UE_LOG(LogTemp, Log, TEXT("07. timeIncrement: %f"), lidarScanData.timeIncrement);
	UE_LOG(LogTemp, Log, TEXT("08. scanTime: %f"), lidarScanData.scanTime);
	UE_LOG(LogTemp, Log, TEXT("09. rangeMin: %f"), lidarScanData.rangeMin);
	UE_LOG(LogTemp, Log, TEXT("10. rangeMax: %f"), lidarScanData.rangeMax);
	UE_LOG(LogTemp, Log, TEXT("11. length of ranges: %d"), lidarScanData.ranges.size());
	UE_LOG(LogTemp, Log, TEXT("12. ranges[0]: %f"), lidarScanData.ranges[0]);
	UE_LOG(LogTemp, Log, TEXT("13. ranges[-1]: %f"), lidarScanData.ranges[lidarScanData.ranges.size() - 1]);
	UE_LOG(LogTemp, Log, TEXT("14. length of intensities: %d"), lidarScanData.intensities.size());
	UE_LOG(LogTemp, Log, TEXT("15. intensities[0]: %f"), lidarScanData.intensities[0]);
	UE_LOG(LogTemp, Log, TEXT("16. intensities[-1]: %f"), lidarScanData.intensities[lidarScanData.intensities.size() - 1]);
}

// Serialize LidarScanData to a byte array
std::vector<uint8_t> LidarScanData::SerializeLidarScanData(const LidarScanData& lidarScanData)
{
	std::vector<uint8_t> buffer;

	buffer.clear();
	uint32_t offset = 0;

	buffer.insert(buffer.end(), reinterpret_cast<const char *>(&lidarScanData.stampSec), reinterpret_cast<const char *>(&lidarScanData.stampSec) + sizeof(lidarScanData.stampSec));
	buffer.insert(buffer.end(), reinterpret_cast<const char *>(&lidarScanData.stampNanosec), reinterpret_cast<const char *>(&lidarScanData.stampNanosec) + sizeof(lidarScanData.stampNanosec));

	uint32_t header_frame_id_size = NetworkUtils::ToNetworkOrder(static_cast<uint32_t>(lidarScanData.frameId.size()));
	buffer.insert(buffer.end(), reinterpret_cast<const char *>(&header_frame_id_size), reinterpret_cast<const char *>(&header_frame_id_size) + sizeof(header_frame_id_size));
	buffer.insert(buffer.end(), lidarScanData.frameId.begin(), lidarScanData.frameId.end());

	buffer.insert(buffer.end(), reinterpret_cast<const char *>(&lidarScanData.angleMin), reinterpret_cast<const char *>(&lidarScanData.angleMin) + sizeof(lidarScanData.angleMin));
	buffer.insert(buffer.end(), reinterpret_cast<const char *>(&lidarScanData.angleMax), reinterpret_cast<const char *>(&lidarScanData.angleMax) + sizeof(lidarScanData.angleMax));
	buffer.insert(buffer.end(), reinterpret_cast<const char *>(&lidarScanData.angleIncrement), reinterpret_cast<const char *>(&lidarScanData.angleIncrement) + sizeof(lidarScanData.angleIncrement));
	buffer.insert(buffer.end(), reinterpret_cast<const char *>(&lidarScanData.timeIncrement), reinterpret_cast<const char *>(&lidarScanData.timeIncrement) + sizeof(lidarScanData.timeIncrement));
	buffer.insert(buffer.end(), reinterpret_cast<const char *>(&lidarScanData.scanTime), reinterpret_cast<const char *>(&lidarScanData.scanTime) + sizeof(lidarScanData.scanTime));
	buffer.insert(buffer.end(), reinterpret_cast<const char *>(&lidarScanData.rangeMin), reinterpret_cast<const char *>(&lidarScanData.rangeMin) + sizeof(lidarScanData.rangeMin));
	buffer.insert(buffer.end(), reinterpret_cast<const char *>(&lidarScanData.rangeMax), reinterpret_cast<const char *>(&lidarScanData.rangeMax) + sizeof(lidarScanData.rangeMax));

	uint32_t ranges_size = NetworkUtils::ToNetworkOrder(static_cast<uint32_t>(lidarScanData.ranges.size()));
	buffer.insert(buffer.end(), reinterpret_cast<const char *>(&ranges_size), reinterpret_cast<const char *>(&ranges_size) + sizeof(ranges_size));
	buffer.insert(buffer.end(), reinterpret_cast<const char *>(lidarScanData.ranges.data()), reinterpret_cast<const char *>(lidarScanData.ranges.data()) + lidarScanData.ranges.size() * sizeof(float));

	uint32_t intensities_size = NetworkUtils::ToNetworkOrder(static_cast<uint32_t>(lidarScanData.intensities.size()));
	buffer.insert(buffer.end(), reinterpret_cast<const char *>(&intensities_size), reinterpret_cast<const char *>(&intensities_size) + sizeof(intensities_size));
	buffer.insert(buffer.end(), reinterpret_cast<const char *>(lidarScanData.intensities.data()), reinterpret_cast<const char *>(lidarScanData.intensities.data()) + lidarScanData.intensities.size() * sizeof(float));

	return buffer;
}