#include "LidarScanData.h"

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
std::vector<char> LidarScanData::SerializeLidarScanData(const LidarScanData& lidarScanData)
{
	std::vector<char> buffer;

	buffer.clear();
	uint32_t offset = 0;

	// std_msgs/msg/Header frame_id size
	uint32_t header_frame_id_size = lidarScanData.frameId.size();
	buffer.resize(offset + sizeof(header_frame_id_size));
	memcpy(buffer.data() + offset, &header_frame_id_size, sizeof(header_frame_id_size));
	offset += sizeof(header_frame_id_size);

	// std_msgs/msg/Header frame_id data
	buffer.resize(offset + header_frame_id_size);
	memcpy(buffer.data() + offset, lidarScanData.frameId.data(), header_frame_id_size);
	offset += header_frame_id_size;

	// builtin_interfaces/msg/Time sec
	buffer.resize(offset + sizeof(lidarScanData.stampSec));
	memcpy(buffer.data() + offset, &lidarScanData.stampSec, sizeof(lidarScanData.stampSec));
	offset += sizeof(lidarScanData.stampSec);

	// builtin_interfaces/msg/Time nanosec
	buffer.resize(offset + sizeof(lidarScanData.stampNanosec));
	memcpy(buffer.data() + offset, &lidarScanData.stampNanosec, sizeof(lidarScanData.stampNanosec));
	offset += sizeof(lidarScanData.stampNanosec);

	// sensor_msgs/msg/LaserScan angle_min
	buffer.resize(offset + sizeof(lidarScanData.angleMin));
	memcpy(buffer.data() + offset, &lidarScanData.angleMin, sizeof(lidarScanData.angleMin));
	offset += sizeof(lidarScanData.angleMin);

	// sensor_msgs/msg/LaserScan angle_max
	buffer.resize(offset + sizeof(lidarScanData.angleMax));
	memcpy(buffer.data() + offset, &lidarScanData.angleMax, sizeof(lidarScanData.angleMax));
	offset += sizeof(lidarScanData.angleMax);

	// sensor_msgs/msg/LaserScan angle_increment
	buffer.resize(offset + sizeof(lidarScanData.angleIncrement));
	memcpy(buffer.data() + offset, &lidarScanData.angleIncrement, sizeof(lidarScanData.angleIncrement));
	offset += sizeof(lidarScanData.angleIncrement);

	// sensor_msgs/msg/LaserScan time_increment
	buffer.resize(offset + sizeof(lidarScanData.timeIncrement));
	memcpy(buffer.data() + offset, &lidarScanData.timeIncrement, sizeof(lidarScanData.timeIncrement));
	offset += sizeof(lidarScanData.timeIncrement);

	// sensor_msgs/msg/LaserScan scan_time
	buffer.resize(offset + sizeof(lidarScanData.scanTime));
	memcpy(buffer.data() + offset, &lidarScanData.scanTime, sizeof(lidarScanData.scanTime));
	offset += sizeof(lidarScanData.scanTime);

	// sensor_msgs/msg/LaserScan range_min
	buffer.resize(offset + sizeof(lidarScanData.rangeMin));
	memcpy(buffer.data() + offset, &lidarScanData.rangeMin, sizeof(lidarScanData.rangeMin));
	offset += sizeof(lidarScanData.rangeMin);

	// sensor_msgs/msg/LaserScan range_max
	buffer.resize(offset + sizeof(lidarScanData.rangeMax));
	memcpy(buffer.data() + offset, &lidarScanData.rangeMax, sizeof(lidarScanData.rangeMax));
	offset += sizeof(lidarScanData.rangeMax);

	// sensor_msgs/msg/LaserScan ranges size
	uint32_t ranges_size = lidarScanData.ranges.size();
	buffer.resize(offset + sizeof(ranges_size));
	memcpy(buffer.data() + offset, &ranges_size, sizeof(ranges_size));
	offset += sizeof(ranges_size);

	// sensor_msgs/msg/LaserScan ranges data
	buffer.resize(offset + ranges_size * sizeof(float));
	memcpy(buffer.data() + offset, lidarScanData.ranges.data(), ranges_size * sizeof(float));
	offset += ranges_size * sizeof(float);

	// sensor_msgs/msg/LaserScan intensities size
	uint32_t intensities_size = lidarScanData.intensities.size();
	buffer.resize(offset + sizeof(intensities_size));
	memcpy(buffer.data() + offset, &intensities_size, sizeof(intensities_size));
	offset += sizeof(intensities_size);

	// sensor_msgs/msg/LaserScan intensities data
	buffer.resize(offset + intensities_size * sizeof(float));
	memcpy(buffer.data() + offset, lidarScanData.intensities.data(), intensities_size * sizeof(float));
	offset += intensities_size * sizeof(float);

	return buffer;
}