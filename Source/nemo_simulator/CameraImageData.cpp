#include "CameraImageData.h"
#include "NetworkUtils.h"

// Print CameraImageData to the console
void CameraImageData::PrintCameraImageData(const CameraImageData& cameraImageData)
{
    UE_LOG(LogTemp, Log, TEXT("01. frameId: %s"), *FString(cameraImageData.frameId.c_str()));
	UE_LOG(LogTemp, Log, TEXT("02. stampSec: %d"), cameraImageData.stampSec);
	UE_LOG(LogTemp, Log, TEXT("03. stampNanosec: %d"), cameraImageData.stampNanosec);
	UE_LOG(LogTemp, Log, TEXT("04. height: %d"), cameraImageData.height);
	UE_LOG(LogTemp, Log, TEXT("05. width: %d"), cameraImageData.width);
	UE_LOG(LogTemp, Log, TEXT("06. encoding: %s"), *FString(cameraImageData.encoding.c_str()));
	UE_LOG(LogTemp, Log, TEXT("07. isBigendian: %d"), cameraImageData.isBigendian);
	UE_LOG(LogTemp, Log, TEXT("08. step: %d"), cameraImageData.step);
	UE_LOG(LogTemp, Log, TEXT("09. data size: %d"), cameraImageData.data.size());
    UE_LOG(LogTemp, Log, TEXT("10. data[0]: %d"), cameraImageData.data[0]);
    UE_LOG(LogTemp, Log, TEXT("11. data[-1]: %d"), cameraImageData.data[cameraImageData.data.size() - 1]);
}

// Serialize CameraImageData to a byte array
std::vector<uint8_t> CameraImageData::SerializeCameraImageData(const CameraImageData& cameraImageData)
{
    std::vector<uint8_t> buffer;
    buffer.clear();
    uint32_t offset = 0;

    // std_msgs/msg/Header frame_id size
    uint32_t header_frame_id_size = cameraImageData.frameId.size();
    uint32_t header_frame_id_size_network_order = NetworkUtils::ToNetworkOrder(header_frame_id_size);
    buffer.resize(offset + sizeof(header_frame_id_size));
    memcpy(buffer.data() + offset, &header_frame_id_size_network_order, sizeof(header_frame_id_size));
    offset += sizeof(header_frame_id_size);

    // std_msgs/msg/Header frame_id data
    buffer.resize(offset + header_frame_id_size);
    memcpy(buffer.data() + offset, cameraImageData.frameId.data(), header_frame_id_size);
    offset += header_frame_id_size;

    // builtin_interfaces/msg/Time sec
    buffer.resize(offset + sizeof(cameraImageData.stampSec));
    memcpy(buffer.data() + offset, &cameraImageData.stampSec, sizeof(cameraImageData.stampSec));
    offset += sizeof(cameraImageData.stampSec);

    // builtin_interfaces/msg/Time nanosec
    buffer.resize(offset + sizeof(cameraImageData.stampNanosec));
    memcpy(buffer.data() + offset, &cameraImageData.stampNanosec, sizeof(cameraImageData.stampNanosec));
    offset += sizeof(cameraImageData.stampNanosec);

    // sensor_msgs/msg/Image height
    buffer.resize(offset + sizeof(cameraImageData.height));
    memcpy(buffer.data() + offset, &cameraImageData.height, sizeof(cameraImageData.height));
    offset += sizeof(cameraImageData.height);

    // sensor_msgs/msg/Image width
    buffer.resize(offset + sizeof(cameraImageData.width));
    memcpy(buffer.data() + offset, &cameraImageData.width, sizeof(cameraImageData.width));
    offset += sizeof(cameraImageData.width);

    // sensor_msgs/msg/Image encoding size
    uint32_t encoding_size = cameraImageData.encoding.size();
    uint32_t encoding_size_network_order = NetworkUtils::ToNetworkOrder(encoding_size);
    buffer.resize(offset + sizeof(encoding_size));
    memcpy(buffer.data() + offset, &encoding_size_network_order, sizeof(encoding_size));
    offset += sizeof(encoding_size);

    // sensor_msgs/msg/Image encoding data
    buffer.resize(offset + encoding_size);
    memcpy(buffer.data() + offset, cameraImageData.encoding.data(), encoding_size);
    offset += encoding_size;

    // sensor_msgs/msg/Image is_bigendian
    buffer.resize(offset + sizeof(cameraImageData.isBigendian));
    memcpy(buffer.data() + offset, &cameraImageData.isBigendian, sizeof(cameraImageData.isBigendian));
    offset += sizeof(cameraImageData.isBigendian);

    // sensor_msgs/msg/Image step
    buffer.resize(offset + sizeof(cameraImageData.step));
    memcpy(buffer.data() + offset, &cameraImageData.step, sizeof(cameraImageData.step));
    offset += sizeof(cameraImageData.step);

    // sensor_msgs/msg/Image data size
    uint32_t data_size = cameraImageData.data.size();
    uint32_t data_size_network_order = NetworkUtils::ToNetworkOrder(data_size);
    buffer.resize(offset + sizeof(data_size));
    memcpy(buffer.data() + offset, &data_size_network_order, sizeof(data_size));
    offset += sizeof(data_size);

    // sensor_msgs/msg/Image data
    buffer.resize(offset + data_size);
    memcpy(buffer.data() + offset, cameraImageData.data.data(), data_size);
    offset += data_size;

    return buffer;
}