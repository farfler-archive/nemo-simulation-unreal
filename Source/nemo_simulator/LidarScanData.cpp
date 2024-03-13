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
